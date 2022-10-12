/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: built model for aipp
 */
#ifndef FRAMEWORK_C_HIAI_BUILT_MODEL_AIPP_H
#define FRAMEWORK_C_HIAI_BUILT_MODEL_AIPP_H
#include "hiai_c_api_export.h"
#include "hiai_built_model.h"
#include "hiai_tensor_aipp_para.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_GetTensorAippInfo(
    const HIAI_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);

AICP_C_API_EXPORT HIAI_Status HIAI_BuiltModel_GetTensorAippPara(const HIAI_BuiltModel* model, int32_t index,
    HIAI_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount);

#ifdef __cplusplus
}
#endif

#endif
