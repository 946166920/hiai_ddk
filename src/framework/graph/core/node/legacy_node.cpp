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

#include "framework/graph/core/node/legacy_node.h"

// api/framework
#include "graph/tensor.h"

#include "infra/base/securestl.h"

#include "framework/graph/debug/ge_log.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/edge/anchor.h"

// src/framework
#include "graph/core/node/node_store.h"

using namespace std;

namespace ge {
LegacyNode::LegacyNode(NodeStore& store)
    : op_(store.OpDesc()),
      inDataAnchors_(store.InDataAnchors()),
      outDataAnchors_(store.OutDataAnchors()),
      inControlAnchor_(store.InCtrlAnchor()),
      outControlAnchor_(store.OutCtrlAnchor())
{
}

GraphErrCodeStatus LegacyNode::AddLinkFrom(Node& input_node, uint32_t index)
{
    // input_node ---> this
    const auto& out_anchors = input_node.GetAllOutDataAnchors();
    if (out_anchors.size() != 1) {
        FMK_LOGE("out_anchor size is:%zu, only support 1", out_anchors.size());
        return GRAPH_PARAM_INVALID;
    }

    op_->UpdateInputDesc(index, input_node.ROLE(NodeSpec).OpDesc().GetOutputDesc(0));
    if (index >= inDataAnchors_.size()) {
        FMK_LOGE("index is out of the range of inDataAnchors_");
        return GRAPH_FAILED;
    }
    (void)out_anchors.at(0)->LinkTo(inDataAnchors_[index]);

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus LegacyNode::AddLinkFrom(NodePtr input_node, uint32_t index)
{
    GE_CHECK_NOTNULL(input_node);
    // input_node ---> this
    return AddLinkFrom(*input_node, index);
}

GraphErrCodeStatus LegacyNode::AddLinkFrom(Node& input_node)
{
    // input_node ---> this
    const auto& out_anchors = input_node.GetAllOutDataAnchors();
    if (out_anchors.size() != 1) {
        FMK_LOGE("out_anchor size is:%zu, only support 1", out_anchors.size());
        return GRAPH_PARAM_INVALID;
    }

    op_->AddInputDesc(input_node.ROLE(NodeSpec).OpDesc().GetOutputDesc(0));
    shared_ptr<InDataAnchor> archor = hiai::make_shared_nothrow<InDataAnchor>(
        std::static_pointer_cast<Node>(shared_from_this()), inDataAnchors_.size());
    if (archor == nullptr) {
        FMK_LOGE("out_anchor size is:%zu, make archor failed", out_anchors.size());
        return GRAPH_FAILED;
    }
    inDataAnchors_.push_back(archor);
    (void)out_anchors.at(0)->LinkTo(inDataAnchors_.back());

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus LegacyNode::AddLinkFrom(NodePtr input_node)
{
    GE_CHECK_NOTNULL(input_node);
    // input_node ---> this
    return AddLinkFrom(*input_node);
}

GraphErrCodeStatus LegacyNode::AddLinkFrom(OutDataAnchorPtr peerOutAnchor)
{
    GE_CHECK_NOTNULL(peerOutAnchor);
    GE_CHECK_NOTNULL(peerOutAnchor->GetOwnerNode());
    // input_node ---> this
    op_->AddInputDesc(peerOutAnchor->GetOwnerNode()->GetOpDesc()->GetOutputDesc(peerOutAnchor->GetIdx()));
    shared_ptr<InDataAnchor> archor = hiai::make_shared_nothrow<InDataAnchor>(
        std::static_pointer_cast<Node>(shared_from_this()), inDataAnchors_.size());
    if (archor == nullptr) {
        FMK_LOGE("make archor failed");
        return GRAPH_FAILED;
    }
    inDataAnchors_.push_back(archor);
    (void)peerOutAnchor->LinkTo(inDataAnchors_.back());

    return GRAPH_SUCCESS;
}

LegacyNode::Vistor<InDataAnchorPtr> LegacyNode::GetAllInDataAnchors() const
{
    return Vistor<InDataAnchorPtr>(std::static_pointer_cast<const Node>(shared_from_this()), inDataAnchors_);
}

LegacyNode::Vistor<OutDataAnchorPtr> LegacyNode::GetAllOutDataAnchors() const
{
    return Vistor<OutDataAnchorPtr>(std::static_pointer_cast<const Node>(shared_from_this()), outDataAnchors_);
}

InDataAnchorPtr LegacyNode::GetInDataAnchor(int idx) const
{
    return ROLE(NodeStore).InDataAnchor(static_cast<unsigned int>(idx));
}

GraphErrCodeStatus LegacyNode::RemoveInDataAnchor(int idx)
{
    if (idx < 0 || idx >= static_cast<int>(inDataAnchors_.size())) {
        FMK_LOGE("node doesn't have %d th inDataAnchor", idx);
        return GRAPH_FAILED;
    }
    const auto& it = inDataAnchors_.cbegin() + idx;
    inDataAnchors_.erase(it);
    return GRAPH_SUCCESS;
}

OutDataAnchorPtr LegacyNode::GetOutDataAnchor(int idx) const
{
    return ROLE(NodeStore).OutDataAnchor(static_cast<unsigned int>(idx));
}

InControlAnchorPtr LegacyNode::GetInControlAnchor() const
{
    return inControlAnchor_;
}

OutControlAnchorPtr LegacyNode::GetOutControlAnchor() const
{
    return outControlAnchor_;
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetInDataNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& p : inDataAnchors_) {
        GE_CHK_BOOL_EXEC((p != nullptr), continue, "inDataAnchor is nullptr");
        const auto& pAnchor = p->GetPeerOutAnchor();
        if (pAnchor == nullptr) {
            continue;
        }
        const auto& node = pAnchor->GetOwnerNode();
        GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
        vec.push_back(node);
    }
    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetInControlNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& pIn : inControlAnchor_->GetPeerOutControlAnchors()) {
        const auto& node = pIn->GetOwnerNode();
        GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
        vec.push_back(node);
    }
    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetInAllNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& inNode : GetInDataNodes()) {
        vec.push_back(inNode);
    }
    for (const auto& inControlNode : GetInControlNodes()) {
        vec.push_back(inControlNode);
    }
    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetOutDataNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& p : outDataAnchors_) {
        GE_CHK_BOOL_EXEC((p != nullptr), { continue; }, "outDataAnchors_ is nullptr");
        for (const auto& pIn : p->GetPeerInDataAnchors()) {
            const auto& node = pIn->GetOwnerNode();
            GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
            vec.push_back(node);
        }
    }
    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetOutControlNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& pIn : outControlAnchor_->GetPeerAnchors()) {
        GE_CHK_BOOL_EXEC((pIn != nullptr), { continue; }, "peer anchor is nullptr");
        const auto& node = pIn->GetOwnerNode();
        GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
        vec.push_back(node);
    }

    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

LegacyNode::Vistor<NodePtr> LegacyNode::GetOutAllNodes() const
{
    std::vector<NodePtr> vec;
    for (const auto& p : outDataAnchors_) {
        GE_CHK_BOOL_EXEC((p != nullptr), { continue; }, "outDataAnchors_ is nullptr");
        for (const auto& pIn : p->GetPeerInDataAnchors()) {
            const auto& node = pIn->GetOwnerNode();
            GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
            vec.push_back(node);
        }
    }

    for (const auto& pIn : outControlAnchor_->GetPeerAnchors()) {
        GE_CHK_BOOL_EXEC((pIn != nullptr), { continue; }, "peer anchor is nullptr");
        const auto& node = pIn->GetOwnerNode();
        GE_CHK_BOOL_EXEC((node != nullptr), { continue; }, "node is nullptr");
        vec.push_back(node);
    }
    return LegacyNode::Vistor<NodePtr>(std::static_pointer_cast<const Node>(shared_from_this()), vec);
}

OpDescPtr LegacyNode::GetOpDesc() const
{
    return ROLE(NodeStore).OpDesc();
}

/**
 * @brief get subgraphs except subgraphs belonging to subgraphs
 * @return node vistor
 */
void LegacyNode::GetDirectSubGraphs(vector<ComputeGraphPtr>& graphs) const
{
    const auto& subGraphs = ROLE(NodeSubGraph).SubGraphs();
    graphs.insert(graphs.cend(), subGraphs.cbegin(), subGraphs.cend());
}

vector<ComputeGraphPtr> LegacyNode::GetDirectSubGraphs() const
{
    return ROLE(NodeSubGraph).SubGraphs();
}
} // namespace ge
