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
