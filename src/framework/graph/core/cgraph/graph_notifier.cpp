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

#include "framework/graph/core/cgraph/graph_notifier.h"

#include <algorithm>

#include "infra/base/assertion.h"

// inc/framework
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/node/node.h"

namespace ge {
hiai::Status GraphNotifier::Register(GraphListener& listener)
{
    GraphListener* p = &listener;
    HIAI_EXPECT_NOT_NULL(p);

    listeners_.push_back(p);
    return hiai::SUCCESS;
}

hiai::Status GraphNotifier::Unregister(const GraphListener& listener)
{
    const auto& it = std::find(listeners_.cbegin(), listeners_.cend(), &listener);
    HIAI_EXPECT_TRUE(it != listeners_.cend());

    listeners_.erase(it);
    return hiai::SUCCESS;
}

template <typename F>
inline void GraphNotifier::Notify(F func)
{
    for (auto const& listener : listeners_) {
        func(*listener);
    }
}

void GraphNotifier::AddNode(Node& node)
{
    Notify([&node](GraphListener& listener) { listener.OnAddNode(node); });
}

void GraphNotifier::DelNode(const Node& node)
{
    Notify([&node](GraphListener& listener) { listener.OnDelNode(node); });
}

void GraphNotifier::AddEdge(const Edge& edge)
{
    Notify([&edge](GraphListener& listener) { listener.OnAddEdge(edge); });
}

void GraphNotifier::DelEdge(const Edge& edge)
{
    Notify([&edge](GraphListener& listener) { listener.OnDelEdge(edge); });
}

void GraphNotifier::TopoChanged()
{
    Notify([](GraphListener& listener) { listener.OnTopoChanged(); });
}
} // namespace ge