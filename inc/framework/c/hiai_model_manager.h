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
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_built_model.h"
#include "hiai_nd_tensor_buffer.h"
#include "hiai_model_manager_types.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_MR_ModelManager* HIAI_MR_ModelManager_Create(void);
AICP_C_API_EXPORT void HIAI_MR_ModelManager_Destroy(HIAI_MR_ModelManager** manager);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_Init(HIAI_MR_ModelManager* manager,
    const HIAI_MR_ModelInitOptions* options, const HIAI_MR_BuiltModel* builtModel,
    const HIAI_MR_ModelManagerListener* listener);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_InitWithSharedMem(HIAI_MR_ModelManager* manager,
    const HIAI_MR_ModelInitOptions* options, const HIAI_MR_BuiltModel* builtModel,
    const HIAI_MR_ModelManagerListener* listener, const HIAI_ModelManagerSharedMemAllocator* allocator);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_SetPriority(
    HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_Run(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_RunAsync(HIAI_MR_ModelManager* manager,
    HIAI_MR_NDTensorBuffer* input[], int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum,
    int32_t timeoutInMS, void* userData);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_Cancel(HIAI_MR_ModelManager* manager);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_Deinit(HIAI_MR_ModelManager* manager);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_H
