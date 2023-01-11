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

#ifndef INFRA_BASE_ASSERTION_H
#define INFRA_BASE_ASSERTION_H

#include "base/error_types.h"
#include "infra/log/ai_log.h"

static const char* INFRA_MODULE_NAME = "INFRA";

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            AI_LOGE(INFRA_MODULE_NAME, #ptr "null, return FAIL."); \
            return hiai::FAILURE; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_NOT_NULL_VOID(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            AI_LOGW(INFRA_MODULE_NAME, #ptr "null, return FAIL."); \
            return; \
        } \
    } while (0)

#define HIAI_EXPECT_NOT_NULL_R(ptr, ret) \
    do { \
        if ((ptr) == nullptr) { \
            AI_LOGE(INFRA_MODULE_NAME, #ptr "null, return FAIL."); \
            return ret; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            AI_LOGE(INFRA_MODULE_NAME, #expr "false, return FAIL."); \
            return hiai::FAILURE; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_TRUE_R(expr, ret) \
    do { \
        if (!(expr)) { \
            AI_LOGE(INFRA_MODULE_NAME, #expr "false, return %s.", #ret); \
            return ret; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_TRUE_VOID(expr) \
    do { \
        if (!(expr)) { \
            AI_LOGW(INFRA_MODULE_NAME, #expr "false, return."); \
            return; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_EXEC(func) \
    do { \
        hiai::Status status = func; \
        if (status != hiai::SUCCESS) { \
            return status; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_EXEC_R(func, ret) \
    do { \
        hiai::Status status = func; \
        if (status != hiai::SUCCESS) { \
            return ret; \
        } \
    } while (0)

/////////////////////////////////////////////////////////////////
#define HIAI_EXPECT_EXEC_VOID(func) \
    do { \
        hiai::Status status = func; \
        if (status != hiai::SUCCESS) { \
            return; \
        } \
    } while (0)

#define HIAI_EXPECT_IN_RANGE_R(val, minValue, maxValue, ret) \
    do { \
        if ((val) < (minValue) || (val) > (maxValue)) { \
            AI_LOGE(INFRA_MODULE_NAME, param[#val] "is not in valid range"); \
            return ret; \
        } \
    } while (0)

#endif // HIAI_FRAMEWORK_INFRA_BASE_ASSERTION_H
