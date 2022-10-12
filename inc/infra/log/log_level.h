/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: log_level.h
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
