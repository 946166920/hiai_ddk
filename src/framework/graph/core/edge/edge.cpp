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

#include "framework/graph/core/edge/edge.h"

// inc/framework
#include "framework/graph/core/edge/endpoint.h"

namespace ge {
Edge::Edge(const Endpoint& src, const Endpoint& dst) : src_(src), dst_(dst)
{
}

Edge::Edge(Node& srcNode, int srcIdx, Node& dstNode, int dstIdx) : src_(srcNode, srcIdx), dst_(dstNode, dstIdx)
{
}

bool Edge::IsCtrl() const
{
    return src_.IsCtrl() && dst_.IsCtrl();
}

bool Edge::IsData() const
{
    return src_.IsData() && dst_.IsData();
}

Node& Edge::SrcNode() const
{
    return src_.Node();
}

int Edge::SrcIdx() const
{
    return src_.Idx();
}

Node& Edge::DstNode() const
{
    return dst_.Node();
}

int Edge::DstIdx() const
{
    return dst_.Idx();
}

const Endpoint& Edge::Src() const
{
    return src_;
}

const Endpoint& Edge::Dst() const
{
    return dst_;
}
} // namespace ge