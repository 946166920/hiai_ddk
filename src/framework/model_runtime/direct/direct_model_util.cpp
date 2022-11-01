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
#include "direct_model_util.h"

#include "framework/infra/log/log.h"
#include "direct_common_util.h"
#include "util/hiai_foundation_dl_helper.h"

namespace hiai {
DirectModelUtil::~DirectModelUtil()
{
    if (isLoaded_) {
        (void)UnLoad();
    }
    if (manager_ != nullptr) {
        (void)DestroyModelManager();
    }
}

void DirectModelUtil::CreateModelManager(HIAI_ModelManagerListenerLegacy* listener)
{
    if (manager_ != nullptr) {
        FMK_LOGE("please release modelmanager first.");
        return;
    }

    void* createModelManagerFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_create");
    if (createModelManagerFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return;
    }

    listener_ = listener;
    manager_ = ((HIAI_ModelManager* (*)(HIAI_ModelManagerListenerLegacy*)) createModelManagerFunc)(listener_);
    if (manager_ == nullptr) {
        FMK_LOGE("HIAI_ModelManager_create failed");
    }
}

void DirectModelUtil::DestroyModelManager()
{
    if (manager_ != nullptr) {
        void* destoryModelManagerFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_destroy");
        if (destoryModelManagerFunc == nullptr) {
            FMK_LOGE("GetSymbol failed");
            return;
        }
        ((void (*)(HIAI_ModelManager*))destoryModelManagerFunc)(manager_);
        manager_ = nullptr;
    }
}

HIAI_ModelManager* DirectModelUtil::GetModelManager()
{
    return manager_;
}

bool DirectModelUtil::CheckCanReuse(HIAI_PerfMode mode, HIAI_ModelManagerListenerLegacy* listener, bool isNeedSame)
{
    if (isLoaded_ && (!isNeedSame || (mode == perfMode_ && listener == listener_))) {
        loadCount_++;
        return true;
    }
    return false;
}

static int Load(HIAI_ModelManager* manager, std::string& modelName, void* data, int size, HIAI_DevPerf freq)
{
    int ret = -1;
    void* loadModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_loadFromModelBuffers");
    void* creatModelBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_create_from_buffer");
    void* freeModelBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_destroy");
    if (loadModelFunc == nullptr || creatModelBufferFunc == nullptr || freeModelBufferFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return ret;
    }

    HIAI_ModelBuffer* buffer = ((HIAI_ModelBuffer* (*)(const char*, void*, int, HIAI_DevPerf)) creatModelBufferFunc)(
        modelName.c_str(), data, size, freq);
    if (buffer == nullptr) {
        FMK_LOGE("create buffer failed");
        return ret;
    }

    std::vector<HIAI_ModelBuffer*> bufferArray;
    bufferArray.push_back(buffer);

    ret = ((int (*)(HIAI_ModelManager*, HIAI_ModelBuffer*[], int))loadModelFunc)(
        manager, bufferArray.data(), bufferArray.size());
    ((void (*)(HIAI_ModelBuffer*))freeModelBufferFunc)(buffer);
    return ret;
}

int DirectModelUtil::LoadModel(std::shared_ptr<BaseBuffer>& modelBuffer, std::string& modelName, HIAI_PerfMode mode)
{
    int ret = -1;
    if (modelBuffer == nullptr || modelBuffer->MutableData() == nullptr || modelBuffer->GetSize() == 0) {
        FMK_LOGE("please restore model.");
        return ret;
    }

    if (manager_ == nullptr) {
        FMK_LOGE("please execute CreateModelManager first.");
        return ret;
    }

    perfMode_ = mode;
    ret = Load(
        manager_, modelName, modelBuffer->MutableData(), modelBuffer->GetSize(), static_cast<HIAI_DevPerf>(perfMode_));
    if (ret >= 0) {
        FMK_LOGI("Load model success.");
        isLoaded_ = true;
        loadCount_++;
    }
    return ret;
}

int DirectModelUtil::LoadModel(std::string& modelFile, std::string& modelName, HIAI_PerfMode mode)
{
    int ret = -1;
    if (manager_ == nullptr) {
        FMK_LOGE("please execute CreateModelManager first.");
        return ret;
    }

    perfMode_ = mode;

    void* loadModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_loadFromModelBuffers");
    void* creatModelBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_create_from_file");
    void* freeModelBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_destroy");
    if (loadModelFunc == nullptr || creatModelBufferFunc == nullptr || freeModelBufferFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return ret;
    }

    HIAI_ModelBuffer* buffer = ((HIAI_ModelBuffer* (*)(const char*, const char*, HIAI_DevPerf)) creatModelBufferFunc)(
        modelName.c_str(), modelFile.c_str(), static_cast<HIAI_DevPerf>(perfMode_));
    if (buffer == nullptr) {
        FMK_LOGE("create buffer failed");
        return ret;
    }

    std::vector<HIAI_ModelBuffer*> bufferArray;
    bufferArray.push_back(buffer);

    ret = ((int (*)(HIAI_ModelManager*, HIAI_ModelBuffer*[], int))loadModelFunc)(
        manager_, bufferArray.data(), bufferArray.size());
    ((void (*)(HIAI_ModelBuffer*))freeModelBufferFunc)(buffer);

    if (ret >= 0) {
        FMK_LOGI("Load model success.");
        isLoaded_ = true;
        loadCount_++;
    }
    return ret;
}

int DirectModelUtil::UnLoad()
{
    void* unloadModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_unloadModel");
    if (unloadModelFunc == nullptr) {
        FMK_LOGE("GetSymbol failed, unload model failed");
        return -1;
    }

    int ret = ((int (*)(HIAI_ModelManager*))unloadModelFunc)(manager_);
    if (ret >= 0) {
        FMK_LOGI("unload model success");
        isLoaded_ = false;
    } else {
        FMK_LOGE("unload model failed");
    }

    return ret;
}

int DirectModelUtil::UnLoadModel()
{
    if (manager_ == nullptr || !isLoaded_) {
        return -1;
    }

    if (listener_ != nullptr) {
        loadCount_ = 0;
        return UnLoad();
    }

    loadCount_--;
    if (loadCount_ == 0) {
        return UnLoad();
    }
    return 0;
}
} // namespace hiai