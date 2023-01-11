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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H

#include <string>
#include <vector>
#include <functional>

// inc/api
#include "graph/graph_api_export.h"

// inc/framework
#include "base/error_types.h"
#include "framework/infra/base/dci.h"
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node_fwd.h"

namespace ge {
DEFINE_ROLE(NodeSubGraph) {
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
    USE_ROLE(Node);

private:
    std::vector<ComputeGraphPtr> subGraphs_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_SUB_GRAPH_H