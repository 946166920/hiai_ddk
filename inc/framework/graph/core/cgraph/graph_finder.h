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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_FINDER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_FINDER_H

#include <string>

// inc/framework
#include "framework/infra/base/dci.h"
#include "framework/graph/core/node/node_pred.h"
#include "framework/graph/core/node/node_fwd.h"

namespace ge {
class GraphStore;

EXPORT_ROLE(GraphFinder)
{
public:
    Node* FindNode(const std::string& name) const;
    Node* FindNode(int64_t id) const;
    Node* FindNode(const NodePred& pred) const;
    NodePtr FindNodePtr(const Node& node) const;

private:
    USE_ROLE(GraphStore);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_FINDER_H