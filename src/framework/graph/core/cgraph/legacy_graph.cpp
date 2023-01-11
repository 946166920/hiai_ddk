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

#include "framework/graph/core/cgraph/legacy_graph.h"

#include <algorithm>

// api/framework
#include "graph/op/all_ops.h"

// inc/framework
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/debug/ge_log.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_topo_walker.h"
#include "framework/graph/core/cgraph/graph_sorter.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/utils/op_desc_utils.h"

// src/framework
#include "infra/base/assertion.h"
#include "graph/core/cgraph/graph_store.h"
#include "graph/core/node/node_maker.h"

// proto
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/interface/attr_map_def.h"

namespace ge {
LegacyGraph::LegacyGraph(GraphStore& store)
    : nodes_(store.AllNodes()),
      inputNodes_(store.InputNodes()),
      outputNodes_(store.OutputNodes()),
      inputs_order_(store.InputOrder())
{
}

LegacyGraph::~LegacyGraph()
{
}

hiai::IAttrMapDef* LegacyGraph::MutableAttrMapDef()
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, nullptr);

    return graphDef->mutable_attr();
}

const hiai::IAttrMapDef* LegacyGraph::GetAttrMapDef() const
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, nullptr);

    return graphDef->attr();
}

LegacyGraph::Vistor<NodePtr> LegacyGraph::GetDirectNodes() const
{
    return Vistor<NodePtr>(shared_from_this(), ROLE(GraphStore).AllNodes());
}

LegacyGraph::Vistor<NodePtr> LegacyGraph::GetAllNodes() const
{
    auto allSubGraphs = GetAllSubGraphs();
    std::vector<NodePtr> allNodes;
    std::for_each(allSubGraphs.begin(), allSubGraphs.end(), [&](const LegacyGraphPtr& subGraph) {
        auto subGraphAllNodes = subGraph->GetDirectNodes();
        allNodes.insert(allNodes.begin(), subGraphAllNodes.begin(), subGraphAllNodes.end());
    });

    allNodes.insert(allNodes.begin(), nodes_.begin(), nodes_.end());
    return Vistor<NodePtr>(shared_from_this(), allNodes);
}

LegacyGraph::Vistor<NodePtr> LegacyGraph::GetOutputNodes() const
{
    if (outputNodes_.size() != 0) {
        return Vistor<NodePtr>(shared_from_this(), outputNodes_);
    }

    vector<NodePtr> outputNodes;
    for (const auto& node : ROLE(GraphStore).AllNodes()) {
        if (node->ROLE(NodeSpec).Type() == NETOUTPUT) {
            outputNodes.push_back(node);
        }
    }

    return Vistor<NodePtr>(shared_from_this(), outputNodes);
}

namespace {
using NodePtrPred = std::function<bool(NodePtr)>;

NodePtr FindNodeByPred(const std::vector<NodePtr>& nodes, NodePtrPred pred)
{
    for (const auto& node : nodes) {
        if (pred(node)) {
            return node;
        }
    }
    return nullptr;
}
}

NodePtr LegacyGraph::FindNode(const std::string& name) const
{
    return FindNodeByPred(nodes_, [&name](NodePtr node) { return node->ROLE(NodeSpec).Name() == name; });
}

NodePtr LegacyGraph::FindNode(const int64_t id) const
{
    return FindNodeByPred(nodes_, [&id](NodePtr node) { return node->ROLE(NodeSpec).Id() == id; });
}

NodePtr LegacyGraph::AddNode(NodePtr node)
{
    return ROLE(GraphModifier).AddNode(node);
}

NodePtr LegacyGraph::AddNode(OpDescPtr op)
{
    Node* node = ROLE(GraphModifier).AddNode(op);
    HIAI_EXPECT_NOT_NULL_R(node, nullptr);
    return ROLE(GraphFinder).FindNodePtr(*node);
}

namespace {
std::vector<Edge> CacheConstInEdge(const Node& node)
{
    std::vector<Edge> edges;
    auto visitor = [&edges](Edge& edge) {
        NodeSpec& spec = edge.SrcNode().ROLE(NodeSpec);
        if (spec.Type() == hiai::op::Const::TYPE || spec.Type() == hiai::op::QuantizedConst::TYPE) {
            edges.push_back(edge);
        }
        return hiai::SUCCESS;
    };
    (void)node.ROLE(NodeWalker).ListInDataEdges(visitor);

    return edges;
}

hiai::Status RemoveConstInput(GraphModifier& modifier, const Node& node)
{
    std::vector<Edge> edges = CacheConstInEdge(node);

    for (const Edge& edge : edges) {
        HIAI_EXPECT_EXEC(modifier.RemoveEdge(edge));

        const Node& constInNode = edge.SrcNode();
        if (constInNode.ROLE(NodeSpec).OutEdgeSize() == 0) {
            HIAI_EXPECT_EXEC(modifier.RemoveNode(constInNode));
        }
    }

    return hiai::SUCCESS;
}
}

GraphErrCodeStatus LegacyGraph::RemoveNode(Node& node)
{
    HIAI_EXPECT_EXEC(RemoveConstInput(ROLE(GraphModifier), node));

    GraphBypasser& bypasser = ROLE(GraphBypasser);
    if (bypasser.PreCheck(node)) {
        return bypasser.ByPassNode(node);
    } else {
        return ROLE(GraphModifier).RemoveNode(node);
    }
}

GraphErrCodeStatus LegacyGraph::RemoveNode(NodePtr node)
{
    HIAI_EXPECT_NOT_NULL_R(node, hiai::INVALID_PARAM);

    return RemoveNode(*node);
}

namespace {
void CacheGraphSubGraphs(const ComputeGraph& graph, std::vector<ComputeGraphPtr>& allSubGraphs)
{
    std::vector<ComputeGraphPtr> directSubGraphs = graph.GetDirectSubGraphs();
    if (directSubGraphs.empty()) {
        return;
    }

    allSubGraphs.insert(allSubGraphs.end(), directSubGraphs.begin(), directSubGraphs.end());
    for (const auto& subGraph : directSubGraphs) {
        CacheGraphSubGraphs(*subGraph, allSubGraphs);
    }
}
}

std::vector<ComputeGraphPtr> LegacyGraph::GetAllSubGraphs() const
{
    std::vector<ComputeGraphPtr> allSubGraphs;
    CacheGraphSubGraphs(ROLE(ComputeGraph), allSubGraphs);

    return allSubGraphs;
}

vector<ComputeGraphPtr> LegacyGraph::GetDirectSubGraphs() const
{
    std::vector<ComputeGraphPtr> directSubGraphs;

    (void)ROLE(GraphListWalker).WalkAllNodes([&directSubGraphs](Node& node) {
        node.GetDirectSubGraphs(directSubGraphs);
        return hiai::SUCCESS;
    });

    return directSubGraphs;
}
} // namespace ge
