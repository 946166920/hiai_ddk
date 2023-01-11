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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_MANAGER_HIAI_MODEL_COMPATIBLE_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_MANAGER_HIAI_MODEL_COMPATIBLE_H
#include <stdint.h>

#include "framework/c/hiai_c_api_export.h"
#include "framework/c/hiai_error_types.h"
#include "framework/c/compatible/hiai_mem_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_MakeDirectCompatibleModel(const HIAI_MemBuffer* input, HIAI_MemBuffer** output);

#ifdef __cplusplus
}
#endif

#endif
