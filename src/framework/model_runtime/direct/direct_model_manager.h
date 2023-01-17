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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_MANAGER_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_MANAGER_H

#include "framework/c/hiai_model_manager.h"
#include "framework/c/hiai_built_model_types.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace hiai {
HIAI_MR_ModelManager* HIAI_DIRECT_ModelManager_Create();
void HIAI_DIRECT_ModelManager_Destroy(HIAI_MR_ModelManager** manager);

HIAI_Status HIAI_DIRECT_ModelManager_Init(HIAI_MR_ModelManager* manager, const HIAI_MR_ModelInitOptions* options,
    const HIAI_MR_BuiltModel* builtModel, const HIAI_MR_ModelManagerListener* listener);

HIAI_Status HIAI_DIRECT_ModelManager_SetPriority(HIAI_MR_ModelManager* manager, HIAI_ModelPriority priority);

HIAI_Status HIAI_DIRECT_ModelManager_Run(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum);

HIAI_Status HIAI_DIRECT_ModelManager_RunAsync(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData);

HIAI_Status HIAI_DIRECT_ModelManager_Cancel(HIAI_MR_ModelManager* manager);

HIAI_Status HIAI_DIRECT_ModelManager_Deinit(HIAI_MR_ModelManager* manager);
} // namespace hiai
#ifdef __cplusplus
}
#endif

#endif
