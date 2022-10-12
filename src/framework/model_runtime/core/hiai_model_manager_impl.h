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
#ifndef FRAMEWORK_C_HIAI_MODEL_MANAGER_IMPL_H
#define FRAMEWORK_C_HIAI_MODEL_MANAGER_IMPL_H

#include "framework/c/hiai_model_manager.h"
#include "hiai_model_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_ModelManager_Impl {
    HIAI_ModelManager* runtimeModelManager;
    const HIAI_ModelRuntime* runtime;
} HIAI_ModelManager_Impl;

HIAI_ModelManager_Impl* HIAI_ModelManager_ToModelManagerImpl(HIAI_ModelManager* manager);

HIAI_Status HIAI_ModelManager_PreRun(const HIAI_ModelRuntime* runtime, HIAI_NDTensorBuffer* input[], int32_t inputNum,
    HIAI_NDTensorBuffer* output[], int32_t outputNum);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_MANAGER_IMPL_H
