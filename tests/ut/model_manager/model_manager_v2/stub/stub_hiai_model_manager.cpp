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
#include <map>
#include <thread>
#include "framework/c/hiai_model_manager.h"
#include "framework/c/hiai_model_manager_aipp.h"
#include "framework/infra/log/log.h"
#include "control_c.h"

using namespace hiai;

HIAI_ModelManager* HIAI_ModelManager_Create()
{
    HIAI_ModelManager* manager = (HIAI_ModelManager*)malloc(1);
    return manager;
}

std::map<HIAI_ModelManager*, const HIAI_ModelManagerListener*> g_StubListener;

void HIAI_ModelManager_Destroy(HIAI_ModelManager** manager)
{
    if (manager == NULL || *manager == nullptr) {
        return;
    }

    free(*manager);
    *manager = NULL;
}

HIAI_Status HIAI_ModelManager_Init(HIAI_ModelManager* manager, const HIAI_ModelInitOptions* options,
    const HIAI_BuiltModel* builtModel, const HIAI_ModelManagerListener* listener)
{
    if (!hiai::ControlC::GetInstance().CheckInitOptions(options)) {
        FMK_LOGE("build options is error.");
        return HIAI_FAILURE;
    }
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        return HIAI_INVALID_PARAM;
    }
    g_StubListener.insert(std::make_pair(manager, listener));

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_SetPriority(HIAI_ModelManager* manager, HIAI_ModelPriority priority)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_Run(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_NDTensorBuffer* output[], int32_t outputNum)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_RunAsync(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_NDTensorBuffer* output[], int32_t outputNum, int32_t timeoutInMS, void* userData)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_Cancel(HIAI_ModelManager* manager)
{
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_Deinit(HIAI_ModelManager* manager)
{
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        g_StubListener.erase(iter);
    }

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_ModelManager_runAippModelV2(HIAI_ModelManager* manager, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, HIAI_NDTensorBuffer* output[], int32_t outputNum,
    int32_t timeoutInMS, void* userData)
{
    auto iter = g_StubListener.find(manager);
    if (iter != g_StubListener.end()) {
        if (iter->second != nullptr) {
            std::thread runDoneThd(iter->second->onRunDone, userData, HIAI_SUCCESS, output, outputNum);
            runDoneThd.detach();
        }
    }

    return ControlC::GetInstance().GetExpectStatus(CKey::C_RUN_AIPP_MODEL_V2_FAILED);
}