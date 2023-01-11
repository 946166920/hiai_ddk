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
#ifndef FRAMEWORK_MODEL_MANAGER_MODEL_BUILDER_H
#define FRAMEWORK_MODEL_MANAGER_MODEL_BUILDER_H
#include "model_builder/model_builder.h"
#include "infra/base/base_buffer.h"

namespace hiai {

class ModelBuilderImpl : public IModelBuilder {
public:
    ModelBuilderImpl() = default;
    ~ModelBuilderImpl() override = default;

    Status Build(const ModelBuildOptions& options, const std::string& modelName,
        const std::shared_ptr<IBuffer>& modelBuffer, std::shared_ptr<IBuiltModel>& builtModel) override;

    Status Build(const ModelBuildOptions& options, const std::string& modelName, const std::string& modelFile,
        std::shared_ptr<IBuiltModel>& builtModel) override;
private:
    std::shared_ptr<BaseBuffer> buffer_{nullptr};
};

} // namespace hiai

#endif
