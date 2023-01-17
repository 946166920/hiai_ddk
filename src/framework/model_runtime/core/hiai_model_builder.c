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
#include "framework/c/hiai_model_builder.h"
#include "hiai_built_model_impl.h"
#include "hiai_model_runtime_repo.h"
#include "framework/infra/log/log.h"

HIAI_Status HIAI_MR_ModelBuilder_Build(const HIAI_MR_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_MR_BuiltModel** builtModel)
{
    HIAI_BuiltModel_Impl* builtImpl = ModelRuntimeRepo_TryBuild(options, modelName, inputModelData, inputModelSize);
    if (builtImpl != NULL) {
        *builtModel = (HIAI_MR_BuiltModel*)builtImpl;
        return HIAI_SUCCESS;
    }

    return HIAI_FAILURE;
}
