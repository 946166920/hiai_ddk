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
#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LEGACY_UTIL_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LEGACY_UTIL_H

#include "framework/c/hiai_nd_tensor_buffer.h"
#include "framework/c/compatible/hiai_model_manager_type.h"

#ifdef __cplusplus
extern "C" {
#endif

static size_t NCHW_DIM_NUM = 4;

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorDescLegacy(const HIAI_NDTensorDesc* desc);
HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromSizeLegacy(const HIAI_NDTensorDesc* desc, size_t size);
HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromFormatLegacy(
    const HIAI_NDTensorDesc* desc, size_t size, HIAI_ImageFormat format);
HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandleLegacy(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle);

int32_t HIAI_NDTensorBuffer_GetFdLegacy(void* buffer, bool isLegacy);
int32_t HIAI_NDTensorBuffer_GetOriginFdLegacy(void* buffer, bool isLegacy);

void HIAI_NDTensorBuffer_DestroyLegacy(void* buffer, bool isLegacy);

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(
    const HIAI_NDTensorDesc* desc, HIAI_NDTensorBuffer* buffer);

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromTensorBuffer(
    const HIAI_NDTensorDesc* desc, HIAI_TensorBuffer* buffer);

void HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(HIAI_NDTensorBuffer** buffer);
void HIAI_NDTensorBuffer_ReleaseTensorBuffer(HIAI_TensorBuffer** buffer);

void* HIAI_NDTensorBuffer_GetDataFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer);
size_t HIAI_NDTensorBuffer_GetSizeFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
