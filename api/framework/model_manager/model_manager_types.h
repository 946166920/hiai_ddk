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
#ifndef HIAI_API_MODEL_MANAGER_TYPES_H
#define HIAI_API_MODEL_MANAGER_TYPES_H

#include <string>
#include <map>
#include "model_builder/model_builder_types.h"

namespace hiai {
enum ModelPriority {
#ifdef AI_SUPPORT_PRIORITY_EXTEND
    PRIORITY_HIGH_PLATFORM1 = 1,
    PRIORITY_HIGH_PLATFORM2,
    PRIORITY_HIGHEST,
    PRIORITY_HIGHER,
#endif
    PRIORITY_HIGH = 5,
    PRIORITY_MIDDLE,
    PRIORITY_LOW,
};

enum class PerfMode {
    UNSET = 0,
    LOW = 1,
    MIDDLE,
    HIGH,
    EXTREME_HIGH,
    HIGH_COMPUTE_UNIT = 103,
};

struct ModelInitOptions {
    PerfMode perfMode = PerfMode::MIDDLE;
    ModelBuildOptions buildOptions;
};

class Context {
public:
    std::string GetValue(const std::string& key) const;
    void SetValue(const std::string& key, const std::string& value);
    void RemoveValue(const std::string& key);
    const std::map<std::string, std::string>& GetContent() const;

private:
    std::map<std::string, std::string> paras_;
};

inline std::string Context::GetValue(const std::string& key) const
{
    auto it = paras_.find(key);
    return it != paras_.end() ? it->second : "";
}

inline void Context::SetValue(const std::string& key, const std::string& value)
{
    paras_[key] = value;
}

inline void Context::RemoveValue(const std::string& key)
{
    paras_.erase(key);
}

inline const std::map<std::string, std::string>& Context::GetContent() const
{
    return paras_;
}
} // namespace hiai
#endif // HIAI_API_MODEL_MANAGER_TYPES_H
