/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: node functor
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H

#include <vector>
#include <string>

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "base/error_types.h"
#include "framework/graph/core/node/node_visitor.h"

namespace ge {
class GRAPH_API_EXPORT NodeFunctor {
public:
    NodeVisitor ToVisitor()
    {
        return [this](Node& node) { return Visit(node); };
    }

    static NodeVisitor Typed(std::vector<std::string> types, NodeVisitor visitor);

    virtual hiai::Status Visit(Node& node) = 0;
    virtual ~NodeFunctor() = default;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H
