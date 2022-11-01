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

#ifndef _DOMI_COMMON_DEBUG_LOG_H_
#define _DOMI_COMMON_DEBUG_LOG_H_

#include "common/string_util.h"
#include "framework/infra/log/log.h"

#define FMK_LOGI_IF(condition, ...) \
    if ((condition)) { \
        FMK_LOGI(__VA_ARGS__); \
    }

#define FMK_LOGW_IF(condition, ...) \
    if ((condition)) { \
        FMK_LOGW(__VA_ARGS__); \
    }

#define FMK_LOGE_IF(condition, ...) \
    if ((condition)) { \
        FMK_LOGE(__VA_ARGS__); \
    }

// 如果expr不为SUCCESS,则打印日志并且return相同的值
#define DOMI_CHK_STATUS_RET(expr, ...) \
    do { \
        const hiai::Status _status = (expr); \
        if (hiai::SUCCESS != _status) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// 如果expr不为SUCCESS,则打印日志, 不执行return
#define DOMI_CHK_STATUS(expr, ...) \
    do { \
        const hiai::Status _status = (expr); \
        if (hiai::SUCCESS != _status) { \
            FMK_LOGE(__VA_ARGS__); \
        } \
    } while (0)

// 如果expr不为SUCCESS,则打印日志, 执行return
#define DOMI_CHK_STATUS_RET_VOID(expr, ...) \
    do { \
        const hiai::Status _status = (expr); \
        if (hiai::SUCCESS != _status) { \
            FMK_LOGE(__VA_ARGS__); \
            return; \
        } \
    } while (0)

// 如果expr不为SUCCESS,则return相同的值
#define DOMI_CHK_STATUS_RET_NOLOG(expr) \
    do { \
        const hiai::Status _status = (expr); \
        if (hiai::SUCCESS != _status) { \
            return _status; \
        } \
    } while (0)

// 如果expr不为SUCCESS, 则打印日志并且执行自定义的语句
#define DOMI_CHK_STATUS_EXEC(expr, exec_expr, ...) \
    do { \
        const hiai::Status _status = (expr); \
        DOMI_CHK_BOOL_EXEC(hiai::SUCCESS == _status, exec_expr, __VA_ARGS__); \
    } while (0)

#ifdef __AICP_TA__
#define DOMI_CHK_SMMU(expr) \
    do { \
        const hiai::Status _status = (expr); \
        if (hiai::SUCCESS != _status) { \
            FMK_LOGE("Call SmmuManager api failed, ret: 0x%X", _status); \
        } \
    } while (0)

#endif

// 如果expr不为true, 则打印日志并且return指定的status
#define DOMI_CHK_BOOL_RET_STATUS(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// 如果expr不为true, 则打印Info日志并且return指定的status
#define DOMI_CHK_BOOL_RET_STATUS_LOGI(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            FMK_LOGI(__VA_ARGS__); \
            return _status; \
        } \
    } while (0)

// 如果expr不为true, 则打印日志并且return指定的status
#define DOMI_CHK_BOOL_RET_STATUS_NOLOG(expr, _status, ...) \
    do { \
        if (!(expr)) { \
            return _status; \
        } \
    } while (0)

// 如果expr不为true, 则打印日志并且执行自定义的语句
#define DOMI_CHK_BOOL_EXEC(expr, exec_expr, ...) \
    { \
        if (!(expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    }
// 如果expr不为true, 则打印日志并且执行自定义的语句
#define DOMI_CHK_BOOL_EXEC_INFO(expr, exec_expr, ...) \
    { \
        if (!(expr)) { \
            FMK_LOGI(__VA_ARGS__); \
            exec_expr; \
        } \
    }

// 如果expr不为true, 则打印日志并且执行自定义的语句
#define DOMI_CHK_BOOL_TRUE_EXEC_INFO(expr, exec_expr, ...) \
    { \
        if ((expr)) { \
            FMK_LOGI(__VA_ARGS__); \
            exec_expr; \
        } \
    }

// 如果expr为true, 则打印日志并且执行自定义的语句
#define DOMI_CHK_BOOL_TRUE_EXEC_WITH_LOG(expr, exec_expr, ...) \
    { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    }
// 如果expr为true, 则打印Information日志并且执行自定义的语句
#define DOMI_CHK_TRUE_EXEC_INFO(expr, exec_expr, ...) \
    { \
        if ((expr)) { \
            FMK_LOGI(__VA_ARGS__); \
            exec_expr; \
        } \
    }
// 如果expr为true, 则打印Error日志并且执行自定义的语句
#define DOMI_CHK_TRUE_EXEC_ERROR(expr, exec_expr, ...) \
    { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
        } \
    }
// 如果expr不为SUCCESS,则打印日志, 执行表达式+return
#define DOMI_CHK_BOOL_TRUE_RET_VOID(expr, exec_expr, ...) \
    do { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
            return; \
        } \
    } while (0)

// 如果expr不为SUCCESS,则打印日志, 执行表达式+return _status,
#define DOMI_CHK_BOOL_TRUE_EXEC_RET_STATUS(expr, _status, exec_expr, ...) \
    do { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr; \
            return _status; \
        } \
    } while (0)

#define DOMI_CHK_BOOL_TRUE_EXEC_RET_STATUS_2(expr, _status, exec_expr0, exec_expr1, ...) \
    do { \
        if ((expr)) { \
            FMK_LOGE(__VA_ARGS__); \
            exec_expr0; \
            exec_expr1; \
            return _status; \
        } \
    } while (0)

// 如果expr不为true, 则执行自定义的语句
#define DOMI_CHK_BOOL_EXEC_NOLOG(expr, exec_expr) \
    { \
        if (!(expr)) { \
            exec_expr; \
        } \
    }

// 如果expr不为true, 则线程等待的语句
#define DOMI_CHK_BOOL_THD_WAIT_NOLOG(expr) \
    do { \
        while (!(expr)) { \
            std::this_thread::sleep_for(std::chrono::seconds(1)); \
        } \
    } while (0)

// -----------------runtime相关宏定义-------------------------------
// 如果expr不为RT_ERROR_NONE,则打印日志
#define DOMI_CHK_RT(expr) \
    do { \
        rtError_t _rt_ret = (expr); \
        if (RT_ERROR_NONE != _rt_ret) { \
            FMK_LOGE("Call rt api failed, ret: 0x%X", _rt_ret); \
        } \
    } while (0)

// 如果expr不为RT_ERROR_NONE,则打印日志, 并执行exec_expr表达式
#define DOMI_CHK_RT_EXEC(expr, exec_expr) \
    { \
        rtError_t _rt_ret = (expr); \
        if (RT_ERROR_NONE != _rt_ret) { \
            FMK_LOGE("Call rt api failed, ret: 0x%X", _rt_ret); \
            exec_expr; \
        } \
    }

// 如果expr不为RT_ERROR_NONE,则打印日志并返回
#define DOMI_CHK_RT_RET(expr) \
    do { \
        rtError_t _rt_ret = (expr); \
        if (RT_ERROR_NONE != _rt_ret) { \
            FMK_LOGE("Call rt api failed, ret: 0x%X", _rt_ret); \
            return hiai::RT_FAILED; \
        } \
    } while (0)

// ------------------------elem相关宏定义----------------------------
// 如果expr不为CC_STATUS_SUCCESS,则打印日志
#define DOMI_CHK_NPU_CORE(expr) \
    do { \
        NpuCoreErrCodeStatus _cc_ret = (expr); \
        if (CC_STATUS_SUCCESS != _cc_ret) { \
            FMK_LOGE("Call elem api failed, ret: 0x%X", _cc_ret); \
        } \
    } while (0)

// 如果expr不为CC_STATUS_SUCCESS,则打印日志, 并执行exec_expr表达式
#define DOMI_CHK_NPU_CORE_EXEC(expr, exec_expr) \
    do { \
        NpuCoreErrCodeStatus _cc_ret = (expr); \
        if (CC_STATUS_SUCCESS != _cc_ret) { \
            FMK_LOGE("Call elem api failed, ret: 0x%X", _cc_ret); \
            exec_expr; \
        } \
    } while (0)

// 如果expr不为CC_STATUS_SUCCESS,则打印日志并返回
#define DOMI_CHK_NPU_CORE_RET(expr) \
    do { \
        NpuCoreErrCodeStatus _cc_ret = (expr); \
        if (CC_STATUS_SUCCESS != _cc_ret) { \
            FMK_LOGE("Call elem api failed, ret: 0x%X", _cc_ret); \
            return hiai::NPU_CORE_FAILED; \
        } \
    } while (0)

// 如果expr为true，则执行exec_expr，不打印日志
#define DOMI_IF_BOOL_EXEC(expr, exec_expr) \
    { \
        if (expr) { \
            exec_expr; \
        } \
    }

// -----------------tee相关宏定义-------------------------------
// 如果expr不为0,则打印日志
#define DOMI_CHK_TEE(expr) \
    do { \
        uint32_t _tee_ret = (expr); \
        if (_tee_ret != 0) { \
            FMK_LOGE("Call tee api failed, ret: 0x%d", _tee_ret); \
        } \
    } while (0)

// 如果expr不为0,则打印日志, 并执行exec_expr表达式
#define DOMI_CHK_TEE_EXEC(expr, exec_expr) \
    { \
        uint32_t _tee_ret = (expr); \
        if (0 != _tee_ret) { \
            FMK_LOGE("Call tee api failed, ret: 0x%d", _tee_ret); \
            exec_expr; \
        } \
    }

// 如果expr不为0,则打印日志并返回
#define DOMI_CHK_TEE_RET(expr) \
    do { \
        uint32_t _tee_ret = (expr); \
        if (_tee_ret != 0) { \
            FMK_LOGE("Call tee api failed, ret: 0x%d", _tee_ret); \
            return hiai::TEE_ERROR; \
        } \
    } while (0)

// 如果make_shared异常，则打印日志并执行语句
#define DOMI_MAKE_SHARED(exec_expr0, exec_expr1) \
    try { \
        exec_expr0; \
    } catch (...) { \
        FMK_LOGE("Make shared failed"); \
        exec_expr1; \
    }

#define DOMI_CHECK_POINT_NULLPTR(p, ...) \
    do { \
        if ((p) == nullptr) { \
            FMK_LOGE(__VA__ARGS__); \
            return hiai::FAILED; \
        } \
    } while (0)

#endif
