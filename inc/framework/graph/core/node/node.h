/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: node
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_H

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "framework/graph/core/node/legacy_node.h"

namespace ge {
class NodeWalker;
class NodeStore;
class NodeSpec;
class NodeConstInput;
class NodeModifier;
class NodeSubGraph;
class NodeCompatibler;
class NodeSerializer;

// NodeSpec must be first.
class GRAPH_API_EXPORT Node : public LegacyNode {
public:
    HAS_ROLE(NodeSpec);
    HAS_ROLE(NodeWalker);
    HAS_ROLE(NodeStore);
    HAS_ROLE(NodeConstInput);
    HAS_ROLE(NodeModifier);
    HAS_ROLE(NodeSubGraph);
    HAS_ROLE(NodeCompatibler);
    HAS_ROLE(NodeSerializer);

protected:
    explicit Node(NodeStore& store);
    virtual ~Node() = default;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_H
