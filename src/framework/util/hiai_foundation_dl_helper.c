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
#include "hiai_foundation_dl_helper.h"

#include <stddef.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>

#include "framework/c/hiai_version.h"
#include "framework/util/rom_version_util.h"
#include "framework/infra/log/log.h"

static void* g_aiClientHandle = NULL;
static char* g_npuSupportedBaseVersion = "100.320.011.000";
static char* g_errorVersion = "100.500.010.012";
static int32_t g_supportNpuState = -1;

#ifdef __aarch64__
static const char* g_aiClientLibName = "/vendor/lib64/libai_client.so";
static const char* g_foundation_huawei = "/system/lib64/libhiai_foundation.huawei.so";
#else
static const char* g_aiClientLibName = "/vendor/lib/libai_client.so";
static const char* g_foundation_huawei = "/system/lib/libhiai_foundation.huawei.so";
#endif

void HIAI_Foundation_Init(void)
{
    g_aiClientHandle = dlopen(g_aiClientLibName, RTLD_NOW);
    if (g_aiClientHandle == NULL) {
        g_aiClientHandle = dlopen(g_foundation_huawei, RTLD_NOW);
        if (g_aiClientHandle == NULL) {
            FMK_LOGW("init hiai foundation failed.");
            return;
        }
    }
}

void HIAI_Foundation_Deinit(void)
{
    if (g_aiClientHandle != NULL) {
        return;
    }
}

void* HIAI_Foundation_GetSymbol(const char* symbolName)
{
    if (symbolName == NULL) {
        FMK_LOGE("symbolName is NULL.");
        return NULL;
    }
    if (g_aiClientHandle == NULL) {
        FMK_LOGE("client handle is Null.");
        return NULL;
    }
    return dlsym(g_aiClientHandle, symbolName);
}

HIAI_NPU_SUPPORT_STATE HIAI_Foundation_IsNpuSupport(void)
{
    if (g_supportNpuState >= 0) {
        return (HIAI_NPU_SUPPORT_STATE)g_supportNpuState;
    }
    if (g_aiClientHandle == NULL) {
        FMK_LOGI("Not support NPU");
        g_supportNpuState = (int32_t)HIAI_NOT_SUPPORT_NPU;
        return HIAI_NOT_SUPPORT_NPU;
    }

    int (*isSupportNpuFunc)(void) = (int (*)(void))HIAI_Foundation_GetSymbol("HIAI_IsSupportNpu");
    if (isSupportNpuFunc != NULL && isSupportNpuFunc() > 0) {
        FMK_LOGI("support NPU");
        g_supportNpuState = (int32_t)HIAI_SUPPORT_NPU;
        return HIAI_SUPPORT_NPU;
    }

    const char* version = HIAI_GetVersion();
    if (version == NULL) {
        FMK_LOGI("Not support NPU");
        g_supportNpuState = (int32_t)HIAI_NOT_SUPPORT_NPU;
        return HIAI_NOT_SUPPORT_NPU;
    }

    if (Is3rdRomVersion(version) || Is3rdOldRomVersion(version)) {
        FMK_LOGI("support NPU");
        g_supportNpuState = (int32_t)HIAI_SUPPORT_NPU;
        return HIAI_SUPPORT_NPU;
    }

    if ((strncmp(version, g_npuSupportedBaseVersion, strlen(version)) > 0) &&
        (strncmp(version, g_errorVersion, strlen(version)) != 0)) {
        FMK_LOGI("support NPU");
        g_supportNpuState = (int32_t)HIAI_SUPPORT_NPU;
        return HIAI_SUPPORT_NPU;
    } else {
        FMK_LOGI("Not support NPU");
        g_supportNpuState = (int32_t)HIAI_NOT_SUPPORT_NPU;
        return HIAI_NOT_SUPPORT_NPU;
    }
}
