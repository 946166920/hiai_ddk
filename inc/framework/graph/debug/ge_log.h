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

#ifndef _GE_DEBUG_LOG_H_
#define _GE_DEBUG_LOG_H_

#include "graph/debug/ge_error_codes.h"
#include "framework/infra/log/log.h"

#define GE_LOGI_IF(condition, ...) \
    do { \
        if ((condition)) { \
            FMK_LOGD(__VA_ARGS__); \
        } \
    } while (0)

#define GE_LOGW_IF(condition, ...) \
    do { \
        if ((condition)) { \
            FMK_LOGW(__VA_ARGS__); \
        } \
    } while (0)

#define GE_LOGE_IF(condition, ...) \
    do { \
        if ((condition)) { \
            FMK_LOGE(__VA_ARGS__); \
        } \
    } while (0)

#define GE_CHK_STATUS_RET_NOLOG(expr) \
    do { \
        const ge::GraphErrCodeStatus _status = (expr); \
        if (_status != ge::GRAPH_SUCCESS) { \
            return _status; \
        } \
    } while (0)

#define GE_CHK_BOOL_RET_STATUS(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

#define GE_CHK_BOOL_EXEC_NOLOG(expr, exec_expr) \
    do { \
        if (!(expr)) { \
            exec_expr; \
        } \
    } while (0)

#define GE_IF_BOOL_EXEC(expr, exec_expr) \
    do { \
        if (expr) { \
            exec_expr; \
        } \
    } while (0)

#define GE_RETURN_WITH_LOG_IF_ERROR(expr, ...) \
    do { \
        const ge::GraphErrCodeStatus _status = (expr); \
        if (_status != ge::GRAPH_SUCCESS) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// ??????expr???true, ?????????????????????????????????????????????
#define GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(expr, exec_expr, ...) \
    do { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    } while (0)

// only check error log
#define GE_CHK_BOOL_ONLY_LOG(expr, ...) \
    do { \
        if (!(expr)) { \
            FMK_LOGI(__VA_ARGS__); \
        } \
    } while (0)

// ??????expr??????true, ?????????????????????return?????????status
#define GE_CHK_BOOL_RET_STATUS_NOLOG(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            return _status; \
        } \
    } while (0)

// ??????expr??????true, ?????????????????????????????????????????????
#define GE_CHK_BOOL_EXEC(expr, exec_expr, ...) \
    { \
        if (!(expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    }

// ??????expr??????GRAPH_SUCCESS,?????????????????????return????????????
#define GE_CHK_STATUS_RET(expr, ...) \
    do { \
        const ge::GraphErrCodeStatus _status = (expr); \
        if (_status != ge::GRAPH_SUCCESS) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// ?????????????????????null. ?????????????????? GRAPH_PARAM_INVALID??? ?????????error??????
#define GE_CHECK_NOTNULL(val) do { \
    if ((val) == nullptr) { \
        FMK_LOGE(param[#val] must not be null.); \
        return ge::GRAPH_PARAM_INVALID; \
    } \
} while (0)

// ?????????????????????null. ??????????????????error??????,?????????????????????
#define GE_CHECK_NOTNULL_EXEC(val, expr) do { \
    if ((val) == nullptr) { \
        FMK_LOGE(param[#val] must not be null.); \
        expr; \
    } \
} while (0)

// ????????????????????????
#define GE_CHECK_EQ(val1, val2) do { \
    if ((val1) != (val2)) { \
        FMK_LOGE(param[#val1] is not equals to[#val2]); \
        return ge::GRAPH_PARAM_INVALID; \
    } \
} while (0)

#endif
