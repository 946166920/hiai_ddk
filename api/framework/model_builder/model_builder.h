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
#ifndef HIAI_API_MODEL_BUILDER_H
#define HIAI_API_MODEL_BUILDER_H

#include <string>
#include <vector>
#include <map>
#include "model_builder_types.h"
#include "model_builder_api_export.h"
#include "model/built_model.h"
#include "tensor/buffer.h"

namespace hiai {
class IModelBuilder {
public:
    virtual ~IModelBuilder() = default;

    virtual Status Build(const ModelBuildOptions& options, const std::string& modelName,
        const std::shared_ptr<IBuffer>& modelBuffer, std::shared_ptr<IBuiltModel>& builtModel) = 0;

    virtual Status Build(const ModelBuildOptions& options, const std::string& modelName, const std::string& modelFile,
        std::shared_ptr<IBuiltModel>& builtModel) = 0;
};

HIAI_MB_API_EXPORT std::shared_ptr<IModelBuilder> CreateModelBuilder();
} // namespace hiai
#endif // HIAI_API_MODEL_BUILDER_H
