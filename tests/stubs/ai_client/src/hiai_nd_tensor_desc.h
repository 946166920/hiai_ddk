/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: tensor desc
 */

#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_DESC_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_DESC_H
#include "hiai_c_api_export.h"
#include "hiai_base_types.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_NDTensorDesc HIAI_NDTensorDesc;

AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_NDTensorDesc_Create(
    const int32_t* dims, size_t dimNum, HIAI_DataType dataType, HIAI_Format format);
AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_NDTensorDesc_Clone(const HIAI_NDTensorDesc* tensorDesc);
AICP_C_API_EXPORT void HIAI_NDTensorDesc_Destroy(HIAI_NDTensorDesc** tensorDesc);

AICP_C_API_EXPORT size_t HIAI_NDTensorDesc_GetDimNum(const HIAI_NDTensorDesc* tensorDesc);
AICP_C_API_EXPORT int32_t HIAI_NDTensorDesc_GetDim(const HIAI_NDTensorDesc* tensorDesc, size_t index);

AICP_C_API_EXPORT HIAI_DataType HIAI_NDTensorDesc_GetDataType(const HIAI_NDTensorDesc* tensorDesc);

AICP_C_API_EXPORT HIAI_Format HIAI_NDTensorDesc_GetFormat(const HIAI_NDTensorDesc* tensorDesc);

AICP_C_API_EXPORT size_t HIAI_NDTensorDesc_GetTotalDimNum(const HIAI_NDTensorDesc* tensorDesc);

AICP_C_API_EXPORT size_t HIAI_NDTensorDesc_GetByteSize(const HIAI_NDTensorDesc* tensorDesc);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_ND_TENSOR_DESC_H
