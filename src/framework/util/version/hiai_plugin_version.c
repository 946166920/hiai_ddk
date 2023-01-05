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
#include <pthread.h>
#include <dlfcn.h>

#include "securec.h"

#include "framework/infra/log/log.h"

#include "util/hiai_foundation_dl_helper.h"

#define MAX_PLUGIN_VERSION_LEN 32
static pthread_mutex_t g_PluginVersionMutex = PTHREAD_MUTEX_INITIALIZER;

static const char* g_RomVersion = NULL;

static const char* HIAI_GetPluginVersion(void)
{
    void* handle = NULL;
    do {
        const char* libraryName = "libhiai_enhance.so";
        handle = dlopen(libraryName, RTLD_LAZY);
        if (handle == NULL) {
            FMK_LOGW("dlopen failed, lib[%s], errmsg[%s]", libraryName, dlerror());
            break;
        }
        const char* functionName = "GetPluginHiAIVersion";
        const char* className = "com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient";
        const char* methodName = "getPluginHiAIVersion";
        const char* methodSignature =
            "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;";
        const char* packageName = "com.huawei.hiai";
        const char* (*function)(const char*, const char*, const char*, const char*, const char*) =
            (const char* (*)(const char*, const char*, const char*, const char*, const char*))dlsym(
                handle, functionName);

        if (function == NULL) {
            FMK_LOGE("dlsym failed, lib[%s], errmsg[%s]", functionName, dlerror());
            break;
        }
        const char* pluginVersion = function(className, methodName, methodSignature, packageName, "");
        if (pluginVersion == NULL || strncmp(pluginVersion, "000.000.000.000", strlen(pluginVersion)) == 0) {
            break;
        }
        static char tmpPluginVersion[MAX_PLUGIN_VERSION_LEN] = {0};
        if (strcpy_s(tmpPluginVersion, MAX_PLUGIN_VERSION_LEN, pluginVersion) != EOK) {
            FMK_LOGE("PluginVersion copy error");
            break;
        }
        if (handle != NULL) {
            (void)dlclose(handle);
        }
        FMK_LOGI("version is %s", (const char*)tmpPluginVersion);
        return (const char*)tmpPluginVersion;
    } while (0);
    if (handle != NULL) {
        (void)dlclose(handle);
    }
    return NULL;
}

typedef char* (*HIAI_GetVersion_ptr)(void);
static const char* HIAI_GetFoundationVersion(void)
{
    static const char* getVersionFuncName = "HIAI_GetVersion";
    HIAI_GetVersion_ptr getVersionFunc = (HIAI_GetVersion_ptr)HIAI_Foundation_GetSymbol(getVersionFuncName);
    if (getVersionFunc == NULL) {
        FMK_LOGE("sym %s not found.", getVersionFuncName);
        return NULL;
    }

    return getVersionFunc();
}

const char* HIAI_GetVersion(void)
{
    if (g_RomVersion != NULL) {
        return g_RomVersion;
    }

    pthread_mutex_lock(&g_PluginVersionMutex);
    if (g_RomVersion != NULL) {
        pthread_mutex_unlock(&g_PluginVersionMutex);
        return g_RomVersion;
    }
    const char* version = HIAI_GetPluginVersion();
    if (version == NULL) {
        version = HIAI_GetFoundationVersion();
    }
    g_RomVersion = version;
    pthread_mutex_unlock(&g_PluginVersionMutex);

    FMK_LOGI("version is %s", g_RomVersion == NULL ? "NULL" : g_RomVersion);
    return g_RomVersion;
}
