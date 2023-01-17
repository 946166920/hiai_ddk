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
#ifndef FRAMEWORK_C_HIAI_ERROR_TYPES_H
#define FRAMEWORK_C_HIAI_ERROR_TYPES_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t HIAI_Status;

static const HIAI_Status HIAI_SUCCESS = 0;
static const HIAI_Status HIAI_FAILURE = 1;
static const HIAI_Status HIAI_UNINITIALIZED = 2;
static const HIAI_Status HIAI_INVALID_PARAM = 3;
static const HIAI_Status HIAI_TIMEOUT = 4;
static const HIAI_Status HIAI_UNSUPPORTED = 5;
static const HIAI_Status HIAI_MEMORY_EXCEPTION = 6;
static const HIAI_Status HIAI_INVALID_API = 7;
static const HIAI_Status HIAI_INVALID_POINTER = 8;
static const HIAI_Status HIAI_CALC_EXCEPTION = 9;
static const HIAI_Status HIAI_FILE_NOT_EXIST = 10;
static const HIAI_Status HIAI_COMM_EXCEPTION = 11;
static const HIAI_Status HIAI_DATA_OVERFLOW = 12;

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_ERROR_TYPES_H
