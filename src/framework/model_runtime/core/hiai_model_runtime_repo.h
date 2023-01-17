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
#ifndef INCL_HIAI_MODEL_RUNTIME_REPO_H
#define INCL_HIAI_MODEL_RUNTIME_REPO_H

#include <stddef.h>
#include <stdbool.h>

#include "framework/c/hiai_c_api_export.h"
#include "framework/c/hiai_model_builder_types.h"
#include "hiai_model_runtime.h"
#include "hiai_built_model_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

// dlclose hcl and binary
AICP_C_API_EXPORT void HIAI_ModelRuntimeRepo_DeInit(void);

// static register for direct, hcl
void HIAI_ModelRuntimeRepo_Add(HIAI_ModelRuntimeType type, HIAI_ModelRuntime* runtime);

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryBuild(
    const HIAI_MR_ModelBuildOptions* options, const char* modelName, const void* modelData, size_t modelSize);

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestore(const void* modelData, size_t modelSize);

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestoreFromFile(const char* file);

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestoreFromFileWithShapeIndex(const char* file, uint8_t shapeIndex);

HIAI_ModelRuntime* HIAI_ModelRuntimeRepo_GetSutiableHclRuntime(void);

bool HIAI_ModelRuntimeRepo_IsOldHclModelRuntime(const HIAI_ModelRuntime* runtime);

#ifdef AI_SUPPORT_LEGACY_APP_COMPATIBLE
bool IsHclModelRuntimeCanBuild(void);
#endif

#ifdef __cplusplus
}
#endif

#endif