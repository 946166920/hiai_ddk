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

#include "framework/graph/core/node/node_spec.h"

#include <algorithm>

// api/framework
#include "graph/op/array_defs.h"
#include "graph/op/const_defs.h"

#include "infra/base/assertion.h"

// inc/framework
#include "framework/infra/log/log.h"
#include "framework/common/hcs_types.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/core/edge/endpoint.h"
#include "framework/graph/utils/checker/node_checker.h"

// src/framework
#include "graph/core/node/node_store.h"

namespace ge {
ComputeGraph& NodeSpec::OwnerComputeGraph() const
{
    return *ROLE(NodeStore).OwnerComputeGraphPtr();
}

ComputeGraphPtr NodeSpec::OwnerComputeGraphPtr() const
{
    return ROLE(NodeStore).OwnerComputeGraphPtr();
}

void NodeSpec::SetOwnerComputeGraph(const ComputeGraphPtr& graph)
{
    ROLE(NodeStore).SetOwnerComputeGraph(graph);
}

const OpDesc& NodeSpec::OpDesc() const
{
    return *(ROLE(NodeStore).OpDesc());
}

OpDesc& NodeSpec::OpDesc()
{
    return *(ROLE(NodeStore).OpDesc());
}

const std::string& NodeSpec::Name() const
{
    return ROLE(NodeStore).OpDesc()->GetName();
}

const std::string& NodeSpec::Type() const
{
    return ROLE(NodeStore).OpDesc()->GetType();
}

int64_t NodeSpec::Id() const
{
    return ROLE(NodeStore).OpDesc()->GetId();
}

namespace {
using MatchPred = std::function<bool(const OutDataAnchorPtr&)>;

std::size_t CountMatchSize(std::vector<InDataAnchorPtr>& anchors, const MatchPred& pred)
{
    std::size_t size = 0;
    for (const InDataAnchorPtr& anchor : anchors) {
        const OutDataAnchorPtr peer = anchor->GetPeerOutAnchor();
        if (peer == nullptr) {
            continue;
        }
        if (pred(peer)) {
            size++;
        }
    }

    return size;
}
} // namespace

std::size_t NodeSpec::InDataEdgeSize() const
{
    auto pred = [](const OutDataAnchorPtr& anchor) {
        (void)anchor;
        return true;
    };
    return CountMatchSize(ROLE(NodeStore).InDataAnchors(), pred);
}

std::size_t NodeSpec::InNonConstSize() const
{
    auto pred = [](const OutDataAnchorPtr& anchor) {
        return (anchor->GetOwnerNode()->ROLE(NodeSpec).Type() != hiai::op::Const::TYPE &&
            anchor->GetOwnerNode()->ROLE(NodeSpec).Type() != hiai::op::QuantizedConst::TYPE);
    };
    return CountMatchSize(ROLE(NodeStore).InDataAnchors(), pred);
}

std::size_t NodeSpec::InCtrlEdgeSize() const
{
    const InControlAnchorPtr& anchor = ROLE(NodeStore).InCtrlAnchor();
    return anchor->GetPeerOutControlAnchors().size();
}

std::size_t NodeSpec::InEdgeSize() const
{
    return InDataEdgeSize() + InCtrlEdgeSize();
}

std::size_t NodeSpec::OutDataEdgeSize() const
{
    std::size_t size = 0;
    for (const OutDataAnchorPtr& anchor : ROLE(NodeStore).OutDataAnchors()) {
        size += anchor->GetPeerInDataAnchors().size();
    }

    return size;
}

std::size_t NodeSpec::OutCtrlEdgeSize() const
{
    const OutControlAnchorPtr& anchor = ROLE(NodeStore).OutCtrlAnchor();
    return anchor->GetPeerInControlAnchors().size();
}

std::size_t NodeSpec::OutEdgeSize() const
{
    return OutDataEdgeSize() + OutCtrlEdgeSize();
}

bool NodeSpec::HasEdge() const
{
    return InEdgeSize() > 0 || OutEdgeSize() > 0;
}

bool NodeSpec::IsInOutAnchorValid() const
{
    /*lint -e571*/
    std::size_t inDescSize = static_cast<std::size_t>(OpDesc().GetInputsDescSize());
    std::size_t outDescSize = static_cast<std::size_t>(OpDesc().GetOutputsSize());
    /*lint +e571*/
    std::size_t inAnchorSize = ROLE(NodeStore).InDataAnchors().size();
    std::size_t outAnchorSize = ROLE(NodeStore).OutDataAnchors().size();

    return inDescSize == inAnchorSize && outDescSize == outAnchorSize;
}

bool NodeSpec::IsInputFullyLinked() const
{
    const auto& type = Type();
    if (NodeChecker::IsNonInputType(type)) {
        return true;
    }

    for (std::size_t i = 0; i < ROLE(NodeStore).InDataAnchors().size(); i++) {
        if (NodeChecker::IsOptionalInput(type, static_cast<uint32_t>(i))) {
            continue;
        }

        const InDataAnchorPtr& inAnchor = ROLE(NodeStore).InDataAnchor(i);
        const OutDataAnchorPtr& outAnchor = inAnchor->GetPeerOutAnchor();
        if (outAnchor == nullptr) {
            FMK_LOGE("Node %s %s %zu not link input.", Name().c_str(), Type().c_str(), i);
            return false;
        }
    }

    return true;
}

hiai::Status NodeSpec::IsValid() const
{
    HIAI_EXPECT_TRUE(IsInOutAnchorValid());

    return hiai::SUCCESS;
}

std::vector<Endpoint> NodeSpec::IdleInputEndpoints() const
{
    std::vector<Endpoint> res;
    for (size_t i = 0; i < ROLE(NodeStore).InDataAnchors().size(); ++i) {
        if (ROLE(NodeStore).InDataAnchor(i)->GetPeerOutAnchor() == nullptr) {
            res.emplace_back(ROLE(Node), i);
        }
    }

    return res;
}

std::vector<Endpoint> NodeSpec::IdleOutputEndpoints() const
{
    std::vector<Endpoint> res;
    for (size_t i = 0; i < ROLE(NodeStore).OutDataAnchors().size(); ++i) {
        for (auto const& anchor : ROLE(NodeStore).OutDataAnchors().at(i)->GetPeerInDataAnchors()) {
            if (anchor == nullptr) {
                res.emplace_back(ROLE(Node), i);
                break;
            }
        }
    }

    return res;
}

std::vector<Node*> NodeSpec::ConstInputs() const
{
    std::vector<Node*> constNodes;
    auto& anchors = ROLE(NodeStore).InDataAnchors();
    std::for_each(anchors.begin(), anchors.end(), [&constNodes](const InDataAnchorPtr& anchor) {
        HIAI_EXPECT_NOT_NULL_VOID(anchor);

        auto peer = anchor->GetPeerOutAnchor();
        HIAI_EXPECT_NOT_NULL_VOID(peer);

        auto peerNode = peer->GetOwnerNode();
        HIAI_EXPECT_NOT_NULL_VOID(peerNode);
        if (peerNode->ROLE(NodeSpec).Type() == hiai::op::Const::TYPE) {
            constNodes.emplace_back(peerNode.get());
        }
    });

    return constNodes;
}

bool NodeSpec::IsDataOp() const
{
    return Type() == hiai::op::Data::TYPE;
}

bool NodeSpec::IsConstOp() const
{
    return Type() == hiai::op::Const::TYPE;
}

bool NodeSpec::IsAippConfig() const
{
    return Type() == ge::AIPP_CONFIG;
}

bool NodeSpec::IsNetOutputOp() const
{
    return Type() == ge::NETOUTPUT;
}

bool NodeSpec::IsGraphOp() const
{
    return Type() == ge::GRAPH_OP_TYPE;
}
} // namespace ge
