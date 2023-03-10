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

#include "infra/log/ai_log.h"

#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#include <syscall.h>

#include "securec.h"
#if defined(AI_SUPPORT_LOG_LEVEL_SETTING)
#include "infra/log/log_level.h"
#endif

#define LOG_BUFF_SIZE 2048

static const char* g_LogLevels[AI_LOG_ID_MAX] = {
    "D", // AI_LOG_DEBUG
    "I", // AI_LOG_INFO
    "W", // AI_LOG_WARN
    "E", // AI_LOG_ERROR
};

__attribute__((__format__(__printf__, 3, 4))) void AI_Log_Print(
    AI_LogPriority priority, const char* moduleName, const char* fmt, ...)
{
    if (priority < AI_LOG_DEBUG || priority > AI_LOG_ERROR) {
        return;
    }
    if (moduleName == NULL) {
        return;
    }

    char logBuff[LOG_BUFF_SIZE] = { 0 };
    AI_LogPriority currLevel = AI_LOG_INFO;
#if defined(AI_SUPPORT_LOG_LEVEL_SETTING)
    currLevel = AI_Log_GetLevel(moduleName);
#endif
    int index = 0;

    int ret = snprintf_s(
        logBuff, LOG_BUFF_SIZE, LOG_BUFF_SIZE - 1, "%s/%s  (%d): ", g_LogLevels[priority], moduleName, getpid());
    if ((ret < 0) || (ret >= (LOG_BUFF_SIZE - 1))) {
        return;
    }
    index += ret;

    va_list arg;
    va_start(arg, fmt);
    ret = vsnprintf_s(logBuff + index, LOG_BUFF_SIZE - index, LOG_BUFF_SIZE - index - 1, fmt, arg);
    va_end(arg);
    if (ret < 0) {
        return;
    }
    // 默认日志级别为INFO
    if (priority >= currLevel) {
#ifndef AI_SUPPORT_MULTIPLE
        printf("%s\n", logBuff);
#else
        syslog(LOG_ERR, "%s\n", logBuff);
#endif
    }
}
