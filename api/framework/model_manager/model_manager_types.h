/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model manager types
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
#ifdef AI_SUPPORT_PREFMODE_EXTEND
    HIGH_ONLY_NPU = 103
#endif
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

inline void Context::SetValue(const std::string& key, const std::string& value) // lint !e1762
{
    paras_[key] = value;
}

inline void Context::RemoveValue(const std::string& key) // lint !e1762
{
    paras_.erase(key);
}

inline const std::map<std::string, std::string>& Context::GetContent() const
{
    return paras_;
}
} // namespace hiai
#endif // HIAI_API_MODEL_MANAGER_TYPES_H
