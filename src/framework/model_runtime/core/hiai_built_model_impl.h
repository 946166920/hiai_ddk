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
#ifndef INCL_HIAI_BUILT_MODEL_IMPL_H
#define INCL_HIAI_BUILT_MODEL_IMPL_H

#include <stddef.h>

#include "framework/c/hiai_built_model.h"
#include "hiai_model_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_BuiltModel_Impl {
    HIAI_MR_BuiltModel* runtimeBuiltModel;
    const HIAI_ModelRuntime* runtime;
} HIAI_BuiltModel_Impl;

HIAI_BuiltModel_Impl* HIAI_BuiltModel_ToBuiltModelImpl(const HIAI_MR_BuiltModel* model);

HIAI_BuiltModel_Impl* HIAI_BuiltModel_CreateModelImpl(HIAI_MR_BuiltModel* model, const HIAI_ModelRuntime* runtime);

HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreOnRuntime(const void* data, size_t size, const HIAI_ModelRuntime* runtime);

HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreFromFileOnRuntime(const char* file, const HIAI_ModelRuntime* runtime);

HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreFromFileWithShapeIndexOnRuntime(
    const char* file, uint8_t shapeIndex, const HIAI_ModelRuntime* runtime);
#ifdef __cplusplus
}
#endif

#endif // INCL_HIAI_BUILT_MODEL_IMPL_H