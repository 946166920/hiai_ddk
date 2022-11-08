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
