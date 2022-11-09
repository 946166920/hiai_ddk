/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: base type
 */
#ifndef FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
#define FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
#include <cstdint>
#include <cstdlib>

#include "hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_NativeHandle HIAI_NativeHandle;

AICP_C_API_EXPORT HIAI_NativeHandle* HIAI_NativeHandle_Create(int fd, int size, int offset);
AICP_C_API_EXPORT void HIAI_NativeHandle_Destroy(HIAI_NativeHandle** handle);

AICP_C_API_EXPORT int HIAI_NativeHandle_GetFd(const HIAI_NativeHandle* handle);
AICP_C_API_EXPORT int HIAI_NativeHandle_GetSize(const HIAI_NativeHandle* handle);
AICP_C_API_EXPORT int HIAI_NativeHandle_GetOffset(const HIAI_NativeHandle* handle);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_NATIVE_HANDLE_H
