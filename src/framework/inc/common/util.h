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
#ifndef DOMI_OMG_COMMON_UTIL_H_
#define DOMI_OMG_COMMON_UTIL_H_

#include <vector>
#include <string>

#include "common/debug/log.h"
#include "common/scope_guard.h"
#include "common/file_util.h"
#include "infra/mmpa/mmpa_api.h"

// 封装常用的资源释放
#define DOMI_MAKE_GUARD_RTMEM(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_RT(rtFreeHost(var)); \
    })

#define DOMI_MAKE_GUARD_RTSTREAM(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_RT(rtStreamDestroy(var)); \
    })

#define DOMI_MAKE_GUARD_RTEVENT(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_RT(rtEventDestroy(var)); \
    })

#define DOMI_MAKE_GUARD_TENSOR(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_NPU_CORE(ccDestroyTensorDescriptor(&(var))); \
    })

#define DOMI_MAKE_GUARD_HANDLE(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_NPU_CORE(ccDestroy(&(var))); \
    })

#define DOMI_MAKE_GUARD_FILTER_DESC(var) \
    DOMI_MAKE_GUARD(var, [&] { \
        if (var) \
            DOMI_CHK_NPU_CORE(ccDestroyFilterDescriptor(&(var))); \
    })

// For propagating errors when calling a function.
#define DOMI_RETURN_IF_ERROR(expr) \
    do { \
        const ::hiai::Status _status = (expr); \
        if (_status != ::hiai::SUCCESS) \
            return _status; \
    } while (0)

#define DOMI_RETURN_WITH_LOG_IF_ERROR(expr, ...) \
    do { \
        const ::hiai::Status _status = (expr); \
        if (_status != ::hiai::SUCCESS) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

#define DOMI_RETURN_WITH_LOG_IF_INFO(expr, ...) \
    do { \
        const ::hiai::Status _status = (expr); \
        if (_status != ::hiai::SUCCESS) { \
            FMK_LOGI(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// 校验参数是否为true. 如果是，返回 hiai::FAILED，并记录error日志
#define DOMI_RETURN_WITH_LOG_IF_TRUE(condition, ...) \
    do { \
        if (condition) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::FAILED; \
        } \
    } while (0)

// 校验参数是否为false. 如果是，返回 hiai::FAILED，并记录error日志
#define DOMI_RETURN_WITH_LOG_IF_FALSE(condition, ...) \
    do { \
        bool _condition = (condition); \
        if (!_condition) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::FAILED; \
        } \
    } while (0)

// 校验参数是否为true. 如果是，返回 hiai::PARAM_INVALID，并记录error日志
#define DOMI_RT_PARAM_INVALID_WITH_LOG_IF_TRUE(condition, ...) \
    do { \
        if (condition) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验参数是否为false. 如果是，返回 hiai::PARAM_INVALID，并记录error日志
#define DOMI_RT_PARAM_INVALID_WITH_LOG_IF_FALSE(condition, ...) \
    do { \
        bool _condition = (condition); \
        if (!_condition) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，返回 hiai::PARAM_INVALID， 并记录error日志
#define DOMI_CHECK_NOTNULL(val) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，返回 指定status, 并记录error日志
#define DOMI_CHECK_NOTNULL_WITH_RETURN(val, status) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            return status; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，返回 hiai::PARAM_INVALID， 并记录error日志
#define DOMI_CHECK_NOTNULL_JUST_RETURN(val) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            return; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，执行exec_expr表达式， 并记录error日志
#define DOMI_CHECK_NOTNULL_EXEC(val, exec_expr) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            exec_expr; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，直接返回， 并记录error日志
#define DOMI_RT_VOID_CHECK_NOTNULL(val) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            return; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，返回false， 并记录error日志
#define DOMI_RT_FALSE_CHECK_NOTNULL(val) \
    do { \
        if (val == nullptr) { \
            FMK_LOGE(param[#val] must not be null.); \
            return false; \
        } \
    } while (0)

// 校验参数是否越界
#define DOMI_CHECK_SIZE(size) \
    do { \
        if (size == 0) { \
            FMK_LOGE(param[#size] is out of range); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验参数是否越界
#define DOMI_CHECK_SIZE_JUST_CONTINUE(size) \
    do { \
        if (size == 0) { \
            FMK_LOGE(param[#size] is out of range); \
            continue; \
        } \
    } while (0)

// 校验容器是否为空
#define DOMI_CHECK_VECTOR_NOT_EMPTY(vector) \
    do { \
        if (vector.empty()) { \
            FMK_LOGE(param[#vector] is empty !); \
            return hiai::FAILED; \
        } \
    } while (0)

// 校验容器是否为空,返回指定状态码
#define DOMI_CHECK_VECTOR_NOT_EMPTY_RET_STATUS(vector, _status) \
    do { \
        if (vector.empty()) { \
            FMK_LOGE(param[#vector] is empty !); \
            return _status; \
        } \
    } while (0)

// 校验容器是否为空，可自定义返回类型
#define DOMI_CHECK_VECTOR_NOT_EMPTY_RET_EXEC(vector, return_exec) \
    do { \
        if (vector.empty()) { \
            FMK_LOGE(param[#vector] is empty !); \
            return_exec; \
        } \
    } while (0)

/* 校验size是否为0或者越界
 * @param: size: 待校验的size
 */
#define DOMI_CHECK_SIZE_RANGE(size) \
    do { \
        if ((size) == 0 || (size) >= UINT_MAX / 4) { \
            FMK_LOGE(param[#size] is out of range); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

#define DOMI_CHECK_SHORT_SIZE_RANGE(size) \
    do { \
        if ((size) == 0 || (size) >= UINT_MAX / 2) { \
            FMK_LOGE(param[#size] is out of range); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

#define DOMI_CHECK_POSITIVE_SIZE_RANGE(size) \
    do { \
        if ((size) <= 0) { \
            FMK_LOGE(param[#size] is not a positive number); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

#define DOMI_CHECK_POSITIVE_SHORT_SIZE_RANGE(size) \
    do { \
        if ((size) <= 0 || (size) == 0 || (size) >= UINT_MAX / 4) { \
            FMK_LOGE(param[#size] is out of range); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验左边的值是否大于等于右边的值
#define DOMI_CHECK_GE(lhs, rhs) \
    do { \
        if ((lhs) < (rhs)) { \
            FMK_LOGE(param[#lhs] is less than[#rhs]); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验左边的值是否大于等于右边的值 指定返回值
#define DOMI_CHECK_GE_WITH_RETURN(lhs, rhs, status) \
    do { \
        if ((lhs) < (rhs)) { \
            FMK_LOGE(param[#lhs] is less than[#rhs]); \
            return status; \
        } \
    } while (0)

// 校验参数是否相等
#define DOMI_CHECK_EQ(val1, val2) \
    do { \
        if ((val1) != (val2)) { \
            FMK_LOGE(param[#val1] is not equals to[#val2]); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验左边的值是否小于等于右边的值
#define DOMI_CHECK_LE(lhs, rhs) \
    do { \
        if ((lhs) > (rhs)) { \
            FMK_LOGE(param[#lhs] is greater than[#rhs]); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 校验参数是否相等
#define DOMI_CHECK_EQ_WITH_LOG(val1, val2, ...) \
    do { \
        if ((val1) != (val2)) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::PARAM_INVALID; \
        } \
    } while (0)

// 如果expr为false, 则执行自定义的语句
#ifndef CHECK_FALSE_EXEC
#define CHECK_FALSE_EXEC(expr, exec_expr, ...) \
    do { \
        bool b = (expr); \
        if (!b) { \
            exec_expr; \
        } \
    } while (0)
#endif

#define DOMI_DELETE_NEW_SINGLE(var) \
    do { \
        if (nullptr != var) { \
            delete var; \
            var = nullptr; \
        } \
    } while (0)

#define DOMI_DELETE_NEW_ARRAY(var) \
    do { \
        if (nullptr != var) { \
            delete[] var; \
            var = nullptr; \
        } \
    } while (0)

// 检查uint32_t类型相乘是否溢出
#define DOMI_MULCHECK_UINT32(a, b, ...) \
    do { \
        if ((b) != 0 && (a) > (UINT32_MAX / (b))) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::INTERNAL_ERROR; \
        } \
    } while (0)

#define DOMI_MULCHECK_INT32(a, b, ...) \
    do { \
        if ((b) != 0 && (a) > (INT32_MAX / (b))) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::INTERNAL_ERROR; \
        } \
    } while (0)

#define DOMI_MULCHECK_INT64(a, b, ...) \
    do { \
        if ((b) != 0 && (a) > (INT64_MAX / (b))) { \
            FMK_LOGE(__VA_ARGS__); \
            return hiai::INTERNAL_ERROR; \
        } \
    } while (0)

/*
 * 无精度损失地求一个整数的天花板值
 */
#define CEIL(N, n) (((N) + (n) - 1) / (n))

#endif // DOMI_OMG_COMMON_UTIL_H_
