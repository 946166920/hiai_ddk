/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: AI Context
 *
 */
#include "compatible/HiAiModelManagerService.h"

#include <string>
#include <vector>
#include <map>

#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {
std::string AiContext::GetPara(const std::string& key) const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    auto iter = paras_.find(key);
    return (iter == paras_.end()) ? "" : iter->second;
}

void AiContext::AddPara(const std::string& key, const std::string& value)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    paras_.insert(std::pair<std::string, std::string>(key, value));
}

void AiContext::SetPara(const std::string& key, const std::string& value)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    paras_[key] = value;
}

void AiContext::DelPara(const std::string& key)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    paras_.erase(key);
}

void AiContext::ClearPara()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    paras_.clear();
}

AIStatus AiContext::GetAllKeys(std::vector<std::string>& keys)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (paras_.empty()) {
        FMK_LOGE("AiContext GetAllKeys failed, paras_ is empty");
        return AI_INVALID_PARA;
    }

    for (auto iter = paras_.begin(); iter != paras_.end(); iter++) {
        keys.push_back(iter->first);
    }

    return AI_SUCCESS;
}
} // namespace hiai
