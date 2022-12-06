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

#include "framework/graph/core/cgraph/compute_graph.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_pruner.h"
#include "framework/graph/core/cgraph/graph_sorter.h"
#include "framework/graph/core/cgraph/graph_topo_walker.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/cgraph/graph_notifier.h"
#include "framework/graph/core/cgraph/graph_serializer.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

#include "graph/persistance/interface/graph_def.h"

namespace ge {
ComputeGraph::ComputeGraph(GraphStore& store) : LegacyGraph(store)
{
}

namespace {
class ComputeGraphImpl :
    private GraphStore, // NOTE: GraphStore must be first. For details, see C++ Multiple Inheritance.
    public ComputeGraph,
    private GraphSpec,
    private GraphFinder,
    private GraphModifier,
    private GraphPruner,
    private GraphSorter,
    private GraphListWalker,
    private GraphTopoWalker,
    private GraphBypasser,
    private GraphSerializer,
    private GraphNotifier {
public:
    explicit ComputeGraphImpl(std::string&& name) : GraphStore(std::move(name)),
        ComputeGraph(static_cast<GraphStore&>(*this))
    {
        GraphNotifier::Register(static_cast<GraphTopoWalker&>(*this));
    }

    ComputeGraphImpl(hiai::IGraphDef* graphDef, bool isOwner) : GraphStore(graphDef, isOwner),
        ComputeGraph(static_cast<GraphStore&>(*this))
    {
        GraphNotifier::Register(static_cast<GraphTopoWalker&>(*this));
    }

    ~ComputeGraphImpl() override
    {
        GraphNotifier::Unregister(static_cast<GraphTopoWalker&>(*this));
    }

private:
    ComputeGraphPtr GetComputeGraph() override
    {
        return std::static_pointer_cast<ComputeGraph>(shared_from_this());
    }

private:
    IMPL_ROLE(ComputeGraph);
    IMPL_ROLE(GraphStore);
    IMPL_ROLE(GraphSpec);
    IMPL_ROLE(GraphFinder);
    IMPL_ROLE(GraphModifier);
    IMPL_ROLE(GraphPruner);
    IMPL_ROLE(GraphSorter);
    IMPL_ROLE(GraphListWalker);
    IMPL_ROLE(GraphTopoWalker);
    IMPL_ROLE(GraphBypasser);
    IMPL_ROLE(GraphNotifier);
    IMPL_ROLE(GraphSerializer);
};
} // namespace

namespace {
void FixDefaultName(std::string& name)
{
    if (name.empty()) {
        name = "default";
    }
}
} // namespace

ComputeGraphPtr ComputeGraph::Make(std::string name)
{
    FixDefaultName(name);

    return hiai::make_shared_nothrow<ComputeGraphImpl>(std::move(name));
}

ComputeGraphPtr ComputeGraph::Make(::hiai::IGraphDef* graphDef, bool isOwner)
{
    return hiai::make_shared_nothrow<ComputeGraphImpl>(graphDef, isOwner);
}
} // namespace ge