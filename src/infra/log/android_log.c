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
