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

#include "framework/util/base_buffer.h"
#include "tensor/nd_tensor_desc.h"
#include "base/error_types.h"
#include "direct_model_util.h"

namespace hiai {
class DirectBuiltModelImpl {
public:
    DirectBuiltModelImpl(std::shared_ptr<BaseBuffer>& buffer, std::string modelName, bool isNeedRelease = false);
    DirectBuiltModelImpl(std::string modelFile);
    ~DirectBuiltModelImpl();

    Status Init();

    Status CheckCompatibility(bool& isCompatible);
    Status GetModelIOTensorNum(uint32_t& inputCount, uint32_t& outputCount);
    Status GetModelIOTensorDescs(
        std::vector<HIAI_NDTensorDesc*>& inputDescs, std::vector<HIAI_NDTensorDesc*>& outputDescs);

#ifdef AI_SUPPORT_AIPP_API
    Status GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount);
    Status GetTensorAippPara(int32_t index, std::vector<void*>& aippParas);
#endif

private:
    Status GetIONDTensorDesc();
    Status InnerLoad();
    Status GetAippPara(int32_t index, unsigned int aippCount, unsigned int batchCount, std::vector<void*>& aippParaVec);

public:
    std::shared_ptr<BaseBuffer> modelBuffer_ {nullptr};
    std::string modelName_ {""};
    std::string modelFile_ {""};
    std::shared_ptr<SharedManagerInfos> SharedManagerInfos_ {nullptr};

private:
    bool isNeedRelease_ {false};
    std::vector<HIAI_NDTensorDesc*> inputs_;
    std::vector<HIAI_NDTensorDesc*> outputs_;
    std::shared_ptr<DirectModelUtil> currentModelUtil_ {nullptr};
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_BUILT_MODEL_IMPL_H
