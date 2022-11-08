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
#ifndef HIAI_MODEL_MANAGER_TYPE_H
#define HIAI_MODEL_MANAGER_TYPE_H

#include "framework/c/hiai_execute_option_types.h"
#include "framework/c/compatible/hiai_tensor_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef HIAI_PerfMode HIAI_DevPerf;
#define HIAI_TENSOR_DESCRIPTION_INIT \
    { \
        0, 0, 0, 0, HIAI_DATATYPE_FLOAT32 \
    }

typedef struct HIAI_TensorBuffer {
    HIAI_TensorDescription desc;
    int size;
    void* data;
    void* impl; /* DON'T MODIFY */
    const char* name;
} HIAI_TensorBuffer;

typedef enum { HIAI_MODELTYPE_ONLINE = 0, HIAI_MODELTYPE_OFFLINE } HIAI_ModelType;

typedef enum {
    HIAI_FRAMEWORK_NONE = 0,
    HIAI_FRAMEWORK_TENSORFLOW = 1,
    HIAI_FRAMEWORK_CAFFE = 3,
    HIAI_FRAMEWORK_TENSORFLOW_8BIT,
    HIAI_FRAMEWORK_CAFFE_8BIT,
    HIAI_FRAMEWORK_OFFLINE,
    HIAI_FRAMEWORK_IR,
    HIAI_FRAMEWORK_INVALID,
} HIAI_Framework;

#ifdef __cplusplus
}
#endif

#endif
