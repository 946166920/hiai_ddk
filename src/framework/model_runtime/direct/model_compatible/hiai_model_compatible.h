/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hiai model compatible
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
