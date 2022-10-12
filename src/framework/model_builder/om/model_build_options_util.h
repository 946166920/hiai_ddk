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
#ifndef FRAMEWORK_MODEL_BUILDER_OM_MODEL_BUILD_OPTIONS_UTIL_H
#define FRAMEWORK_MODEL_BUILDER_OM_MODEL_BUILD_OPTIONS_UTIL_H

#include "model_builder/model_builder_types.h"
#include "framework/c/hiai_model_builder_types.h"

namespace hiai {
class ModelBuildOptionsUtil {
public:
    static HIAI_ModelBuildOptions* ConvertToCBuildOptions(const ModelBuildOptions& options);
    static bool IsHasBuildOptions(const ModelBuildOptions& options);
};
}
#endif