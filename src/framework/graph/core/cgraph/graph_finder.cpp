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

#include "framework/graph/core/cgraph/graph_finder.h"

#include <algorithm>

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_pred.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

namespace ge {
Node* GraphFinder::FindNode(const std::string& name) const
{
    return FindNode([&name](const Node& node) { return node.ROLE(NodeSpec).Name() == name; });
}

Node* GraphFinder::FindNode(int64_t id) const
{
    return FindNode([&id](Node& node) { return node.ROLE(NodeSpec).Id() == id; });
}

Node* GraphFinder::FindNode(const NodePred& pred) const
{
    for (const auto& node : ROLE(GraphStore).AllNodes()) {
        if (pred(*node)) {
            return node.get();
        }
    }

    return nullptr;
}

NodePtr GraphFinder::FindNodePtr(const Node& node) const
{
    const auto& nodes = ROLE(GraphStore).AllNodes();

    const auto& it = std::find_if(nodes.cbegin(), nodes.cend(), [&node](const NodePtr& p) { return p.get() == &node; });
    return it != nodes.cend() ? *it : nullptr;
}
} // namespace ge