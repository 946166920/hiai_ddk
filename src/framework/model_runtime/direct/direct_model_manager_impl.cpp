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

#include <vector>
#include <map>

// api/framework
#include "util/version_util.h"

// inc
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/c/compatible/hiai_tensor_aipp_para.h"
#include "framework/c/compatible/hiai_tensor_buffer.h"
#include "framework/infra/log/log.h"

// src/framework
#include "util/hiai_foundation_dl_helper.h"

#include "direct_model_manager_container.h"
#include "direct_common_util.h"

namespace hiai {
namespace {
struct DirectModelCallBack {
    DirectModelManagerImpl* modelMgr {nullptr};
    void* userData {nullptr};
    int32_t outputNum {};
    HIAI_MR_NDTensorBuffer** output {};
    bool isLoadOrUnload {false};
    bool hasResponsed {false};
    bool result {false};
};

std::mutex g_loadMutex;
std::mutex g_unloadMutex;
std::condition_variable g_loadCond;
std::condition_variable g_unLoadCond;
std::mutex g_callBackLock;
std::map<int, DirectModelCallBack> g_callBackMap;
} // namespace

DirectModelManagerImpl::~DirectModelManagerImpl()
{
    DeInit();

    std::lock_guard<std::mutex> lock_(g_callBackLock);
    for (auto iter = g_callBackMap.begin(); iter != g_callBackMap.end();) {
        if (iter->second.modelMgr == this) {
            g_callBackMap.erase(iter++);
        } else {
            iter++;
        }
    }
}

Status DirectModelManagerImpl::CreateLegacyListener(const HIAI_MR_ModelManagerListener* listener,
    HIAI_ModelManagerListenerLegacy*& cListener)
{
    if (listener != nullptr) {
        cListener = new (std::nothrow) HIAI_ModelManagerListenerLegacy();
        HIAI_EXPECT_NOT_NULL(cListener);

        cListener->onLoadDone = OnLoadDone;
        cListener->onRunDone = OnRunDone;
        cListener->onUnloadDone = OnUnloadDone;
        cListener->onTimeout = OnTimeout;
        cListener->onError = OnError;
        cListener->onServiceDied = OnServiceDied;
    }
    return SUCCESS;
}

void DirectModelManagerImpl::UnloadAsync(HIAI_ModelManager* manager)
{
    int timeStamp = -1;
    {
        std::lock_guard<std::mutex> cblock(g_callBackLock);
        timeStamp = DirectModelManagerUtil::UnLoadModel(manager);
        HIAI_EXPECT_TRUE_VOID(timeStamp >= 0);

        DirectModelCallBack cb;
        cb.modelMgr = this;
        cb.isLoadOrUnload = true;
        g_callBackMap[timeStamp] = cb;
    }
    std::unique_lock<std::mutex> ulock(g_unloadMutex);
    if (!g_unLoadCond.wait_for(ulock, std::chrono::seconds(10), [timeStamp]() {
        std::lock_guard<std::mutex> cblock(g_callBackLock);
        auto iter = g_callBackMap.find(timeStamp);
        if (iter != g_callBackMap.end()) {
            return g_callBackMap[timeStamp].hasResponsed;
        }
        return false;
    })) {
        FMK_LOGE("Direct UnLoad Async CallBack Timeout");
    }

    std::lock_guard<std::mutex> cblock(g_callBackLock);
    g_callBackMap.erase(timeStamp);
}

std::shared_ptr<HIAI_ModelManager> DirectModelManagerImpl::CreateLegacyManager(
    const HIAI_ModelManagerListenerLegacy* cListener)
{
    HIAI_ModelManager* cManager = DirectModelManagerUtil::CreateModelManager(cListener);
    HIAI_EXPECT_NOT_NULL_R(cManager, nullptr);
    return std::shared_ptr<HIAI_ModelManager>(cManager, [cListener, this](HIAI_ModelManager* mm) {
                if (cListener != nullptr) {
                    this->UnloadAsync(mm);
                } else {
                    (void)DirectModelManagerUtil::UnLoadModel(mm);
                }
                DirectModelManagerUtil::DestroyModelManager(&mm);

                delete cListener;
            });
}

namespace {
HIAI_PerfMode GetPerfMode(const HIAI_MR_ModelInitOptions& options)
{
    HIAI_PerfMode mode = HIAI_MR_ModelInitOptions_GetPerfMode(&options);
    if (mode == HIAI_PerfMode::HIAI_PERFMODE_UNSET) {
        return HIAI_PerfMode::HIAI_PERFMODE_NORMAL;
    }

    const char* romVersion = VersionUtil::GetVersion();
    HIAI_EXPECT_NOT_NULL_R(romVersion, HIAI_PerfMode::HIAI_PERFMODE_UNSET);

    const char* supportExtremeVersion = "100.500.010";
    if (strncmp(romVersion, supportExtremeVersion, strlen(supportExtremeVersion)) < 0) {
        mode = mode > HIAI_PerfMode::HIAI_PERFMODE_HIGH ? HIAI_PerfMode::HIAI_PERFMODE_HIGH : mode;
    }
    return mode;
}
} // namespace

Status DirectModelManagerImpl::InitAsync(HIAI_ModelManager* manager, const ModelLoadInfo& loadInfo)
{
    int taskStamp;
    {
        // ensure g_callBackMap update before use it
        std::lock_guard<std::mutex> cblock(g_callBackLock);
        taskStamp = DirectModelManagerUtil::LoadModel(manager, loadInfo);
        HIAI_EXPECT_TRUE(taskStamp >= 0);

        DirectModelCallBack cb;
        cb.modelMgr = this;
        cb.isLoadOrUnload = true;
        g_callBackMap[taskStamp] = cb;
    }

    std::unique_lock<std::mutex> loadlock(g_loadMutex);
    bool result {false};
    if (!g_loadCond.wait_for(loadlock, std::chrono::seconds(10), [taskStamp, &result]() {
        std::lock_guard<std::mutex> cblock(g_callBackLock);
        auto iter = g_callBackMap.find(taskStamp);
        if (iter != g_callBackMap.end() && iter->second.hasResponsed) {
            result = iter->second.result;
            return true;
        }
        return false;
    })) {
        FMK_LOGE("async load callBack timeout.");
    }

    std::lock_guard<std::mutex> cblock(g_callBackLock);
    g_callBackMap.erase(taskStamp);
    return result ? SUCCESS : FAILURE;
}

Status DirectModelManagerImpl::InitSync(HIAI_ModelManager* manager, const ModelLoadInfo& loadInfo)
{
    int taskStamp = DirectModelManagerUtil::LoadModel(manager, loadInfo);
    return taskStamp >= 0 ? SUCCESS : FAILURE;
}

Status DirectModelManagerImpl::Init(const DirectBuiltModelImpl& builtModel, const HIAI_MR_ModelInitOptions& options,
    const HIAI_MR_ModelManagerListener* listener)
{
    if (manager_ != nullptr) {
        FMK_LOGE("model has been loaded.");
        return FAILURE;
    }

    auto buildOpts = HIAI_MR_ModelInitOptions_GetBuildOptions((HIAI_MR_ModelInitOptions*)(&options));
    HIAI_EXPECT_TRUE(DirectCommonUtil::IsSupportBuildOptions(buildOpts));

    HIAI_ModelManagerListenerLegacy* cListener = nullptr;
    HIAI_EXPECT_EXEC(CreateLegacyListener(listener, cListener));

    std::unique_ptr<HIAI_ModelManagerListenerLegacy> listnerGuard(cListener);
    auto manager = CreateLegacyManager(listnerGuard.get());
    HIAI_EXPECT_NOT_NULL(manager);
    listnerGuard.release();

    ModelLoadInfo info(
        builtModel.GetModelName(), builtModel.GetModelFile(), builtModel.GetModelBuffer(), GetPerfMode(options));

    Status status = (listener == nullptr) ? InitSync(manager.get(), info) : InitAsync(manager.get(), info);
    if (status == SUCCESS) {
        modelName_ = info.modelName;
        manager_ = manager;
        userListener_ = listener;
        DirectModelManagerContainer::GetInstance().AddModelManager(&builtModel, manager_);
    }
    return status;
}

Status DirectModelManagerImpl::SetPriority(HIAI_ModelPriority priority)
{
    if (priority < HIAI_PRIORITY_HIGH || priority > HIAI_PRIORITY_LOW) {
        FMK_LOGE("priority is invalid.");
        return FAILURE;
    }
    HIAI_EXPECT_NOT_NULL(manager_);

    void* func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_setModelPriority");
    HIAI_EXPECT_NOT_NULL(func);

    int ret = ((int (*)(HIAI_ModelManager*, const char*, HIAI_ModelPriority))func)(
        manager_.get(), modelName_.c_str(), priority);
    if (ret != 0) {
        FMK_LOGE("set priority faied, model name: %s",  modelName_.c_str());
        return FAILURE;
    }

    return SUCCESS;
}

namespace {
template <typename T>
std::vector<T*> GetBufferHandleVec(HIAI_MR_NDTensorBuffer* tensor[], int32_t num)
{
    std::vector<T*> bufferVec;
    for (int32_t i = 0; i < num; i++) {
        auto buffer = static_cast<T*>(HIAI_MR_NDTensorBuffer_GetHandle(tensor[i]));
        HIAI_EXPECT_NOT_NULL_R(buffer, {});
        bufferVec.push_back(buffer);
    }
    return bufferVec;
}

template <typename T>
int Process(HIAI_ModelManager* manager, const std::string& modelName, HIAI_NDTensorBuffers& buffers,
            int32_t timeout, void* runFunc)
{
    int stamp = -1;
    std::vector<T*> inputVec = GetBufferHandleVec<T>(buffers.input, buffers.inputNum);
    HIAI_EXPECT_TRUE_R(inputVec.size() == static_cast<uint32_t>(buffers.inputNum), stamp);

    std::vector<T*> outputVec = GetBufferHandleVec<T>(buffers.output, buffers.outputNum);
    HIAI_EXPECT_TRUE_R(outputVec.size() == static_cast<uint32_t>(buffers.outputNum), stamp);

    return ((int (*)(HIAI_ModelManager*, T*[], int, T*[], int, int, const char*))runFunc)(
        manager, inputVec.data(), buffers.inputNum, outputVec.data(), buffers.outputNum, timeout, modelName.c_str());
}

bool HasTensorDimLargerThan4D(HIAI_MR_NDTensorBuffer* tensor[], int32_t num)
{
    for (int32_t i = 0; i < num; i++) {
        HIAI_NDTensorDesc* desc = HIAI_MR_NDTensorBuffer_GetNDTensorDesc(tensor[i]);
        size_t dimNum = HIAI_NDTensorDesc_GetDimNum(desc);
        if (dimNum > 4) {
            return true;
        }
    }
    return false;
}

int RunModel(HIAI_ModelManager* manager, const std::string& modelName, HIAI_NDTensorBuffers& buffers, int32_t timeout)
{
    int stamp = -1;
    // Check whether the ROM supports ND. The ND interface is preferred.
    void* runModelV3Func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runModel_v3");
    if (runModelV3Func != nullptr) {
        return Process<HIAI_NDTensorBuffers>(manager, modelName, buffers, timeout, runModelV3Func);
    }

    if (HasTensorDimLargerThan4D(buffers.input, buffers.inputNum) ||
        HasTensorDimLargerThan4D(buffers.output, buffers.outputNum)) {
        FMK_LOGE("5D or more tensor cannot run on non-ND rom.");
        return stamp;
    }

    void* runModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runModel");
    HIAI_EXPECT_NOT_NULL_R(runModelFunc, stamp);
    return Process<HIAI_TensorBuffer>(manager, modelName, buffers, timeout, runModelFunc);
}
} // namespace

Status DirectModelManagerImpl::Run(HIAI_NDTensorBuffers& buffers)
{
    HIAI_EXPECT_NOT_NULL(manager_);
    int stamp = RunModel(manager_.get(), modelName_, buffers, 0);
    HIAI_EXPECT_TRUE(stamp >= 0);
    return SUCCESS;
}

Status DirectModelManagerImpl::RunAsync(HIAI_NDTensorBuffers& buffers, int32_t timeout, void* userData)
{
    HIAI_EXPECT_NOT_NULL(manager_);
    HIAI_EXPECT_NOT_NULL(userListener_);

    std::lock_guard<std::mutex> cblock(g_callBackLock);
    int stamp = RunModel(manager_.get(), modelName_, buffers, timeout);
    HIAI_EXPECT_TRUE(stamp >= 0);

    DirectModelCallBack cb;
    cb.modelMgr = this;
    cb.userData = userData;
    cb.output = buffers.output;
    cb.outputNum = buffers.outputNum;
    g_callBackMap[stamp] = cb;

    return SUCCESS;
}

#ifdef AI_SUPPORT_AIPP_API
namespace {
Status GetTensorAippParas(
    HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, std::vector<HIAI_TensorAippPara*>& aippParas)
{
    for (int32_t i = 0; i < aippParaNum; i++) {
        auto aipp = static_cast<HIAI_TensorAippPara*>(HIAI_MR_TensorAippPara_GetHandle(aippPara[i]));
        HIAI_EXPECT_NOT_NULL(aipp);
        aippParas.push_back(aipp);
    }
    return SUCCESS;
}
} // namespace

template <typename T>
int DirectModelManagerImpl::ProcessAipp(HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[],
                                        int32_t aippParaNum, int32_t timeout, void* runfunc)
{
    int stamp = -1;
    HIAI_EXPECT_NOT_NULL_R(manager_, stamp);

    auto inputVec = GetBufferHandleVec<T>(buffers.input, buffers.inputNum);
    HIAI_EXPECT_TRUE_R(inputVec.size() == static_cast<uint32_t>(buffers.inputNum), stamp);

    auto outputVec = GetBufferHandleVec<T>(buffers.output, buffers.outputNum);
    HIAI_EXPECT_TRUE_R(outputVec.size() == static_cast<uint32_t>(buffers.outputNum), stamp);

    std::vector<HIAI_TensorAippPara*> aippParas;
    HIAI_EXPECT_EXEC_R(GetTensorAippParas(aippPara, aippParaNum, aippParas), stamp);

    return ((int (*)(HIAI_ModelManager*, T*[], int, HIAI_TensorAippPara*[], int, T*[], int, int, const char*))runfunc)(
            manager_.get(), inputVec.data(), buffers.inputNum, aippParas.data(), aippParaNum, outputVec.data(),
            buffers.outputNum, timeout, modelName_.c_str());
}

int DirectModelManagerImpl::RunAippModel(
    HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[], int32_t aippParaNum, int32_t timeoutInMS)
{
    int stamp = -1;
    void* runModelV3Func = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runAippModel_v3");
    if (runModelV3Func != nullptr) {
        return ProcessAipp<HIAI_NDTensorBuffer>(buffers, aippPara, aippParaNum, timeoutInMS, runModelV3Func);
    }

    if (HasTensorDimLargerThan4D(buffers.input, buffers.inputNum) ||
        HasTensorDimLargerThan4D(buffers.output, buffers.outputNum)) {
        FMK_LOGE("5D or more tensor cannot run on non-ND rom.");
        return stamp;
    }
    void* runModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_runAippModel");
    HIAI_EXPECT_NOT_NULL_R(runModelFunc, stamp);
    return ProcessAipp<HIAI_TensorBuffer>(buffers, aippPara, aippParaNum, timeoutInMS, runModelFunc);
}

Status DirectModelManagerImpl::RunAipp(HIAI_NDTensorBuffers& buffers, HIAI_MR_TensorAippPara* aippPara[],
    int32_t aippParaNum, int32_t timeoutInMS, void* userData)
{
    if (userListener_ == nullptr) {
        int stamp = RunAippModel(buffers, aippPara, aippParaNum, timeoutInMS);
        HIAI_EXPECT_TRUE(stamp >= 0);
    } else {
        std::lock_guard<std::mutex> cblock(g_callBackLock);
        int stamp = RunAippModel(buffers, aippPara, aippParaNum, timeoutInMS);
        HIAI_EXPECT_TRUE(stamp >= 0);

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
    HIAI_EXPECT_NOT_NULL_VOID(manager_);

    void* cancelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_cancelCompute");
    HIAI_EXPECT_NOT_NULL_VOID(cancelFunc);
    ((void (*)(HIAI_ModelManager*, const char*))cancelFunc)(manager_.get(), modelName_.c_str());
}

void DirectModelManagerImpl::DeInit()
{
    DirectModelManagerContainer::GetInstance().RemoveModelManager(manager_);
    manager_ = nullptr;
    userListener_ = nullptr;
    modelName_ = "";
}

void DirectModelManagerImpl::OnLoadDone(void* userdata, int taskStamp)
{
    (void)userdata;
    std::lock_guard<std::mutex> cblock(g_callBackLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("unable to find load callback, taskstamp:%d.", taskStamp);
        return;
    }
    iter->second.hasResponsed = true;
    iter->second.result = true;
    g_loadCond.notify_all();
}

void DirectModelManagerImpl::OnError(void* userdata, int taskStamp, int errCode)
{
    (void)userdata;
    std::lock_guard<std::mutex> cblock(g_callBackLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("taskstamp:%d can not match OnError callback, errCode:%d.", taskStamp, errCode);
        return;
    }
    /* error callback for async load */
    if (iter->second.isLoadOrUnload) {
        iter->second.hasResponsed = true;
        g_loadCond.notify_all();
    } else {
        /* error callback for async run */
        auto cb = iter->second;
        HIAI_EXPECT_NOT_NULL_VOID(cb.modelMgr);
        HIAI_EXPECT_NOT_NULL_VOID(cb.modelMgr->userListener_);

        cb.modelMgr->userListener_->onRunDone(cb.userData, errCode, cb.output, cb.outputNum);
        g_callBackMap.erase(iter);
    }
}

void DirectModelManagerImpl::OnServiceDied(void* userdata)
{
    (void)userdata;
    std::lock_guard<std::mutex> cblock(g_callBackLock);
    for (auto iter = g_callBackMap.begin(); iter != g_callBackMap.end(); iter++) {
        auto modelManager = iter->second.modelMgr;
        HIAI_EXPECT_NOT_NULL_VOID(modelManager);
        HIAI_EXPECT_NOT_NULL_VOID(modelManager->userListener_);

        modelManager->userListener_->onServiceDied(modelManager->userListener_->userData);
    }
    g_callBackMap.clear();
}

void DirectModelManagerImpl::OnRunDone(void* userdata, int taskStamp)
{
    (void)userdata;
    std::lock_guard<std::mutex> cblock(g_callBackLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("unable to find run-done callback, taskstamp:%d.", taskStamp);
        return;
    }
    auto cb = iter->second;
    HIAI_EXPECT_NOT_NULL_VOID(cb.modelMgr);
    HIAI_EXPECT_NOT_NULL_VOID(cb.modelMgr->userListener_);

    cb.modelMgr->userListener_->onRunDone(cb.userData, 0, cb.output, cb.outputNum);
    g_callBackMap.erase(iter);
}

void DirectModelManagerImpl::OnUnloadDone(void* userdata, int taskStamp)
{
    (void)userdata;
    std::lock_guard<std::mutex> cblock(g_callBackLock);
    auto iter = g_callBackMap.find(taskStamp);
    if (iter == g_callBackMap.end()) {
        FMK_LOGE("unable to find unload callback, taskstamp:%d.", taskStamp);
        return;
    }
    iter->second.hasResponsed = true;
    iter->second.result = true;
    g_unLoadCond.notify_all();
}

void DirectModelManagerImpl::OnTimeout(void* userdata, int taskStamp)
{
    (void)userdata;
    (void)taskStamp;
}
} // namespace hiai