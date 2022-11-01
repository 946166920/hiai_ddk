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
#ifndef FRAMEWORK_MODEL_BUILT_MODEL_IMPL_H
#define FRAMEWORK_MODEL_BUILT_MODEL_IMPL_H
// api/framework
#ifdef AI_SUPPORT_AIPP_API
#include "model/built_model_aipp.h"
#else
#include "model/built_model.h"
#endif
// inc
#include "framework/util/base_buffer.h"
// src/framework
#include "model_runtime/core/hiai_built_model_impl.h"

namespace hiai {
#ifdef AI_SUPPORT_AIPP_API
class BuiltModelImpl : public IBuiltModelAipp {
#else
class BuiltModelImpl : public IBuiltModel {
#endif
public:
    BuiltModelImpl() = default;
    BuiltModelImpl(std::shared_ptr<HIAI_BuiltModel> builtModel);
    ~BuiltModelImpl() = default;

    std::shared_ptr<HIAI_BuiltModel> GetBuiltModelImpl();

private:
    Status SaveToExternalBuffer(std::shared_ptr<IBuffer>& buffer, size_t& realSize) const override;
    Status SaveToBuffer(std::shared_ptr<IBuffer>& buffer) const override;
    Status SaveToFile(const char* file) const override;

    Status RestoreFromBuffer(const std::shared_ptr<IBuffer>& buffer) override;
    Status RestoreFromFile(const char* file) override;

    Status CheckCompatibility(bool& compatible) const override;

    std::string GetName() const override;
    void SetName(const std::string& name) override;

    std::vector<NDTensorDesc> GetInputTensorDescs() const override;
    std::vector<NDTensorDesc> GetOutputTensorDescs() const override;

    void SetCustomData(const CustomModelData& customModelData) override;
    const CustomModelData& GetCustomData() override;

#ifdef AI_SUPPORT_AIPP_API
    Status GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount) override;
    Status GetTensorAippPara(int32_t index, std::vector<void*>& aippParas) const override;
#endif

protected:
    std::shared_ptr<HIAI_BuiltModel> builtModelImpl_ {nullptr};

private:
    std::shared_ptr<BaseBuffer> buffer_ {nullptr};
    CustomModelData customModelData_;
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_BUILT_MODEL_IMPL_H
