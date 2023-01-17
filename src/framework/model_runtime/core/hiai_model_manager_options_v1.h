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
#ifndef MODEL_RUNTIME_CORE_HIAI_MODEL_MANAGER_OPTIONS_V1_H
#define MODEL_RUNTIME_CORE_HIAI_MODEL_MANAGER_OPTIONS_V1_H
#include "framework/c/hiai_model_manager_types.h"
#include "hiai_model_build_options_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

HIAI_MR_ModelInitOptions* HIAI_ModelInitOptionsV1_Create(void);

void HIAI_ModelInitOptionsV1_SetPerfMode(HIAI_MR_ModelInitOptions* options, HIAI_PerfMode devPerf);
HIAI_PerfMode HIAI_ModelInitOptionsV1_GetPerfMode(const HIAI_MR_ModelInitOptions* options);

void HIAI_ModelInitOptionsV1_SetBuildOptions(
    HIAI_MR_ModelInitOptions* options, HIAI_MR_ModelBuildOptions* buildOptions);
HIAI_MR_ModelBuildOptions* HIAI_ModelInitOptionsV1_GetBuildOptions(const HIAI_MR_ModelInitOptions* options);

void HIAI_ModelInitOptionsV1_Destroy(HIAI_MR_ModelInitOptions** options);

#ifdef __cplusplus
}
#endif

#endif
