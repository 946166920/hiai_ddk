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
#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_DEF_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_DEF_H
#include <stddef.h>
#include <stdbool.h>

#include "framework/c/hiai_nd_tensor_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_NDTensorBufferV2 {
    HIAI_NDTensorDesc* desc;
    size_t size;
    void* data;
    void* handle;
    bool owner;
    bool isLegacy; // when the value is true, it means handle is HIAI_TensorBuffer
    int8_t reserve[14];
} HIAI_NDTensorBufferV2;

#ifdef __cplusplus
}
#endif

#endif