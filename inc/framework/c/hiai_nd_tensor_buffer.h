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

#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_H
#include "hiai_c_api_export.h"
#include "hiai_base_types.h"
#include "hiai_nd_tensor_desc.h"
#include "hiai_native_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_NDTensorBuffer HIAI_NDTensorBuffer;

AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorDesc(const HIAI_NDTensorDesc* desc);
AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromSize(const HIAI_NDTensorDesc* desc, size_t size);
AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromFormat(
    const HIAI_NDTensorDesc* desc, size_t size, HIAI_ImageFormat format);
AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromBuffer(
    const HIAI_NDTensorDesc* desc, const void* data, size_t dataSize);
AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandle(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle);
AICP_C_API_EXPORT HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateNoCopy(
    const HIAI_NDTensorDesc* desc, const void* data, size_t dataSize);

AICP_C_API_EXPORT HIAI_NDTensorDesc* HIAI_NDTensorBuffer_GetNDTensorDesc(const HIAI_NDTensorBuffer* tensorBuffer);
AICP_C_API_EXPORT size_t HIAI_NDTensorBuffer_GetSize(const HIAI_NDTensorBuffer* tensorBuffer);
AICP_C_API_EXPORT void* HIAI_NDTensorBuffer_GetData(const HIAI_NDTensorBuffer* tensorBuffer);

AICP_C_API_EXPORT void* HIAI_NDTensorBuffer_GetHandle(const HIAI_NDTensorBuffer* tensorBuffer);
AICP_C_API_EXPORT int32_t HIAI_NDTensorBuffer_GetFd(const HIAI_NDTensorBuffer* tensorBuffer);
AICP_C_API_EXPORT int32_t HIAI_NDTensorBuffer_GetOriginFd(const HIAI_NDTensorBuffer* tensorBuffer);

AICP_C_API_EXPORT void HIAI_NDTensorBuffer_Destroy(HIAI_NDTensorBuffer** tensorBuffer);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_H
