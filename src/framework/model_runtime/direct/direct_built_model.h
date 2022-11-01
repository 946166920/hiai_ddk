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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_H

#include "framework/c/hiai_nd_tensor_desc.h"
#include "framework/c/hiai_built_model_types.h"
#include "framework/c/hiai_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace hiai { // lint !e450
HIAI_Status HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(
    const HIAI_BuiltModel* model, void* data, size_t size, size_t* realSize);
HIAI_Status HIAI_DIRECT_BuiltModel_Save(const HIAI_BuiltModel* model, void** data, size_t* size);
HIAI_Status HIAI_DIRECT_BuiltModel_SaveToFile(const HIAI_BuiltModel* model, const char* file);

HIAI_BuiltModel* HIAI_DIRECT_BuiltModel_Restore(const void* data, size_t size);

HIAI_BuiltModel* HIAI_DIRECT_BuiltModel_RestoreFromFile(const char* file);

HIAI_Status HIAI_DIRECT_BuiltModel_CheckCompatibility(
    const HIAI_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility);

const char* HIAI_DIRECT_BuiltModel_GetName(const HIAI_BuiltModel* model);

HIAI_Status HIAI_DIRECT_BuiltModel_SetName(const HIAI_BuiltModel* model, const char* name);

int32_t HIAI_DIRECT_BuiltModel_GetInputTensorNum(const HIAI_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetInputTensorDesc(const HIAI_BuiltModel* model, size_t index);

int32_t HIAI_DIRECT_BuiltModel_GetOutputTensorNum(const HIAI_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(const HIAI_BuiltModel* model, size_t index);

void HIAI_DIRECT_BuiltModel_Destroy(HIAI_BuiltModel** model);
} // namespace hiai

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_H
