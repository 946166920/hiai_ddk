/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: error types
 */
#ifndef FRAMEWORK_ERROR_TYPES_H
#define FRAMEWORK_ERROR_TYPES_H

#include <stdint.h>

namespace hiai {
using Status = uint32_t;

constexpr Status SUCCESS = 0;
constexpr Status FAILURE = 1;
constexpr Status UNINITIALIZED = 2;
constexpr Status INVALID_PARAM = 3;
constexpr Status TIMEOUT = 4;
constexpr Status UNSUPPORTED = 5;
constexpr Status MEMORY_EXCEPTION = 6;
constexpr Status INVALID_API = 7;
constexpr Status INVALID_POINTER = 8;
constexpr Status CALC_EXCEPTION = 9;
constexpr Status FILE_NOT_EXIST = 10;
constexpr Status COMM_EXCEPTION = 11;
constexpr Status DATA_OVERFLOW = 12;
} // namespace hiai
#endif // FRAMEWORK_ERROR_TYPES_H
