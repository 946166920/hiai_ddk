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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_RUNTIME_IMPL_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_RUNTIME_IMPL_H
#include "direct_built_model_impl.h"
#include "framework/c/compatible/HIAIModelManager.h"
#include "framework/c/hiai_model_manager_types.h"
#include "om/event_manager/om_wrapper.h"
#include "direct_model_util.h"

#include <mutex>
#include <condition_variable>

#ifdef AI_SUPPORT_AIPP_API
#include "model_manager/model_manager_aipp.h"
#endif
#include "base/error_types.h"

namespace hiai {
class ProfEventListener;

typedef struct HIAI_NDTensorBuffers {
    int32_t inputNum;
    int32_t outputNum;
    HIAI_NDTensorBuffer** input;
    HIAI_NDTensorBuffer** output;
} HIAI_NDTensorBuffers;

class DirectModelManagerImpl {
public:
    DirectModelManagerImpl() = default;
    ~DirectModelManagerImpl();

    Status Init(const HIAI_ModelInitOptions* options, const HIAI_ModelManagerListener* listener);

    Status SetPriority(HIAI_ModelPriority priority);

    Status Run(HIAI_NDTensorBuffers& buffers);

    Status RunAsync(HIAI_NDTensorBuffers& buffers, int32_t timeoutInMS, void* userData);

#ifdef AI_SUPPORT_AIPP_API
    Status RunAipp(HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum,
        int32_t timeoutInMS, void* userData);
#endif
    void Cancel();

    void DeInit();

private:
    int NDTensorProcess(HIAI_NDTensorBuffers& buffers, int32_t timeout, void* func);

    int TensorProcess(HIAI_NDTensorBuffers& buffers, int32_t timeout, void* func);

    Status Process(HIAI_NDTensorBuffers& buffers, int32_t timeoutInMS, int& stamp);

#ifdef AI_SUPPORT_AIPP_API
    int NDTensorAippProcess(HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum,
        int32_t timeout, void* func);

    int TensorAippProcess(HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum,
        int32_t timeout, void* func);
#endif

    Status InitModelListener(const HIAI_ModelManagerListener* listener);

    static void OnLoadDone(void* userdata, int taskStamp);
    static void OnRunDone(void* userdata, int taskStamp);
    static void OnUnloadDone(void* userdata, int taskStamp);
    static void OnTimeout(void* userdata, int taskStamp);
    static void OnError(void* userdata, int taskStamp, int errCode);
    static void OnServiceDied(void* userdata);

    void ClearCallbckcb();

    Status AsyncWaitResult(int iTStamp);

private:
    bool isLoadCallBack_ {false};
    bool isLoadStatus_ {false};
    bool isUnloadCallBack_ {false};

    HIAI_ModelManagerListener* userListener_ {nullptr};
    HIAI_ModelManagerListenerLegacy* listener_ {nullptr};

    std::mutex syncStopmutex_;
    std::mutex syncRunMutex_;
    std::condition_variable conditionUnLoad_;
    std::condition_variable condition_;
    std::shared_ptr<ProfEventListener> profEventListener_ {nullptr};

    std::shared_ptr<DirectModelUtil> currentModelUtil_ {nullptr};

public:
    std::shared_ptr<SharedManagerInfos> SharedManagerInfos_ {nullptr};
    bool isLoaded_ {false};
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_RUNTIME_IMPL_H