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
#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LOCAL_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LOCAL_H

#include "hiai_nd_tensor_buffer_def.h"
#include "framework/c/hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

HIAI_MR_NDTensorBuffer* HIAI_NDTensorBuffer_CreateLocalBufferFromSize(const HIAI_NDTensorDesc* desc, size_t size);

HIAI_MR_NDTensorBuffer* HIAI_NDTensorBuffer_CreateLocalBufferFromNDTensorDesc(const HIAI_NDTensorDesc* desc);

void HIAI_NDTensorBuffer_ReleaseLocal(HIAI_MR_NDTensorBuffer** buffer);

#ifdef __cplusplus
}
#endif

#endif