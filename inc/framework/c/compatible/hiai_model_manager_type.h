/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: hiai_model_manager_type.h
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
