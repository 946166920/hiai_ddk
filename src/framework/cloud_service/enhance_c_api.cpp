/*!
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * \file enhance_c_api.cc
 * \brief C API of enhance Function for interact with plugin.
 */
#include "enhance_c_api.h"
#include <string>
#include <securec.h>
#include "mutex"
#include "enhance_utils.h"
#include "framework/infra/log/log.h"

long GetPluginSoHandle(const char* className, const char* functionName, const char* methodSignature,
                       const char* packageName, const char* soName)
{
    return hiai::CheckVaild() ? hiai::CallEnhanceFunction<long>(std::string(className),
        std::string(functionName), std::string(methodSignature), hiai::String2jString(std::string(packageName)),
        hiai::String2jString(std::string(soName))) : 0;
}

long GetPluginSoHandleDefault(const char* soName)
{
    const char* className = "com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient";
    const char* functionName = "loadNativeRemoteLibrary";
    const char* methodSignature = "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)J";
    const char* packageName = "com.huawei.hiai";
    return GetPluginSoHandle(className, functionName, methodSignature, packageName, soName);
}

const char* GetPluginHiAIVersion(const char* className, const char* functionName,
    const char* methodSignature, const char* packageName,
    const char* notSupportCase)
{
    const char* version = nullptr;
    if (hiai::CheckVaild()) {
        std::string tmpVersion = hiai::CallEnhanceFunction<std::string>(std::string(className),
            std::string(functionName), std::string(methodSignature),
            hiai::String2jString(std::string(packageName)),
            hiai::String2jString(std::string(notSupportCase)));
        version = hiai::String2ConstCharPtr(tmpVersion);
    }
    return version;
}

const char* GetPluginHiAIVersionDefault(void)
{
    const char* className = "com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient";
    const char* functionName = "getPluginHiAIVersion";
    const char* methodSignature = "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;";
    const char* packageName = "com.huawei.hiai";
    const char* notSupportCase = "";

    return GetPluginHiAIVersion(className, functionName, methodSignature, packageName, notSupportCase);
}

const char* GetPluginVersionName(void)
{
    const std::string className ("com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient");
    const std::string packageName ("com.huawei.hiai");
    const std::string functionName ("getPluginHiAIVersion");
    const std::string methodSignature (
        "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    const std::string scene ("getPluginVersionName");
    constexpr int MAX_PLUGIN_VERSIONNAME_LEN = 13;
    static char versionName[MAX_PLUGIN_VERSIONNAME_LEN] = {0};
    const std::string tmpVersion = hiai::CallEnhanceFunction<std::string>(className, functionName, methodSignature,
        hiai::String2jString(packageName), hiai::String2jString(scene));
    if (tmpVersion.size() > MAX_PLUGIN_VERSIONNAME_LEN) {
        FMK_LOGE("PluginVersion is too old to get");
        return nullptr;
    }
    static std::mutex versionNameMutex;
    versionNameMutex.lock();
    if (strcpy_s(versionName, MAX_PLUGIN_VERSIONNAME_LEN, tmpVersion.c_str()) != EOK) {
        FMK_LOGE("PluginVersion copy error");
        versionNameMutex.unlock();
        return nullptr;
    }
    versionNameMutex.unlock();
    FMK_LOGI("computecapability APP versionName is %s", tmpVersion.c_str());
    return static_cast<const char*>(versionName);
}

bool CloudServiceEnable(void* jniEnv, void* context)
{
    return hiai::UtilsEnable(jniEnv, context);
}

bool CloudServiceDisable(void)
{
    return hiai::UtilsDisable();
}