/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager for aipp
 */
#ifndef HIAI_API_MODEL_MANAGER_AIPP_H
#define HIAI_API_MODEL_MANAGER_AIPP_H

#include "model_manager.h"
#include "tensor/aipp_para.h"

namespace hiai {
class IModelManagerAipp : public IModelManager {
public:
    virtual ~IModelManagerAipp() = default;

    virtual Status RunAippModel(const Context& context, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        const std::vector<std::shared_ptr<IAIPPPara>>& aippParas,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs, int32_t timeoutInMS) = 0;
};
} // namespace hiai
#endif // HIAI_API_MODEL_MANAGER_AIPP_H
