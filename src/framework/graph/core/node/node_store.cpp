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

#include "graph/core/node/node_store.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/op/op_desc.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
template <typename T>
hiai::Status InitDataAnchors(std::vector<std::shared_ptr<T>>& anchors, NodePtr& node, std::size_t num)
{
    for (std::size_t i = 0; i < num; i++) {
        std::shared_ptr<T> anchor = hiai::make_shared_nothrow<T>(node, i);
        HIAI_EXPECT_NOT_NULL(anchor);
        anchors.push_back(anchor);
    }

    return hiai::SUCCESS;
}

template <typename T>
hiai::Status InitCtrlAnchors(std::shared_ptr<T>& anchor, NodePtr& node)
{
    anchor = hiai::make_shared_nothrow<T>(node);
    return anchor != nullptr ? hiai::SUCCESS : hiai::FAILURE;
}
} // namespace

hiai::Status NodeStore::Init()
{
    if (hasInit_) {
        return hiai::SUCCESS;
    }

    NodePtr node = ROLE(Node);
    HIAI_EXPECT_EXEC(InitDataAnchors<ge::InDataAnchor>(inDataAnchors_, node, op_->GetInputsDescSize()));
    HIAI_EXPECT_EXEC(InitDataAnchors<ge::OutDataAnchor>(outDataAnchors_, node, op_->GetOutputsSize()));
    HIAI_EXPECT_EXEC(InitCtrlAnchors<ge::InControlAnchor>(inControlAnchor_, node));
    HIAI_EXPECT_EXEC(InitCtrlAnchors<ge::OutControlAnchor>(outControlAnchor_, node));

    hasInit_ = true;
    return hiai::SUCCESS;
}

NodeStore::NodeStore(const ComputeGraphPtr& graph, const OpDescPtr& op) : graph_(graph), op_(op)
{
}

NodeStore::~NodeStore()
{
    for (const auto& p : inDataAnchors_) {
        p->UnlinkAll();
    }
    for (const auto& p : outDataAnchors_) {
        p->UnlinkAll();
    }
    if (inControlAnchor_) {
        inControlAnchor_->UnlinkAll();
    }
    if (outControlAnchor_) {
        outControlAnchor_->UnlinkAll();
    }
}

ComputeGraphPtr NodeStore::OwnerComputeGraphPtr() const
{
    return graph_.lock();
}

void NodeStore::SetOwnerComputeGraph(const ComputeGraphPtr& graph)
{
    graph_ = std::weak_ptr<ComputeGraph>(graph);
}

std::vector<InDataAnchorPtr>& NodeStore::InDataAnchors()
{
    return inDataAnchors_;
}

std::vector<OutDataAnchorPtr>& NodeStore::OutDataAnchors()
{
    return outDataAnchors_;
}

InControlAnchorPtr& NodeStore::InCtrlAnchor()
{
    return inControlAnchor_;
}

OutControlAnchorPtr& NodeStore::OutCtrlAnchor()
{
    return outControlAnchor_;
}

OpDescPtr& NodeStore::OpDesc()
{
    return op_;
}

InDataAnchorPtr NodeStore::InDataAnchor(std::size_t idx) const
{
    return idx < inDataAnchors_.size() ? inDataAnchors_[idx] : nullptr;
}

OutDataAnchorPtr NodeStore::OutDataAnchor(std::size_t idx) const
{
    return idx < outDataAnchors_.size() ? outDataAnchors_[idx] : nullptr;
}

namespace {
constexpr bool IsCtrlEdge(int srcIdx, int dstIdx)
{
    return srcIdx == -1 && dstIdx == -1;
}

constexpr bool IsDataEdge(int srcIdx, int dstIdx)
{
    return srcIdx >= 0 && dstIdx >= 0;
}

template <typename F>
hiai::Status ModifyCtrlEdge(NodeStore& src, NodeStore& dst, F func)
{
    OutControlAnchorPtr& srcAnchor = src.OutCtrlAnchor();
    InControlAnchorPtr& dstAnchor = dst.InCtrlAnchor();

    return (srcAnchor.get()->*func)(dstAnchor);
}

template <typename F>
hiai::Status ModifyDataEdge(NodeStore& src, int srcIdx, NodeStore& dst, int dstIdx, F func)
{
    OutDataAnchorPtr srcAnchor = src.OutDataAnchor(srcIdx);
    HIAI_EXPECT_NOT_NULL(srcAnchor);
    InDataAnchorPtr dstAnchor = dst.InDataAnchor(dstIdx);
    HIAI_EXPECT_NOT_NULL(dstAnchor);

    return (srcAnchor.get()->*func)(dstAnchor);
}
} // namespace

hiai::Status NodeStore::AddEdge(NodeStore& src, int srcIdx, NodeStore& dst, int dstIdx)
{
    if (IsCtrlEdge(srcIdx, dstIdx)) {
        return ModifyCtrlEdge(src, dst, &OutControlAnchor::LinkTo);
    } else if (IsDataEdge(srcIdx, dstIdx)) {
        return ModifyDataEdge(src, srcIdx, dst, dstIdx, &OutDataAnchor::LinkTo);
    }

    return hiai::FAILURE;
}

hiai::Status NodeStore::RemoveEdge(NodeStore& src, int srcIdx, NodeStore& dst, int dstIdx)
{
    if (IsCtrlEdge(srcIdx, dstIdx)) {
        return ModifyCtrlEdge(src, dst, &Anchor::Unlink);
    } else if (IsDataEdge(srcIdx, dstIdx)) {
        return ModifyDataEdge(src, srcIdx, dst, dstIdx, &Anchor::Unlink);
    }

    return hiai::FAILURE;
}

hiai::Status NodeStore::RemoveEdge(const Node& src, const Node& dst)
{
    for (const InDataAnchorPtr& dstAnchor : dst.ROLE(NodeStore).InDataAnchors()) {
        const OutDataAnchorPtr& srcAnchor = dstAnchor->GetPeerOutAnchor();
        if (srcAnchor != nullptr && srcAnchor->GetOwnerNode().get() == &src) {
            return srcAnchor->Unlink(dstAnchor);
        }
    }

    return hiai::FILE_NOT_EXIST;
}

inline void NodeStore::RemoveInEdges()
{
    for (auto& inAnchor : InDataAnchors()) {
        inAnchor->UnlinkAll();
    }
    InCtrlAnchor()->UnlinkAll();
}

inline void NodeStore::RemoveOutEdges()
{
    for (auto& outAnchor : OutDataAnchors()) {
        outAnchor->UnlinkAll();
    }
    OutCtrlAnchor()->UnlinkAll();
}

void NodeStore::RemoveEdges()
{
    RemoveInEdges();
    RemoveOutEdges();
}
} // namespace ge