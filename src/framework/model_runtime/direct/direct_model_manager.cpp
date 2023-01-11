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

// inc
#include "framework/infra/log/log.h"
#include "infra/base/assertion.h"

// src/framework
#include "direct_model_manager_impl.h"

#ifdef AI_SUPPORT_AIPP_API
#include "direct_model_manager_aipp.h"
#endif

namespace hiai {
HIAI_MR_ModelManager* HIAI_DIRECT_ModelManager_Create()
{
    auto manager = new (std::nothrow) DirectModelManagerImpl();
    HIAI_EXPECT_NOT_NULL_R(manager, nullptr);

    return (HIAI_MR_ModelManager*)manager;
}

void HIAI_DIRECT_ModelManager_Destroy(HIAI_MR_ModelManager** manager)
{
    HIAI_EXPECT_NOT_NULL_VOID(manager);
    HIAI_EXPECT_NOT_NULL_VOID(*manager);

    auto impl = reinterpret_cast<DirectModelManagerImpl*>(*manager);
    if (impl != nullptr) {
        delete impl;
        *manager = nullptr;
    }
}

HIAI_Status HIAI_DIRECT_ModelManager_Init(HIAI_MR_ModelManager* manager, const HIAI_MR_ModelInitOptions* options,
    const HIAI_MR_BuiltModel* builtModel, const HIAI_MR_ModelManagerListener* listener)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    auto builtModelImpl = reinterpret_cast<const DirectBuiltModelImpl*>(builtModel);
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);
    HIAI_EXPECT_NOT_NULL_R(options, HIAI_FAILURE);

    return impl->Init(*builtModelImpl, *options, listener);
}

HIAI_Status HIAI_DIRECT_ModelManager_SetPriority(HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    return impl->SetPriority(priority);
}

HIAI_Status HIAI_DIRECT_ModelManager_Run(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    if (inputNum <= 0 || outputNum <= 0 || input == nullptr || output == nullptr) {
        FMK_LOGE("input or output invalid.");
        return HIAI_FAILURE;
    }
    HIAI_NDTensorBuffers buffers(inputNum, outputNum, input, output);

    return impl->Run(buffers);
}

HIAI_Status HIAI_DIRECT_ModelManager_RunAsync(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    if (inputNum <= 0 || outputNum <= 0 || input == nullptr || output == nullptr || userData == nullptr) {
        FMK_LOGE("input or output or userData invalid.");
        return HIAI_FAILURE;
    }
    HIAI_NDTensorBuffers buffers(inputNum, outputNum, input, output);

    return impl->RunAsync(buffers, timeoutInMS, userData);
}

#ifdef AI_SUPPORT_AIPP_API
HIAI_Status HIAI_DIRECT_ModelManager_runAippModelV2(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_MR_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    if (inputNum <= 0 || outputNum <= 0 || aippParaNum <= 0 || input == nullptr || output == nullptr ||
        aippPara == nullptr) {
        FMK_LOGE("input or output or aippPara invalid.");
        return HIAI_FAILURE;
    }
    HIAI_NDTensorBuffers buffers(inputNum, outputNum, input, output);

    return impl->RunAipp(buffers, aippPara, aippParaNum, timeoutInMS, userData);
}
#endif

HIAI_Status HIAI_DIRECT_ModelManager_Cancel(HIAI_MR_ModelManager* manager)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    (void)impl->Cancel();
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_ModelManager_Deinit(HIAI_MR_ModelManager* manager)
{
    auto impl = reinterpret_cast<DirectModelManagerImpl*>(manager);
    HIAI_EXPECT_NOT_NULL_R(impl, HIAI_FAILURE);

    (void)impl->DeInit();
    return HIAI_SUCCESS;
}
} // namespace hiai