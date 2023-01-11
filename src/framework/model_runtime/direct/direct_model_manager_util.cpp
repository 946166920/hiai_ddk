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
#include "direct_model_manager_util.h"

#include <vector>
#include <functional>

// inc
#include "framework/c/compatible/hiai_model_buffer.h"
#include "framework/infra/log/log.h"
#include "infra/base/assertion.h"

// src/framework
#include "util/hiai_foundation_dl_helper.h"

namespace hiai {

ModelLoadInfo::ModelLoadInfo(const std::string& name, const std::string& file, const std::shared_ptr<BaseBuffer>& buf,
    HIAI_PerfMode mode) : modelName(name), modelFile(file), modelBuffer(buf), perf(mode)
{
}

HIAI_ModelManager* DirectModelManagerUtil::CreateModelManager(
    const HIAI_ModelManagerListenerLegacy* listener)
{
    void* createManagerFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_create");
    HIAI_EXPECT_NOT_NULL_R(createManagerFunc, nullptr);

    return ((HIAI_ModelManager* (*)(HIAI_ModelManagerListenerLegacy*)) createManagerFunc)(
                                    const_cast<HIAI_ModelManagerListenerLegacy*>(listener));
}

void DirectModelManagerUtil::DestroyModelManager(HIAI_ModelManager** manager)
{
    if (manager != nullptr && *manager != nullptr) {
        void* destoryFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_destroy");
        HIAI_EXPECT_NOT_NULL_VOID(destoryFunc);

        ((void (*)(HIAI_ModelManager*))destoryFunc)(*manager);
        *manager = nullptr;
    }
}

namespace {
inline bool IsModelBufferValid(const std::shared_ptr<BaseBuffer>& modelBuffer)
{
    return (modelBuffer != nullptr) && (modelBuffer->GetData() != nullptr) &&
                  (modelBuffer->GetSize() > 0);
}

using FreeBufferFunc = std::function<void(HIAI_ModelBuffer*)>;
std::unique_ptr<HIAI_ModelBuffer, FreeBufferFunc> CreateModelBuffer(const ModelLoadInfo& loadInfo)
{
    HIAI_ModelBuffer* buffer = nullptr;
    if (IsModelBufferValid(loadInfo.modelBuffer)) {
        void* creatBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_create_from_buffer");
        HIAI_EXPECT_NOT_NULL_R(creatBufferFunc, nullptr);
        buffer = ((HIAI_ModelBuffer* (*)(const char*, void*, int, HIAI_DevPerf)) creatBufferFunc)(
                loadInfo.modelName.c_str(), loadInfo.modelBuffer->MutableData(), loadInfo.modelBuffer->GetSize(),
                static_cast<HIAI_DevPerf>(loadInfo.perf));
    } else {
        void* creatBufferFunc = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_create_from_file");
        HIAI_EXPECT_NOT_NULL_R(creatBufferFunc, nullptr);
        buffer = ((HIAI_ModelBuffer* (*)(const char*, const char*, HIAI_DevPerf)) creatBufferFunc)(
                loadInfo.modelName.c_str(), loadInfo.modelFile.c_str(), static_cast<HIAI_DevPerf>(loadInfo.perf));
    }

    return std::unique_ptr<HIAI_ModelBuffer, FreeBufferFunc>(buffer, [](HIAI_ModelBuffer* p) {
                void* func = HIAI_Foundation_GetSymbol("HIAI_ModelBuffer_destroy");
                HIAI_EXPECT_NOT_NULL_VOID(func);
                ((void (*)(HIAI_ModelBuffer*))func)(p);
            });
}
} // namespace

int DirectModelManagerUtil::LoadModel(HIAI_ModelManager* manager, const ModelLoadInfo& loadInfo)
{
    int timeStamp = -1;
    HIAI_EXPECT_NOT_NULL_R(manager, timeStamp);

    void* loadModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_loadFromModelBuffers");
    HIAI_EXPECT_NOT_NULL_R(loadModelFunc, timeStamp);

    std::unique_ptr<HIAI_ModelBuffer, FreeBufferFunc> buffer = CreateModelBuffer(loadInfo);
    HIAI_EXPECT_NOT_NULL_R(buffer, timeStamp);

    std::vector<HIAI_ModelBuffer*> bufferArray{buffer.get()};
    timeStamp = ((int (*)(HIAI_ModelManager*, HIAI_ModelBuffer*[], int))loadModelFunc)(
                        manager, bufferArray.data(), bufferArray.size());
    if (timeStamp < 0) {
        FMK_LOGE("load failed, modelName:%s ", loadInfo.modelName.c_str());
    } else {
        FMK_LOGI("load success, modelName:%s ", loadInfo.modelName.c_str());
    }
    return timeStamp;
}

int DirectModelManagerUtil::UnLoadModel(HIAI_ModelManager* manager)
{
    HIAI_EXPECT_NOT_NULL_R(manager, -1);

    void* unloadModelFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_unloadModel");
    HIAI_EXPECT_NOT_NULL_R(unloadModelFunc, -1);

    int timeStamp = ((int (*)(HIAI_ModelManager*))unloadModelFunc)(manager);
    if (timeStamp < 0) {
        FMK_LOGE("unload model failed");
    } else {
        FMK_LOGI("unload model success");
    }
    return timeStamp;
}

} // namespace hiai