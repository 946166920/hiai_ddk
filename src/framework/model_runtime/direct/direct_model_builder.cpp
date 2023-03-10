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
#include "direct_model_builder.h"

// api/framework
#include "tensor/buffer.h"

// inc
#include "infra/base/base_buffer.h"
#include "framework/model/model_type_util.h"
#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

// src/framework
#include "util/hiai_foundation_dl_helper.h"

#include "direct_built_model_impl.h"
#include "direct_common_util.h"

namespace hiai {
using HIAI_ModelManager = struct HIAI_ModelManager;
using HIAI_ModelManagerListener = struct HIAI_ModelManagerListener;

static HIAI_Status BuildModel(std::shared_ptr<BaseBuffer>& inputBuffer, HIAI_Framework fmkType,
    HIAI_FORMAT_MODE_OPTION& fmtOption, std::shared_ptr<IBuffer>& outputBuffer, uint32_t& outputSize)
{
    void* modelManagerCreateFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_create");
    void* modelManagerBuildFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_buildModel");
    void* modelManagerBuildV2Func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_buildModel_v2");
    void* modelManagerDestroyFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_destroy");
    if (modelManagerCreateFunc == nullptr || modelManagerBuildFunc == nullptr || modelManagerDestroyFunc == nullptr) {
        FMK_LOGE("build func not support.");
        return HIAI_FAILURE;
    }

    std::unique_ptr<HIAI_ModelManager, void (*)(HIAI_ModelManager*)> mmHandle(
        ((HIAI_ModelManager* (*)(HIAI_ModelManagerListener*)) modelManagerCreateFunc)(nullptr),
        [](HIAI_ModelManager* p) {
            void* modelManagerDestroyFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_destroy");
            ((void (*)(HIAI_ModelManager*))modelManagerDestroyFunc)(p);
        });
    if (mmHandle == nullptr) {
        FMK_LOGE("create model manager failed.");
        return HIAI_FAILURE;
    }

    HIAI_MemBuffer outMemBuffer {static_cast<unsigned int>(outputBuffer->GetSize()), outputBuffer->GetData()};
    HIAI_MemBuffer inMemBuffer {static_cast<unsigned int>(inputBuffer->GetSize()),
                                static_cast<void*>(const_cast<uint8_t*>(inputBuffer->GetData()))};
    HIAI_MemBuffer* inMemBuffers[] = {&inMemBuffer};

    using BuildModelFunc = int (*)(
        HIAI_ModelManager*, HIAI_Framework, HIAI_MemBuffer*[], const unsigned int, HIAI_MemBuffer*, unsigned int*);
    using BuildModelV2Func = int (*)(HIAI_ModelManager*, HIAI_Framework, HIAI_MemBuffer*[], const unsigned int,
        HIAI_MemBuffer*, unsigned int*, bool);

    // rom?????????buildModelV2????????????useOriginFormat=true????????????
    bool isOriginFormat = (fmtOption == HIAI_FORMAT_MODE_OPTION::HIAI_FORMAT_MODE_USE_ORIGIN);
    if (modelManagerBuildV2Func == nullptr && isOriginFormat) {
        FMK_LOGE("this version don't support HIAI_FORMAT_MODE_USE_ORIGIN");
        return HIAI_FAILURE;
    }

    int ret = 0;
    if (modelManagerBuildV2Func != nullptr) {
        auto buildModelV2 = reinterpret_cast<BuildModelV2Func>(modelManagerBuildV2Func);
        ret = buildModelV2(mmHandle.get(), fmkType, inMemBuffers, 1, &outMemBuffer, &outputSize, isOriginFormat);
    } else {
        auto buildModel = reinterpret_cast<BuildModelFunc>(modelManagerBuildFunc);
        ret = buildModel(mmHandle.get(), fmkType, inMemBuffers, 1, &outMemBuffer, &outputSize);
    }

    if (ret != 0) {
        FMK_LOGE("BuildModelByDirect failed");
        return HIAI_FAILURE;
    }
    FMK_LOGI("BuildModelByDirect success");
    return HIAI_SUCCESS;
}

namespace {
inline HIAI_Status CheckParameters(const HIAI_MR_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, const size_t& inputModelSize)
{
    // ????????????
    if (modelName == nullptr || inputModelData == nullptr || inputModelSize == 0) {
        FMK_LOGE("param is invalid.");
        return HIAI_FAILURE;
    }

    // options ????????????
    if (options != nullptr && !DirectCommonUtil::IsSupportBuildOptions(options)) {
        FMK_LOGE("BuildOption isn't supported, please reset.");
        return HIAI_FAILURE;
    }
    return HIAI_SUCCESS;
}
}

HIAI_Status HIAI_DIRECT_ModelBuilder_Build(const HIAI_MR_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_MR_BuiltModel** builtModel)
{
    // WARNING[begin]: These log used for SUT test. Must confirm with tester before modification.
    FMK_LOGI("start to build model by direct");
    // WARNING[end]

    if (CheckParameters(options, modelName, inputModelData, inputModelSize) != HIAI_SUCCESS) {
        return HIAI_FAILURE;
    }

    // ??????????????????
    ModelType modelType = IR_API_GRAPH_MODEL;
    if (ModelTypeUtil::GetModelType(inputModelData, inputModelSize, modelType) != ge::SUCCESS) {
        FMK_LOGE("GetModelType failed");
        return HIAI_FAILURE;
    }

    HIAI_FORMAT_MODE_OPTION formatOption = HIAI_MR_ModelBuildOptions_GetFormatModeOption(options);

    size_t estimatedOutputSize = HIAI_MR_ModelBuildOptions_GetEstimatedOutputSize(options);
    if (estimatedOutputSize > MAX_COMPILED_TARGET_SIZE) {
        FMK_LOGE("estimatedOutputSize is too large.");
        return HIAI_FAILURE;
    }
    estimatedOutputSize =
        (estimatedOutputSize > 0 ? estimatedOutputSize : MAX_COMPILED_TARGET_SIZE); // ????????????200M??????

    HIAI_Framework fmkType = HIAI_FRAMEWORK_OFFLINE;
    switch (modelType) {
        case IR_API_GRAPH_MODEL:
            if (!DirectCommonUtil::IsSupportIRBuild()) {
                FMK_LOGE("unsupport IR Build");
                return HIAI_FAILURE;
            }
            fmkType = HIAI_FRAMEWORK_IR;
            break;
        case IR_GRAPH_MODEL:
        case HCS_PARTITION_MODEL:
        case STANDARD_IR_GRAPH_MODEL:
            if (formatOption != HIAI_FORMAT_MODE_OPTION::HIAI_FORMAT_MODE_USE_NCHW) {
                FMK_LOGE("model type %d unsupport origin format", modelType);
                return HIAI_FAILURE;
            }
            break;
        default:
            FMK_LOGE("unsupport model type %d", modelType);
            break;
    }

    // ???????????????
    auto buffer = static_cast<uint8_t*>(const_cast<void*>(inputModelData));
    auto input = make_shared_nothrow<BaseBuffer>(buffer, static_cast<size_t>(inputModelSize), false);
    auto compatibleBuffer = DirectCommonUtil::MakeCompatibleBuffer(input);
    HIAI_EXPECT_NOT_NULL_R(compatibleBuffer, HIAI_FAILURE);

    // ????????????????????????builtModel??????
    std::unique_ptr<void, std::function<void(void*)>> outputData(malloc(estimatedOutputSize),
        [](void* data) { free(data); });
    HIAI_EXPECT_NOT_NULL(outputData);

    std::shared_ptr<IBuffer> outputBuffer = CreateLocalBuffer(outputData.get(), estimatedOutputSize);
    HIAI_EXPECT_NOT_NULL(outputBuffer);

    uint32_t outputModelSize = 0;

    // ????????????
    auto ret = BuildModel(compatibleBuffer, fmkType, formatOption, outputBuffer, outputModelSize);
    if (ret != HIAI_SUCCESS || outputModelSize == 0) {
        FMK_LOGE("Build model failed.");
        return HIAI_FAILURE;
    }

    std::function<void (uint8_t*)> freeFunc = [](uint8_t* p) {
        HIAI_EXPECT_NOT_NULL_VOID(p);
        free(p);
    };
    std::shared_ptr<BaseBuffer> realBuffer = make_shared_nothrow<BaseBuffer>(
            static_cast<uint8_t*>(outputBuffer->GetData()), outputModelSize, freeFunc);
    HIAI_EXPECT_NOT_NULL(realBuffer);

    outputData.release();
    *builtModel = (HIAI_MR_BuiltModel*)new (std::nothrow) DirectBuiltModelImpl(realBuffer, modelName);
    return HIAI_SUCCESS;
}
} // namespace hiai
