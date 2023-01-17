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

#ifndef DOMI_OMG_COMMON_MATH_MATH_UTIL_H
#define DOMI_OMG_COMMON_MATH_MATH_UTIL_H

#include <cstdint>
#include <climits>
#include <cmath>

#include "framework/infra/log/log.h"
#include "framework/common/fmk_error_codes.h"


namespace hiai {
/**
 * @ingroup math_util
 * @brief check whether int32 addition can result in overflow
 * @param [in] a  addend
 * @param [in] b  addend
 * @return hiai::Status
 */
inline hiai::Status CheckIntAddOverflow(int a, int b)
{
    if (((b > 0) && (a > (INT_MAX - b))) || ((b < 0) && (a < (INT_MIN - b)))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int32 addition can result in overflow
 * @param [in] a  addend
 * @param [in] b  addend
 * @return hiai::Status
 */
inline hiai::Status CheckInt32AddOverflow(int32_t a, int32_t b)
{
    if (((b > 0) && (a > (INT32_MAX - b))) || ((b < 0) && (a < (INT32_MIN - b)))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether uint32 addition can result in overflow
 * @param [in] a  addend
 * @param [in] b  addend
 * @return hiai::Status
 */
inline hiai::Status CheckUint32AddOverflow(uint32_t a, uint32_t b)
{
    if (a > (UINT32_MAX - b)) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int subtraction can result in overflow
 * @param [in] a  subtrahend
 * @param [in] b  minuend
 * @return hiai::Status
 */
inline hiai::Status CheckIntSubOverflow(int a, int b)
{
    if (((b > 0) && (a < (INT_MIN + b))) || ((b < 0) && (a > (INT_MAX + b)))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int32 subtraction can result in overflow
 * @param [in] a  subtrahend
 * @param [in] b  minuend
 * @return hiai::Status
 */
inline hiai::Status CheckInt32SubOverflow(int32_t a, int32_t b)
{
    if (((b > 0) && (a < (INT32_MIN + b))) || ((b < 0) && (a > (INT32_MAX + b)))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int multiplication can result in overflow
 * @param [in] a  multiplicator
 * @param [in] b  multiplicator
 * @return hiai::Status
 */
inline hiai::Status CheckIntMulOverflow(int a, int b)
{
    if (a > 0) {
        if (b > 0) {
            if (a > (INT_MAX / b)) {
                return hiai::FAILED;
            }
        } else {
            if (b < (INT_MIN / a)) {
                return hiai::FAILED;
            }
        }
    } else {
        if (b > 0) {
            if (a < (INT_MIN / b)) {
                return hiai::FAILED;
            }
        } else {
            if ((a != 0) && (b < (INT_MAX / a))) {
                return hiai::FAILED;
            }
        }
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int32 multiplication can result in overflow
 * @param [in] a  multiplicator
 * @param [in] b  multiplicator
 * @return hiai::Status
 */
inline hiai::Status CheckInt32MulOverflow(int32_t a, int32_t b)
{
    if (a > 0) {
        if (b > 0) {
            if (a > (INT32_MAX / b)) {
                return hiai::FAILED;
            }
        } else {
            if (b < (INT32_MIN / a)) {
                return hiai::FAILED;
            }
        }
    } else {
        if (b > 0) {
            if (a < (INT32_MIN / b)) {
                return hiai::FAILED;
            }
        } else {
            if ((a != 0) && (b < (INT32_MAX / a))) {
                return hiai::FAILED;
            }
        }
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether uint32 multiplication can result in overflow
 * @param [in] a  multiplicator
 * @param [in] b  multiplicator
 * @return hiai::Status
 */
inline hiai::Status CheckUint32MulOverflow(uint32_t a, uint32_t b)
{
    if (a == 0 || b == 0) {
        return hiai::SUCCESS;
    }

    if (a > (UINT32_MAX / b)) {
        return hiai::FAILED;
    }

    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int division can result in overflow
 * @param [in] a  dividend
 * @param [in] b  divisor
 * @return hiai::Status
 */
inline hiai::Status CheckIntDivOverflow(int a, int b)
{
    if ((b == 0) || ((a == INT_MIN) && (b == -1))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

/**
 * @ingroup math_util
 * @brief check whether int32 division can result in overflow
 * @param [in] a  dividend
 * @param [in] b  divisor
 * @return hiai::Status
 */
inline hiai::Status CheckInt32DivOverflow(int32_t a, int32_t b)
{
    if ((b == 0) || ((a == INT32_MIN) && (b == -1))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

inline hiai::Status CheckInt64MulOverflow(int64_t a, int64_t b)
{
    if (a > 0) {
        if (b > 0) {
            if (a > (INT64_MAX / b)) {
                return hiai::FAILED;
            }
        } else {
            if (b < (INT64_MIN / a)) {
                return hiai::FAILED;
            }
        }
    } else {
        if (b > 0) {
            if (a < (INT64_MIN / b)) {
                return hiai::FAILED;
            }
        } else {
            if ((a != 0) && (b < (INT64_MAX / a))) {
                return hiai::FAILED;
            }
        }
    }
    return hiai::SUCCESS;
}

inline hiai::Status CheckInt64AddOverflow(int64_t a, int64_t b)
{
    if (((b > 0) && (a > (INT64_MAX - b))) || ((b < 0) && (a < (INT64_MIN - b)))) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

#define FMK_INT_ADDCHECK(a, b) \
    if (CheckIntAddOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("Int %d and %d addition can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT32_ADDCHECK(a, b) \
    if (CheckInt32AddOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("Int32 %d and %d addition can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_UINT32_ADDCHECK(a, b) \
    if (CheckUint32AddOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("UINT32 %d and %d addition can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT_SUBCHECK(a, b) \
    if (CheckIntSubOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT %d and %d subtraction can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT32_SUBCHECK(a, b) \
    if (CheckInt32SubOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT32 %d and %d subtraction can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT_MULCHECK(a, b) \
    if (CheckIntMulOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT %d and %d multiplication can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT32_MULCHECK(a, b) \
    if (CheckInt32MulOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT32 %d and %d multiplication can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_UINT32_MULCHECK(a, b) \
    if (CheckUint32MulOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("UINT32 %d and %d multiplication can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT_DIVCHECK(a, b) \
    if (CheckIntDivOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT %d and %d division can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

#define FMK_INT32_DIVCHECK(a, b) \
    if (CheckInt32DivOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("INT32 %d and %d division can result in overflow!", (a), (b)); \
        return hiai::INTERNAL_ERROR; \
    }

/*
 * 检查参数相乘是否越界
 */
#define FMK_INT64_MULCHECK(a, b) \
    if (CheckInt64MulOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("Integer %ld(%s) and %ld(%s) multiplication can result in overflow!", (a), #a, (b), #b); \
        return hiai::INTERNAL_ERROR; \
    }

/*
 * 检查参数相加是否越界
 */
#define FMK_INT64_ADDCHECK(a, b) \
    if (CheckInt64AddOverflow((a), (b)) != hiai::SUCCESS) { \
        FMK_LOGE("Integer %ld(%s) and %ld(%s) add can result in overflow!", (a), #a, (b), #b); \
        return hiai::INTERNAL_ERROR; \
    }

} // namespace hiai
#endif
