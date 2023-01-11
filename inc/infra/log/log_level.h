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

#ifndef _INC_INFRA_LOG_LOG_LEVEL_H_
#define _INC_INFRA_LOG_LOG_LEVEL_H_
#include <stdbool.h>

#include "infra/log/ai_log.h"

#ifdef __cplusplus
extern "C" {
#endif

bool AI_Log_SetLevel(const char* moduleName, AI_LogPriority priority);

AI_LogPriority AI_Log_GetLevel(const char* moduleName);

#ifdef __cplusplus
}
#endif

#endif // _INC_INFRA_LOG_LOG_LEVEL_H_
