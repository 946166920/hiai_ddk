/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: node checker
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