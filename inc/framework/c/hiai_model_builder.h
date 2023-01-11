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
#ifndef FRAMEWORK_C_HIAI_MODEL_BUILDER_H
#define FRAMEWORK_C_HIAI_MODEL_BUILDER_H
#include "hiai_c_api_export.h"
#include "hiai_built_model.h"
#include "hiai_model_builder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// build interface
AICP_C_API_EXPORT HIAI_Status HIAI_MR_ModelBuilder_Build(const HIAI_MR_ModelBuildOptions* options,
    const char* modelName, const void* inputModelData, size_t inputModelSize, HIAI_MR_BuiltModel** builtModel);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_BUILDER_H
