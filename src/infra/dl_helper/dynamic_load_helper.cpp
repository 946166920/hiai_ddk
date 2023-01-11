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
#include "infra/base/dynamic_load_helper.h"
#include <dlfcn.h>
#include <linux/limits.h>
#include "securec.h"

#include "infra/log/ai_log.h"

namespace hiai {
namespace {
const char* const INFRA_MODULE_NAME = "INFRA";
}

DynamicLoadHelper::DynamicLoadHelper()
{
}

DynamicLoadHelper::~DynamicLoadHelper()
{
    Deinit();
    handle_ = nullptr;
}

bool DynamicLoadHelper::Init(const std::string& file, bool isNeedClose)
{
    std::lock_guard<std::mutex> lock(funcMapMutex_);
    if (handle_ != nullptr) {
        AI_LOGE(INFRA_MODULE_NAME, "alread loaded.");
        return false;
    }

    if (file.empty()) {
        AI_LOGE(INFRA_MODULE_NAME, "empty path");
        return false;
    }

    char path[PATH_MAX] = {0x00};
    if (file.find("/") == std::string::npos || file.find("\\") == std::string::npos) {
        if (strcpy_s(path, PATH_MAX, file.c_str()) != EOK) {
            AI_LOGE(INFRA_MODULE_NAME, "invalid file");
            return false;
        }
    } else {
        if (realpath(file.c_str(), path) == nullptr) {
            AI_LOGE(INFRA_MODULE_NAME, "invalid file");
            return false;
        }
    }

    handle_ = dlopen(path, 2);
    if (handle_ == nullptr) {
        AI_LOGE(INFRA_MODULE_NAME, "load lib failed,errmsg [%s]", dlerror());
        return false;
    }
    isNeedClose_ = isNeedClose;
    return true;
}

void DynamicLoadHelper::Deinit()
{
    std::lock_guard<std::mutex> lock(funcMapMutex_);
    if (handle_ == nullptr) {
        AI_LOGE(INFRA_MODULE_NAME, "file not loaded.");
        return;
    }
    if (isNeedClose_ && dlclose(handle_) != 0) {
        AI_LOGE(INFRA_MODULE_NAME, "dlclose failed.");
    }
    handle_ = nullptr;
}

bool DynamicLoadHelper::IsValid()
{
    return handle_ != nullptr;
}

void* DynamicLoadHelper::GetSymbol(const std::string& symbol)
{
    std::lock_guard<std::mutex> lock(funcMapMutex_);
    if (handle_ == nullptr) {
        AI_LOGE(INFRA_MODULE_NAME, "file not loaded.");
        return nullptr;
    }
    std::map<std::string, void*>::const_iterator it = funcMap_.find(symbol);
    if (it != funcMap_.cend()) {
        return it->second;
    }
    auto funcPtr = dlsym(handle_, symbol.c_str());
    if (funcPtr == nullptr) {
        AI_LOGE(INFRA_MODULE_NAME, "dlsym failed,errmsg [%s]", dlerror());
    }
    funcMap_[symbol] = funcPtr;
    return funcPtr;
}
} // namespace hiai
