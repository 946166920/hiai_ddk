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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_IMPL_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_IMPL_H

#include <string>
#include <memory>

// api/frmaework
#include "tensor/nd_tensor_desc.h"

// api/infra
#include "base/error_types.h"

// inc
#include "infra/base/base_buffer.h"
#include "framework/c/hiai_nd_tensor_desc.h"
#include "framework/c/compatible/HIAIModelManager.h"

namespace hiai {
class DirectBuiltModelImpl {
public:
    DirectBuiltModelImpl(std::shared_ptr<BaseBuffer>& buffer, std::string modelName);
    DirectBuiltModelImpl(std::string modelFile);
    ~DirectBuiltModelImpl();

    Status CheckCompatibility(bool& isCompatible);
    int32_t GetModelIOTensorNum(bool isInput);
    HIAI_NDTensorDesc* GetModelIOTensorDesc(size_t index, bool isInput);

#ifdef AI_SUPPORT_AIPP_API
    Status GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);
    Status GetTensorAippPara(int32_t index, std::vector<void*>& aippParas);
#endif
    Status Save(void** data, size_t* size);
    Status SaveToFile(const char* file);
    Status SaveToExternalBuffer(void* data, size_t size, size_t* realSize);

    const std::string& GetModelName() const;
    const std::string& GetModelFile() const;
    const std::shared_ptr<BaseBuffer>& GetModelBuffer() const;
    void SetModelName(std::string modelName);

private:
    std::shared_ptr<HIAI_ModelManager> GetLoadedModelManager();
    Status GetModelIOTensorNum(uint32_t& inputCount, uint32_t& outputCount);
    Status GetIONDTensorDesc();
#ifdef AI_SUPPORT_AIPP_API
    Status GetAippPara(HIAI_ModelManager* manager, int32_t index, unsigned int aippCount,
        unsigned int batchCount, std::vector<void*>& aippParaVec);
    Status GetAippParaByIndex(HIAI_ModelManager* manager, const std::string& modelName,
        int32_t index, std::vector<void*>& aippParaVec);
#endif
    Status LoadToBuffer();

private:
    std::vector<HIAI_NDTensorDesc*> inputs_;
    std::vector<HIAI_NDTensorDesc*> outputs_;
    std::shared_ptr<BaseBuffer> modelBuffer_ {nullptr};
    std::string modelName_ {};
    std::string modelFile_ {};
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_IMPL_H
