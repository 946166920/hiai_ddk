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

#ifndef HIAI_FRAMEWORK_LOG_H
#define HIAI_FRAMEWORK_LOG_H

#include "infra/log/ai_log.h"

#if defined(HIAI_DDK) || defined(PLATFORM_UNIV100)
static const char* FMK_MODULE_NAME = "HIAI_DDK_MSG";
#else
static const char* FMK_MODULE_NAME = "AI_FMK";
#endif

#ifdef DEBUG_ON
#define FMK_LOGD(...) AI_LOGD(FMK_MODULE_NAME, __VA_ARGS__)
#else
#define FMK_LOGD(...)
#endif

#define FMK_LOGI(...) AI_LOGI(FMK_MODULE_NAME, __VA_ARGS__)
#define FMK_LOGW(...) AI_LOGW(FMK_MODULE_NAME, __VA_ARGS__)
#define FMK_LOGE(...) AI_LOGE(FMK_MODULE_NAME, __VA_ARGS__)

#define MALLOC_NULL_CHECK_RET_VALUE(obj, retValue) \
    do { \
        if (obj == NULL) { \
            FMK_LOGE("malloc failed."); \
            return retValue; \
        } \
    } while (0);

#endif
