/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: ge_error_codes
 */

#ifndef GE_ERROR_CODES_H
#define GE_ERROR_CODES_H

#include <cstdint>
namespace ge {
using GraphErrCodeStatus = uint32_t;
constexpr GraphErrCodeStatus GRAPH_FAILED = 1;
constexpr GraphErrCodeStatus GRAPH_SUCCESS = 0;
constexpr GraphErrCodeStatus GRAPH_PARAM_INVALID = 3;
} // namespace ge

#endif // GE_ERROR_CODES_H
