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
