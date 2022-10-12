/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: node visitor
 */

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H
#define FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H

// api/framework
#include "graph/graph_api_export.h"

namespace ge {
class Node;

class GRAPH_API_EXPORT Endpoint {
public:
    Endpoint(ge::Node& node, int idx);
    ~Endpoint() = default;

    bool IsCtrl() const;
    bool IsData() const;

    ge::Node& Node() const;
    int Idx() const;

    bool operator==(const Endpoint& other) const;

private:
    ge::Node* node_;
    int idx_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H