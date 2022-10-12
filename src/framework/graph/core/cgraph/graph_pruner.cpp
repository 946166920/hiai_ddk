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

#include "framework/graph/core/cgraph/graph_pruner.h"

#include <unordered_set>

// inc/framework
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_notifier.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_functor.h"
#include "framework/graph/core/node/node_walker.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"
#include "graph/core/node/node_store.h"

namespace ge {
namespace {
class ActiveNodeCache : public NodeFunctor {
public:
    explicit ActiveNodeCache(const std::vector<NodePtr>& inputNodes)
    {
        // input nodes should not delete.
        for (const auto& inputNode : inputNodes) {
            nodes_.insert(inputNode.get());
        }
    }
    ~ActiveNodeCache() = default;

    bool IsDead(const Node& node) const
    {
        return nodes_.find(&node) == nodes_.end();
    }

private:
    hiai::Status Visit(Node& node) override
    {
        nodes_.insert(&node);

        auto& walker = node.ROLE(NodeWalker);
        (void)walker.ListInDataNodes(ToVisitor());
        (void)walker.ListInCtrlNodes(ToVisitor());

        return hiai::SUCCESS;
    }

private:
    std::unordered_set<const Node*> nodes_ {};
};
} // namespace

hiai::Status GraphPruner::Prune()
{
    ActiveNodeCache cache(ROLE(GraphStore).InputNodes());
    (void)ROLE(GraphListWalker).WalkOutNodes(cache.ToVisitor());

    auto isDead = [&cache](Node& node) { return cache.IsDead(node); };
    auto prune = [this](Node& node) {
        ROLE(GraphNotifier).DelNode(node);
        node.ROLE(NodeStore).RemoveEdges();
    };
    ROLE(GraphStore).RemoveNodes(isDead, prune);

    return hiai::SUCCESS;
}
} // namespace ge
