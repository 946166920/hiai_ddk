/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: built model
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

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_SaveToExternalBuffer(
    const HIAI_BuiltModel* model, void* data, size_t size, size_t* realSize);
AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_Save(const HIAI_BuiltModel* model, void** data, size_t* size);
AICP_C_API_EXPORT HIAI_BuiltModel* HIAI_BuiltModel_Restore(const void* data, size_t size);

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_SaveToFile(const HIAI_BuiltModel* model, const char* file);
AICP_C_API_EXPORT HIAI_BuiltModel* HIAI_BuiltModel_RestoreFromFile(const char* file);

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_CheckCompatibility(
    const HIAI_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility);

AICP_C_API_EXPORT const char* HIAI_BuiltModel_GetName(const HIAI_BuiltModel* model);

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_SetName(const HIAI_BuiltModel* model, const char* name);

AICP_C_API_EXPORT int32_t HIAI_BuiltModel_GetInputTensorNum(const HIAI_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_BuiltModel_GetInputTensorDesc(const HIAI_BuiltModel* model, size_t index);

AICP_C_API_EXPORT int32_t HIAI_BuiltModel_GetOutputTensorNum(const HIAI_BuiltModel* model);
// the return value need to destroy by HIAI_NDTensorDesc_Destroy
AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_BuiltModel_GetOutputTensorDesc(const HIAI_BuiltModel* model, size_t index);

AICP_C_API_EXPORT void HIAI_BuiltModel_Destroy(HIAI_BuiltModel** model);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_BUILT_MODEL_H
