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
#ifndef FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
#define FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
#include <stdlib.h>

#include "hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_NativeHandle HIAI_NativeHandle;

AICP_C_API_EXPORT HIAI_NativeHandle* HIAI_NativeHandle_Create(int fd, int size, int offset);
AICP_C_API_EXPORT void HIAI_NativeHandle_Destroy(HIAI_NativeHandle** handle);

AICP_C_API_EXPORT int HIAI_NativeHandle_GetFd(const HIAI_NativeHandle* handle);
AICP_C_API_EXPORT int HIAI_NativeHandle_GetSize(const HIAI_NativeHandle* handle);
AICP_C_API_EXPORT int HIAI_NativeHandle_GetOffset(const HIAI_NativeHandle* handle);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
