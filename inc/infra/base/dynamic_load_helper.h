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

#ifndef INFRA_BASE_DYNAMIC_LOAD_HELPER_H
#define INFRA_BASE_DYNAMIC_LOAD_HELPER_H
#include <string>
#include <mutex>
#include <map>

#include "api_export.h"

namespace hiai {
class INFRA_API_EXPORT DynamicLoadHelper {
public:
    DynamicLoadHelper();
    ~DynamicLoadHelper();

    bool Init(const std::string& file);
    void Deinit();

    bool IsValid();

    template <typename T>
    T* GetFunction(const std::string& symbol)
    {
        auto funcPtr = GetSymbol(symbol);
        return reinterpret_cast<T*>(funcPtr);
    };

    void* GetSymbol(const std::string& symbol);

private:
    void* handle_;
    std::mutex funcMapMutex_;
    std::map<std::string, void*> funcMap_;
};
} // namespace hiai

#endif