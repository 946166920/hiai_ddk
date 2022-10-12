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
#include "framework/c/hiai_built_model_aipp.h"
#include "hiai_built_model_impl.h"
#include "framework/infra/log/log.h"

typedef HIAI_Status (*HIAI_BuiltModel_GetTensorAippInfo_Ptr)(
    const HIAI_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);

HIAI_Status HIAI_BuiltModel_GetTensorAippInfo(
    const HIAI_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    const HIAI_BuiltModel_Impl* modelImpl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (modelImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_GetTensorAippInfo_Ptr get =
        (HIAI_BuiltModel_GetTensorAippInfo_Ptr)modelImpl->runtime->symbolList[HRANI_BUILTMODEL_GET_TENSOR_AIPP_INFO];
    if (get != NULL) {
        return get(modelImpl->runtimeBuiltModel, index, aippParaNum, batchCount);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_TENSOR_AIPP_INFO);
    return HIAI_UNSUPPORTED;
}

typedef HIAI_Status (*HIAI_BuiltModel_GetTensorAippPara_Ptr)(const HIAI_BuiltModel* model, int32_t index,
    HIAI_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount);

HIAI_Status HIAI_BuiltModel_GetTensorAippPara(const HIAI_BuiltModel* model, int32_t index,
    HIAI_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount)
{
    const HIAI_BuiltModel_Impl* modelImpl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (modelImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_GetTensorAippPara_Ptr get =
        (HIAI_BuiltModel_GetTensorAippPara_Ptr)modelImpl->runtime->symbolList[HRANI_BUILTMODEL_GET_TENSOR_AIPP_PARA];
    if (get != NULL) {
        return get(modelImpl->runtimeBuiltModel, index, aippParas, aippParaNum, batchCount);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_TENSOR_AIPP_PARA);
    return HIAI_UNSUPPORTED;
}