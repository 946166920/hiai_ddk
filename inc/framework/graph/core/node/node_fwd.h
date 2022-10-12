/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: node fwd
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_FWD_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_FWD_H

#include <memory>

namespace ge {
class Node;
using NodePtr = std::shared_ptr<Node>;
using ConstNodePtr = std::shared_ptr<const Node>;
using NodeRef = std::weak_ptr<Node>;
using ConstNode = const Node;

class OpDesc;
using OpDescPtr = std::shared_ptr<OpDesc>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_FWD_H