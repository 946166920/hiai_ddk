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
#include "direct_model_manager_container.h"

// inc
#include "infra/base/assertion.h"
#include "framework/infra/log/log.h"

// src/framework
#include "util/hiai_foundation_dl_helper.h"

#include "direct_model_manager_util.h"

namespace hiai {

DirectModelManagerContainer& DirectModelManagerContainer::GetInstance()
{
    static DirectModelManagerContainer instance;
    return instance;
}

void DirectModelManagerContainer::AddModelManager(const void* builtModel, std::shared_ptr<HIAI_ModelManager>& manager)
{
    HIAI_EXPECT_NOT_NULL_VOID(builtModel);
    HIAI_EXPECT_NOT_NULL_VOID(manager);

    /* modelmanager is saved for model GET funciton, no need saved repeately base on BuildModel */
    std::lock_guard<std::mutex> lock(containerMutex_);
    cModelManagerMap_.insert(std::pair<const void*, std::shared_ptr<HIAI_ModelManager>>(builtModel, manager));
}

void DirectModelManagerContainer::RemoveModelManager(const std::shared_ptr<HIAI_ModelManager>& manager)
{
    HIAI_EXPECT_NOT_NULL_VOID(manager);

    std::lock_guard<std::mutex> lock(containerMutex_);
    for (auto it = cModelManagerMap_.begin(); it != cModelManagerMap_.end();) {
        if (it->second == manager) {
            cModelManagerMap_.erase(it++);
        } else {
            it++;
        }
    }
}

std::shared_ptr<HIAI_ModelManager> DirectModelManagerContainer::GetModelManager(const void* builtModel)
{
    std::lock_guard<std::mutex> lock(containerMutex_);
    std::map<const void*, std::shared_ptr<HIAI_ModelManager>>::iterator it = cModelManagerMap_.find(builtModel);
    if (it != cModelManagerMap_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace hiai