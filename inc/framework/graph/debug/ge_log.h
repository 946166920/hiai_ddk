/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: ge_log
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

// 如果expr为true, 则打印日志并且执行自定义的语句
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

// 如果expr不为true, 不打印日志并且return指定的status
#define GE_CHK_BOOL_RET_STATUS_NOLOG(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            return _status; \
        } \
    } while (0)

// 如果expr不为true, 则打印日志并且执行自定义的语句
#define GE_CHK_BOOL_EXEC(expr, exec_expr, ...) \
    { \
        if (!(expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    }

// 如果expr不为GRAPH_SUCCESS,则打印日志并且return相同的值
#define GE_CHK_STATUS_RET(expr, ...) \
    do { \
        const ge::GraphErrCodeStatus _status = (expr); \
        if (_status != ge::GRAPH_SUCCESS) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// 校验参数是否为null. 如果是，返回 GRAPH_PARAM_INVALID， 并记录error日志
#define GE_CHECK_NOTNULL(val) do { \
    if ((val) == nullptr) { \
        FMK_LOGE(param[#val] must not be null.); \
        return ge::GRAPH_PARAM_INVALID; \
    } \
} while (0)

// 校验参数是否为null. 如果是，记录error日志,并执行指定语句
#define GE_CHECK_NOTNULL_EXEC(val, expr) do { \
    if ((val) == nullptr) { \
        FMK_LOGE(param[#val] must not be null.); \
        expr; \
    } \
} while (0)

// 校验参数是否相等
#define GE_CHECK_EQ(val1, val2) do { \
    if ((val1) != (val2)) { \
        FMK_LOGE(param[#val1] is not equals to[#val2]); \
        return ge::GRAPH_PARAM_INVALID; \
    } \
} while (0)

#endif
