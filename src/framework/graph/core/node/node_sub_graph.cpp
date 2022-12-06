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

#include "framework/graph/core/node/node_sub_graph.h"

#include <algorithm>

#include "infra/base/assertion.h"

#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_spec.h"

namespace ge {
const std::vector<ComputeGraphPtr>& NodeSubGraph::SubGraphs() const
{
    return subGraphs_;
}

std::size_t NodeSubGraph::SubGraphsSize() const
{
    return subGraphs_.size();
}

hiai::Status NodeSubGraph::WalkSubGraphs(GraphVisitor visitor)
{
    for (auto& subGraph : subGraphs_) {
        HIAI_EXPECT_EXEC(visitor(subGraph));
    }

    return hiai::SUCCESS;
}

hiai::Status NodeSubGraph::AddSubGraph(ComputeGraphPtr& graph)
{
    HIAI_EXPECT_NOT_NULL(graph);

    subGraphs_.push_back(graph);
    return hiai::SUCCESS;
}

hiai::Status NodeSubGraph::DelSubGraph(const ComputeGraph& graph)
{
    const auto& it = std::find_if(subGraphs_.cbegin(), subGraphs_.cend(),
        [&graph](const ComputeGraphPtr& subGraph) { return subGraph.get() == &graph; });

    if (it != subGraphs_.cend()) {
        subGraphs_.erase(it);
        return hiai::SUCCESS;
    } else {
        return hiai::FILE_NOT_EXIST;
    }
}

hiai::Status NodeSubGraph::UpdateSubGraph(ComputeGraphPtr& graph)
{
    HIAI_EXPECT_NOT_NULL(graph);

    const string& name = graph->ROLE(GraphSpec).Name();
    auto it = std::find_if(subGraphs_.begin(), subGraphs_.end(),
        [&name](ComputeGraphPtr& subGraph) { return subGraph->ROLE(GraphSpec).Name() == name; });
    if (it != subGraphs_.cend()) {
        *it = graph;
        return hiai::SUCCESS;
    } else {
        return hiai::FILE_NOT_EXIST;
    }
}

ComputeGraph* NodeSubGraph::FindSubGraph(const std::string& name) const
{
    const auto& it = std::find_if(subGraphs_.cbegin(), subGraphs_.cend(),
        [&name](const ComputeGraphPtr& subGraph) { return subGraph->ROLE(GraphSpec).Name() == name; });
    return it != subGraphs_.cend() ? (*it).get() : nullptr;
}

ComputeGraphPtr NodeSubGraph::FindSubGraphPtr(const std::string& name) const
{
    const auto& it = std::find_if(subGraphs_.cbegin(), subGraphs_.cend(),
        [&name](const ComputeGraphPtr& subGraph) { return subGraph->ROLE(GraphSpec).Name() == name; });
    return it != subGraphs_.cend() ? *it : nullptr;
}
} // namespace ge
