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
#include "framework/c/hiai_model_manager.h"
#include "hiai_model_manager_impl.h"
#include "hiai_built_model_impl.h"
#include "framework/infra/log/log.h"
#include "securec.h"

HIAI_MR_ModelManager* HIAI_MR_ModelManager_Create()
{
    HIAI_ModelManager_Impl* impl = malloc(sizeof(HIAI_ModelManager_Impl));
    MALLOC_NULL_CHECK_RET_VALUE(impl, NULL);
    (void)memset_s(impl, sizeof(HIAI_ModelManager_Impl), 0, sizeof(HIAI_ModelManager_Impl));

    return (HIAI_MR_ModelManager*)impl;
}

typedef void (*HIAI_ModelManager_Destroy_Ptr)(HIAI_MR_ModelManager** manager);
void HIAI_MR_ModelManager_Destroy(HIAI_MR_ModelManager** manager)
{
    if (manager == NULL) {
        return;
    }

    HIAI_ModelManager_Impl* impl = (HIAI_ModelManager_Impl*)(*manager);
    if (impl == NULL) {
        return;
    }

    if (impl->runtimeModelManager == NULL || impl->runtime == NULL) {
        goto FREE;
    }

    HIAI_ModelManager_Destroy_Ptr destroy =
        (HIAI_ModelManager_Destroy_Ptr)impl->runtime->symbolList[HRANI_MODELMANAGER_DESTROY];
    if (destroy == NULL) {
        FMK_LOGE("sym %d not found.", HRANI_MODELMANAGER_DESTROY);
        goto FREE;
    }

    destroy(&impl->runtimeModelManager);

FREE:
    free(*manager);
    *manager = NULL;
}

typedef HIAI_MR_ModelManager* (*HIAI_ModelManager_Create_Ptr)(void);
static HIAI_Status HIAI_ModelManager_RunTimeCreate(HIAI_MR_ModelManager* manager,
    const HIAI_BuiltModel_Impl* builtModelImpl)
{
    const HIAI_ModelRuntime* runtime = builtModelImpl->runtime;
    HIAI_ModelManager_Create_Ptr create = (HIAI_ModelManager_Create_Ptr)runtime->symbolList[HRANI_MODELMANAGER_CREATE];
    if (create == NULL) {
        return HIAI_FAILURE;
    }
    HIAI_MR_ModelManager* runtimeModelManager = create();
    if (runtimeModelManager == NULL) {
        FMK_LOGE("create failed.");
        return HIAI_FAILURE;
    }
    HIAI_ModelManager_Impl* managerImpl = (HIAI_ModelManager_Impl*)manager;
    managerImpl->runtimeModelManager = runtimeModelManager;
    managerImpl->runtime = runtime;
    return HIAI_SUCCESS;
}

typedef HIAI_Status (*HIAI_ModelManager_Init_Ptr)(HIAI_MR_ModelManager* manager,
    const HIAI_MR_ModelInitOptions* options, const HIAI_MR_BuiltModel* builtModel,
    const HIAI_MR_ModelManagerListener* listener);

HIAI_Status HIAI_MR_ModelManager_Init(HIAI_MR_ModelManager* manager, const HIAI_MR_ModelInitOptions* options,
    const HIAI_MR_BuiltModel* builtModel, const HIAI_MR_ModelManagerListener* listener)
{
    if (manager == NULL || options == NULL) {
        return HIAI_FAILURE;
    }

    const HIAI_BuiltModel_Impl* builtModelImpl = HIAI_BuiltModel_ToBuiltModelImpl(builtModel);
    if (builtModelImpl == NULL) {
        return HIAI_FAILURE;
    }

    HIAI_Status ret = HIAI_ModelManager_RunTimeCreate(manager, builtModelImpl);
    if (ret != HIAI_SUCCESS) {
        FMK_LOGE("Model manager create runtime failed.");
        return HIAI_FAILURE;
    }

    HIAI_MR_ModelManager* runtimeModelManager = ((HIAI_ModelManager_Impl*)manager)->runtimeModelManager;
    const HIAI_ModelRuntime* runtime = builtModelImpl->runtime;
    HIAI_ModelManager_Init_Ptr initV2 = (HIAI_ModelManager_Init_Ptr)runtime->symbolList[HRANI_MODELMANAGER_INITV2];
    if (initV2 != NULL) {
        return initV2(runtimeModelManager, options, builtModelImpl->runtimeBuiltModel, listener);
    }

    HIAI_ModelManager_Init_Ptr init = (HIAI_ModelManager_Init_Ptr)runtime->symbolList[HRANI_MODELMANAGER_INIT];
    if (init != NULL) {
        return init(runtimeModelManager, options, builtModelImpl->runtimeBuiltModel, listener);
    }

    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_InitWithSharedMem_Ptr)(HIAI_MR_ModelManager* manager,
    const HIAI_MR_ModelInitOptions* options, const HIAI_MR_BuiltModel* builtModel,
    const HIAI_MR_ModelManagerListener* listener, const HIAI_ModelManagerSharedMemAllocator* allocator);

HIAI_Status HIAI_ModelManager_InitWithSharedMem(HIAI_MR_ModelManager* manager,
    const HIAI_MR_ModelInitOptions* options, const HIAI_MR_BuiltModel* builtModel,
    const HIAI_MR_ModelManagerListener* listener, const HIAI_ModelManagerSharedMemAllocator* allocator)
{
    if (manager == NULL || options == NULL) {
        return HIAI_FAILURE;
    }

    const HIAI_BuiltModel_Impl* builtModelImpl = HIAI_BuiltModel_ToBuiltModelImpl(builtModel);
    if (builtModelImpl == NULL) {
        return HIAI_FAILURE;
    }

    HIAI_Status ret = HIAI_ModelManager_RunTimeCreate(manager, builtModelImpl);
    if (ret != HIAI_SUCCESS) {
        FMK_LOGE("Model manager create runtime failed.");
        return HIAI_FAILURE;
    }

    HIAI_MR_ModelManager* runtimeModelManager = ((HIAI_ModelManager_Impl*)manager)->runtimeModelManager;
    const HIAI_ModelRuntime* runtime = builtModelImpl->runtime;
    HIAI_ModelManager_InitWithSharedMem_Ptr init =
        (HIAI_ModelManager_InitWithSharedMem_Ptr)runtime->symbolList[HRANI_MODELMANAGER_INIT_WITH_SHARED_MEM_ALLOCATOR];
    if (init != NULL) {
        return init(runtimeModelManager, options, builtModelImpl->runtimeBuiltModel, listener, allocator);
    }
    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_SetPriority_Ptr)(HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority);
HIAI_Status HIAI_MR_ModelManager_SetPriority(HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority)
{
    if (manager == NULL) {
        FMK_LOGE("manager is NULL");
        return HIAI_INVALID_PARAM;
    }

    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }
#ifdef AI_SUPPORT_PRIORITY_EXTEND
    if (priority < HIAI_PRIORITY_HIGH_PLATFORM1 || priority > HIAI_PRIORITY_LOW) {
#else
    if (priority < HIAI_PRIORITY_HIGH || priority > HIAI_PRIORITY_LOW) {
#endif
        FMK_LOGE("priority is invalid.");
        return HIAI_INVALID_PARAM;
    }

    HIAI_ModelManager_SetPriority_Ptr set =
        (HIAI_ModelManager_SetPriority_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_SET_PRIORITY];
    if (set != NULL) {
        return set(managerImpl->runtimeModelManager, priority);
    }
    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_Run_Ptr)(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum);
HIAI_Status HIAI_MR_ModelManager_Run(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_MR_NDTensorBuffer* output[], int32_t outputNum)
{
    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_Status ret = HIAI_ModelManager_PreRun(managerImpl->runtime, input, inputNum, output, outputNum);
    if (ret != HIAI_SUCCESS) {
        return HIAI_FAILURE;
    }

    HIAI_ModelManager_Run_Ptr runV3 =
        (HIAI_ModelManager_Run_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN_V3];
    if (runV3 != NULL) {
        return runV3(managerImpl->runtimeModelManager, input, inputNum, output, outputNum);
    }

    HIAI_ModelManager_Run_Ptr runV1 =
        (HIAI_ModelManager_Run_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN];
    if (runV1 != NULL) {
        return runV1(managerImpl->runtimeModelManager, input, inputNum, output, outputNum);
    }
    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_RunAsync_Ptr)(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData);
HIAI_Status HIAI_MR_ModelManager_RunAsync(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_Status ret = HIAI_ModelManager_PreRun(managerImpl->runtime, input, inputNum, output, outputNum);
    if (ret != HIAI_SUCCESS) {
        return HIAI_FAILURE;
    }

    HIAI_ModelManager_RunAsync_Ptr runAsyncV3 =
        (HIAI_ModelManager_RunAsync_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN_ASYNC_V3];
    if (runAsyncV3 != NULL) {
        return runAsyncV3(managerImpl->runtimeModelManager, input, inputNum, output, outputNum, timeoutInMS, userData);
    }

    HIAI_ModelManager_RunAsync_Ptr runAsyncV1 =
        (HIAI_ModelManager_RunAsync_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN_ASYNC];
    if (runAsyncV1 != NULL) {
        return runAsyncV1(managerImpl->runtimeModelManager, input, inputNum, output, outputNum, timeoutInMS, userData);
    }
    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_Cancel_Ptr)(HIAI_MR_ModelManager* manager);
HIAI_Status HIAI_MR_ModelManager_Cancel(HIAI_MR_ModelManager* manager)
{
    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_ModelManager_Cancel_Ptr cancel =
        (HIAI_ModelManager_Cancel_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_CANCEL];
    if (cancel != NULL) {
        return cancel(managerImpl->runtimeModelManager);
    }
    return HIAI_FAILURE;
}

typedef HIAI_Status (*HIAI_ModelManager_Deinit_Ptr)(HIAI_MR_ModelManager* manager);
HIAI_Status HIAI_MR_ModelManager_Deinit(HIAI_MR_ModelManager* manager)
{
    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_ModelManager_Deinit_Ptr deinit =
        (HIAI_ModelManager_Deinit_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_DEINIT];
    if (deinit != NULL) {
        return deinit(managerImpl->runtimeModelManager);
    }
    return HIAI_FAILURE;
}