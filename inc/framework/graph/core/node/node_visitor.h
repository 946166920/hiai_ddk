/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: node visitor
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_VISITOR_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_VISITOR_H

#include <functional>

namespace ge {
class Node;

using NodeVisitor = std::function<hiai::Status(Node&)>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_VISITOR_H
