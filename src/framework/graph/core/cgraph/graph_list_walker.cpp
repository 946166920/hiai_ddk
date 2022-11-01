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

#include "framework/graph/core/cgraph/graph_list_walker.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_sorter.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_functor.h"
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/utils/checker/node_checker.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

namespace ge {
namespace {
template <typename T>
hiai::Status Walk(std::vector<T>& nodes, NodeVisitor& visitor)
{
    for (const auto& node : nodes) {
        HIAI_EXPECT_EXEC(visitor(*node));
    }
    return hiai::SUCCESS;
}
} // namespace

hiai::Status GraphListWalker::WalkAllNodes(NodeVisitor visitor)
{
    return Walk(ROLE(GraphStore).AllNodes(), visitor);
}

hiai::Status GraphListWalker::WalkAllNodesModifiable(const NodePred& pred, NodeVisitor visitor)
{
    std::vector<NodePtr> caches;
    for (const auto& node : ROLE(GraphStore).AllNodes()) {
        if (pred(*node)) {
            caches.push_back(node);
        }
    }
    return Walk(caches, visitor);
}

hiai::Status GraphListWalker::WalkInNodes(NodeVisitor visitor)
{
    if (!ROLE(GraphStore).InputNodes().empty()) {
        return Walk(ROLE(GraphStore).InputNodes(), visitor);
    } else {
        std::vector<Node*> inputs;
        auto inputVisitor = [&inputs](Node& node) {
            if (NodeChecker::IsGraphInputType(node.ROLE(NodeSpec).Type())) {
                inputs.push_back(&node);
            }
            return hiai::SUCCESS;
        };
        (void)WalkAllNodes(std::move(inputVisitor));
        GraphSorter::StableSort(inputs, ROLE(GraphStore).InputOrder(),
            [](uint32_t idxI, uint32_t idxJ) { return idxI < idxJ; });
        return Walk(inputs, visitor);
    }
}

hiai::Status GraphListWalker::WalkOutNodes(NodeVisitor visitor)
{
    if (!ROLE(GraphStore).OutputNodes().empty()) {
        return Walk(ROLE(GraphStore).OutputNodes(), visitor);
    } else {
        std::vector<Node*> outputs;
        NodeVisitor outputVisitor = NodeFunctor::Typed({NETOUTPUT}, [&outputs](Node& node) {
            outputs.push_back(&node);
            return hiai::SUCCESS;
        });
        (void)WalkAllNodes(std::move(outputVisitor));
        return Walk(outputs, visitor);
    }
}
} // namespace ge