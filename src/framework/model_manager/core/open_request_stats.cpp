/**
 * Copyright 2022-2022 Huawei Technologies Co., Ltd
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
#include "open_request_stats.h"

#include <dlfcn.h>
#include <unistd.h>

#include "framework/infra/log/log.h"
#include "infra/base/process_util.h"

namespace hiai {

#ifdef __LP64__
static const char* libraryName = "/vendor/lib64/libai_client.so";
#else
static const char* libraryName = "/vendor/lib/libai_client.so";
#endif

Status OpenRequestStats::StatsRequest(const std::string& statsData)
{
    void* handle = dlopen(libraryName, RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr) {
        FMK_LOGW("dlopen failed, lib[%s], errmsg[%s]", libraryName, dlerror());
        return hiai::FAILURE;
    }

    const char* functionName = "StatsService_RecordRequest";
    void* functionAddr = dlsym(handle, functionName);
    if (functionAddr == nullptr) {
        FMK_LOGE("dlsym failed, lib[%s], errmsg[%s]", functionName, dlerror());
        dlclose(handle);
        return hiai::FAILURE;
    }

    using FuncHandle = int (*)(const char*, uint32_t);
    FuncHandle functionHandle = reinterpret_cast<FuncHandle>(functionAddr);
    if (functionHandle == nullptr) {
        FMK_LOGE("dlsym failed, lib[%s], errmsg[%s]", functionName, dlerror());
        dlclose(handle);
        return hiai::FAILURE;
    }

    int ret = functionHandle(statsData.c_str(), statsData.length());
    dlclose(handle);
    return ret == 0 ? hiai::SUCCESS : hiai::FAILURE;
}

Status OpenRequestStats::CloudDdkVersionStats(const char* clientDdkVersion, const char* interfaceName, int result)
{
    std::string StatsData;
    std::string uid = std::to_string(getpid());
    StatsData += "uid=" + uid + ",";
    std::size_t pos = std::string(clientDdkVersion).find_last_of(":");
    if (pos != std::string::npos) {
        constexpr int POS_OFFSET = 1;
        StatsData += "interfaceName=" + std::string(interfaceName) + ";" +
            std::string("clientDdkVersion:") + std::string(clientDdkVersion).substr(pos + POS_OFFSET) + ",";
    } else {
        StatsData += "interfaceName=" + std::string(interfaceName) + ",";
    }
    std::string processName = hiai::GetProcessName();
    StatsData += "processName=" + processName + ",";
    StatsData += "engineType=1,";
    std::string statsResult = std::to_string(result);
    StatsData += "result=" + statsResult;
    return StatsRequest(StatsData);
}
} // namespace hiai