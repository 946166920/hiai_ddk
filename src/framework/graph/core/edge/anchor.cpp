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

#include "framework/graph/core/edge/anchor.h"

#include <algorithm>

// api/framework
#include "graph/debug/ge_error_codes.h"

// inc/framework
#include "framework/graph/debug/ge_log.h"
#include "framework/graph/debug/ge_util.h"
#include "framework/graph/core/node/node.h"
#include "infra/base/assertion.h"

namespace ge {
Anchor::Anchor(NodePtr ownerNode, int idx) : ownerNode_(ownerNode), idx_(idx)
{
}

bool Anchor::IsTypeOf(TYPE type) const
{
    return (Anchor::TypeOf<Anchor>() == type);
}

Anchor::Vistor<AnchorPtr> Anchor::GetPeerAnchors() const
{
    vector<AnchorPtr> ret;
    for (const auto& anchor : peerAnchors_) {
        ret.push_back(anchor.lock());
    }
    return Anchor::Vistor<AnchorPtr>(shared_from_this(), ret);
}

NodePtr Anchor::GetOwnerNode() const
{
    return ownerNode_.lock();
}

void Anchor::UnlinkAll()
{
    if (!peerAnchors_.empty()) {
        int i = static_cast<int>(peerAnchors_.size() - 1);
        for (; i >= 0; i--) {
            auto peerAnchorPtr = peerAnchors_[i].lock();
            GE_LOGI_IF((Unlink(peerAnchorPtr) != GRAPH_SUCCESS), "unlink peerAnchorPtr failed.");
        }
        peerAnchors_.clear();
    }
}

GraphErrCodeStatus Anchor::Unlink(AnchorPtr peer)
{
    if (peer == nullptr) {
        FMK_LOGE("peer anchor is invalid.");
        return GRAPH_FAILED;
    }
    const auto& it = std::find_if(peerAnchors_.cbegin(), peerAnchors_.cend(), [peer](std::weak_ptr<Anchor> an) {
        auto anchor = an.lock();
        return peer->Equal(anchor);
    });

    if (it == peerAnchors_.end()) {
        FMK_LOGW("this anchor is not connected to peer");
        return GRAPH_FAILED;
    }

    const auto& it_peer = std::find_if(peer->peerAnchors_.cbegin(), peer->peerAnchors_.cend(),
        [this](std::weak_ptr<Anchor> an) {
        auto anchor = an.lock();
        return Equal(anchor);
    });

    GE_CHK_BOOL_RET_STATUS(it_peer != peer->peerAnchors_.end(), GRAPH_FAILED, "peer is not connected to this anchor");

    peerAnchors_.erase(it);
    peer->peerAnchors_.erase(it_peer);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus Anchor::ReplacePeer(AnchorPtr oldPeer, AnchorPtr firstPeer, AnchorPtr secondPeer)
{
    GE_CHK_BOOL_RET_STATUS(oldPeer != nullptr, GRAPH_FAILED, "this old peer anchor is nullptr");
    GE_CHK_BOOL_RET_STATUS(firstPeer != nullptr, GRAPH_FAILED, "this first peer anchor is nullptr");
    GE_CHK_BOOL_RET_STATUS(secondPeer != nullptr, GRAPH_FAILED, "this second peer anchor is nullptr");
    auto this_it = std::find_if(peerAnchors_.begin(), peerAnchors_.end(), [oldPeer](std::weak_ptr<Anchor> an) {
        auto anchor = an.lock();
        return oldPeer->Equal(anchor);
    });

    GE_CHK_BOOL_RET_STATUS(this_it != peerAnchors_.end(), GRAPH_FAILED, "this anchor is not connected to oldPeer");

    auto old_it =
        std::find_if(oldPeer->peerAnchors_.begin(), oldPeer->peerAnchors_.end(), [this](std::weak_ptr<Anchor> an) {
            auto anchor = an.lock();
            return Equal(anchor);
        });

    GE_CHK_BOOL_RET_STATUS(
        old_it != oldPeer->peerAnchors_.end(), GRAPH_FAILED, "oldPeer is not connected to this anchor");
    *this_it = firstPeer;
    firstPeer->peerAnchors_.push_back(shared_from_this());
    *old_it = secondPeer;
    secondPeer->peerAnchors_.push_back(oldPeer);
    return GRAPH_SUCCESS;
}

bool Anchor::IsLinkedWith(AnchorPtr peer)
{
    GE_CHK_BOOL_RET_STATUS(peer != nullptr, false, "this old peer anchor is nullptr");
    const auto& it = std::find_if(peerAnchors_.cbegin(), peerAnchors_.cend(), [peer](std::weak_ptr<Anchor> an) {
        auto anchor = an.lock();
        return peer->Equal(anchor);
    });
    return (it != peerAnchors_.end());
}

int Anchor::GetIdx() const
{
    return idx_;
}

DataAnchor::DataAnchor(NodePtr ownerNode, int idx) : Anchor(ownerNode, idx)
{
}

bool DataAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<DataAnchor>() == type) {
        return true;
    }
    return Anchor::IsTypeOf(type);
}

InDataAnchor::InDataAnchor(NodePtr ownerNode, int idx) : DataAnchor(ownerNode, idx)
{
}

OutDataAnchorPtr InDataAnchor::GetPeerOutAnchor() const
{
    if (peerAnchors_.empty()) {
        return nullptr;
    } else {
        return Anchor::DynamicAnchorCast<OutDataAnchor>(peerAnchors_.begin()->lock());
    }
}

GraphErrCodeStatus InDataAnchor::LinkFrom(OutDataAnchorPtr src)
{
    // InDataAnchor must be only linkfrom once
    HIAI_EXPECT_NOT_NULL(src);
    HIAI_EXPECT_NOT_NULL(src->GetOwnerNode());
    HIAI_EXPECT_TRUE(peerAnchors_.empty());
    peerAnchors_.push_back(src);
    src->peerAnchors_.push_back(shared_from_this());
    return GRAPH_SUCCESS;
}

bool InDataAnchor::Equal(const AnchorPtr& anchor) const
{
    auto inDataAnchor = Anchor::DynamicAnchorCast<InDataAnchor>(anchor);
    if (inDataAnchor != nullptr) {
        if (GetOwnerNode() == inDataAnchor->GetOwnerNode() && GetIdx() == inDataAnchor->GetIdx()) {
            return true;
        }
    }
    return false;
}

bool InDataAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<InDataAnchor>() == type) {
        return true;
    }
    return DataAnchor::IsTypeOf(type);
}

OutDataAnchor::OutDataAnchor(NodePtr ownerNode, int idx) : DataAnchor(ownerNode, idx)
{
}

OutDataAnchor::Vistor<InDataAnchorPtr> OutDataAnchor::GetPeerInDataAnchors() const
{
    vector<InDataAnchorPtr> ret;
    for (auto anchor : peerAnchors_) {
        auto inDataAnchor = Anchor::DynamicAnchorCast<InDataAnchor>(anchor.lock());
        if (inDataAnchor != nullptr) {
            ret.push_back(inDataAnchor);
        }
    }
    return OutDataAnchor::Vistor<InDataAnchorPtr>(shared_from_this(), ret);
}

GraphErrCodeStatus OutDataAnchor::LinkTo(InDataAnchorPtr dest)
{
    HIAI_EXPECT_NOT_NULL(dest);
    HIAI_EXPECT_NOT_NULL(dest->GetOwnerNode());
    HIAI_EXPECT_TRUE(dest->peerAnchors_.empty());
    peerAnchors_.push_back(dest);
    dest->peerAnchors_.push_back(shared_from_this());
    return GRAPH_SUCCESS;
}

bool OutDataAnchor::Equal(const AnchorPtr& anchor) const
{
    CHECK_FALSE_EXEC(anchor != nullptr, return false);
    auto outDataAnchor = Anchor::DynamicAnchorCast<OutDataAnchor>(anchor);
    if (outDataAnchor != nullptr) {
        if (GetOwnerNode() == outDataAnchor->GetOwnerNode() && GetIdx() == outDataAnchor->GetIdx()) {
            return true;
        }
    }
    return false;
}

bool OutDataAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<OutDataAnchor>() == type) {
        return true;
    }
    return DataAnchor::IsTypeOf(type);
}

ControlAnchor::ControlAnchor(NodePtr ownerNode) : Anchor(ownerNode, -1)
{
}

bool ControlAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<ControlAnchor>() == type) {
        return true;
    }
    return Anchor::IsTypeOf(type);
}

InControlAnchor::InControlAnchor(NodePtr ownerNode) : ControlAnchor(ownerNode)
{
}

InControlAnchor::Vistor<OutControlAnchorPtr> InControlAnchor::GetPeerOutControlAnchors() const
{
    vector<OutControlAnchorPtr> ret;
    for (auto anchor : peerAnchors_) {
        auto outControlAnchor = Anchor::DynamicAnchorCast<OutControlAnchor>(anchor.lock());
        if (outControlAnchor != nullptr) {
            ret.push_back(outControlAnchor);
        }
    }
    return InControlAnchor::Vistor<OutControlAnchorPtr>(shared_from_this(), ret);
}

GraphErrCodeStatus InControlAnchor::LinkFrom(OutControlAnchorPtr src)
{
    HIAI_EXPECT_NOT_NULL(src);
    HIAI_EXPECT_NOT_NULL(src->GetOwnerNode());
    if (find_if(peerAnchors_.cbegin(), peerAnchors_.cend(),
        [&src](std::weak_ptr<Anchor> peer) { return peer.lock().get() == src.get(); }) != peerAnchors_.cend()) {
        FMK_LOGE("src anchor is already exist.");
        return GRAPH_FAILED;
    }

    peerAnchors_.push_back(src);
    src->peerAnchors_.push_back(shared_from_this());
    return GRAPH_SUCCESS;
}

bool InControlAnchor::Equal(const AnchorPtr& anchor) const
{
    CHECK_FALSE_EXEC(anchor != nullptr, return false);
    auto inControlAnchor = Anchor::DynamicAnchorCast<InControlAnchor>(anchor);
    if (inControlAnchor != nullptr) {
        if (GetOwnerNode() == inControlAnchor->GetOwnerNode()) {
            return true;
        }
    }
    return false;
}

bool InControlAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<InControlAnchor>() == type) {
        return true;
    }
    return ControlAnchor::IsTypeOf(type);
}

OutControlAnchor::OutControlAnchor(NodePtr ownerNode) : ControlAnchor(ownerNode)
{
}

OutControlAnchor::Vistor<InControlAnchorPtr> OutControlAnchor::GetPeerInControlAnchors() const
{
    vector<InControlAnchorPtr> ret;
    for (auto anchor : peerAnchors_) {
        auto inControlAnchor = Anchor::DynamicAnchorCast<InControlAnchor>(anchor.lock());
        if (inControlAnchor != nullptr) {
            ret.push_back(inControlAnchor);
        }
    }
    return OutControlAnchor::Vistor<InControlAnchorPtr>(shared_from_this(), ret);
}

GraphErrCodeStatus OutControlAnchor::LinkTo(InControlAnchorPtr dest)
{
    HIAI_EXPECT_NOT_NULL(dest);
    HIAI_EXPECT_NOT_NULL(dest->GetOwnerNode());

    if (find_if(peerAnchors_.cbegin(), peerAnchors_.cend(),
        [&dest](std::weak_ptr<Anchor> peer) { return peer.lock().get() == dest.get(); }) != peerAnchors_.cend()) {
        FMK_LOGE("dest anchor is already exist.");
        return GRAPH_FAILED;
    }

    peerAnchors_.push_back(dest);
    dest->peerAnchors_.push_back(shared_from_this());
    return GRAPH_SUCCESS;
}

bool OutControlAnchor::Equal(const AnchorPtr& anchor) const
{
    auto outControlAnchor = Anchor::DynamicAnchorCast<OutControlAnchor>(anchor);
    if (outControlAnchor != nullptr) {
        if (GetOwnerNode() == outControlAnchor->GetOwnerNode()) {
            return true;
        }
    }
    return false;
}

bool OutControlAnchor::IsTypeOf(TYPE type) const
{
    if (Anchor::TypeOf<OutControlAnchor>() == type) {
        return true;
    }
    return ControlAnchor::IsTypeOf(type);
}
} // namespace ge
