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

#ifndef FRAMEWORK_GRAPH_UTILS_CHECKER_NODE_CHECKER_H
#define FRAMEWORK_GRAPH_UTILS_CHECKER_NODE_CHECKER_H

#include <string>

#include "graph/graph_api_export.h"

namespace ge {
class NodeChecker {
public:
    GRAPH_API_EXPORT static bool IsGraphInputType(const std::string& type);

    static bool IsNonInputType(const std::string& type);

    static bool IsOptionalInput(const std::string& type, uint32_t idx);
};
}

#endif // FRAMEWORK_GRAPH_UTILS_CHECKER_NODE_CHECKER_H