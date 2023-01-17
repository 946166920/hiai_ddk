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

#ifndef _GE_COMMON_UTIL_H_
#define _GE_COMMON_UTIL_H_

#include <vector>
#include <string>
#include <sstream>

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
