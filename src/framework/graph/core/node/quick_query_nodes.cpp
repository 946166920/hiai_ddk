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

#include "graph/core/node/quick_query_nodes.h"

#include <algorithm>

// api/framework
#include "graph/op/array_defs.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"

namespace ge {
const std::vector<NodePtr>& QuickQueryNodes::Nodes() const
{
    return store_;
}

bool QuickQueryNodes::HasNode(const Node& node) const
{
    return quickQuery_.find(&node) != quickQuery_.end();
}

void QuickQueryNodes::AddNode(const NodePtr& node)
{
    store_.push_back(node);
    quickQuery_.insert(node.get());
}

void QuickQueryNodes::AddNodeFront(const NodePtr& node)
{
    std::size_t offset =
        static_cast<std::size_t>(!store_.empty() && store_[0]->ROLE(NodeSpec).Type() == hiai::op::Data::TYPE);
    store_.insert(store_.cbegin() + offset, node);
    quickQuery_.insert(node.get());
}

hiai::Status QuickQueryNodes::DelNode(const Node& node)
{
    const auto& it = std::find_if(store_.cbegin(), store_.cend(), [&node](const NodePtr& p) {
        return p.get() == &node;
    });
    if (it == store_.cend()) {
        return hiai::FILE_NOT_EXIST;
    }

    quickQuery_.erase(&node);
    store_.erase(it);
    return hiai::SUCCESS;
}

namespace {
template <typename T, typename F>
T PredNodeDoAction(T& it, F& nodes, Node& node, const NodePred& pred, const NodeAction& preAction)
{
    if (pred(node)) {
        preAction(node);
        return nodes.erase(it);
    } else {
        return ++it;
    }
}
} // namespace

void QuickQueryNodes::DelNodes(const NodePred& pred, const NodeAction& preAction)
{
    for (auto it = quickQuery_.begin(); it != quickQuery_.end();) {
        it = PredNodeDoAction(it, quickQuery_, const_cast<Node&>(**it), pred, preAction);
    }

    for (auto it = store_.begin(); it != store_.end();) {
        it = PredNodeDoAction(it, store_, **it, pred, preAction);
    }
}

hiai::Status QuickQueryNodes::UpdateNodes(const std::vector<Node*>& nodes)
{
    auto currIdx = store_.begin();

    for (auto& node : nodes) {
        const auto& swapIdx = std::find_if(currIdx, store_.end(), [&node](const NodePtr& p) {
            return p.get() == node;
        });
        if (swapIdx == store_.end()) {
            return hiai::FILE_NOT_EXIST;
        }

        std::swap(*swapIdx, *currIdx++);
    }

    return (currIdx == store_.end()) ? hiai::SUCCESS : hiai::FAILURE;
}

const NodePtr QuickQueryNodes::FindNodePtr(const Node& node) const
{
    const auto& it = std::find_if(store_.cbegin(), store_.cend(), [&node](const NodePtr& p) {
        return p.get() == &node;
    });
    return it != store_.cend() ? *it : nullptr;
}
} // namespace ge