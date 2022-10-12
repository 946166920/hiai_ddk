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
#include "framework/c/hiai_model_manager_aipp.h"
#include "hiai_model_manager_impl.h"
#include "framework/infra/log/log.h"

typedef HIAI_Status (*HIAI_ModelManager_runAippModelV2_Ptr)(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[],
    int32_t inputNum, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_NDTensorBuffer* output[],
    int32_t outputNum, int32_t timeoutInMS, void* userData);
HIAI_Status HIAI_ModelManager_runAippModelV2(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_NDTensorBuffer* output[], int32_t outputNum,
    int32_t timeoutInMS, void* userData)
{
    HIAI_ModelManager_Impl* managerImpl = HIAI_ModelManager_ToModelManagerImpl(manager);
    if (managerImpl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_Status ret = HIAI_ModelManager_PreRun(managerImpl->runtime, input, inputNum, output, outputNum);
    if (ret != HIAI_SUCCESS) {
        return ret;
    }
    if (aippPara == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_ModelManager_runAippModelV2_Ptr runAippV3 =
        (HIAI_ModelManager_runAippModelV2_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN_AIPP_MODEL_V3];
    if (runAippV3 != NULL) {
        return runAippV3(managerImpl->runtimeModelManager, input, inputNum, aippPara, aippParaNum, output, outputNum,
            timeoutInMS, userData);
    }

    HIAI_ModelManager_runAippModelV2_Ptr runAippV2 =
        (HIAI_ModelManager_runAippModelV2_Ptr)managerImpl->runtime->symbolList[HRANI_MODELMANAGER_RUN_AIPP_MODEL_V2];
    if (runAippV2 != NULL) {
        return runAippV2(managerImpl->runtimeModelManager, input, inputNum, aippPara, aippParaNum, output, outputNum,
            timeoutInMS, userData);
    }

    FMK_LOGW("sym %d not found.", HRANI_MODELMANAGER_RUN_AIPP_MODEL_V2);
    return HIAI_UNSUPPORTED;
}