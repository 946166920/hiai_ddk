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
#include "framework/infra/log/log.h"
#include "control_c.h"

HIAI_Status HIAI_ModelBuilder_Build(const HIAI_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_BuiltModel** builtModel)
{
    if (!hiai::ControlC::GetInstance().CheckBuildOptions(options)) {
        FMK_LOGE("build options is error.");
        return HIAI_FAILURE;
    }

    *builtModel = (HIAI_BuiltModel*)malloc(1);
    return HIAI_SUCCESS;
}
