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

struct HIAI_TensorAippPara {
    size_t size;
    void* data;
    void* handle;
};

HIAI_Status HIAI_MR_BuiltModel_GetTensorAippInfo(
    const HIAI_MR_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    if (model == NULL || aippParaNum == NULL || batchCount == NULL) {
        return HIAI_FAILURE;
    }
    *aippParaNum = 2; /* 2:AippParaNum打桩 */
    *batchCount = 1;

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_MR_BuiltModel_GetTensorAippPara(const HIAI_MR_BuiltModel* model, int32_t index,
    HIAI_MR_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount)
{
    uint32_t getAippNum = 0;
    uint32_t getbatchCount = 0;
    HIAI_Status ret = HIAI_MR_BuiltModel_GetTensorAippInfo(model, index, &getAippNum, &getbatchCount);
    if (ret != HIAI_SUCCESS || getAippNum != aippParaNum || getbatchCount != batchCount) {
        return HIAI_FAILURE;
    }

    for (uint32_t i = 0; i < aippParaNum; i++) {
        aippParas[i] = HIAI_MR_TensorAippPara_Create(batchCount);
        if (aippParas[i] == NULL) {
            return HIAI_FAILURE;
        }
    }
    return HIAI_SUCCESS;
}