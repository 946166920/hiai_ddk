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

#include "framework/graph/core/edge/endpoint.h"

namespace ge {
Endpoint::Endpoint(ge::Node& node, int idx) : node_(&node), idx_(idx)
{
}

bool Endpoint::IsCtrl() const
{
    return idx_ == -1;
}

bool Endpoint::IsData() const
{
    return !IsCtrl();
}

Node& Endpoint::Node() const
{
    return *node_;
}

int Endpoint::Idx() const
{
    return idx_;
}

bool Endpoint::operator==(const Endpoint& other) const
{
    return (idx_ == other.idx_) && (node_ == other.node_);
}
} // namespace ge