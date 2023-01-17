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
#ifndef FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_CONTAINER_H
#define FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_CONTAINER_H

#include <map>
#include <memory>
#include <mutex>

#include "framework/c/compatible/HIAIModelManager.h"

namespace hiai {

class DirectModelManagerContainer {
public:
    static DirectModelManagerContainer& GetInstance();

    void AddModelManager(const void* builtModel, std::shared_ptr<HIAI_ModelManager>& manager);
    void RemoveModelManager(const std::shared_ptr<HIAI_ModelManager>& manager);
    std::shared_ptr<HIAI_ModelManager> GetModelManager(const void* builtModel);

private:
    DirectModelManagerContainer() = default;
    DirectModelManagerContainer(const DirectModelManagerContainer& container);
    DirectModelManagerContainer& operator=(const DirectModelManagerContainer& container);
    ~DirectModelManagerContainer() = default;

private:
    std::mutex containerMutex_;
    std::map<const void*, std::shared_ptr<HIAI_ModelManager>> cModelManagerMap_;
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_RUNTIME_DIRECT_DIRECT_MODEL_MANAGER_CONTAINER_H