/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: node_utils
 */

#ifndef FRAMEWORK_GRAPH_UTILS_NODE_UTILS_H
#define FRAMEWORK_GRAPH_UTILS_NODE_UTILS_H

// api/framework
#include <string>
#include "graph/graph_api_export.h"
#include "graph/debug/ge_error_codes.h"

namespace ge {
class Node;

class NodeUtils {
public:
    GRAPH_API_EXPORT static GraphErrCodeStatus SetAllAnchorStatus(Node& node);

    GRAPH_API_EXPORT static bool IsAnchorStatusSet(const Node& node);

    GRAPH_API_EXPORT static bool IsWithSubGraphType(const std::string& type);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_UTILS_NODE_UTILS_H
