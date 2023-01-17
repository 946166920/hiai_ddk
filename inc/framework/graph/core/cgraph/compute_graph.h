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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_COMPUTE_GRAPH_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_COMPUTE_GRAPH_H

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/cgraph/legacy_graph.h"

namespace hiai {
class IGraphDef;
}

namespace ge {
class GraphSpec;
class GraphStore;
class GraphFinder;
class GraphModifier;
class GraphPruner;
class GraphSorter;
class GraphTopoWalker;
class GraphListWalker;
class GraphBypasser;
class GraphNotifier;
class GraphSerializer;

class GRAPH_API_EXPORT ComputeGraph : public LegacyGraph {
public:
    static ComputeGraphPtr Make(std::string name);
    static ComputeGraphPtr Make(hiai::IGraphDef* graphDef, bool isOwner);

    HAS_ROLE(GraphSpec);
    HAS_ROLE(GraphFinder);
    HAS_ROLE(GraphModifier);
    HAS_ROLE(GraphPruner);
    HAS_ROLE(GraphSorter);
    HAS_ROLE(GraphTopoWalker);
    HAS_ROLE(GraphListWalker);
    HAS_ROLE(GraphBypasser);
    HAS_ROLE(GraphNotifier);
    HAS_ROLE(GraphSerializer);

protected:
    explicit ComputeGraph(GraphStore& store);
    virtual ~ComputeGraph() = default;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_COMPUTE_GRAPH_H
