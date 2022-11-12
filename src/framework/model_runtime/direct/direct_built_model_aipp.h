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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_AIPP_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_AIPP_H

#include "framework/c/hiai_built_model_types.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/c/hiai_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace hiai {
HIAI_Status HIAI_DIRECT_BuiltModel_GetTensorAippInfo(
    const HIAI_BuiltModel* model, int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);

HIAI_Status HIAI_DIRECT_BuiltModel_GetTensorAippPara(const HIAI_BuiltModel* model, int32_t index,
    HIAI_TensorAippPara* aippParas[], uint32_t aippParaNum, uint32_t batchCount);
} // namespace hiai

#ifdef __cplusplus
}
#endif

#endif
