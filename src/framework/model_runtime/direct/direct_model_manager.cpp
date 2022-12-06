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
#include "direct_model_manager.h"
#ifdef AI_SUPPORT_AIPP_API
#include "direct_model_manager_aipp.h"
#endif
#include "direct_model_manager_impl.h"
#include "framework/infra/log/log.h"

namespace hiai {
HIAI_ModelManager* HIAI_DIRECT_ModelManager_Create()
{
    auto manager = new (std::nothrow) DirectModelManagerImpl();
    if (manager == nullptr) {
        FMK_LOGE("manager is nullptr,create manager failed.");
        return nullptr;
    }
    return (HIAI_ModelManager*)manager;
}

void HIAI_DIRECT_ModelManager_Destroy(HIAI_ModelManager** manager)
{
    if (manager == nullptr || *manager == nullptr) {
        return;
    }
    auto directmanager = reinterpret_cast<DirectModelManagerImpl*>(*manager);
    if (directmanager != nullptr) {
        delete directmanager;
        directmanager = nullptr;
        *manager = nullptr;
    }
}

HIAI_Status HIAI_DIRECT_ModelManager_Init(HIAI_ModelManager* manager, const HIAI_ModelInitOptions* options,
    const HIAI_BuiltModel* builtModel, const HIAI_ModelManagerListener* listener)
{
    if (builtModel == nullptr || manager == nullptr) {
        FMK_LOGE("manager or builtModel is nullptr.");
        return HIAI_FAILURE;
    }

    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    if (impl == nullptr) {
        FMK_LOGE("manager is invalid.");
        return HIAI_FAILURE;
    }

    if (impl->isLoaded_) {
        FMK_LOGE("model has been loaded.");
        return HIAI_FAILURE;
    }

    auto directBuiltModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(builtModel));
    if (directBuiltModelImpl == nullptr) {
        FMK_LOGE("builtmodel is invalid.");
        return HIAI_FAILURE;
    }

    if (directBuiltModelImpl->SharedManagerInfos_ == nullptr && impl->SharedManagerInfos_ == nullptr) {
        if (directBuiltModelImpl->Init() != HIAI_SUCCESS) {
            FMK_LOGE("directBuiltModelImpl init failed.");
            return HIAI_FAILURE;
        }
        if (directBuiltModelImpl->SharedManagerInfos_ == nullptr) {
            return HIAI_FAILURE;
        }
        impl->SharedManagerInfos_ = directBuiltModelImpl->SharedManagerInfos_;
    }

    if (directBuiltModelImpl->SharedManagerInfos_ != nullptr && impl->SharedManagerInfos_ == nullptr) {
        impl->SharedManagerInfos_ = directBuiltModelImpl->SharedManagerInfos_;
    }

    if (directBuiltModelImpl->SharedManagerInfos_ == nullptr && impl->SharedManagerInfos_ != nullptr) {
        directBuiltModelImpl->SharedManagerInfos_ = impl->SharedManagerInfos_;
    }

    if (impl->Init(options, listener) != HIAI_SUCCESS) {
        FMK_LOGE("init failed.");
        return HIAI_FAILURE;
    }
    FMK_LOGI("LoadModelsByDirect success");
    return HIAI_SUCCESS;
}

static DirectModelManagerImpl* ConvertManager(HIAI_ModelManager* manager)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    if (impl == nullptr) {
        FMK_LOGE("manager is invalid.");
        return nullptr;
    }

    if (!impl->isLoaded_) {
        FMK_LOGE("no load model.");
        return nullptr;
    }
    return impl;
}

HIAI_Status HIAI_DIRECT_ModelManager_SetPriority(HIAI_ModelManager* manager, HIAI_ModelPriority priority)
{
    if (priority < HIAI_PRIORITY_HIGH || priority > HIAI_PRIORITY_LOW) {
        FMK_LOGE("priority is invalid.");
        return HIAI_FAILURE;
    }

    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    return impl->SetPriority(priority);
}

HIAI_Status HIAI_DIRECT_ModelManager_Run(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_NDTensorBuffer* output[], int32_t outputNum)
{
    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    if (inputNum <= 0 || outputNum <= 0 || input == nullptr || output == nullptr) {
        FMK_LOGE("input or output invalid.");
        return HIAI_FAILURE;
    }

    HIAI_NDTensorBuffers buffers;
    buffers.inputNum = inputNum;
    buffers.outputNum = outputNum;
    buffers.input = input;
    buffers.output = output;

    return impl->Run(buffers);
}

HIAI_Status HIAI_DIRECT_ModelManager_RunAsync(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    if (inputNum <= 0 || outputNum <= 0 || input == nullptr || output == nullptr || userData == nullptr) {
        FMK_LOGE("input or output or userData invalid.");
        return HIAI_FAILURE;
    }

    HIAI_NDTensorBuffers buffers;
    buffers.inputNum = inputNum;
    buffers.outputNum = outputNum;
    buffers.input = input;
    buffers.output = output;

    return impl->RunAsync(buffers, timeoutInMS, userData);
}

#ifdef AI_SUPPORT_AIPP_API
HIAI_Status HIAI_DIRECT_ModelManager_runAippModelV2(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    if (inputNum <= 0 || outputNum <= 0 || aippParaNum <= 0 || input == nullptr || output == nullptr ||
        aippPara == nullptr) {
        FMK_LOGE("input or output or aippPara invalid.");
        return HIAI_FAILURE;
    }

    HIAI_NDTensorBuffers buffers;
    buffers.inputNum = inputNum;
    buffers.outputNum = outputNum;
    buffers.input = input;
    buffers.output = output;

    return impl->RunAipp(buffers, aippPara, aippParaNum, timeoutInMS, userData);
}
#endif

HIAI_Status HIAI_DIRECT_ModelManager_Cancel(HIAI_ModelManager* manager)
{
    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    (void)impl->Cancel();
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_ModelManager_Deinit(HIAI_ModelManager* manager)
{
    auto impl = ConvertManager(manager);
    if (impl == nullptr) {
        return HIAI_FAILURE;
    }

    (void)impl->DeInit();
    return HIAI_SUCCESS;
}
} // namespace hiai