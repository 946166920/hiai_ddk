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

#include "model_manager_impl.h"

#include <algorithm>

#include "infra/base/assertion.h"
#include "infra/base/securestl.h"

#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"

#include "model/built_model/built_model_impl.h"
#include "model_builder/om/model_build_options_util.h"
#include "tensor/base/nd_tensor_buffer_impl.h"
#include "model_manager/core/open_request_stats.h"

#ifdef AI_SUPPORT_AIPP_API
#include "model/built_model_aipp.h"
#include "model/aipp/aipp_input_converter.h"
#include "framework/c/hiai_model_manager_aipp.h"
#include "tensor/aipp/aipp_para_impl.h"
#endif

namespace hiai {

ModelManagerImpl::~ModelManagerImpl()
{
    if (modelManager_ != nullptr) {
        UnLoad();
    }
}

Status ModelManagerImpl::PrepareModelManagerListener(const std::shared_ptr<IModelManagerListener>& listener)
{
    if (listener != nullptr) {
        std::lock_guard<std::mutex> lock(listenerMutex_);

        cListener_ = make_shared_nothrow<HIAI_ModelManagerListener>();
        HIAI_EXPECT_NOT_NULL(cListener_);

        cListener_->onRunDone = ModelManagerImpl::OnRunDone;
        cListener_->onServiceDied = ModelManagerImpl::OnServiceDied;
        listener_ = listener;
    }
    return SUCCESS;
}

static void DeleteModelInitOptions(HIAI_ModelInitOptions* p)
{
    HIAI_ModelInitOptions_Destroy(&p);
}

static std::unique_ptr<HIAI_ModelInitOptions, decltype(DeleteModelInitOptions)*> ConvertToCInitOptions(
    const ModelInitOptions& options)
{
    HIAI_ModelInitOptions* tmpOptions = HIAI_ModelInitOptions_Create();
    if (tmpOptions == nullptr) {
        return std::unique_ptr<HIAI_ModelInitOptions, decltype(DeleteModelInitOptions)*>(
            nullptr, DeleteModelInitOptions);
    }
    HIAI_ModelInitOptions_SetPerfMode(tmpOptions, static_cast<HIAI_PerfMode>(options.perfMode));

    if (ModelBuildOptionsUtil::IsHasBuildOptions(options.buildOptions)) {
        auto buildOptions = ModelBuildOptionsUtil::ConvertToCBuildOptions(options.buildOptions);
        (void)HIAI_ModelInitOptions_SetBuildOptions(tmpOptions, buildOptions);
    };

    return std::unique_ptr<HIAI_ModelInitOptions, decltype(DeleteModelInitOptions)*>(
        tmpOptions, DeleteModelInitOptions);
}

Status ModelManagerImpl::PrepareModelManager(
    const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel)
{
    std::shared_ptr<BuiltModelImpl> builtModelImpl =
        std::dynamic_pointer_cast<BuiltModelImpl>(std::const_pointer_cast<IBuiltModel>(builtModel));
    HIAI_EXPECT_NOT_NULL(builtModelImpl);

    auto cBuiltModel = builtModelImpl->GetBuiltModelImpl();
    HIAI_EXPECT_NOT_NULL(cBuiltModel);

    auto cModelManager = HIAI_ModelManager_Create();
    if (cModelManager == nullptr) {
        return FAILURE;
    }

    modelManager_ =
        std::shared_ptr<HIAI_ModelManager>(cModelManager, [](HIAI_ModelManager* p) { HIAI_ModelManager_Destroy(&p); });

    auto cOptions = ConvertToCInitOptions(options);
    HIAI_EXPECT_NOT_NULL(cOptions);

    return HIAI_ModelManager_Init(modelManager_.get(), cOptions.get(), cBuiltModel.get(), cListener_.get());
}

Status ModelManagerImpl::Init(const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel,
    const std::shared_ptr<IModelManagerListener>& listener)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::lock_guard<std::mutex> lock(modelManagerMutex_);
    HIAI_EXPECT_TRUE(modelManager_ == nullptr);

    HIAI_EXPECT_EXEC(PrepareModelManagerListener(listener));

    customModelData_ = builtModel->GetCustomData();

    Status result = PrepareModelManager(options, builtModel);

#ifdef HIAI_DDK
    (void)OpenRequestStats::CloudDdkVersionStats(AI_DDK_VERSION, "InterfaceV2Init", result);
#endif

    return result;
}

void ModelManagerImpl::OnRunDone(
    const Context& context, Status errCode, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs)
{
    std::lock_guard<std::mutex> lock(listenerMutex_);

    if (listener_ != nullptr) {
        listener_->OnRunDone(context, errCode, outputs);
    }
}

void ModelManagerImpl::OnServiceDied()
{
    std::lock_guard<std::mutex> lock(listenerMutex_);

    if (listener_ != nullptr) {
        listener_->OnServiceDied();
    }
}

void ModelManagerImpl::OnRunDone(void* userData, HIAI_Status errCode, HIAI_NDTensorBuffer* output[], int32_t outputNum)
{
    /* third param: output */
    /* fourth param: outputNum */
    (void)output;
    (void)outputNum;
    RunAsyncContext* runAsyncContext = (RunAsyncContext*)userData;
    runAsyncContext->modelManager->OnRunDone(
        runAsyncContext->context, static_cast<Status>(errCode), runAsyncContext->outputs);
    delete runAsyncContext;
}

void ModelManagerImpl::OnServiceDied(void* userData)
{
    RunAsyncContext* context = (RunAsyncContext*)userData;
    context->modelManager->OnServiceDied();
    delete context;
}

Status ModelManagerImpl::SetPriority(ModelPriority priority)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::lock_guard<std::mutex> lock(modelManagerMutex_);
    return HIAI_ModelManager_SetPriority(modelManager_.get(), static_cast<HIAI_ModelPriority>(priority));
}

static std::unique_ptr<HIAI_NDTensorBuffer* []> Convert2CNDTensorBuffers(
    const std::vector<std::shared_ptr<INDTensorBuffer>>& buffers)
{
    std::unique_ptr<HIAI_NDTensorBuffer* []> cBuffers { new (std::nothrow) HIAI_NDTensorBuffer* [buffers.size()] };
    HIAI_EXPECT_NOT_NULL_R(cBuffers, nullptr);

    for (size_t i = 0; i < buffers.size(); i++) {
        cBuffers[i] = GetRawBufferFromNDTensorBuffer(buffers[i]);
        HIAI_EXPECT_NOT_NULL_R(cBuffers[i], nullptr);
    }
    return cBuffers;
}

Status ModelManagerImpl::Run(
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (!customModelData_.type.empty()) {
        std::vector<std::shared_ptr<INDTensorBuffer>> dataInputs;
        std::vector<std::shared_ptr<IAIPPPara>> paraInputs;

        if (AippInputConverter::ConvertInputs(inputs, customModelData_, dataInputs, paraInputs) != hiai::SUCCESS) {
            return INVALID_PARAM;
        }
        Context context;
        return RunAippModel(context, dataInputs, paraInputs, outputs, 1000);
    }

    std::unique_ptr<HIAI_NDTensorBuffer* []> cInputs = Convert2CNDTensorBuffers(inputs);
    HIAI_EXPECT_NOT_NULL_R(cInputs, INVALID_PARAM);

    std::unique_ptr<HIAI_NDTensorBuffer* []> cOutputs = Convert2CNDTensorBuffers(outputs);
    HIAI_EXPECT_NOT_NULL_R(cOutputs, INVALID_PARAM);

    std::lock_guard<std::mutex> lock(modelManagerMutex_);
    return HIAI_ModelManager_Run(modelManager_.get(), cInputs.get(), inputs.size(), cOutputs.get(), outputs.size());
}

Status ModelManagerImpl::RunAsync(const Context& context, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
    std::vector<std::shared_ptr<INDTensorBuffer>>& outputs, int32_t timeout)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::unique_ptr<HIAI_NDTensorBuffer* []> cInputs = Convert2CNDTensorBuffers(inputs);
    HIAI_EXPECT_NOT_NULL_R(cInputs, INVALID_PARAM);

    std::unique_ptr<HIAI_NDTensorBuffer* []> cOutputs = Convert2CNDTensorBuffers(outputs);
    HIAI_EXPECT_NOT_NULL_R(cOutputs, INVALID_PARAM);

    std::lock_guard<std::mutex> lock(modelManagerMutex_);

    HIAI_EXPECT_NOT_NULL_R(listener_, UNSUPPORTED);

    RunAsyncContext* runContext = new (std::nothrow) RunAsyncContext();
    HIAI_EXPECT_NOT_NULL_R(runContext, MEMORY_EXCEPTION);

    runContext->context = context;
    runContext->modelManager = this;
    runContext->outputs = outputs;

    Status ret = HIAI_ModelManager_RunAsync(
        modelManager_.get(), cInputs.get(), inputs.size(), cOutputs.get(), outputs.size(), timeout, runContext);
    if (ret != SUCCESS) {
        delete runContext;
    }

    return ret;
}

#ifdef AI_SUPPORT_AIPP_API

static std::unique_ptr<HIAI_TensorAippPara* []> Convert2CTensorAippParas(
    const std::vector<std::shared_ptr<IAIPPPara>>& aippParas)
{
    std::unique_ptr<HIAI_TensorAippPara* []> cAippParas { new (std::nothrow) HIAI_TensorAippPara* [aippParas.size()] };
    if (cAippParas == nullptr) {
        FMK_LOGE("new failed.");
        return nullptr;
    }

    for (size_t i = 0; i < aippParas.size(); i++) {
        cAippParas[i] = GetTensorAippParaFromAippPara(aippParas[i]);
    }
    return cAippParas;
}

Status ModelManagerImpl::RunAippModel(const Context& context,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
    const std::vector<std::shared_ptr<IAIPPPara>>& aippParas, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs,
    int32_t timeoutInMS)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::unique_ptr<HIAI_NDTensorBuffer* []> cInputs = Convert2CNDTensorBuffers(inputs);
    HIAI_EXPECT_NOT_NULL_R(cInputs, INVALID_PARAM);

    std::unique_ptr<HIAI_NDTensorBuffer* []> cOutputs = Convert2CNDTensorBuffers(outputs);
    HIAI_EXPECT_NOT_NULL_R(cOutputs, INVALID_PARAM);

    std::unique_ptr<HIAI_TensorAippPara* []> cAippParas = Convert2CTensorAippParas(aippParas);
    HIAI_EXPECT_NOT_NULL_R(cAippParas, INVALID_PARAM);

    std::lock_guard<std::mutex> lock(modelManagerMutex_);

    RunAsyncContext* runContext = new (std::nothrow) RunAsyncContext();
    HIAI_EXPECT_NOT_NULL_R(runContext, MEMORY_EXCEPTION);

    runContext->context = context;
    runContext->modelManager = this;
    runContext->outputs = outputs;

    Status ret = HIAI_ModelManager_runAippModelV2(modelManager_.get(), cInputs.get(), inputs.size(), cAippParas.get(),
        aippParas.size(), cOutputs.get(), outputs.size(), timeoutInMS, runContext);
    if (listener_ == nullptr || ret != HIAI_SUCCESS) {
        delete runContext;
    }
    return ret;
}
#endif

Status ModelManagerImpl::Cancel()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::lock_guard<std::mutex> lock(modelManagerMutex_);

    return HIAI_ModelManager_Cancel(modelManager_.get());
}

void ModelManagerImpl::UnLoad()
{
    std::lock_guard<std::mutex> lock(modelManagerMutex_);
    (void)HIAI_ModelManager_Deinit(modelManager_.get());
    modelManager_.reset();
}

void ModelManagerImpl::DeInit()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    UnLoad();
    {
        std::lock_guard<std::mutex> lock(listenerMutex_);
        cListener_ = nullptr;
        listener_ = nullptr;
    }
}

std::shared_ptr<IModelManager> CreateModelManager()
{
    return make_shared_nothrow<ModelManagerImpl>();
}
} // namespace hiai