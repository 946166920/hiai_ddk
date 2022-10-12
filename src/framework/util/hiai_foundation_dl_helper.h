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
#ifndef FRAMEWORK_C_HIAI_FOUNDATION_DL_HELPER_H
#define FRAMEWORK_C_HIAI_FOUNDATION_DL_HELPER_H

#include "framework/c/hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { HIAI_NOT_SUPPORT_NPU = 0, HIAI_SUPPORT_NPU } HIAI_NPU_SUPPORT_STATE;

AICP_C_API_EXPORT void HIAI_Foundation_Init(void);
AICP_C_API_EXPORT void HIAI_Foundation_Deinit(void);

AICP_C_API_EXPORT void* HIAI_Foundation_GetSymbol(const char* symbolName);

AICP_C_API_EXPORT HIAI_NPU_SUPPORT_STATE HIAI_Foundation_IsNpuSupport(void);

#ifdef __cplusplus
}
#endif

#endif
