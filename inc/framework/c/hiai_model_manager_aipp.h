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
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
#include "hiai_c_api_export.h"
#include "hiai_error_types.h"
#include "hiai_model_manager.h"
#include "hiai_tensor_aipp_para.h"

#ifdef __cplusplus
extern "C" {
#endif

AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelManager_runAippModelV2(HIAI_MR_ModelManager* manager,
    HIAI_MR_NDTensorBuffer* input[], int32_t inputNum, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum,
    HIAI_MR_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_AIPP_H
