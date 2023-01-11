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

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_EDGE_H
#define FRAMEWORK_GRAPH_CORE_EDGE_EDGE_H

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/graph/core/edge/endpoint.h"

namespace ge {
class Node;

class GRAPH_API_EXPORT Edge {
public:
    // If no saved endpoint, it is more efficient to use this interface.
    Edge(Node& srcNode, int srcIdx, Node& dstNode, int dstIdx);
    Edge(const Endpoint& src, const Endpoint& dst);
    ~Edge() = default;

    bool IsCtrl() const;
    bool IsData() const;

    Node& SrcNode() const;
    int SrcIdx() const;

    Node& DstNode() const;
    int DstIdx() const;

    const Endpoint& Src() const;

    const Endpoint& Dst() const;

private:
    Endpoint src_;
    Endpoint dst_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_EDGE_H