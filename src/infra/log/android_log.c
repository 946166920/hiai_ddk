/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: android_log.c
 */

#include "infra/log/ai_log.h"

#include <android/log.h>

#if defined(AI_SUPPORT_LOG_LEVEL_SETTING)
#include "infra/log/log_level.h"
#endif

static const int g_LogLevels[AI_LOG_ID_MAX] = {
    ANDROID_LOG_DEBUG, // AI_LOG_DEBUG
    ANDROID_LOG_INFO, // AI_LOG_INFO
    ANDROID_LOG_WARN, // AI_LOG_WARN
    ANDROID_LOG_ERROR, // AI_LOG_ERROR
};

__attribute__((__format__(__printf__, 3, 4))) void AI_Log_Print(
    AI_LogPriority priority, const char* moduleName, const char* fmt, ...)
{
    if (priority < AI_LOG_DEBUG || priority > AI_LOG_ERROR) {
        return;
    }

    va_list arg;
    va_start(arg, fmt);
    AI_LogPriority currLevel = AI_LOG_INFO; // 默认日志级别为INFO
#if defined(AI_SUPPORT_LOG_LEVEL_SETTING)
    currLevel = AI_Log_GetLevel(moduleName);
#endif
    if (priority >= currLevel) {
        __android_log_vprint(g_LogLevels[priority], moduleName, fmt, arg);
    }
    va_end(arg);
}
