/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: hiai tensor desc
 */
#ifndef FRAMEWORK_C_COMPATIBLE_HIAI_TENSOR_DESC_H
#define FRAMEWORK_C_COMPATIBLE_HIAI_TENSOR_DESC_H

#include "framework/c/hiai_base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_TensorDescription {
    int number;
    int channel;
    int height;
    int width;
    HIAI_DataType dataType; /* optional */
} HIAI_TensorDescription;

#ifdef __cplusplus
}
#endif

#endif
