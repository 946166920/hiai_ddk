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
