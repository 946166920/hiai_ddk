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
#ifndef FRAMEWORK_INC_OPEN_REQUEST_STATS_H
#define FRAMEWORK_INC_OPEN_REQUEST_STATS_H

#include <string>
#include "base/error_types.h"

namespace hiai {
class OpenRequestStats {
public:
    static Status StatsRequest(const std::string& statsData);
    static Status CloudDdkVersionStats(const char* clientDdkVersion, const char* interfaceName, int result);

private:
    OpenRequestStats() = default;
    ~OpenRequestStats() = default;
};
} // end namespace hiai
#endif