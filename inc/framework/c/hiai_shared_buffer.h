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

#ifndef FRAMEWORK_C_HIAI_SHARED_BUFFER_H
#define FRAMEWORK_C_HIAI_SHARED_BUFFER_H
#include "hiai_c_api_export.h"
#include "hiai_base_types.h"
#include "hiai_native_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_SharedBuffer HIAI_SharedBuffer;

AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_Create(size_t size);
AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromBuffer(void* buffer, size_t size);
AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromFile(const char* fileName);

AICP_C_API_EXPORT size_t HIAI_SharedBuffer_GetSize(const HIAI_SharedBuffer* buffer);
AICP_C_API_EXPORT void* HIAI_SharedBuffer_GetData(const HIAI_SharedBuffer* buffer);

AICP_C_API_EXPORT void HIAI_ShardBuffer_Destroy(HIAI_SharedBuffer** buffer);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_SHARED_BUFFER_H
