/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: model manager client implementation src
 *
 */
#include "cloud_service/cloud_service.h"

#include <unistd.h>

#include "framework/infra/log/log.h"
#include "enhance_c_api.h"
#include "infra/base/process_util.h"
#include "framework/infra/log/log_fmk_interface.h"
#include "model_manager/core/open_request_stats.h"

namespace hiai {

#ifdef AI_SUPPORT_UPGRADE_APK
static uint32_t CloudPluginVersionStats(const char* pluginVersion)
{
    std::string StatsData;
    std::string uid = std::to_string(getpid());
    StatsData += "uid=" + uid + ",";
    StatsData += "interfaceName=CloudServiceEnable;" + std::string("pluginVersion:") + std::string(pluginVersion) + ",";
    std::string processName = hiai::GetProcessName();
    StatsData += "processName=" + processName + ",";
    StatsData += "engineType=1,";
    return OpenRequestStats::StatsRequest(StatsData);
}
#endif

bool CloudService::Enable(void* env, void* appContext)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (env == nullptr) {
        FMK_LOGE("CloudService::Enable failed, env is nullptr");
        return false;
    }
    if (appContext == nullptr) {
        FMK_LOGE("CloudService::Enable failed, appContext is nullptr");
        return false;
    }
    bool ret = CloudServiceEnable(env, appContext);
    if (!ret) {
        FMK_LOGE("CloudService::enable failed");
        return false;
    }
    const char* pluginVersion = GetPluginHiAIVersionDefault();
    if ((pluginVersion == nullptr) || (strcmp(pluginVersion, "000.000.000.000") == 0)) {
        FMK_LOGE("plugin app version is invalid.");
        ret = CloudServiceDisable();
        if (!ret) {
            FMK_LOGE("CloudService::enable failed");
        }
        return false;
    }
    const char* pluginServerLib = "libhiai_executor_app.so";
    long serverHandle = GetPluginSoHandleDefault(pluginServerLib);
    if (serverHandle == 0L) {
        FMK_LOGE("Load plugin app libhiai_executor_app.so failed.");
        ret = CloudServiceDisable();
        if (!ret) {
            FMK_LOGE("CloudService::enable failed");
        }
        return false;
    }

#ifdef AI_SUPPORT_UPGRADE_APK
    const char* pluginVersionNum = GetComputeCapabilityVersion();
    FMK_LOGI("GetComputeCapabilityVersion %s", pluginVersionNum);
    if (pluginVersionNum != nullptr) {
        (void)CloudPluginVersionStats(pluginVersionNum);
    }
#endif

    return true;
}
const char* CloudService::GetComputeCapabilityVersion()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return GetPluginVersionName();
}
} // namespace hiai
