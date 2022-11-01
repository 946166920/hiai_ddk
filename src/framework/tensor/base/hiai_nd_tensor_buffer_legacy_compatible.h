/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: nd tensor buffer legacy comaptible
 */
#ifndef FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LEGACY_COMPATIBLE_H
#define FRAMEWORK_C_HIAI_ND_TENSOR_BUFFER_LEGACY_COMPATIBLE_H

#include <stdbool.h>

#include "framework/c/hiai_nd_tensor_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

bool HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(HIAI_NDTensorBuffer* buffers[], int32_t num);

#ifdef __cplusplus
}
#endif

#endif
