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
#ifndef INCL_HIAI_MODEL_BUILDER_IMPL_H
#define INCL_HIAI_MODEL_BUILDER_IMPL_H

#include "hiai_model_runtime.h"
#include "hiai_built_model_impl.h"
#include "framework/c/hiai_model_builder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

HIAI_BuiltModel_Impl* HIAI_ModelBuilder_BuildOnRuntime(const HIAI_MR_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_ModelRuntime* runtime);

#ifdef __cplusplus
}
#endif

#endif // INCL_HIAI_MODEL_BUILDER_IMPL_H