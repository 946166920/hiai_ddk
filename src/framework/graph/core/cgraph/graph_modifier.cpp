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

#include "framework/graph/core/cgraph/graph_modifier.h"

#include <algorithm>

#include "infra/base/assertion.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_notifier.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/endpoint.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"
#include "graph/core/node/node_store.h"
#include "graph/core/node/node_maker.h"
#include "graph/op/const_defs.h"

namespace ge {
Node* GraphModifier::AddNode(const OpDescPtr& op)
{
    HIAI_EXPECT_NOT_NULL_R(op, nullptr);

    auto node = NodeMaker::Make(op, ROLE(ComputeGraph));
    HIAI_EXPECT_NOT_NULL_R(node, nullptr);

    ROLE(GraphStore).AddNode(node);
    return node.get();
}

NodePtr GraphModifier::AddNode(NodePtr node)
{
    HIAI_EXPECT_NOT_NULL_R(node, nullptr);
    HIAI_EXPECT_TRUE_R(!ROLE(GraphStore).HasNode(*node), nullptr);

    NodeSpec& spec = node->ROLE(NodeSpec);
    spec.OpDesc().SetId(NewNodeId());
    spec.SetOwnerComputeGraph(ROLE(ComputeGraph));

    ROLE(GraphStore).AddNode(node);
    return node;
}

Node* GraphModifier::AddNodeFront(const OpDescPtr& op)
{
    HIAI_EXPECT_NOT_NULL_R(op, nullptr);

    NodePtr node = NodeMaker::Make(op, ROLE(ComputeGraph));
    HIAI_EXPECT_NOT_NULL_R(node, nullptr);

    ROLE(GraphStore).AddNodeFront(node);

    return node.get();
}

hiai::Status GraphModifier::RemoveNode(const Node& node)
{
    HIAI_EXPECT_TRUE_R(ROLE(GraphStore).HasNode(node), hiai::FAILURE);

    node.ROLE(NodeStore).RemoveEdges();

    return ROLE(GraphStore).RemoveNode(node);
}

hiai::Status GraphModifier::RemoveNodeWithConstInputs(const Node& node)
{
    HIAI_EXPECT_TRUE_R(ROLE(GraphStore).HasNode(node), hiai::FAILURE);

    std::vector<Edge> constEdges;
    auto visitor = [&constEdges](Edge& edge) {
        NodeSpec& spec = edge.SrcNode().ROLE(NodeSpec);
        if (spec.Type() == hiai::op::Const::TYPE || spec.Type() == hiai::op::QuantizedConst::TYPE) {
            constEdges.push_back(edge);
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(node.ROLE(NodeWalker).ListInDataEdges(visitor));

    for (const Edge& edge : constEdges) {
        HIAI_EXPECT_EXEC(RemoveEdge(edge));
        auto& src = edge.SrcNode();
        if (src.ROLE(NodeSpec).OutEdgeSize() == 0) {
            HIAI_EXPECT_EXEC(ROLE(GraphStore).RemoveNode(src));
        }
    }

    node.ROLE(NodeStore).RemoveEdges();

    return ROLE(GraphStore).RemoveNode(node);
}

hiai::Status GraphModifier::RemoveNodes(const std::vector<Node*>& node)
{
    std::for_each(node.begin(), node.end(), [this](const Node* n) -> hiai::Status {
        HIAI_EXPECT_EXEC(RemoveNode(*n));
        return hiai::SUCCESS;
    });

    return hiai::SUCCESS;
}

hiai::Status GraphModifier::RemoveOutputNode(const Node& node)
{
    HIAI_EXPECT_TRUE_R(ROLE(GraphStore).HasNode(node), hiai::FAILURE);

    return ROLE(GraphStore).RemoveOutputNode(node);
}

namespace {
hiai::Status AddNodePtrByNode(Node& node, std::vector<NodePtr>& nodes, GraphFinder& finder)
{
    NodePtr ptr = finder.FindNodePtr(node);
    HIAI_EXPECT_NOT_NULL(ptr);

    nodes.push_back(ptr);
    return hiai::SUCCESS;
}
} // namespace

hiai::Status GraphModifier::AddInput(Node& node)
{
    auto& nodes = ROLE(GraphStore).InputNodes();
    return AddNodePtrByNode(node, nodes, ROLE(GraphFinder));
}

hiai::Status GraphModifier::AddOutput(Node& node)
{
    auto& nodes = ROLE(GraphStore).OutputNodes();
    return AddNodePtrByNode(node, nodes, ROLE(GraphFinder));
}

hiai::Status GraphModifier::SetInputs(const std::vector<Node*>& nodes)
{
    return ROLE(GraphStore).SetInputs(nodes);
}

hiai::Status GraphModifier::SetOutputs(const std::vector<Node*>& nodes)
{
    return ROLE(GraphStore).SetOutputs(nodes);
}

hiai::Status GraphModifier::SetInputOrder(std::map<std::string, uint32_t> order)
{
    ROLE(GraphStore).InputOrder() = std::move(order);
    return hiai::SUCCESS;
}

hiai::Status GraphModifier::AddEdge(const Endpoint& src, const Endpoint& dst)
{
    HIAI_EXPECT_TRUE(&src.Node() != &dst.Node());
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(src.Node()));
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(dst.Node()));

    auto& srcStore = src.Node().ROLE(NodeStore);
    auto& dstStore = dst.Node().ROLE(NodeStore);
    HIAI_EXPECT_EXEC(NodeStore::AddEdge(srcStore, src.Idx(), dstStore, dst.Idx()));

    ROLE(GraphNotifier).AddEdge({src, dst});
    return hiai::SUCCESS;
}

hiai::Status GraphModifier::RemoveEdge(const Edge& edge)
{
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(edge.SrcNode()));
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(edge.DstNode()));

    auto& srcStore = edge.SrcNode().ROLE(NodeStore);
    auto& dstStore = edge.DstNode().ROLE(NodeStore);
    HIAI_EXPECT_EXEC(NodeStore::RemoveEdge(srcStore, edge.SrcIdx(), dstStore, edge.DstIdx()));

    ROLE(GraphNotifier).DelEdge(edge);
    return hiai::SUCCESS;
}

hiai::Status GraphModifier::RemoveEdge(const Node& src, const Node& dst)
{
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(src));
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(dst));

    return src.ROLE(NodeStore).RemoveEdge(src, dst);
}

namespace {
bool HasSingleIdleInputAndSingleOutput(GraphStore& graphStore, const Node& node)
{
    HIAI_EXPECT_TRUE(graphStore.HasNode(node));
    auto& nodeSpec = node.ROLE(NodeSpec);
    return (nodeSpec.IdleInputEndpoints().size() == 1 && nodeSpec.OpDesc().GetOutputsSize() == 1);
}

bool HasInputDataNode(GraphStore& graphStore, const Node& node, size_t idx)
{
    HIAI_EXPECT_TRUE(graphStore.HasNode(node));
    return node.ROLE(NodeWalker).InDataEdge(idx).Exist();
}

bool HasOutputDataNode(GraphStore& graphStore, const Node& node, size_t idx)
{
    HIAI_EXPECT_TRUE(graphStore.HasNode(node));
    return (node.ROLE(NodeSpec).OutDataEdgeSize() > 0 && idx < node.ROLE(NodeSpec).OpDesc().GetOutputsSize());
}

Endpoint GetInputEdgeSrcEndpoint(const Node& node, size_t idx)
{
    auto inDataEdge = node.ROLE(NodeWalker).InDataEdge(idx);
    return Endpoint(inDataEdge->SrcNode(), inDataEdge->SrcIdx());
}
} // namespace

hiai::Status GraphModifier::MoveNodeFrom(ComputeGraph& graph, Node& target)
{
    HIAI_EXPECT_TRUE(!ROLE(GraphStore).HasNode(target));

    NodePtr targetPtr = graph.ROLE(GraphFinder).FindNodePtr(target);
    HIAI_EXPECT_NOT_NULL(targetPtr);

    HIAI_EXPECT_EXEC(graph.ROLE(GraphBypasser).ByPassNode(target));
    HIAI_EXPECT_NOT_NULL(AddNode(targetPtr));
    return hiai::SUCCESS;
}

hiai::Status GraphModifier::InsertBefore(Node& target, size_t idx, Node& insertNode)
{
    HIAI_EXPECT_TRUE(HasInputDataNode(ROLE(GraphStore), target, idx));
    HIAI_EXPECT_TRUE(HasSingleIdleInputAndSingleOutput(ROLE(GraphStore), insertNode));

    Endpoint srcEndpoint = GetInputEdgeSrcEndpoint(target, idx);
    Endpoint dstEndpoint(target, idx);
    HIAI_EXPECT_EXEC(RemoveEdge(srcEndpoint, dstEndpoint));
    HIAI_EXPECT_EXEC(AddEdge(srcEndpoint, Endpoint(insertNode, 0)));
    return AddEdge(Endpoint(insertNode, 0), dstEndpoint);
}

hiai::Status GraphModifier::InsertAfter(Node& target, size_t idx, Node& insertNode)
{
    HIAI_EXPECT_TRUE(HasSingleIdleInputAndSingleOutput(ROLE(GraphStore), insertNode));

    if (HasOutputDataNode(ROLE(GraphStore), target, idx)) {
        auto relinkOutputDataNode = [&insertNode, this](Edge& outputDataEdge) {
            HIAI_EXPECT_EXEC(RemoveEdge(outputDataEdge));
            return AddEdge(Endpoint(insertNode, 0), Endpoint(outputDataEdge.DstNode(), outputDataEdge.DstIdx()));
        };
        HIAI_EXPECT_EXEC(target.ROLE(NodeWalker).ListOutDataEdges(idx, std::move(relinkOutputDataNode)));
    }

    return AddEdge(Endpoint(target, idx), Endpoint(insertNode, 0));
}

hiai::Status GraphModifier::RemoveEdge(const Endpoint& src, const Endpoint& dst)
{
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(src.Node()));
    HIAI_EXPECT_TRUE(ROLE(GraphStore).HasNode(dst.Node()));

    auto& srcNodeStore = src.Node().ROLE(NodeStore);
    auto& dstNodeStore = dst.Node().ROLE(NodeStore);

    return NodeStore::RemoveEdge(srcNodeStore, src.Idx(), dstNodeStore, dst.Idx());
}

hiai::Status GraphModifier::MoveNode(Node& node, GraphModifier& newGraph)
{
    auto nodePtr = ROLE(GraphStore).MoveNode(node);
    HIAI_EXPECT_NOT_NULL(nodePtr);

    HIAI_EXPECT_TRUE(!newGraph.ROLE(GraphStore).HasNode(*nodePtr.get()));

    NodeSpec& spec = nodePtr->ROLE(NodeSpec);
    spec.SetOwnerComputeGraph(newGraph.ROLE(ComputeGraph));

    newGraph.ROLE(GraphStore).AddNode(nodePtr);

    return hiai::SUCCESS;
}
} // namespace ge