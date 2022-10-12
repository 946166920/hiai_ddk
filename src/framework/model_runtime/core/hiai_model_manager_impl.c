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
#include "hiai_model_manager_impl.h"

#include <stdbool.h>

#include "framework/infra/log/log.h"
#include "tensor/base/hiai_nd_tensor_buffer_legacy_compatible.h"
#include "hiai_model_runtime_repo.h"

HIAI_ModelManager_Impl* HIAI_ModelManager_ToModelManagerImpl(HIAI_ModelManager* manager)
{
    if (manager == NULL) {
        FMK_LOGE("input manager is null.");
        return NULL;
    }

    HIAI_ModelManager_Impl* impl = (HIAI_ModelManager_Impl*)manager;
    if (impl->runtimeModelManager == NULL || impl->runtime == NULL) {
        FMK_LOGE("input manager is invalid.");
        return NULL;
    }
    return impl;
}

HIAI_Status HIAI_ModelManager_PreRun(const HIAI_ModelRuntime* runtime, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_NDTensorBuffer* output[], int32_t outputNum)
{
    if ((runtime == NULL) || (input == NULL) || (output == NULL)) {
        return HIAI_INVALID_PARAM;
    }

#ifdef AI_SUPPORT_LEGACY_ROM_COMPATIBLE
    if (runtime->handle == NULL) { // static lib
        return HIAI_SUCCESS;
    }

    if (runtime->runtimeType == PLUGIN_MODEL_RUNTIME_HCL &&
        HIAI_ModelRuntimeRepo_IsOldHclModelRuntime((HIAI_ModelRuntime*)runtime)) {
        bool isChangeInputSuccess = HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(input, inputNum);
        bool isChangeOutputSuccess = HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(output, outputNum);
        if (!isChangeInputSuccess || !isChangeOutputSuccess) {
            return HIAI_FAILURE;
        }
    }
#endif
    return HIAI_SUCCESS;
}