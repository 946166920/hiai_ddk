/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hiai built model aipp impl
 */
#include "framework/c/hiai_built_model_aipp.h"

HIAI_Status HIAI_BuiltModel_GetTensorAippInfo(
    const HIAI_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_BuiltModel_GetTensorAippPara(const HIAI_BuiltModel* model, int32_t index,
    HIAI_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount)
{
    return HIAI_SUCCESS;
}