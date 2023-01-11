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
#include "direct_built_model_aipp.h"

// inc
#include "framework/infra/log/log.h"

#include "direct_built_model_impl.h"

namespace hiai {
HIAI_Status HIAI_DIRECT_BuiltModel_GetTensorAippInfo(
    const HIAI_MR_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is nullptr");
        return HIAI_FAILURE;
    }

    return builtModelImpl->GetTensorAippInfo(index, aippParaNum, batchCount);
}

HIAI_Status HIAI_DIRECT_BuiltModel_GetTensorAippPara(const HIAI_MR_BuiltModel* model, int32_t index,
    HIAI_MR_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount)
{
    (void)batchCount;
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is nullptr");
        return HIAI_FAILURE;
    }

    if (aippParas == nullptr) {
        FMK_LOGE("aippParas is nullptr");
        return HIAI_FAILURE;
    }

    std::vector<void*> aippParaVec;
    auto ret = builtModelImpl->GetTensorAippPara(index, aippParaVec);
    if (ret != SUCCESS || aippParaVec.size() != aippParaNum) {
        FMK_LOGE("GetTensorAippPara fail, aippParaVec.size=%zu", aippParaVec.size());
        for (auto& aippPara : aippParaVec) {
            auto tesorAippPara = static_cast<HIAI_MR_TensorAippPara*>(aippPara);
            HIAI_MR_TensorAippPara_Destroy(&tesorAippPara);
        }
        return HIAI_FAILURE;
    }

    for (uint32_t i = 0; i < aippParaNum; i++) {
        aippParas[i] = static_cast<HIAI_MR_TensorAippPara*>(aippParaVec[i]);
    }

    return HIAI_SUCCESS;
}
}