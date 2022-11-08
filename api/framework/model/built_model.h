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
#ifndef HIAI_API_BUILT_MODEL_H
#define HIAI_API_BUILT_MODEL_H

#include <string>
#include "model_api_export.h"
#include "tensor/nd_tensor_buffer.h"
#include "base/error_types.h"

namespace hiai {
struct CustomModelData {
    std::string type;
    std::string value;
};

class IBuiltModel {
public:
    virtual ~IBuiltModel() = default;

    virtual Status SaveToExternalBuffer(std::shared_ptr<IBuffer>& buffer, size_t& realSize) const = 0;

    virtual Status SaveToBuffer(std::shared_ptr<IBuffer>& buffer) const = 0;

    virtual Status RestoreFromBuffer(const std::shared_ptr<IBuffer>& buffer) = 0;

    virtual Status SaveToFile(const char* file) const = 0;

    virtual Status RestoreFromFile(const char* file) = 0;

    virtual Status CheckCompatibility(bool& compatible) const = 0;

    virtual std::string GetName() const = 0;
    virtual void SetName(const std::string& name) = 0;

    virtual std::vector<NDTensorDesc> GetInputTensorDescs() const = 0;
    virtual std::vector<NDTensorDesc> GetOutputTensorDescs() const = 0;

    virtual void SetCustomData(const CustomModelData& customModelData) = 0;
    virtual const CustomModelData& GetCustomData() = 0;
};

HIAI_M_API_EXPORT std::shared_ptr<IBuiltModel> CreateBuiltModel();
} // namespace hiai
#endif // HIAI_API_BUILT_MODEL_H