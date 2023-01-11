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
#ifndef FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_IMPL_H
#define FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_IMPL_H

#include <mutex>
#include <condition_variable>

// api/infra
#include "base/error_types.h"

// inc
#include "framework/c/compatible/HIAIModelManager.h"
#include "framework/c/hiai_model_manager_types.h"
#include "framework/c/hiai_tensor_aipp_para.h"

#include "direct_built_model_impl.h"
#include "direct_model_manager_util.h"

#ifdef AI_SUPPORT_AIPP_API
#include "model_manager/model_manager_aipp.h"
#endif

namespace hiai {
typedef struct HIAI_NDTensorBuffers {
    int32_t inputNum;
    int32_t outputNum;
    HIAI_MR_NDTensorBuffer** input;
    HIAI_MR_NDTensorBuffer** output;

    HIAI_NDTensorBuffers(int32_t inN, int32_t outN, HIAI_MR_NDTensorBuffer** inBuf, HIAI_MR_NDTensorBuffer** outBuf)
        : inputNum(inN), outputNum(outN), input(inBuf), output(outBuf) {}
} HIAI_NDTensorBuffers;

class DirectModelManagerImpl {
public:
    DirectModelManagerImpl() = default;
    ~DirectModelManagerImpl();

    Status Init(const DirectBuiltModelImpl& builtModel, const HIAI_MR_ModelInitOptions& options,
                        const HIAI_MR_ModelManagerListener* listener);
    Status SetPriority(HIAI_ModelPriority priority);

    Status Run(HIAI_NDTensorBuffers& buffers);
    Status RunAsync(HIAI_NDTensorBuffers& buffers, int32_t timeStamp, void* userData);
#ifdef AI_SUPPORT_AIPP_API
    Status RunAipp(HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum,
        int32_t timeoutInMS, void* userData);
#endif
    void Cancel();
    void DeInit();

private:
    Status CreateLegacyListener(const HIAI_MR_ModelManagerListener* listener,
                                HIAI_ModelManagerListenerLegacy*& cListener);
    std::shared_ptr<HIAI_ModelManager> CreateLegacyManager(
        const HIAI_ModelManagerListenerLegacy* cListener);
    Status InitSync(HIAI_ModelManager* manager, const ModelLoadInfo& loadInfo);
    Status InitAsync(HIAI_ModelManager* manager, const ModelLoadInfo& loadInfo);
    void UnloadAsync(HIAI_ModelManager* manager);

#ifdef AI_SUPPORT_AIPP_API
    template <typename T>
    int ProcessAipp(HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[],
                    int32_t aippParaNum, int32_t timeout, void* runfunc);
    int RunAippModel(
        HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, int32_t timeoutInMS);
#endif

private:
    static void OnLoadDone(void* userdata, int taskStamp);
    static void OnRunDone(void* userdata, int taskStamp);
    static void OnUnloadDone(void* userdata, int taskStamp);
    static void OnTimeout(void* userdata, int taskStamp);
    static void OnError(void* userdata, int taskStamp, int errCode);
    static void OnServiceDied(void* userdata);

private:
    const HIAI_MR_ModelManagerListener* userListener_ {nullptr};
    std::shared_ptr<HIAI_ModelManager> manager_ {nullptr};
    std::string modelName_ {};
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_IMPL_H