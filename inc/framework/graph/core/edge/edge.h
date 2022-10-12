/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: edge
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