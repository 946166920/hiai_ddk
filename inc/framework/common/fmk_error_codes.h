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

#ifndef DOMI_COMMON_FMK_ERROR_CODES_H_
#define DOMI_COMMON_FMK_ERROR_CODES_H_

#include "base/error_types.h"

namespace hiai {
// general error code
constexpr Status FAILED =  1;
constexpr Status PARAM_INVALID = 3;
constexpr Status RT_FAILED = 15;
constexpr Status INTERNAL_ERROR = 16;
constexpr Status PARSE_MODEL_FAILED = 17;
constexpr Status PARSE_WEIGHTS_FAILED = 18;
constexpr Status TO_BE_DELETED = 19;
constexpr Status NOT_CHANGED = 20;

} // namespace hiai
#endif // fmk_error_codes_h__
