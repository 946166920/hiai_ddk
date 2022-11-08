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
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_TYPES_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_TYPES_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_nd_tensor_buffer.h"
#include "hiai_model_builder_types.h"
#include "hiai_execute_option_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_ModelInitOptions HIAI_ModelInitOptions;

AICP_C_API_EXPORT HIAI_ModelInitOptions* HIAI_ModelInitOptions_Create();
AICP_C_API_EXPORT void HIAI_ModelInitOptions_SetPerfMode(HIAI_ModelInitOptions* options, HIAI_PerfMode devPerf);
AICP_C_API_EXPORT HIAI_PerfMode HIAI_ModelInitOptions_GetPerfMode(const HIAI_ModelInitOptions* options);
AICP_C_API_EXPORT void HIAI_ModelInitOptions_SetBuildOptions(
    HIAI_ModelInitOptions* options, HIAI_ModelBuildOptions* buildOptions);
AICP_C_API_EXPORT HIAI_ModelBuildOptions* HIAI_ModelInitOptions_GetBuildOptions(HIAI_ModelInitOptions* options);
AICP_C_API_EXPORT void HIAI_ModelInitOptions_Destroy(HIAI_ModelInitOptions** options);

typedef struct HIAI_ModelManager HIAI_ModelManager;

typedef struct HIAI_ModelManagerListener {
    void (*onRunDone)(void*, HIAI_Status, HIAI_NDTensorBuffer* [], int32_t);
    void (*onServiceDied)(void*);
    void* userData;
} HIAI_ModelManagerListener;

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_H
