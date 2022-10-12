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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_NOTIFIER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_NOTIFIER_H

#include <vector>

// inc/framework
#include "base/error_types.h"
#include "framework/infra/base/dci.h"
#include "framework/graph/core/cgraph/graph_listener.h"

namespace ge {
DEFINE_ROLE(GraphNotifier)
{
public:
    hiai::Status Register(GraphListener& listener);
    hiai::Status Unregister(const GraphListener& listener);

public:
    void AddNode(Node& node);
    void DelNode(const Node& node);
    void AddEdge(const Edge& edge);
    void DelEdge(const Edge& edge);
    void TopoChanged();

private:
    template <typename F>
    inline void Notify(F func);

private:
    std::vector<GraphListener*> listeners_ {};
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_NOTIFIER_H