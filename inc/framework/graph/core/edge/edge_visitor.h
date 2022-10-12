/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: edge visitor
 */

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_EDGE_VISITOR_H
#define FRAMEWORK_GRAPH_CORE_EDGE_EDGE_VISITOR_H

#include <functional>

namespace ge {
class Edge;

using EdgeVisitor = std::function<hiai::Status(Edge&)>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_EDGE_VISITOR_H
