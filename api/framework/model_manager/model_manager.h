/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager
 */
#ifndef HIAI_API_MODEL_MANAGER_H
#define HIAI_API_MODEL_MANAGER_H

#include "model_manager_api_export.h"
#include "model_manager_types.h"
#include "model/built_model.h"
#include "tensor/nd_tensor_buffer.h"
#include "base/error_types.h"

namespace hiai {
class IModelManagerListener {
public:
    virtual ~IModelManagerListener() = default;

    virtual void OnRunDone(
        const Context& context, Status result, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs) = 0;

    virtual void OnServiceDied() = 0;
};

class IModelManager {
public:
    virtual ~IModelManager() = default;

    virtual Status Init(const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel,
        const std::shared_ptr<IModelManagerListener>& listener) = 0;

    virtual Status SetPriority(ModelPriority priority) = 0;

    virtual Status Run(const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs) = 0;

    virtual Status RunAsync(const Context& context, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs, int32_t timeout) = 0;

    virtual Status Cancel() = 0;

    virtual void DeInit() = 0;
};

HIAI_MM_API_EXPORT std::shared_ptr<IModelManager> CreateModelManager();
} // namespace hiai
#endif // HIAI_API_MODEL_MANAGER_H
