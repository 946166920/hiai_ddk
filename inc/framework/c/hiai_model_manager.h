/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager
 */
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_built_model.h"
#include "hiai_nd_tensor_buffer.h"
#include "hiai_model_manager_types.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_ModelManager* HIAI_ModelManager_Create();
AICP_C_API_EXPORT void HIAI_ModelManager_Destroy(HIAI_ModelManager** manager);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_Init(HIAI_ModelManager* manager, const HIAI_ModelInitOptions* options,
    const HIAI_BuiltModel* builtModel, const HIAI_ModelManagerListener* listener);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_SetPriority(HIAI_ModelManager* manager, HIAI_ModelPriority priority);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_Run(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_NDTensorBuffer* output[], int32_t outputNum);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_RunAsync(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_Cancel(HIAI_ModelManager* manager);

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_Deinit(HIAI_ModelManager* manager);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_H
