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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SPEC_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SPEC_H

#include <cstddef>
#include <string>
#include <vector>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"

namespace ge {
class Node;
class ComputeGraph;
class GraphStore;
class GraphListWalker;

EXPORT_ROLE(GraphSpec)
{
public:
    bool IsEmpty() const;
    std::size_t NodesNum() const;
    std::size_t InNodesNum() const;
    std::size_t OutNodesNum() const;

    const std::string& Name() const;
    hiai::Status SetName(std::string name);

    hiai::Status IsValid() const;
    const ge::Node* OwnerNode() const;
    hiai::Status SetOwnerNode(Node* ownerNode);

private:
    USE_ROLE(ComputeGraph);
    USE_ROLE(GraphStore);
    USE_ROLE(GraphListWalker);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SPEC_H
