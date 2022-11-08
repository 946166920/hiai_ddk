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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_WALKER_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_WALKER_H

#include <cstddef>
#include <memory>
#include <functional>

// inc/infra
#include "framework/infra/mem/maybe.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/node/node_visitor.h"
#include "framework/graph/core/edge/edge_visitor.h"

namespace ge {
class Node;
class Edge;
class NodeStore;

using EdgePred = std::function<bool(const Edge&)>;
EXPORT_ROLE(NodeWalker)
{
public:
    Node* InDataNode(std::size_t idx);

    hiai::Status ListInDataNodes(NodeVisitor visitor);
    hiai::Status ListInCtrlNodes(NodeVisitor visitor);
    hiai::Status ListInNodes(NodeVisitor visitor);

    Node* OutDataNode(std::size_t idx0, std::size_t idx1);
    hiai::Status ListOutDataNodes(std::size_t idx, NodeVisitor visitor);
    hiai::Status ListOutDataNodes(NodeVisitor visitor);
    hiai::Status ListOutCtrlNodes(NodeVisitor visitor);
    hiai::Status ListOutNodes(NodeVisitor visitor);

    hiai::Maybe<Edge> InDataEdge(std::size_t idx);
    hiai::Status ListInDataEdges(EdgeVisitor visitor);
    hiai::Status ListInDataEdgesNonConst(EdgeVisitor visitor);
    hiai::Status ListInCtrlEdges(EdgeVisitor visitor);
    hiai::Status ListInEdges(EdgeVisitor visitor);

    hiai::Status ListOutDataEdges(std::size_t idx, EdgeVisitor visitor);
    hiai::Status ListOutDataEdges(EdgeVisitor visitor);
    hiai::Status ListOutCtrlEdges(EdgeVisitor visitor);
    hiai::Status ListOutEdges(EdgeVisitor visitor);

    int32_t CountInDataEdges(const EdgePred& pred);
private:
    USE_ROLE(Node);
    USE_ROLE(NodeStore);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_WALKER_H