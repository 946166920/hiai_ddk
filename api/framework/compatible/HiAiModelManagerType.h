/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: hiai model manager type define
 */
#ifndef FRAMEWOKR_UTIL_COMPATIBLE_HIAI_MODEL_MANAGER_TYPE_H
#define FRAMEWOKR_UTIL_COMPATIBLE_HIAI_MODEL_MANAGER_TYPE_H
#include <map>
#include <vector>

#include "compatible/hiai_base_types_cpt.h"
#include "compatible/AiTensor.h"
#include "compatible/AippTensor.h"
#include "compatible/MemBuffer.h"

namespace hiai {
class HIAI_MM_API_EXPORT AiContext {
public:
    std::string GetPara(const std::string& key) const;
    void AddPara(const std::string& key, const std::string& value);
    void SetPara(const std::string& key, const std::string& value);
    void DelPara(const std::string& key);
    void ClearPara();
    AIStatus GetAllKeys(std::vector<std::string>& keys);

private:
    std::map<std::string, std::string> paras_;
};
} // namespace hiai
#endif
