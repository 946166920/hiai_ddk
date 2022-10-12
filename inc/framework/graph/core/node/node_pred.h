/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: node pred
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_PRED_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_PRED_H

#include <functional>

namespace ge {
class Node;

using NodePred = std::function<bool(Node&)>;

using NodeAction = std::function<void(Node&)>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_PRED_H