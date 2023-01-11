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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_MODIFIER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_MODIFIER_H

#include <vector>
#include <map>
#include <string>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/cgraph/graph_fwd.h"

namespace ge {
class Edge;
class Endpoint;
class GraphStore;
class GraphListWalker;
class GraphTopoWalker;
class GraphNotifier;
class GraphFinder;

EXPORT_ROLE(GraphModifier)
{
public:
    Node* AddNode(const OpDescPtr& op);
    NodePtr AddNode(NodePtr node);
    Node* AddNodeFront(const OpDescPtr& op);

    hiai::Status RemoveNode(const Node& node);
    hiai::Status RemoveNodeWithConstInputs(const Node& node);
    hiai::Status RemoveNodes(const std::vector<Node*>& node);
    hiai::Status RemoveOutputNode(const Node& node);
    hiai::Status AddInput(Node& node);
    hiai::Status AddOutput(Node& node);

    hiai::Status SetInputs(const std::vector<Node*>& nodes);
    hiai::Status SetOutputs(const std::vector<Node*>& nodes);
    hiai::Status SetInputOrder(std::map<std::string, uint32_t> order);

    hiai::Status AddEdge(const Endpoint& src, const Endpoint& dst);
    hiai::Status RemoveEdge(const Edge& edge);
    hiai::Status RemoveEdge(const Node& src, const Node& dst);
    hiai::Status RemoveEdge(const Endpoint& src, const Endpoint& dst);
    hiai::Status MoveNode(Node& node, GraphModifier& newGraph);

    hiai::Status MoveNodeFrom(ComputeGraph& graph, Node& target);

    /**
     * @brief Insert a single-idl-input single-output node before the idx input of the target node
     * @param [in] target target node
     * @param [in] idx the index input of the target node
     * @param [in] insertNode a single-idl-input single-output node to be inserted.
     * @return hiai::Status SUCCESS : insert OK, otherwise insert failed.
     **/
    hiai::Status InsertBefore(Node& target, size_t idx, Node& insertNode);

    /**
     * @brief Insert a single-idl-input single-output node after the idx output of the target node
     * @param [in] target target node
     * @param [in] idx the index output of the target node
     * @param [in] insertNode a single-idl-input single-output node to be inserted.
     * @return hiai::Status SUCCESS : insert OK, otherwise insert failed.
     **/
    hiai::Status InsertAfter(Node& target, size_t idx, Node& insertNode);

private:
    // Note: USE_ROLE(ComputeGraph) return ComputeGraph&, but MakeNode need ComputeGraphPtr.
    // Delete the interface after shared_ptr cleared.
    virtual ComputeGraphPtr GetComputeGraph() = 0;

private:
    USE_ROLE(GraphStore);
    USE_ROLE(GraphFinder);
    USE_ROLE(GraphListWalker);
    USE_ROLE(GraphTopoWalker);
    USE_ROLE(GraphNotifier);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_MODIFIER_H