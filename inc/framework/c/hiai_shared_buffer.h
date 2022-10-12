/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: shared buffer
 */

#ifndef FRAMEWORK_C_HIAI_SHARED_BUFFER_H
#define FRAMEWORK_C_HIAI_SHARED_BUFFER_H
#include "hiai_c_api_export.h"
#include "hiai_base_types.h"
#include "hiai_native_handle.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_SharedBuffer HIAI_SharedBuffer;

AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_Create(size_t size);
AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromBuffer(void* buffer, size_t size);
AICP_C_API_EXPORT HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromFile(const char* fileName);

AICP_C_API_EXPORT size_t HIAI_SharedBuffer_GetSize(const HIAI_SharedBuffer* buffer);
AICP_C_API_EXPORT void* HIAI_SharedBuffer_GetData(const HIAI_SharedBuffer* buffer);

AICP_C_API_EXPORT void HIAI_ShardBuffer_Destroy(HIAI_SharedBuffer** buffer);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_SHARED_BUFFER_H
