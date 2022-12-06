/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: The description of Stats util in framework.
 */
#include "infra/base/process_util.h"
#include <unistd.h>
#include <climits>
#include "infra/log/ai_log.h"

using std::string;
namespace hiai {
string GetProcessName(int pid)
{
    // default get current pid
    if (pid == -1) {
        pid = getpid();
    }
    string processName;
    static const int PROCESS_NAME_MAX_LENGTH = 256;
    char clientProcessName[PROCESS_NAME_MAX_LENGTH] = {0};
    string clientProcessPath;
    clientProcessPath += "/proc/";
    clientProcessPath += std::to_string(pid);
    clientProcessPath += "/cmdline";
    if (clientProcessPath.size() > PATH_MAX) {
        AI_LOGE("INFRA", "GetProcessName: clientProcessPath is invalid or not exist.");
        return processName;
    }

    char processPath[PATH_MAX + 1] = {0};
    if (realpath(clientProcessPath.c_str(), processPath) == nullptr) {
        AI_LOGE("INFRA", "GetProcessName: invalid process Path.");
        return processName;
    }

    FILE* fp = fopen(processPath, "r");
    if (fp != nullptr) {
        if (fgets(clientProcessName, PROCESS_NAME_MAX_LENGTH, fp) != nullptr) {
            fclose(fp);
            processName = clientProcessName;
            return processName;
        }
        AI_LOGE("INFRA", "GetProcessName: fgets get process name.");
        fclose(fp);
    }
    return processName;
}
} // namespace hiai
