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
#include "direct_model_runtime.h"
#include "direct_model_builder.h"
#include "direct_built_model.h"
#include "direct_model_manager.h"
#ifdef AI_SUPPORT_AIPP_API
#include "direct_model_manager_aipp.h"
#include "direct_built_model_aipp.h"
#endif
#include "model_runtime/core/model_runtime_register.h"

namespace hiai {
void HIAI_ModelRuntime_Init(HIAI_ModelRuntime* runtime)
{
    // ModelBuilder
    runtime->symbolList[HRANI_MODELBUILDER_BUILD] = reinterpret_cast<void*>(HIAI_DIRECT_ModelBuilder_Build);
    runtime->symbolList[HRANI_MODELBUILDER_BUILD_FROM_SHARED_BUFFER] = nullptr;

    // BuiltModel
    runtime->symbolList[HRANI_BUILTMODEL_RESTORE] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_Restore);
    runtime->symbolList[HRANI_BUILTMODEL_RESTORE_FROM_FILE] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_RestoreFromFile);
    runtime->symbolList[HRANI_BUILTMODEL_SAVE_TO_EXTERNAL_BUFFER] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_SaveToExternalBuffer);
    runtime->symbolList[HRANI_BUILTMODEL_SAVE] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_Save);
    runtime->symbolList[HRANI_BUILTMODEL_SAVE_TO_FILE] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_SaveToFile);
    runtime->symbolList[HRANI_BUILTMODEL_CHECK_COMPATIBILITY] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_CheckCompatibility);
    runtime->symbolList[HRANI_BUILTMODEL_GET_NAME] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetName);
    runtime->symbolList[HRANI_BUILTMODEL_SET_NAME] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_SetName);
    runtime->symbolList[HRANI_BUILTMODEL_GET_INPUT_TENSOR_NUM] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetInputTensorNum);
    runtime->symbolList[HRANI_BUILTMODEL_GET_INPUT_TENSOR_DESC] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetInputTensorDesc);
    runtime->symbolList[HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_NUM] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetOutputTensorNum);
    runtime->symbolList[HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_DESC] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetOutputTensorDesc);
    runtime->symbolList[HRANI_BUILTMODEL_DESTROY] = reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_Destroy);

#ifdef AI_SUPPORT_AIPP_API
    // BuiltModel AIPP
    runtime->symbolList[HRANI_BUILTMODEL_GET_TENSOR_AIPP_INFO] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetTensorAippInfo);
    runtime->symbolList[HRANI_BUILTMODEL_GET_TENSOR_AIPP_PARA] =
        reinterpret_cast<void*>(HIAI_DIRECT_BuiltModel_GetTensorAippPara);
#endif

    // ModelManager
    runtime->symbolList[HRANI_MODELMANAGER_CREATE] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Create);
    runtime->symbolList[HRANI_MODELMANAGER_DESTROY] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Destroy);
    runtime->symbolList[HRANI_MODELMANAGER_INIT] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Init);
    runtime->symbolList[HRANI_MODELMANAGER_SET_PRIORITY] =
        reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_SetPriority);
    runtime->symbolList[HRANI_MODELMANAGER_RUN] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Run);
    runtime->symbolList[HRANI_MODELMANAGER_RUN_ASYNC] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_RunAsync);
    runtime->symbolList[HRANI_MODELMANAGER_CANCEL] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Cancel);
    runtime->symbolList[HRANI_MODELMANAGER_DEINIT] = reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_Deinit);

    // ModelManager Ext
    runtime->symbolList[HRANI_MODELMANAGER_INIT_WITH_SHARED_MEM_ALLOCATOR] = nullptr;

#ifdef AI_SUPPORT_AIPP_API
    // ModelManager AIPP
    runtime->symbolList[HRANI_MODELMANAGER_RUN_AIPP_MODEL_V2] =
        reinterpret_cast<void*>(HIAI_DIRECT_ModelManager_runAippModelV2);
#endif
}

ModelRuntimeRegister g_directModelRuntime(HIAI_ModelRuntime_Init, HIAI_MODEL_RUNTIME_DIRECT);
} // namespace hiai