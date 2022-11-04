/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: common fmk error
 */

/*lint -e* */
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
