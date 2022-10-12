/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager types
 */
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_TYPES_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_TYPES_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_nd_tensor_buffer.h"
#include "hiai_model_builder_types.h"
#include "hiai_execute_option_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_ModelInitOptions HIAI_ModelInitOptions;

AICP_C_API_EXPORT HIAI_ModelInitOptions* HIAI_ModelInitOptions_Create();
AICP_C_API_EXPORT void HIAI_ModelInitOptions_SetPerfMode(HIAI_ModelInitOptions* options, HIAI_PerfMode devPerf);
AICP_C_API_EXPORT HIAI_PerfMode HIAI_ModelInitOptions_GetPerfMode(const HIAI_ModelInitOptions* options);
AICP_C_API_EXPORT void HIAI_ModelInitOptions_SetBuildOptions(
    HIAI_ModelInitOptions* options, HIAI_ModelBuildOptions* buildOptions);
AICP_C_API_EXPORT HIAI_ModelBuildOptions* HIAI_ModelInitOptions_GetBuildOptions(HIAI_ModelInitOptions* options);
AICP_C_API_EXPORT void HIAI_ModelInitOptions_Destroy(HIAI_ModelInitOptions** options);

typedef struct HIAI_ModelManager HIAI_ModelManager;

typedef struct HIAI_ModelManagerListener {
    void (*onRunDone)(void*, HIAI_Status, HIAI_NDTensorBuffer* [], int32_t);
    void (*onServiceDied)(void*);
    void* userData;
} HIAI_ModelManagerListener;

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_H
