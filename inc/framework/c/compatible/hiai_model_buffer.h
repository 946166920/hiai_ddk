/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: model buffer(deprecated)
 */
#ifndef FRAMEWORK_C_LEGACY_HIAI_MODEL_BUFFER_H
#define FRAMEWORK_C_LEGACY_HIAI_MODEL_BUFFER_H

#include "framework/c/hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HIAI_ModelBuffer {
    const char* name;
    const char* path;
    HIAI_DevPerf perf;
    int size;
    void* impl; /* DON'T MODIFY */
};

AICP_C_API_EXPORT HIAI_ModelBuffer* HIAI_ModelBuffer_create_from_file(
    const char* name, const char* path, HIAI_DevPerf perf);
AICP_C_API_EXPORT HIAI_ModelBuffer* HIAI_ModelBuffer_create_from_buffer(
    const char* name, void* modelBuf, int size, HIAI_DevPerf perf);

AICP_C_API_EXPORT const char* HIAI_ModelBuffer_getName(HIAI_ModelBuffer* b);
AICP_C_API_EXPORT const char* HIAI_ModelBuffer_getPath(HIAI_ModelBuffer* b);
AICP_C_API_EXPORT HIAI_DevPerf HIAI_ModelBuffer_getPerf(HIAI_ModelBuffer* b);

AICP_C_API_EXPORT int HIAI_ModelBuffer_getSize(HIAI_ModelBuffer* b);

AICP_C_API_EXPORT void HIAI_ModelBuffer_destroy(HIAI_ModelBuffer* b);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FRAMEWORK_C_LEGACY_HIAI_MODEL_BUFFER_H
