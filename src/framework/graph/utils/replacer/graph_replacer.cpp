/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: graph replacer
 */

#include "framework/graph/utils/replacer/graph_replacer.h"

#include <algorithm>

#include "graph/op/const_defs.h"

#include "infra/base/assertion.h"

#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_visitor.h"
#include "framework/graph/core/edge/edge_visitor.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/cgraph/graph_modifier.h"

namespace ge {
std::vector<Edge> GraphSrcBoundary::InDataEdges()
{
    std::vector<Edge> res;
    std::for_each(inputs_.begin(), inputs_.end(),
        [&res](Endpoint& ept) { res.push_back(*ept.Node().ROLE(NodeWalker).InDataEdge(ept.Idx())); });

    return res;
}

namespace {
std::vector<Endpoint> InBoundaryPeerEndpoints(const std::vector<Edge>& edges)
{
    std::vector<Endpoint> peerEpts;
    std::for_each(edges.begin(), edges.end(), [&peerEpts](const Edge& e) {
        if (std::find_if(peerEpts.begin(), peerEpts.end(), [&e](Endpoint& ept) {
                return &(e.SrcNode()) == &(ept.Node()) && e.SrcIdx() == ept.Idx();
            }) == peerEpts.end()) {
            peerEpts.emplace_back(e.Src());
        }
    });

    return peerEpts;
}
} // namespace

hiai::Status GraphSrcBoundary::ReplaceTo(const GraphDstBoundary& dst)
{
    HIAI_EXPECT_EXEC(RelinkInputs(dst));
    HIAI_EXPECT_EXEC(RelinkOutputs(dst));

    return RemoveAllNodes();
}

hiai::Status GraphSrcBoundary::RelinkInputs(const GraphDstBoundary& dst)
{
    auto inEdges = InDataEdges();
    auto linkToEpts = dst.IdleInEndpoints();

    auto peerEpts = InBoundaryPeerEndpoints(inEdges);
    HIAI_EXPECT_TRUE(peerEpts.size() == linkToEpts.size());

    std::for_each(inEdges.begin(), inEdges.end(), [this](Edge& e) {
        HIAI_EXPECT_EXEC(modifier_.RemoveEdge(e));
        return hiai::SUCCESS;
    });

    for (size_t i = 0; i < peerEpts.size(); ++i) {
        HIAI_EXPECT_EXEC(modifier_.AddEdge(peerEpts[i], linkToEpts[i]));
    }

    return hiai::SUCCESS;
}

hiai::Status GraphSrcBoundary::RelinkOutputs(const GraphDstBoundary& dst)
{
    auto linkFromEpts = dst.IdleOutEndpoints();
    HIAI_EXPECT_TRUE(linkFromEpts.size() == outputs_.size());

    for (size_t i = 0; i < outputs_.size(); ++i) {
        HIAI_EXPECT_EXEC(outputs_[i].Node().ROLE(NodeWalker).ListOutDataEdges(outputs_[i].Idx(), [&](Edge& e) {
            HIAI_EXPECT_EXEC(modifier_.RemoveEdge(e));
            HIAI_EXPECT_EXEC(modifier_.AddEdge(linkFromEpts[i], e.Dst()));
            return hiai::SUCCESS;
        }));
    }

    return hiai::SUCCESS;
}

namespace {
bool AlreadyMarked(const Node* n, const std::vector<Node*>& delNodes)
{
    return std::find(delNodes.begin(), delNodes.end(), n) != delNodes.end();
}

bool MatchedInBoundary(Node* node, std::vector<Endpoint>& inputs)
{
    return std::find_if(inputs.begin(), inputs.end(), [node](Endpoint& ept) { return &(ept.Node()) == node; }) !=
        inputs.end();
}

bool MatchedOutBoundary(Node* node, std::vector<Endpoint>& outputs)
{
    return std::find_if(outputs.begin(), outputs.end(), [node](Endpoint& ept) { return &(ept.Node()) == node; }) !=
        outputs.end();
}
} // namespace

void GraphSrcBoundary::MarkBackward(Node* n, std::vector<Node*>& delNodes)
{
    if (n->ROLE(NodeSpec).InDataEdgeSize() == 0) {
        return;
    }

    if (MatchedInBoundary(n, inputs_)) {
        return;
    }

    n->ROLE(NodeWalker).ListInDataNodes([&](Node& node) -> hiai::Status {
        MarkRecursive(&node, delNodes);
        return hiai::SUCCESS;
    });
}

void GraphSrcBoundary::MarkForward(Node* n, std::vector<Node*>& delNodes)
{
    if (n->ROLE(NodeSpec).OutDataEdgeSize() == 0) {
        return;
    }

    if (MatchedOutBoundary(n, outputs_)) {
        return;
    }

    n->ROLE(NodeWalker).ListOutDataNodes([&](Node& node) -> hiai::Status {
        MarkRecursive(&node, delNodes);
        return hiai::SUCCESS;
    });
}

void GraphSrcBoundary::MarkRecursive(Node* n, std::vector<Node*>& delNodes)
{
    if (AlreadyMarked(n, delNodes)) {
        return;
    }

    delNodes.emplace_back(n);

    MarkForward(n, delNodes);
    MarkBackward(n, delNodes);
}

void GraphSrcBoundary::MarkConstInputs(Node* n, std::vector<Node*>& delNodes)
{
    n->ROLE(NodeWalker).ListInDataNodes([&delNodes](Node& node) -> hiai::Status {
        if (node.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE) {
            delNodes.emplace_back(&node);
        }
        return hiai::SUCCESS;
    });
}

hiai::Status GraphSrcBoundary::RemoveAllNodes()
{
    std::set<Node*> startNodes;
    for (const auto& input : inputs_) {
        startNodes.emplace(&(input.Node()));
    }
    std::vector<Node*> delNodes;
    for (const auto n : startNodes) {
        MarkRecursive(n, delNodes);
        MarkConstInputs(n, delNodes);
    }

    return modifier_.RemoveNodes(delNodes);
}

std::vector<Endpoint> GraphDstBoundary::IdleInEndpoints() const
{
    return inputs_;
}

std::vector<Endpoint> GraphDstBoundary::IdleOutEndpoints() const
{
    return outputs_;
}
} // namespace ge