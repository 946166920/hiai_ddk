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
#ifndef FRAMEWORK_C_HIAI_BUILT_MODEL_H
#define FRAMEWORK_C_HIAI_BUILT_MODEL_H
#include <stdint.h>
#include <stdlib.h>

#include "hiai_c_api_export.h"
#include "hiai_nd_tensor_desc.h"
#include "hiai_error_types.h"
#include "hiai_built_model_types.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_SaveToExternalBuffer(
    const HIAI_MR_BuiltModel* model, void* data, size_t size, size_t* realSize);
AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_Save(const HIAI_MR_BuiltModel* model, void** data, size_t* size);
AICP_C_API_EXPORT HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_Restore(const void* data, size_t size);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_SaveToFile(const HIAI_MR_BuiltModel* model, const char* file);
AICP_C_API_EXPORT HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_RestoreFromFile(const char* file);

AICP_C_API_EXPORT HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_RestoreFromFileWithShapeIndex(
    const char* file, uint8_t shapeIndex);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_CheckCompatibility(
    const HIAI_MR_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility);

AICP_C_API_EXPORT const char* HIAI_MR_BuiltModel_GetName(const HIAI_MR_BuiltModel* model);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_SetName(const HIAI_MR_BuiltModel* model, const char* name);

AICP_C_API_EXPORT int32_t HIAI_MR_BuiltModel_GetInputTensorNum(const HIAI_MR_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_MR_BuiltModel_GetInputTensorDesc(
    const HIAI_MR_BuiltModel* model, size_t index);

AICP_C_API_EXPORT int32_t HIAI_MR_BuiltModel_GetOutputTensorNum(const HIAI_MR_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_MR_BuiltModel_GetOutputTensorDesc(
    const HIAI_MR_BuiltModel* model, size_t index);

AICP_C_API_EXPORT void HIAI_MR_BuiltModel_Destroy(HIAI_MR_BuiltModel** model);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_BUILT_MODEL_H
