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
#include "direct_model_manager_impl.h"

#include <algorithm>
#include <vector>
#include <map>

#include "infra/base/securestl.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/infra/log/log.h"
#include "direct_common_util.h"

#include "util/hiai_foundation_dl_helper.h"

namespace hiai {
class ProfEventListener : public EventListener {
public:
    void Handle(const AI_OM_EventMsg& event) override;
    bool GetDump()
    {
        return dump_;
    }
    bool GetProfiling()
    {
        return profiling_;
    }

private:
    bool profiling_ {false};
    bool dump_ {false};
};

struct DirectModelCallBack {
    DirectModelManagerImpl* modelMgr {nullptr};
    void* userData {nullptr};
    int32_t outputNum {};
    HIAI_NDTensorBuffer** output {};
};

static std::mutex g_mapLock;
static std::mutex g_callBackLock;
static std::map<int, DirectModelCallBack> g_callBackMap;
static std::map<int, DirectModelCallBack> g_loadMap;

#ifdef HIAI_DDK
void ProfEventListener::Handle(const AI_OM_EventMsg& event)
{
    switch (event.type) {
        case AI_OM_EventType::AI_PROFILING_ON:
            profiling_ = true;
            break;
        case AI_OM_EventType::AI_PROFILING_OFF:
            profiling_ = false;
            break;
        case AI_OM_EventType::AI_DUMP_ON:
            dump_ = true;
            break;
        case AI_OM_EventType::AI_DUMP_OFF:
            dump_ = false;
            break;
        default:
            break;
    }
}
#endif

DirectModelManagerImpl::~DirectModelManagerImpl()
{
    if (isLoaded_) {
        (void)DeInit();
    }
    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
        userListener_ = nullptr;
    }

    ClearCallbckcb();
}

Status DirectModelManagerImpl::InitModelListener(const HIAI_ModelManagerListener* listener)
{
    userListener_ = const_cast<HIAI_ModelManagerListener*>(listener);
    if (listener != nullptr) {
        listener_ = new (std::nothrow) HIAI_ModelManagerListenerLegacy();
        if (listener_ == nullptr) {
            FMK_LOGE("create listener failed");
            return FAILURE;
        }

        listener_->onLoadDone = OnLoadDone;
        listener_->onRunDone = OnRunDone;
        listener_->onUnloadDone = OnUnloadDone;
        listener_->onTimeout = OnTimeout;
        listener_->onError = OnError;
        listener_->onServiceDied = OnServiceDied;
    }

#ifdef HIAI_DDK
    profEventListener_ = make_shared_nothrow<ProfEventListener>();
    if (profEventListener_ == nullptr) {
        return FAILURE;
    }
    profEventListener_->Register();
#endif
    return SUCCESS;
}

static void ConvertPerfMode(HIAI_PerfMode& mode)
{
    const char* funcName = "HIAI_GetVersion";
    const char* (*getVersionFunc)() = (const char* (*)())HIAI_Foundation_GetSymbol(funcName);
    if (getVersionFunc == nullptr) {
        return;
    }

    const char* version = getVersionFunc();
    if (version == nullptr) {
        return;
    }
    if (mode == HIAI_PerfMode::HIAI_PERFMODE_UNSET) {
        mode = HIAI_PerfMode::HIAI_PERFMODE_NORMAL;
        return;
    }

    std::string romVersion = version;
    constexpr int32_t ROM_VERSION_PRODUCT_LEN = 11;
    const std::string supportExtremeVersion = "100.500.010";
    std::string baseVersion = romVersion.substr(0, ROM_VERSION_PRODUCT_LEN);
    if (baseVersion < supportExtremeVersion) {
        mode = mode > HIAI_PerfMode::HIAI_PERFMODE_HIGH ? HIAI_PerfMode::HIAI_PERFMODE_HIGH : mode;
    }
}

Status DirectModelManagerImpl::Init(const HIAI_ModelInitOptions* options, const HIAI_ModelManagerListener* listener)
{
    if (SharedManagerInfos_ == nullptr) {
        return FAILURE;
    }

    if (!DirectCommonUtil::CheckBuildOptions(
        HIAI_ModelInitOptions_GetBuildOptions(const_cast<HIAI_ModelInitOptions*>(options)))) {
        FMK_LOGE("Don't support this BuildOptions.");
        return FAILURE;
    }

    HIAI_PerfMode perfMode = HIAI_ModelInitOptions_GetPerfMode(options);
    (void)ConvertPerfMode(perfMode);

    if (listener_ != nullptr) {
        delete listener_;
        listener_ = nullptr;
        userListener_ = nullptr;
    }
    if (currentModelUtil_ != nullptr) {
        (void)currentModelUtil_->DestroyModelManager();
    }

    if (InitModelListener(listener) != SUCCESS) { // 创建监听
        return FAILURE;
    }

    if (listener_ == nullptr) {
        for (auto& info : SharedManagerInfos_->managers) {
            if (info != nullptr && info->CheckCanReuse(perfMode, listener_, true)) {
                currentModelUtil_ = info;
                isLoaded_ = true;
                FMK_LOGI("reuse model manager, load success.");
                return SUCCESS;
            }
        }
    }

    // 需要加载模型
    currentModelUtil_ = make_shared_nothrow<DirectModelUtil>();
    if (currentModelUtil_ == nullptr) {
        FMK_LOGE("create modelUtil failed");
        return FAILURE;
    }
    (void)currentModelUtil_->CreateModelManager(listener_);
    int taskStamp;
    {
        // ensure g_callBackMap update before use it
        std::lock_guard<std::mutex> loadLock(g_callBackLock);
        if (SharedManagerInfos_->modelBuffer != nullptr) {
            taskStamp =
            currentModelUtil_->LoadModel(SharedManagerInfos_->modelBuffer, SharedManagerInfos_->modelName, perfMode);
        } else {
            taskStamp =
            currentModelUtil_->LoadModel(SharedManagerInfos_->modelFile, SharedManagerInfos_->modelName, perfMode);
        }

        if (taskStamp < 0) {
            FMK_LOGE("LoadModel failed.");
            return FAILURE;
        }

        std::lock_guard<std::mutex> lock(g_mapLock);
        DirectModelCallBack cb;
        cb.modelMgr = this;
        g_callBackMap[taskStamp] = cb;
        g_loadMap[taskStamp] = cb;
    }

    Status status = SUCCESS;
    if (listener_ != nullptr) {
        status = AsyncWaitResult(taskStamp);
    }

    SharedManagerInfos_->managers.emplace_back(currentModelUtil_);
    isLoaded_ = true;
    return status;
}

Status DirectModelManagerImpl::SetPriority(HIAI_ModelPriority priority)
{
    void* setModelPriorityFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_setModelPriority");
    if (setModelPriorityFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    int ret = ((int (*)(HIAI_ModelManager*, const char*, HIAI_ModelPriority))setModelPriorityFunc)(
        currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName.c_str(), priority);
    if (ret != 0) {
        FMK_LOGE("set priority faied, model name: %s", SharedManagerInfos_->modelName.c_str());
        return FAILURE;
    }

    return SUCCESS;
}

static Status GetNDTensorBuffers(
    HIAI_NDTensorBuffer* tensor[], int32_t num, std::vector<HIAI_NDTensorBuffer*>& ndTensorBuffers)
{
    for (int32_t i = 0; i < num; i++) {
        auto buffer = static_cast<HIAI_NDTensorBuffer*>(HIAI_NDTensorBuffer_GetHandle(tensor[i]));
        if (buffer == nullptr) {
            FMK_LOGE("nd buffer is null");
            return FAILURE;
        }
        ndTensorBuffers.push_back(buffer);
    }
    return SUCCESS;
}

static Status GetTensorBuffers(
    HIAI_NDTensorBuffer* tensor[], int32_t num, std::vector<HIAI_TensorBuffer*>& tensorBuffers)
{
    for (int32_t i = 0; i < num; i++) {
        auto buffer = static_cast<HIAI_TensorBuffer*>(HIAI_NDTensorBuffer_GetHandle(tensor[i]));
        if (buffer == nullptr) {
            FMK_LOGE("nd buffer is null");
            return FAILURE;
        }
        tensorBuffers.push_back(buffer);
    }
    return SUCCESS;
}

static bool IsTensorDimsLargerThan4D(HIAI_NDTensorBuffer* tensor[], int32_t num)
{
    for (int32_t i = 0; i < num; i++) {
        HIAI_NDTensorDesc* desc = HIAI_NDTensorBuffer_GetNDTensorDesc(tensor[i]);
        size_t dimNum = HIAI_NDTensorDesc_GetDimNum(desc);
        if (dimNum > 4) {
            FMK_LOGI("this is 5D or more tensor.");
            return true;
        }
    }
    return false;
}

int DirectModelManagerImpl::NDTensorProcess(HIAI_NDTensorBuffers& buffers, int32_t timeout, void* func)
{
    int stamp = -1;
    std::vector<HIAI_NDTensorBuffer*> inputNDTensorBuffs;
    std::vector<HIAI_NDTensorBuffer*> outputNDTensorBuffs;
    Status inputStatus = GetNDTensorBuffers(buffers.input, buffers.inputNum, inputNDTensorBuffs);
    Status outputStatus = GetNDTensorBuffers(buffers.output, buffers.outputNum, outputNDTensorBuffs);
    if (inputStatus != SUCCESS || outputStatus != SUCCESS) {
        FMK_LOGE("inputs or output is invalid.");
        return stamp;
    }

    return ((int (*)(HIAI_ModelManager*, HIAI_NDTensorBuffer*[], int, HIAI_NDTensorBuffer*[], int, int,
        const char*))func)(currentModelUtil_->GetModelManager(), inputNDTensorBuffs.data(), buffers.inputNum,
        outputNDTensorBuffs.data(), buffers.outputNum, timeout, SharedManagerInfos_->modelName.c_str());
}

int DirectModelManagerImpl::TensorProcess(HIAI_NDTensorBuffers& buffers, int32_t timeout, void* func)
{
    int stamp = -1;
    if (IsTensorDimsLargerThan4D(buffers.input, buffers.inputNum) ||
        IsTensorDimsLargerThan4D(buffers.output, buffers.outputNum)) {
        FMK_LOGE("5D or more tensor cannot run on non-ND rom.");
        return stamp;
    }

    std::vector<HIAI_TensorBuffer*> inputTensorBuffs;
    std::vector<HIAI_TensorBuffer*> outputTensorBuffs;
    Status inputStatus = GetTensorBuffers(buffers.input, buffers.inputNum, inputTensorBuffs);
    Status outputStatus = GetTensorBuffers(buffers.output, buffers.outputNum, outputTensorBuffs);
    if (inputStatus != SUCCESS || outputStatus != SUCCESS) {
        FMK_LOGE("inputs or output is invalid.");
        return stamp;
    }

    return ((int (*)(HIAI_ModelManager*, HIAI_TensorBuffer*[], int, HIAI_TensorBuffer*[], int, int, const char*))func)(
        currentModelUtil_->GetModelManager(), inputTensorBuffs.data(), buffers.inputNum, outputTensorBuffs.data(),
        buffers.outputNum, timeout, SharedManagerInfos_->modelName.c_str());
}

Status DirectModelManagerImpl::Process(HIAI_NDTensorBuffers& buffers, int32_t timeoutInMS, int& stamp)
{
    void* runModelV3Func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runModel_v3");
    void* runModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runModel");

    // 构造输入输出 buffer. 先检查是否是支持ND的ROM，优先使用ND接口
    if (runModelV3Func != nullptr) {
        stamp = NDTensorProcess(buffers, timeoutInMS, runModelV3Func);
    } else if (runModelFunc != nullptr) {
        stamp = TensorProcess(buffers, timeoutInMS, runModelFunc);
    } else {
        FMK_LOGE("GetSymbol failed.");
        return FAILURE;
    }
    return SUCCESS;
}

Status DirectModelManagerImpl::Run(HIAI_NDTensorBuffers& buffers)
{
    std::lock_guard<std::mutex> lock(g_mapLock);
    int stamp = -1;

    Status result = Process(buffers, 0, stamp);
    if (stamp < 0 || result != SUCCESS) {
        FMK_LOGE("Model process failed.");
        return FAILURE;
    }

    FMK_LOGI("Model run success.");
    return SUCCESS;
}

Status DirectModelManagerImpl::RunAsync(HIAI_NDTensorBuffers& buffers, int32_t timeoutInMS, void* userData)
{
    std::lock_guard<std::mutex> lock(g_mapLock);
    int stamp = -1;

    Status result = Process(buffers, timeoutInMS, stamp);
    if (stamp < 0 || result != SUCCESS) {
        FMK_LOGE("Model process failed.");
        return FAILURE;
    }

    if (listener_ != nullptr) {
        DirectModelCallBack cb;
        cb.modelMgr = this;
        cb.userData = userData;
        cb.output = buffers.output;
        cb.outputNum = buffers.outputNum;
        g_callBackMap[stamp] = cb;
    }
    return SUCCESS;
}

#ifdef AI_SUPPORT_AIPP_API
static Status GetTensorAippParas(
    HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, std::vector<HIAI_TensorAippPara*>& aippParas)
{
    for (int32_t i = 0; i < aippParaNum; i++) {
        auto aipp = static_cast<HIAI_TensorAippPara*>(HIAI_TensorAippPara_GetHandle(aippPara[i]));
        if (aipp == nullptr) {
            FMK_LOGE("handle is null");
            return FAILURE;
        }
        aippParas.push_back(aipp);
    }
    return SUCCESS;
}

int DirectModelManagerImpl::NDTensorAippProcess(
    HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, int32_t timeout, void* func)
{
    int stamp = -1;
    std::vector<HIAI_NDTensorBuffer*> inputNDTensorBuffs;
    std::vector<HIAI_NDTensorBuffer*> outputNDTensorBuffs;
    std::vector<HIAI_TensorAippPara*> aippParas;
    Status inputStatus = GetNDTensorBuffers(buffers.input, buffers.inputNum, inputNDTensorBuffs);
    Status outputStatus = GetNDTensorBuffers(buffers.output, buffers.outputNum, outputNDTensorBuffs);
    if (inputStatus != SUCCESS || outputStatus != SUCCESS) {
        FMK_LOGE("inputs or output is invalid.");
        return stamp;
    }

    Status status = GetTensorAippParas(aippPara, aippParaNum, aippParas);
    if (status != SUCCESS) {
        FMK_LOGE("aippPara is invalid.");
        return stamp;
    }

    return ((int (*)(HIAI_ModelManager*, HIAI_NDTensorBuffer*[], int, HIAI_TensorAippPara*[], int,
        HIAI_NDTensorBuffer*[], int, int, const char*))func)(currentModelUtil_->GetModelManager(),
        inputNDTensorBuffs.data(), buffers.inputNum, aippParas.data(), aippParaNum, outputNDTensorBuffs.data(),
        buffers.outputNum, timeout, SharedManagerInfos_->modelName.c_str());
}

int DirectModelManagerImpl::TensorAippProcess(
    HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[], int32_t aippParaNum, int32_t timeout, void* func)
{
    int stamp = -1;
    if (IsTensorDimsLargerThan4D(buffers.input, buffers.inputNum) ||
        IsTensorDimsLargerThan4D(buffers.output, buffers.outputNum)) {
        FMK_LOGE("5D or more tensor cannot run on non-ND rom.");
        return stamp;
    }

    std::vector<HIAI_TensorBuffer*> inputTensorBuffs;
    std::vector<HIAI_TensorBuffer*> outputTensorBuffs;
    std::vector<HIAI_TensorAippPara*> aippParas;
    Status inputStatus = GetTensorBuffers(buffers.input, buffers.inputNum, inputTensorBuffs);
    Status outputStatus = GetTensorBuffers(buffers.output, buffers.outputNum, outputTensorBuffs);
    if (inputStatus != SUCCESS || outputStatus != SUCCESS) {
        FMK_LOGE("inputs or output is invalid.");
        return stamp;
    }

    Status status = GetTensorAippParas(aippPara, aippParaNum, aippParas);
    if (status != SUCCESS) {
        FMK_LOGE("aippPara is invalid.");
        return stamp;
    }

    return ((int (*)(HIAI_ModelManager*, HIAI_TensorBuffer*[], int, HIAI_TensorAippPara*[], int, HIAI_TensorBuffer*[],
        int, int, const char* modelName))func)(currentModelUtil_->GetModelManager(), inputTensorBuffs.data(),
        buffers.inputNum, aippParas.data(), aippParaNum, outputTensorBuffs.data(), buffers.outputNum, timeout,
        SharedManagerInfos_->modelName.c_str());
}

Status DirectModelManagerImpl::RunAipp(HIAI_NDTensorBuffers& buffers, HIAI_TensorAippPara* aippPara[],
    int32_t aippParaNum, int32_t timeoutInMS, void* userData)
{
    void* runModelV3Func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runAippModel_v3");
    void* runModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runAippModel");

    std::lock_guard<std::mutex> lock(g_mapLock);
    int stamp = -1;
    if (runModelV3Func != nullptr) {
        stamp = NDTensorAippProcess(buffers, aippPara, aippParaNum, timeoutInMS, runModelV3Func);
    } else if (runModelFunc != nullptr) {
        stamp = TensorAippProcess(buffers, aippPara, aippParaNum, timeoutInMS, runModelFunc);
    } else {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    if (stamp < 0) {
        FMK_LOGE("Model process failed with stamp %d", stamp);
        return FAILURE;
    }

    FMK_LOGI("RunAippModel stamp = %d", stamp);

    if (listener_ != nullptr) {
        DirectModelCallBack cb;
        cb.modelMgr = this;
        cb.userData = userData;
        cb.outputNum = buffers.outputNum;
        cb.output = buffers.output;
        g_callBackMap[stamp] = cb;
    }
    return SUCCESS;
}
#endif

void DirectModelManagerImpl::Cancel()
{
    void* cancelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_cancelCompute");
    if (cancelFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return;
    }
    ((void (*)(HIAI_ModelManager*, const char*))cancelFunc)(
        currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName.c_str());
}

void DirectModelManagerImpl::DeInit()
{
    int timeStamp = currentModelUtil_->UnLoadModel();
    if (listener_ != nullptr) {
        DirectModelCallBack cb;
        cb.modelMgr = this;
        {
            std::lock_guard<std::mutex> lock(g_mapLock);
            g_callBackMap[timeStamp] = cb;
        }
        std::unique_lock<std::mutex> ulock(syncStopmutex_);
        isUnloadCallBack_ = false;
        if (conditionUnLoad_.wait_for(ulock, std::chrono::seconds(10), [this]() { return this->isUnloadCallBack_; })) {
            FMK_LOGI("DirectModelManagerImpl::UnLoad Async CallBack Received");
        } else {
            FMK_LOGE("DirectModelManagerImpl::UnLoad Async CallBack Timeout");
        }
    }
    isLoaded_ = false;
}

void DirectModelManagerImpl::OnLoadDone(void* userdata, int taskStamp)
{
    (void)userdata;
    DirectModelManagerImpl* modelManager = nullptr;
    std::lock_guard<std::mutex> loadLock(g_callBackLock);
    std::lock_guard<std::mutex> lock_(g_mapLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("Unable t find load done callback listener matched taskstamp %d", taskStamp);
        return;
    }

    auto it = g_loadMap.find(taskStamp);
    if (it == g_loadMap.end()) {
        FMK_LOGE("Unable t find load done callback listener matched taskstamp %d", taskStamp);
        return;
    }

    modelManager = iter->second.modelMgr;
    if (modelManager == nullptr) {
        FMK_LOGE("onLoadDone failed, modelManager is nullptr");
        return;
    }

    FMK_LOGI("Call back to load completed notify message,taskstamp %d", taskStamp);

    // 异步调用通知
    std::lock_guard<std::mutex> lock(modelManager->syncRunMutex_);
    modelManager->isLoadCallBack_ = true;
    modelManager->isLoadStatus_ = true;
    modelManager->condition_.notify_all();
    g_callBackMap.erase(iter);
    g_loadMap.erase(it);
}

void DirectModelManagerImpl::OnError(void* userdata, int taskStamp, int errCode)
{
    (void)userdata;
    DirectModelCallBack stCb;
    DirectModelManagerImpl* modelManager = nullptr;

    std::lock_guard<std::mutex> lock(g_mapLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("Unable t find on error callback listener matched taskstamp %d, Error code %d", taskStamp, errCode);
        return;
    }

    stCb = iter->second;
    modelManager = stCb.modelMgr;
    if (modelManager == nullptr) {
        FMK_LOGE("onError failed, modelManager is nullptr");
        return;
    }
    if (modelManager->userListener_ == nullptr) {
        FMK_LOGE("onError failed, modelManager->userListener_ is nullptr");
        return;
    }

    if (g_loadMap.find(taskStamp) != g_loadMap.end()) {
        FMK_LOGI("Call back to load completed notify message,taskstamp %d", taskStamp);
        modelManager->isLoadCallBack_ = true;
        modelManager->isLoadStatus_ = false;
        modelManager->condition_.notify_all();
        g_callBackMap.erase(iter);
        g_loadMap.erase(g_loadMap.find(taskStamp));
        return;
    }

    FMK_LOGI("Call back to user's process done function");
    modelManager->userListener_->onRunDone(stCb.userData, errCode, stCb.output, stCb.outputNum);
    g_callBackMap.erase(iter);
}

void DirectModelManagerImpl::OnServiceDied(void* userdata)
{
    (void)userdata;
    DirectModelManagerImpl* modelManager = nullptr;
    std::lock_guard<std::mutex> lock(g_mapLock);
    std::map<int, DirectModelCallBack>::iterator iter;
    for (iter = g_callBackMap.begin(); iter != g_callBackMap.end(); iter++) {
        modelManager = iter->second.modelMgr;
        if (modelManager == nullptr) {
            FMK_LOGE("onServiceDied failed, modelManager is nullptr");
            return;
        }
        if (modelManager->userListener_ == nullptr) {
            FMK_LOGE("onServiceDied failed, modelManager->userListener_ is nullptr");
            return;
        }

        FMK_LOGI("Call back to user's service died function");
        modelManager->userListener_->onServiceDied(modelManager->userListener_->userData);
    }
    g_callBackMap.clear();
}

void DirectModelManagerImpl::OnRunDone(void* userdata, int taskStamp)
{
    (void)userdata;
    DirectModelCallBack stCb;
    DirectModelManagerImpl* modelManager = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_mapLock);
        auto iter = g_callBackMap.find(taskStamp);
        if (iter == g_callBackMap.end()) {
            FMK_LOGE("Unable t find run done callback listener matched taskstamp %d", taskStamp);
            return;
        }

        stCb = iter->second;
        g_callBackMap.erase(iter);
    }

    modelManager = stCb.modelMgr;
    if (modelManager == nullptr) {
        FMK_LOGE("onRunDone failed, modelManager is nullptr");
        return;
    }
    if (modelManager->userListener_ == nullptr) {
        FMK_LOGE("onRunDone failed, modelManager->userListener_ is nullptr");
        return;
    }

    FMK_LOGI("Call back to user's process done function");
    modelManager->userListener_->onRunDone(stCb.userData, 0, stCb.output, stCb.outputNum);
}

void DirectModelManagerImpl::OnUnloadDone(void* userdata, int taskStamp)
{
    (void)userdata;
    DirectModelManagerImpl* modelManager = nullptr;
    std::lock_guard<std::mutex> lock_(g_mapLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("Unable t find unload done callback listener matched taskstamp %d", taskStamp);
        return;
    }

    modelManager = iter->second.modelMgr;
    if (modelManager == nullptr) {
        FMK_LOGE("onUnloadDone failed, modelManager is nullptr");
        return;
    }

    FMK_LOGI("Call back to unload completed notify message,taskstamp %d", taskStamp);

    // 异步调用通知
    std::lock_guard<std::mutex> lock(modelManager->syncStopmutex_);
    modelManager->isUnloadCallBack_ = true;
    modelManager->conditionUnLoad_.notify_all();
    g_callBackMap.erase(iter);
}

void DirectModelManagerImpl::OnTimeout(void* userdata, int taskStamp)
{
    (void)userdata;
    (void)taskStamp;
}

Status DirectModelManagerImpl::AsyncWaitResult(int iTStamp)
{
    std::unique_lock<std::mutex> ulock(syncRunMutex_);
    isLoadCallBack_ = false;
    isLoadStatus_ = false;
    if (!condition_.wait_for(ulock, std::chrono::seconds(10), [this]() { return this->isLoadCallBack_; })) {
        FMK_LOGE("DirectModelManagerImpl::Load Async CallBack Timeout, condition_.wait_for failed");
        return FAILURE;
    }
    if (isLoadStatus_) {
        return SUCCESS;
    }
    return FAILURE;
}

void DirectModelManagerImpl::ClearCallbckcb()
{
    std::lock_guard<std::mutex> lock(g_mapLock);
    for (auto iter = g_callBackMap.begin(); iter != g_callBackMap.end();) {
        if (iter->second.modelMgr == this) {
            g_callBackMap.erase(iter++);
        } else {
            iter++;
        }
    }
    for (auto iter = g_loadMap.begin(); iter != g_loadMap.end();) {
        if (iter->second.modelMgr == this) {
            g_loadMap.erase(iter++);
        } else {
            iter++;
        }
    }
}
} // namespace hiai