/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager aipp
 */
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_model_manager.h"
#include "hiai_tensor_aipp_para.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_ModelManager_runAippModelV2(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
