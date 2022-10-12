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

#ifndef MODEL_TYPE_UTIL_H
#define MODEL_TYPE_UTIL_H
#include "framework/model/base_buffer.h"
#include "framework/common/types.h"
#include "framework/common/hcs_types.h"
#include "framework/ge_error_code.h"

namespace hiai {
class HCS_API_EXPORT ModelTypeUtil {
public:
    static ge::Status GetModelType(const ge::BaseBuffer& buffer, hiai::ModelType& type);
    static ge::Status GetModelTypeFromFile(const std::string& modelPath, hiai::ModelType& type);
#if defined (AI_SUPPORT_SPECIAL_3RD_MODEL) || defined(PLATFORM_UNIV100)
    static ge::Status IsSpecial3rdModel(const ge::BaseBuffer& buff, bool& isSpecial);
    static ge::Status IsSpecial3rdModelFromFile(const std::string& modelPath, bool& isSpecial);
#endif
};
} // namespace hiai
#endif // MODEL_TYPE_UTIL_H
