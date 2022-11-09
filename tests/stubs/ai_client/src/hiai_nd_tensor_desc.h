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
