/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: ge_util
 */

#ifndef _GE_COMMON_UTIL_H_
#define _GE_COMMON_UTIL_H_

#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include "graph/debug/ge_error_codes.h"

#if !defined(__ANDROID__) && !defined(ANDROID)
#define GE_DYNAMIC_CAST dynamic_cast
#define GE_DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#else
#define GE_DYNAMIC_CAST static_cast
#define GE_DYNAMIC_POINTER_CAST std::static_pointer_cast
#endif

/*
 * @ingroup GE_common
 * @brief 该宏提供禁止拷贝构造函数和赋值操作符能力，一般放在private 下面
 */
#define GE_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete; \
    void operator=(const TypeName&) = delete

// 如果expr为false, 则执行自定义的语句
#ifdef CHECK_FALSE_EXEC
#undef CHECK_FALSE_EXEC
#endif
#define CHECK_FALSE_EXEC(expr, exec_expr, ...) \
    do { \
        bool b = (expr); \
        if (!b) { \
            exec_expr; \
        } \
    } while (0);

#define CHECK_TRUE_EXEC(expr, exec_expr) \
    { \
        if (expr) { \
            exec_expr; \
        } \
    }

#define GE_DELETE_NEW_SINGLE(var) \
    do { \
        if (nullptr != (var)) { \
            delete (var); \
            (var) = nullptr; \
        } \
    } while (0);

#define GE_DELETE_NEW_ARRAY(var) \
    do { \
        if (nullptr != (var)) { \
            delete[] (var); \
            (var) = nullptr; \
        } \
    } while (0);
#endif
