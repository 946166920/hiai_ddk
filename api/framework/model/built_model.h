/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: built model
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