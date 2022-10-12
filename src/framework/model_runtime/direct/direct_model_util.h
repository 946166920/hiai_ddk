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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_UTIL_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_UTIL_H

#include <string>
#include <vector>
#include <memory>
#include "framework/c/compatible/HIAIModelManager.h"
#include "framework/util/base_buffer.h"

namespace hiai {
class DirectModelUtil {
public:
    DirectModelUtil() = default;
    ~DirectModelUtil();

    void CreateModelManager(HIAI_ModelManagerListenerLegacy* listener);
    void DestroyModelManager();

    int LoadModel(std::shared_ptr<BaseBuffer>& modelBuffer, std::string& modelName, HIAI_PerfMode mode);
    int LoadModel(std::string& modelFile, std::string& modelName, HIAI_PerfMode mode);
    int UnLoadModel();

    HIAI_ModelManager* GetModelManager();
    bool CheckCanReuse(HIAI_PerfMode mode, HIAI_ModelManagerListenerLegacy* listener, bool isNeedSame);

private:
    int UnLoad();

public:
    HIAI_PerfMode perfMode_ {HIAI_PerfMode::HIAI_PERFMODE_NORMAL};
    bool isLoaded_ {false};
    HIAI_ModelManager* manager_ {nullptr};
    HIAI_ModelManagerListenerLegacy* listener_ {nullptr};
    uint32_t loadCount_ {0}; // 加载次数
};

struct SharedManagerInfos {
    std::vector<std::shared_ptr<DirectModelUtil>> managers;
    std::shared_ptr<BaseBuffer> modelBuffer {nullptr};
    std::string modelName {""};
    std::string modelFile {""};
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_MODEL_UTIL_H