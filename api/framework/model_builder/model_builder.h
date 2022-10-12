/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model builder
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
