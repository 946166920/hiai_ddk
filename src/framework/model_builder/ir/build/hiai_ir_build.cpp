/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "model_builder/hiai_ir_build.h"

#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <set>

#include "model_builder/model_builder.h"
#include "model_builder/compatible/BuildOptionUtil.h"
#ifdef USE_STATIC_LIB
#include "HIAIModelManager_internal.h"
#else
#include "framework/c/compatible/HIAIModelManager.h"
#include "framework/c/compatible/hiai_mem_buffer.h"
#endif

#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/compatible/ir_transformer.h"
#include "model/built_model_aipp.h"
#include "model_builder/ir/aipp/compatible/hiai_ir_aipp_compatible_adapter_dl.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"
#include "framework/infra/log/log_fmk_interface.h"
#include "framework/infra/log/log.h"

using namespace std;
namespace hiai {
#ifdef __LP64__
static const string AI_CLIENT_SO_PATH = "/vendor/lib64/libai_client.so";
static const string AI_FOUNDATION_SO_PATH = "/system/lib64/libai_client.so";
#else
static const string AI_CLIENT_SO_PATH = "/vendor/lib/libai_client.so";
static const string AI_FOUNDATION_SO_PATH = "/system/lib64/libai_client.so";
#endif

using HIAI_MEMBUFFER_CREATE_FUNC = HIAI_MemBuffer* (*)(HIAI_Framework, HIAI_MemBuffer*[], const unsigned int);
using HIAI_MEMBUFFER_CREATE_FUNC_V2 = HIAI_MemBuffer* (*)(HIAI_Framework, HIAI_MemBuffer*[], unsigned int,
    unsigned int);
using HIAI_GET_HIAIVERISON_FUNC = char* (*)();

const uint32_t MAX_BUILD_MODEL_SIZE = (200 * 1024 * 1024); // max model size: 200MB

namespace {
bool isSupportFunc(void* libHandle, const std::string& funcname)
{
    if (libHandle == nullptr || funcname.empty()) {
        return false;
    }
    void* func = dlsym(libHandle, funcname.c_str());
    if (func == nullptr) {
        return false;
    }

    return true;
}

bool createV2(void* libHandle, ge::Model& irModel, uint32_t customSize, HIAI_MemBuffer*& omModelBuffer)
{
    if (libHandle == nullptr) {
        FMK_LOGE("libHandle is nullptr.");
        return false;
    }
    HIAI_MEMBUFFER_CREATE_FUNC_V2 createMembuffer = nullptr;
    *reinterpret_cast<void**>(&createMembuffer) = dlsym(libHandle, "HIAI_MemBuffer_createV2");
    char* errorInfo = dlerror();
    if (createMembuffer == nullptr) {
        FMK_LOGE("dlsym fail:%s", errorInfo);
        return false;
    }
    ge::Buffer irmodel_buff;
    irModel.Save(irmodel_buff);

    HIAI_MemBuffer* irModelBuffer[1];
    irModelBuffer[0] = reinterpret_cast<HIAI_MemBuffer*>(malloc(sizeof(HIAI_MemBuffer)));
    if (irModelBuffer[0] == nullptr) {
        FMK_LOGE("create model buffer failed. malloc fail!");
        return false;
    }

    irModelBuffer[0]->size = irmodel_buff.GetSize();
    omModelBuffer = createMembuffer(HIAI_Framework::HIAI_FRAMEWORK_IR, irModelBuffer, 1, customSize);
    if (omModelBuffer == nullptr) {
        FMK_LOGE("create model buffer failed.");
        free(irModelBuffer[0]);
        return false;
    }

    free(irModelBuffer[0]);
    return true;
}

bool create(void* libHandle, HIAI_MemBuffer*& omModelBuffer)
{
    if (libHandle == nullptr) {
        FMK_LOGE("libHandle is nullptr.");
        return false;
    }
    HIAI_MEMBUFFER_CREATE_FUNC createMembuffer = nullptr;
    *reinterpret_cast<void**>(&createMembuffer) = dlsym(libHandle, "HIAI_MemBuffer_create");
    char* errorInfo = dlerror();
    if (createMembuffer == nullptr) {
        FMK_LOGE("dlsym fail:%s", errorInfo);
        return false;
    }

    HIAI_MemBuffer* irModelBuffer[1];
    omModelBuffer = createMembuffer(HIAI_Framework::HIAI_FRAMEWORK_IR, irModelBuffer, 1);
    if (omModelBuffer == nullptr) {
        FMK_LOGE("createMembuffer failed.");
        return false;
    }
    return true;
}
}; // namespace

GRAPH_API_EXPORT bool HiaiIrBuild::CreateModelBuff(ge::Model& irModel,
    ModelBufferData& output)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    const uint32_t defaultSize = 200; // default size 200M
    const uint32_t megaSize = 1024 * 1024; // mega size

    return CreateModelBuff(irModel, output, defaultSize * megaSize);
}

GRAPH_API_EXPORT bool HiaiIrBuild::CreateModelBuff(
    ge::Model& irModel, ModelBufferData& output, uint32_t customSize)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);

    if (customSize > MAX_BUILD_MODEL_SIZE) {
        FMK_LOGE("customSize limit: [209715200], now size is: %u", customSize);
        return false;
    }
    bool flag = false;
    string libraryPath = AI_CLIENT_SO_PATH;
    void* libHandle = dlopen(libraryPath.c_str(), RTLD_GLOBAL);
    if (libHandle == nullptr) {
        FMK_LOGE("dlopen ai client failed.");
        return false;
    }

    HIAI_GET_HIAIVERISON_FUNC getHIAIVersion = nullptr;
    *reinterpret_cast<void**>(&getHIAIVersion) = dlsym(libHandle, "HIAI_GetVersion");
    char* errorInfo = dlerror();
    if (getHIAIVersion == nullptr || getHIAIVersion() == nullptr) {
        FMK_LOGE("dlsym fail:%s or version is null.", errorInfo);
        dlclose(libHandle);
        return false;
    }

    string hiaiVersion = getHIAIVersion();
    if (hiaiVersion.empty()) {
        FMK_LOGE("there is no NPU.");
        dlclose(libHandle);
        return false;
    }

    HIAI_MemBuffer* omModelBuffer = nullptr;

    if (isSupportFunc(libHandle, "HIAI_MemBuffer_createV2") == true) {
        FMK_LOGI("use createV2.");
        flag = createV2(libHandle, irModel, customSize, omModelBuffer);
    } else {
        FMK_LOGI("use create.");
        flag = create(libHandle, omModelBuffer);
    }

    if (!flag) {
        dlclose(libHandle);
        return false;
    }

    output.data = omModelBuffer->data;
    output.length = omModelBuffer->size;
    free(omModelBuffer);
    dlclose(libHandle);
    return true;
}

GRAPH_API_EXPORT void HiaiIrBuild::ReleaseModelBuff(ModelBufferData& output)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    output.length = 0;
    if (output.data != nullptr) {
        free(output.data);
        output.data = nullptr;
    }
}

static std::shared_ptr<hiai::IBuiltModel> BuildModel(
    const hiai::ModelBuildOptions& buildOptions, const std::string& modelName, ge::Model& irModel)
{
    std::shared_ptr<IModelBuilder> modelBuilder = CreateModelBuilder();
    HIAI_EXPECT_NOT_NULL_R(modelBuilder, nullptr);

    ge::Buffer irModelBuff;
    if (irModel.Save(irModelBuff) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("serialize IR model failed");
        return nullptr;
    }
    void* data = static_cast<void*>(const_cast<uint8_t*>(irModelBuff.GetData()));
    std::shared_ptr<IBuffer> inputBuffer = CreateLocalBuffer(data, irModelBuff.GetSize());
    HIAI_EXPECT_NOT_NULL_R(inputBuffer, nullptr);

    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = modelBuilder->Build(buildOptions, modelName, inputBuffer, builtModel);
    if (ret != SUCCESS || builtModel == nullptr) {
        FMK_LOGE("IR API build failed.");
    }
    return builtModel;
}

bool HiaiIrBuild::BuildModelOnline(ge::Model& irModel, ModelBufferData& output, const BuildOptions& options)
{
    ModelBuildOptions buildOptions = BuildOptionUtil::Convert2ModelBuildOptions(options);
    buildOptions.estimatedOutputSize = output.length;
    std::string modelName = std::string("default_") + irModel.GetName();

    auto builtModel = BuildModel(buildOptions, modelName, irModel);
    HIAI_EXPECT_NOT_NULL_R(builtModel, false);

    std::shared_ptr<IBuffer> outputBuffer = CreateLocalBuffer(output.data, output.length, false);
    size_t realSize = 0;
    auto ret = builtModel->SaveToExternalBuffer(outputBuffer, realSize);
    HIAI_EXPECT_TRUE_R(ret == hiai::SUCCESS, false);
    HIAI_EXPECT_TRUE_R(realSize > 0, false);

    output.length = static_cast<uint32_t>(realSize);
    // max model size: 200MB
    if (output.length > MAX_BUILD_MODEL_SIZE) {
        FMK_LOGE("model size limit: [209715200], now size is: %u", output.length);
        return false;
    }

    FMK_LOGI("build ir model successfully.");
    return true;
}

static bool VerifyIRAPI(ge::ComputeGraphPtr graph)
{
    HIAI_EXPECT_NOT_NULL_R(graph, false);
    HIAI_EXPECT_TRUE_R(hiai::IRTransformer::VerifyIrReservedField(graph), false);
    return true;
}

GRAPH_API_EXPORT bool HiaiIrBuild::BuildIRModel(
    ge::Model& irModel, ModelBufferData& output, const BuildOptions& options)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    ge::AttrUtils::SetInt(&irModel, "stream_num", 1);
    ge::ComputeGraphPtr graph = ge::GraphUtils::GetComputeGraph(irModel.GetGraph());
    HIAI_EXPECT_TRUE_R(VerifyIRAPI(graph), false);

    // IR??????????????????
    return BuildModelOnline(irModel, output, options);
}

GRAPH_API_EXPORT bool HiaiIrBuild::BuildIRModel(ge::Model& irModel, ModelBufferData& output)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    BuildOptions options;
    return BuildIRModel(irModel, output, options);
}

namespace {
class ExtendedCompatibleModel {
public:
    explicit ExtendedCompatibleModel(ge::Model* model)
    {
        model_ = model;
    }

    ge::Model* model_;
    std::string aippConfig_;
};

bool IsSupportNewAipp()
{
    return false;
}

static hiai::Status MakeAippCompatible(ExtendedCompatibleModel& extendedModel)
{
    if (IsSupportNewAipp()) {
        return hiai::SUCCESS;
    }

    ge::ComputeGraphPtr graph = ge::GraphUtils::GetComputeGraph(extendedModel.model_->GetGraph());
    HIAI_EXPECT_NOT_NULL(graph);

    const char* HIAI_IR_BUILD_AIPP = "libhiai_ir_build_aipp.so";
    void* handle = dlopen(HIAI_IR_BUILD_AIPP, RTLD_LAZY);
    if (handle == nullptr) {
        FMK_LOGW("have no libhiai_ir_build_aipp.so!");
        return hiai::SUCCESS;
    }
    auto func = reinterpret_cast<Status (*)(ge::ComputeGraph&, std::string&)>(
        dlsym(handle, "GenerateAippCompatibleInfoAdapter"));
    HIAI_EXPECT_NOT_NULL(func);

    return func(*graph, extendedModel.aippConfig_);
}

static std::unique_ptr<ExtendedCompatibleModel> MakeCompatibleModel(const std::shared_ptr<ge::Model>& model)
{
    (void)ge::AttrUtils::SetInt(&*model, "stream_num", 1);
    std::unique_ptr<ExtendedCompatibleModel> compatibleModel = ge::make_unique<ExtendedCompatibleModel>(model.get());

    ge::ComputeGraphPtr graph = ge::GraphUtils::GetComputeGraph(model->GetGraph());
    HIAI_EXPECT_NOT_NULL_R(graph, nullptr);

    HIAI_EXPECT_TRUE_R(VerifyIRAPI(graph), nullptr);

    HIAI_EXPECT_EXEC_R(MakeAippCompatible(*compatibleModel.get()), nullptr);

    return compatibleModel;
}
} // namespace

static std::shared_ptr<hiai::IBuiltModel> BuildCompatibleModel(const hiai::ModelBuildOptions& options,
    const std::string& modelName, std::unique_ptr<ExtendedCompatibleModel>& compatibleModel)
{
    auto builtModel = BuildModel(options, modelName, *(compatibleModel->model_));
    HIAI_EXPECT_NOT_NULL_R(builtModel, nullptr);

    if (compatibleModel->aippConfig_.size() > 0) {
        hiai::CustomModelData customModelData {hiai::AIPP_PREPROCESS_TYPE, compatibleModel->aippConfig_};
        builtModel->SetCustomData(customModelData);
    }

    FMK_LOGI("Build ir model success.");
    return builtModel;
}

GRAPH_API_EXPORT Status HiaiIrBuild::Build(const hiai::ModelBuildOptions& options, const std::string& modelName,
    const std::shared_ptr<ge::Model>& model, std::shared_ptr<hiai::IBuiltModel>& builtModel)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    auto compatibleModel = MakeCompatibleModel(model);
    HIAI_EXPECT_NOT_NULL(compatibleModel);

    builtModel = BuildCompatibleModel(options, modelName, compatibleModel);
    HIAI_EXPECT_NOT_NULL(builtModel);

    return hiai::SUCCESS;
}
} // namespace hiai
