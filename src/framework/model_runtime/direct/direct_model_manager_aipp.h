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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_MANAGER_AIPP_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_MANAGER_AIPP_H

#include "framework/c/hiai_error_types.h"
#include "framework/c/hiai_tensor_aipp_para.h"

#include "direct_model_manager.h"

#ifdef __cplusplus
extern "C" {
#endif
namespace hiai {
HIAI_Status HIAI_DIRECT_ModelManager_runAippModelV2(HIAI_MR_ModelManager* manager, HIAI_MR_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_MR_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData);
}
#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_MANAGER_AIPP_H
