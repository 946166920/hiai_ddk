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
#include <map>
#include <thread>
#include "framework/c/hiai_model_manager.h"
#include "framework/c/hiai_model_manager_aipp.h"
#include "framework/infra/log/log.h"
#include "control_c.h"

using namespace hiai;

using HIAI_NativeHandle = struct HIAI_NativeHandle {
    int fd;
    int size;
    int offset;
};

HIAI_MR_ModelManager* HIAI_MR_ModelManager_Create()
{
    HIAI_MR_ModelManager* manager = (HIAI_MR_ModelManager*)malloc(1);
    return manager;
}

std::map<HIAI_MR_ModelManager*, const HIAI_MR_ModelManagerListener*> g_StubListener;

void HIAI_MR_ModelManager_Destroy(HIAI_MR_ModelManager** manager)
{
    if (manager == NULL || *manager == nullptr) {
        return;
    }

    free(*manager);
    *manager = NULL;
}

HIAI_Status HIAI_MR_ModelManager_Init(HIAI_MR_ModelManager* manager, const HIAI_MR_ModelInitOptions* options,
    const HIAI_MR_BuiltModel* builtModel, const HIAI_MR_ModelManagerListener* listener)
{
    if (!hiai::ControlC::GetInstance().CheckInitOptions(options)) {
        FMK_LOGE("build options is error.");
        return HIAI_FAILURE;
    }
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        return HIAI_INVALID_PARAM;
    }
    g_StubListener.insert(std::make_pair(manager, listener));

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_InitWithSharedMem(HIAI_MR_ModelManager* manager, const HIAI_MR_ModelInitOptions* options,
    const HIAI_MR_BuiltModel* builtModel, const HIAI_MR_ModelManagerListener* listener,
    const HIAI_ModelManagerSharedMemAllocator* allocator)
{
    if (allocator == nullptr || allocator->onAllocate == nullptr || allocator->onFree == nullptr ||
        allocator->userData == nullptr) {
        FMK_LOGE("Invalid shared mem allocator.");
        return HIAI_INVALID_PARAM;
    }

    const uint32_t maxNum = 10;
    const uint32_t allocSize = 1;
    HIAI_NativeHandle cHandle[maxNum] = {0};
    HIAI_NativeHandle* cHandlesPtr[maxNum] = {&cHandle[0], &cHandle[1]};
    size_t cHandleSize = 0;

    allocator->onAllocate(allocator->userData, allocSize, cHandlesPtr, &cHandleSize);
    if (cHandleSize == 0 || cHandlesPtr[0]->fd == 0 || cHandlesPtr[0]->size == 0) {
        return HIAI_FAILURE;
    }

    allocator->onFree(allocator->userData, cHandlesPtr, cHandleSize);

    return HIAI_MR_ModelManager_Init(manager, options, builtModel, listener);
}

HIAI_Status HIAI_MR_ModelManager_SetPriority(HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_ModelManager_Run(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_MR_NDTensorBuffer* output[], int32_t outputNum)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_ModelManager_RunAsync(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_ModelManager_Cancel(HIAI_MR_ModelManager* manager)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_ModelManager_Deinit(HIAI_MR_ModelManager* manager)
{
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        g_StubListener.erase(iter);
    }

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_ModelManager_runAippModelV2(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_MR_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        if (iter->second != nullptr) {
            std::thread runDoneThd(iter->second->onRunDone, userData, HIAI_SUCCESS, output, outputNum);
            runDoneThd.detach();
        }
    }

    return ControlC::GetInstance().GetExpectStatus(CKey::C_RUN_AIPP_MODEL_V2_FAILED);
}