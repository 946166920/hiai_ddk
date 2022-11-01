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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_QUICK_QUERY_NODES_H
#define FRAMEWORK_GRAPH_CORE_NODE_QUICK_QUERY_NODES_H

#include <vector>
#include <unordered_set>

// inc/framework
#include "base/error_types.h"
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/node/node_pred.h"

namespace ge {
class QuickQueryNodes {
public:
    const std::vector<NodePtr>& Nodes() const;
    bool HasNode(const Node& node) const;

    void AddNode(const NodePtr& node);
    void AddNodeFront(const NodePtr& node);

    hiai::Status DelNode(const Node& node);
    void DelNodes(const NodePred& pred, const NodeAction& preAction);

    hiai::Status UpdateNodes(const std::vector<Node*>& nodes);

    const NodePtr FindNodePtr(const Node& node) const;

private:
    std::vector<NodePtr> store_;
    std::unordered_set<const Node*> quickQuery_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_QUICK_QUERY_NODES_H