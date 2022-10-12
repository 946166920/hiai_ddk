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

#ifdef AI_SUPPORT_UPGRADE_APK
#define MAX_PLUGIN_VERSION_LEN 32
static pthread_mutex_t g_PluginVersionMutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static const char* g_RomVersion = NULL;

const char* HIAI_GetVersion(void)
{
    if (g_RomVersion != NULL) {
        return g_RomVersion;
    }
#ifdef AI_SUPPORT_UPGRADE_APK
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
        pthread_mutex_lock(&g_PluginVersionMutex);
        if (strcpy_s(tmpPluginVersion, MAX_PLUGIN_VERSION_LEN, pluginVersion) != EOK) {
            pthread_mutex_unlock(&g_PluginVersionMutex);
            FMK_LOGE("PluginVersion copy error");
            break;
        }
        g_RomVersion = tmpPluginVersion;
        pthread_mutex_unlock(&g_PluginVersionMutex);
        if (handle != NULL) {
            (void)dlclose(handle);
        }
        FMK_LOGI("version is %s", (const char*)tmpPluginVersion);
        return (const char*)tmpPluginVersion;
    } while (0);
    if (handle != NULL) {
        (void)dlclose(handle);
    }
#endif
    static const char* getVersionFuncName = "HIAI_GetVersion";
    const char* (*getVersionFunc)(void) = (const char* (*)())HIAI_Foundation_GetSymbol(getVersionFuncName);
    if (getVersionFunc == NULL) {
        FMK_LOGE("sym %s not found.", getVersionFuncName);
        return NULL;
    }

    g_RomVersion = getVersionFunc();
    FMK_LOGI("version is %s", g_RomVersion);
    return g_RomVersion;
}
