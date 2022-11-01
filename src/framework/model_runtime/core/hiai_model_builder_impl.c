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
#include "hiai_model_builder_impl.h"
#include "hiai_built_model_impl.h"
#include "framework/infra/log/log.h"

typedef HIAI_Status (*HIAI_ModelBuilder_Build_Ptr)(const HIAI_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_BuiltModel** builtModel);

HIAI_BuiltModel_Impl* HIAI_ModelBuilder_BuildOnRuntime(const HIAI_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_ModelRuntime* runtime)
{
    if (runtime == NULL) {
        return NULL;
    }
    HIAI_BuiltModel* builtModel = NULL;

    HIAI_ModelBuilder_Build_Ptr buildV2Func =
        (HIAI_ModelBuilder_Build_Ptr)runtime->symbolList[HRANI_MODELBUILDER_BUILDV2];
    if (buildV2Func != NULL) {
        HIAI_Status status = buildV2Func(options, modelName, inputModelData, inputModelSize, &builtModel);
        if (status != HIAI_SUCCESS) {
            return NULL;
        }
        return HIAI_BuiltModel_CreateModelImpl(builtModel, runtime);
    }

    HIAI_ModelBuilder_Build_Ptr buildFunc = (HIAI_ModelBuilder_Build_Ptr)runtime->symbolList[HRANI_MODELBUILDER_BUILD];
    if (buildFunc != NULL) {
        HIAI_Status status = buildFunc(options, modelName, inputModelData, inputModelSize, &builtModel);
        if (status != HIAI_SUCCESS) {
            return NULL;
        }
        return HIAI_BuiltModel_CreateModelImpl(builtModel, runtime);
    }

    return NULL;
}
