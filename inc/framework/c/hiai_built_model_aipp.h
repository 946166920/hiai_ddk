/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FRAMEWORK_C_HIAI_BUILT_MODEL_AIPP_H
#define FRAMEWORK_C_HIAI_BUILT_MODEL_AIPP_H
#include "hiai_c_api_export.h"
#include "hiai_built_model.h"
#include "hiai_tensor_aipp_para.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_GetTensorAippInfo(
    const HIAI_MR_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);

AICP_C_API_EXPORT HIAI_Status HIAI_MR_BuiltModel_GetTensorAippPara(const HIAI_MR_BuiltModel* model, int32_t index,
    HIAI_MR_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount);

#ifdef __cplusplus
}
#endif

#endif
