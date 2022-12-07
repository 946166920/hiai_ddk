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
