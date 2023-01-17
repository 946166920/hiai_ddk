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
#include "direct_model_compatible_proxy.h"
#include "framework/infra/log/log.h"

namespace aicp {
static const char* g_compatibleLibName = "libhiai_model_compatible.so";

DirectModelCompatibleProxy* DirectModelCompatibleProxy::GetInstance()
{
    static DirectModelCompatibleProxy instance;
    return &instance;
}

DirectModelCompatibleProxy::DirectModelCompatibleProxy()
{
    dlHelper_ = new (std::nothrow) DynamicLoadHelper();
    if (dlHelper_ == nullptr) {
        FMK_LOGE("Create DynamicLoadHelper fail!");
        return;
    }
    if (!dlHelper_->Init(g_compatibleLibName)) {
        delete dlHelper_;
        dlHelper_ = nullptr;
        return;
    }
}

DirectModelCompatibleProxy::~DirectModelCompatibleProxy()
{
    if (dlHelper_ != nullptr) {
        dlHelper_->Deinit();
        delete dlHelper_;
        dlHelper_ = nullptr;
    }
}

void* DirectModelCompatibleProxy::GetSymbol(const std::string& symbol)
{
    if (GetInstance()->dlHelper_ == nullptr) {
        return nullptr;
    }

    return GetInstance()->dlHelper_->GetSymbol(symbol);
}
} // namespace aicp