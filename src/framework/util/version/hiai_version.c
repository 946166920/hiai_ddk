/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: version implementation
 */
#include "framework/c/hiai_version.h"

#include <stdint.h>
#include <stddef.h>

#include "securec.h"

#include "infra/mmpa/mmpa_api.h"
#include "framework/infra/log/log.h"

#include "util/hiai_foundation_dl_helper.h"

static const char* g_RomVersion = NULL;

typedef char* (*HIAI_GetVersion_ptr)(void);
const char* HIAI_GetVersion(void)
{
    if (g_RomVersion != NULL) {
        return g_RomVersion;
    }
    static const char* getVersionFuncName = "HIAI_GetVersion";
    HIAI_GetVersion_ptr getVersionFunc = (HIAI_GetVersion_ptr)HIAI_Foundation_GetSymbol(getVersionFuncName);
    if (getVersionFunc == NULL) {
        FMK_LOGE("sym %s not found.", getVersionFuncName);
        return NULL;
    }

    g_RomVersion = getVersionFunc();
    FMK_LOGI("version is %s", g_RomVersion);
    return g_RomVersion;
}
