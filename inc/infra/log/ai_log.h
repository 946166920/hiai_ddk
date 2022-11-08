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

#ifndef _INC_INFRA_LOG_AI_LOG_H_
#define _INC_INFRA_LOG_AI_LOG_H_

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum AI_LogPriority {
    AI_LOG_DEBUG = 0,
    AI_LOG_INFO = 1,
    AI_LOG_WARN = 2,
    AI_LOG_ERROR = 3,
    AI_LOG_ID_MAX,
} AI_LogPriority;

void AI_Log_Print(AI_LogPriority priority, const char* moduleName, const char* fmt, ...);

#define REAL_FILE_NAME strrchr(__FILE__, '/')

#define AI_LOGD(moduleName, fmt, ...) \
    AI_Log_Print( \
        AI_LOG_DEBUG, moduleName, "%s %s(%d)::" #fmt, (REAL_FILE_NAME + 1), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define AI_LOGI(moduleName, fmt, ...) \
    AI_Log_Print( \
        AI_LOG_INFO, moduleName, "%s %s(%d)::" #fmt, (REAL_FILE_NAME + 1), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define AI_LOGW(moduleName, fmt, ...) \
    AI_Log_Print( \
        AI_LOG_WARN, moduleName, "%s %s(%d)::" #fmt, (REAL_FILE_NAME + 1), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define AI_LOGE(moduleName, fmt, ...) \
    AI_Log_Print( \
        AI_LOG_ERROR, moduleName, "%s %s(%d)::" #fmt, (REAL_FILE_NAME + 1), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // _INC_INFRA_LOG_AI_LOG_H_
