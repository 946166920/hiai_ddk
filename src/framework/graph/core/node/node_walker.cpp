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
#include "framework/graph/core/node/node_walker.h"

// inc/api
#include "graph/op/const_defs.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/anchor.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/node/node_store.h"

namespace ge {
Node* NodeWalker::InDataNode(std::size_t idx)
{
    const auto& inDataAnchors = ROLE(NodeStore).InDataAnchors();
    HIAI_EXPECT_TRUE_R(idx < inDataAnchors.size(), nullptr);
    const InDataAnchorPtr& inDataAnchor = inDataAnchors[idx];

    const OutDataAnchorPtr& peerAnchor = inDataAnchor->GetPeerOutAnchor();
    HIAI_EXPECT_NOT_NULL_R(peerAnchor, nullptr);

    return peerAnchor->GetOwnerNode().get();
}

hiai::Status NodeWalker::ListInDataNodes(NodeVisitor visitor)
{
    for (const auto& inDataAnchor : ROLE(NodeStore).InDataAnchors()) {
        const auto& peer = inDataAnchor->GetPeerOutAnchor();
        if (peer != nullptr) {
            Node& node = *peer->GetOwnerNode();
            HIAI_EXPECT_EXEC(visitor(node));
        }
    }
    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListInCtrlNodes(NodeVisitor visitor)
{
    const InControlAnchorPtr& inCtrlAnchor = ROLE(NodeStore).InCtrlAnchor();
    for (const OutControlAnchorPtr& peerAnchor : inCtrlAnchor->GetPeerOutControlAnchors()) {
        Node& node = *peerAnchor->GetOwnerNode();
        HIAI_EXPECT_EXEC(visitor(node));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListInNodes(NodeVisitor visitor)
{
    HIAI_EXPECT_EXEC(ListInDataNodes(visitor));
    HIAI_EXPECT_EXEC(ListInCtrlNodes(visitor));

    return hiai::SUCCESS;
}

Node* NodeWalker::OutDataNode(std::size_t idx0, std::size_t idx1)
{
    const auto& outAnchors = ROLE(NodeStore).OutDataAnchors();
    HIAI_EXPECT_TRUE_R(idx0 < outAnchors.size(), nullptr);

    const OutDataAnchorPtr& outAnchor = outAnchors[idx0];
    const auto& inAnchors = outAnchor->GetPeerInDataAnchors();
    HIAI_EXPECT_TRUE_R(idx1 < inAnchors.size(), nullptr);

    return inAnchors.at(idx1)->GetOwnerNode().get();
}

hiai::Status NodeWalker::ListOutDataNodes(std::size_t idx, NodeVisitor visitor)
{
    const auto& outDataAnchors = ROLE(NodeStore).OutDataAnchors();
    HIAI_EXPECT_TRUE(idx < outDataAnchors.size());
    const OutDataAnchorPtr& outAnchor = outDataAnchors[idx];

    for (const InDataAnchorPtr& peer : outAnchor->GetPeerInDataAnchors()) {
        Node& node = *peer->GetOwnerNode();
        HIAI_EXPECT_EXEC(visitor(node));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListOutDataNodes(NodeVisitor visitor)
{
    for (const OutDataAnchorPtr& outDataAnchor : ROLE(NodeStore).OutDataAnchors()) {
        for (const InDataAnchorPtr& peerAnchor : outDataAnchor->GetPeerInDataAnchors()) {
            Node& node = *peerAnchor->GetOwnerNode();
            HIAI_EXPECT_EXEC(visitor(node));
        }
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListOutCtrlNodes(NodeVisitor visitor)
{
    const OutControlAnchorPtr& outCtrlAnchor = ROLE(NodeStore).OutCtrlAnchor();
    for (const InControlAnchorPtr& peerAnchor : outCtrlAnchor->GetPeerInControlAnchors()) {
        Node& node = *peerAnchor->GetOwnerNode();
        HIAI_EXPECT_EXEC(visitor(node));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListOutNodes(NodeVisitor visitor)
{
    HIAI_EXPECT_EXEC(ListOutDataNodes(visitor));
    HIAI_EXPECT_EXEC(ListOutCtrlNodes(visitor));

    return hiai::SUCCESS;
}

hiai::Maybe<Edge> NodeWalker::InDataEdge(std::size_t idx)
{
    const auto& inDataAnchors = ROLE(NodeStore).InDataAnchors();
    if (idx < inDataAnchors.size()) {
        const auto& dstAnchor = inDataAnchors[idx];
        const auto srcAnchor = dstAnchor->GetPeerOutAnchor();
        if (srcAnchor != nullptr) {
            Node& srcNode = *srcAnchor->GetOwnerNode().get();
            return hiai::Maybe<Edge>(Edge(srcNode, srcAnchor->GetIdx(), ROLE(Node), dstAnchor->GetIdx()));
        }
    }

    return hiai::Maybe<Edge>(hiai::NULL_MAYBE);
}

namespace {
using MatchPred = std::function<bool(const OutDataAnchorPtr&)>;

hiai::Status VisitMatchEdges(
    Node& dstNode, const std::vector<InDataAnchorPtr>& anchors, const MatchPred& pred, EdgeVisitor& visitor)
{
    for (const auto& anchor : anchors) {
        const auto& peer = anchor->GetPeerOutAnchor();
        if (pred(peer)) {
            Node& srcNode = *peer->GetOwnerNode();
            Edge edge(srcNode, peer->GetIdx(), dstNode, anchor->GetIdx());
            HIAI_EXPECT_EXEC(visitor(edge));
        }
    }

    return hiai::SUCCESS;
}
} // namespace

hiai::Status NodeWalker::ListInDataEdges(EdgeVisitor visitor)
{
    Node& dstNode = ROLE(Node);
    auto pred = [](const OutDataAnchorPtr& anchor) { return anchor != nullptr; };
    return VisitMatchEdges(dstNode, ROLE(NodeStore).InDataAnchors(), pred, visitor);
}

hiai::Status NodeWalker::ListInDataEdgesNonConst(EdgeVisitor visitor)
{
    Node& dstNode = ROLE(Node);
    auto pred = [](const OutDataAnchorPtr& anchor) {
        return anchor != nullptr && anchor->GetOwnerNode()->ROLE(NodeSpec).Type() != hiai::op::Const::TYPE &&
            anchor->GetOwnerNode()->ROLE(NodeSpec).Type() != hiai::op::QuantizedConst::TYPE;
    };
    return VisitMatchEdges(dstNode, ROLE(NodeStore).InDataAnchors(), pred, visitor);
}

hiai::Status NodeWalker::ListInCtrlEdges(EdgeVisitor visitor)
{
    Node& dstNode = ROLE(Node);
    const InControlAnchorPtr& dstAnchor = ROLE(NodeStore).InCtrlAnchor();

    for (const auto& srcAnchor : dstAnchor->GetPeerOutControlAnchors()) {
        Node& srcNode = *srcAnchor->GetOwnerNode().get();
        Edge edge(srcNode, srcAnchor->GetIdx(), dstNode, dstAnchor->GetIdx());

        HIAI_EXPECT_EXEC(visitor(edge));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListInEdges(EdgeVisitor visitor)
{
    HIAI_EXPECT_EXEC(ListInDataEdges(visitor));
    HIAI_EXPECT_EXEC(ListInCtrlEdges(visitor));

    return hiai::SUCCESS;
}

namespace {
hiai::Status ListOutDataEdgesWithAnchor(Node& srcNode, const OutDataAnchorPtr& srcAnchor, EdgeVisitor visitor)
{
    for (const auto& dstAnchor : srcAnchor->GetPeerInDataAnchors()) {
        Node& dstNode = *dstAnchor->GetOwnerNode();
        Edge edge(srcNode, srcAnchor->GetIdx(), dstNode, dstAnchor->GetIdx());

        HIAI_EXPECT_EXEC(visitor(edge));
    }

    return hiai::SUCCESS;
}
} // namespace

hiai::Status NodeWalker::ListOutDataEdges(std::size_t idx, EdgeVisitor visitor)
{
    const auto& outDataAnchors = ROLE(NodeStore).OutDataAnchors();
    HIAI_EXPECT_TRUE(idx < outDataAnchors.size());

    const auto& srcAnchor = outDataAnchors.at(idx);
    return ListOutDataEdgesWithAnchor(ROLE(Node), srcAnchor, visitor);
}

hiai::Status NodeWalker::ListOutDataEdges(EdgeVisitor visitor)
{
    Node& srcNode = ROLE(Node);

    for (const auto& srcAnchor : ROLE(NodeStore).OutDataAnchors()) {
        HIAI_EXPECT_EXEC(ListOutDataEdgesWithAnchor(srcNode, srcAnchor, visitor));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListOutCtrlEdges(EdgeVisitor visitor)
{
    Node& srcNode = ROLE(Node);
    const OutControlAnchorPtr& srcAnchor = ROLE(NodeStore).OutCtrlAnchor();

    for (const auto& dstAnchor : srcAnchor->GetPeerInControlAnchors()) {
        Node& dstNode = *dstAnchor->GetOwnerNode().get();
        Edge edge(srcNode, srcAnchor->GetIdx(), dstNode, dstAnchor->GetIdx());

        HIAI_EXPECT_EXEC(visitor(edge));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeWalker::ListOutEdges(EdgeVisitor visitor)
{
    HIAI_EXPECT_EXEC(ListOutDataEdges(visitor));
    HIAI_EXPECT_EXEC(ListOutCtrlEdges(visitor));

    return hiai::SUCCESS;
}

int32_t NodeWalker::CountInDataEdges(const EdgePred& pred)
{
    int32_t num = 0;
    const auto edgeVisitor = [&num, &pred](Edge& e) {
        if (pred(e)) {
            num++;
        }
        return hiai::SUCCESS;
    };
    (void)ListInDataEdges(edgeVisitor);
    return num;
}
} // namespace ge