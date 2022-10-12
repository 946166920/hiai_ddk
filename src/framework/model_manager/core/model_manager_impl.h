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
#ifndef FRAMEWORK_INC_MODEL_MANAGER_MODEL_MANAGER_IMPL_H
#define FRAMEWORK_INC_MODEL_MANAGER_MODEL_MANAGER_IMPL_H

// inc/api
#ifdef AI_SUPPORT_AIPP_API
#include "model_manager/model_manager_aipp.h"
#else
#include "model_manager/model_manager.h"
#endif

#include "framework/c/hiai_model_manager.h"

#include <mutex>

namespace hiai {

class ModelManagerImpl;
struct RunAsyncContext {
    Context context;
    ModelManagerImpl* modelManager;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
};

#ifdef AI_SUPPORT_AIPP_API
class ModelManagerImpl : public IModelManagerAipp {
#else
class ModelManagerImpl : public IModelManager {
#endif
public:
    ModelManagerImpl() = default;
    ~ModelManagerImpl() override;

private:
    Status Init(const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel,
        const std::shared_ptr<IModelManagerListener>& listener) override;

    Status SetPriority(ModelPriority priority) override;

    Status Run(const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs) override;

    Status RunAsync(const Context& context, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs, int32_t timeout) override;

#ifdef AI_SUPPORT_AIPP_API
    Status RunAippModel(const Context& context, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        const std::vector<std::shared_ptr<IAIPPPara>>& aippParas,
        std::vector<std::shared_ptr<INDTensorBuffer>>& outputs, int32_t timeoutInMS) override;
#endif
    Status Cancel() override;

    void DeInit() override;

private:
    Status PrepareModelManagerListener(const std::shared_ptr<IModelManagerListener>& listener);

    Status PrepareModelManager(const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel);

    void OnRunDone(const Context& context, Status errCode, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs);
    void OnServiceDied();

    static void OnRunDone(void* userData, HIAI_Status errCode, HIAI_NDTensorBuffer* output[], int32_t outputNum);
    static void OnServiceDied(void* userData);

    void UnLoad();

private:
    std::mutex modelManagerMutex_;
    std::shared_ptr<HIAI_ModelManager> modelManager_ {nullptr};

    std::mutex listenerMutex_;
    std::shared_ptr<IModelManagerListener> listener_ {nullptr};
    std::shared_ptr<HIAI_ModelManagerListener> cListener_ {nullptr};

    CustomModelData customModelData_;
};
} // namespace hiai
#endif // FRAMEWORK_INC_MODEL_MANAGER_MODEL_MANAGER_IMPL_H
