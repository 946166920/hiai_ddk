/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: node sub graph
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H

#include <string>
#include <vector>
#include <functional>

// inc/api
#include "graph/graph_api_export.h"

// inc/framework
#include "base/error_types.h"
#include "framework/graph/core/cgraph/graph_fwd.h"

namespace ge {
class NodeSubGraph {
using GraphVisitor = std::function<hiai::Status(ComputeGraphPtr&)>;

public:
    virtual ~NodeSubGraph() = default;

    GRAPH_API_EXPORT const std::vector<ComputeGraphPtr>& SubGraphs() const;

    GRAPH_API_EXPORT std::size_t SubGraphsSize() const;
    GRAPH_API_EXPORT hiai::Status WalkSubGraphs(GraphVisitor visitor);

    GRAPH_API_EXPORT hiai::Status AddSubGraph(ComputeGraphPtr& graph);
    GRAPH_API_EXPORT hiai::Status DelSubGraph(const ComputeGraph& graph);

    GRAPH_API_EXPORT hiai::Status UpdateSubGraph(ComputeGraphPtr& graph);

    GRAPH_API_EXPORT ComputeGraph* FindSubGraph(const std::string& name) const;
    GRAPH_API_EXPORT ComputeGraphPtr FindSubGraphPtr(const std::string& name) const;

private:
    std::vector<ComputeGraphPtr> subGraphs_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H