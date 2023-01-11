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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_H

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "framework/graph/core/node/legacy_node.h"

namespace ge {
class NodeWalker;
class NodeStore;
class NodeSpec;
class NodeConstInput;
class NodeSubGraph;
class NodeCompatibler;
class NodeSerializer;

// NodeSpec must be first.
class GRAPH_API_EXPORT Node : public LegacyNode {
public:
    HAS_ROLE(NodeSpec);
    HAS_ROLE(NodeWalker);
    HAS_ROLE(NodeStore);
    HAS_ROLE(NodeConstInput);
    HAS_ROLE(NodeSubGraph);
    HAS_ROLE(NodeCompatibler);
    HAS_ROLE(NodeSerializer);

protected:
    explicit Node(NodeStore& store);
    virtual ~Node() = default;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_H
