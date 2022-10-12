/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: graph checker
 */

#ifndef FRAMEWORK_GRAPH_UTILS_CHECKER_GRAPH_CHECKER_H
#define FRAMEWORK_GRAPH_UTILS_CHECKER_GRAPH_CHECKER_H

#include "graph/graph_api_export.h"

namespace ge {
class ComputeGraph;

class GraphChecker {
public:
    GRAPH_API_EXPORT static bool IsInputsFullyLinked(const ComputeGraph& graph);
};
}

#endif // FRAMEWORK_GRAPH_UTILS_CHECKER_GRAPH_CHECKER_H