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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SORTER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SORTER_H

#include <list>
#include <vector>
#include <functional>
#include <map>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/node/node_fwd.h"

namespace ge {
class GraphStore;
class GraphListWalker;

using Comparator = std::function<bool(uint32_t left, uint32_t right)>;

EXPORT_ROLE(GraphSorter)
{
public:
    static void StableSort(
        std::vector<NodePtr>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order);
    static void StableSort(
        std::list<Node*>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order);
    static void StableSort(
        std::vector<Node*>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order);

public:
    hiai::Status SortNodesDFS();

private:
    USE_ROLE(GraphStore);
    USE_ROLE(GraphListWalker);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SORTER_H
