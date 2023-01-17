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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_DIRECT_MODEL_COMPATIBLE_PROXY_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_DIRECT_MODEL_COMPATIBLE_PROXY_H
#include "infra/alias/aicp.h"
#include "infra/base/dynamic_load_helper.h"
namespace aicp {
class DirectModelCompatibleProxy {
public:
    static void* GetSymbol(const std::string& symbol);

private:
    DirectModelCompatibleProxy();
    ~DirectModelCompatibleProxy();
    DirectModelCompatibleProxy(const DirectModelCompatibleProxy&) = delete;
    DirectModelCompatibleProxy& operator=(const DirectModelCompatibleProxy&) = delete;
    static DirectModelCompatibleProxy* GetInstance();

private:
    DynamicLoadHelper* dlHelper_;
};
} // namespace aicp
#endif // FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_DIRECT_MODEL_COMPATIBLE_PROXY_H
