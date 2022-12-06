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

#include "framework/graph/core/node/node.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_serializer.h"
#include "framework/graph/core/node/node_const_input.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/node/node_compatibler.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/node/node_store.h"
#include "graph/core/node/node_maker.h"

namespace ge {
Node::Node(NodeStore& store) : LegacyNode(store)
{
}

namespace {
// NOTE: NodeStore must be first. For details, see C++ Multiple Inheritance.
class NodeImpl :
    private NodeStore,
    private NodeSpec,
    private NodeWalker,
    private NodeConstInput,
    private NodeSubGraph,
    private NodeCompatibler,
    private NodeSerializer,
    public Node {
public:
    NodeImpl(int64_t nodeId, const OpDescPtr& op, const ComputeGraphPtr& graph)
        : NodeStore(graph, op), Node(static_cast<NodeStore&>(*this))
    {
        NodeSpec::OpDesc().SetId(nodeId);
    }
    ~NodeImpl() override = default;

private:
    NodePtr GetNode() override
    {
        return std::static_pointer_cast<Node>(shared_from_this());
    }

private:
    IMPL_ROLE(Node);
    IMPL_ROLE(NodeStore);
    IMPL_ROLE(NodeSpec);
    IMPL_ROLE(NodeWalker);
    IMPL_ROLE(NodeConstInput);
    IMPL_ROLE(NodeSubGraph);
    IMPL_ROLE(NodeCompatibler);
    IMPL_ROLE(NodeSerializer);
};
} // namespace

int64_t NewNodeId()
{
    static int64_t nodeId_ = 0;
    return nodeId_++;
}

NodePtr NodeMaker::Make(const OpDescPtr& op, const ComputeGraphPtr& ownerGraph)
{
    HIAI_EXPECT_NOT_NULL_R(op, nullptr);

    NodePtr node = hiai::make_shared_nothrow<NodeImpl>(NewNodeId(), op, ownerGraph);
    HIAI_EXPECT_NOT_NULL_R(node, nullptr);

    HIAI_EXPECT_EXEC_R(node->ROLE(NodeStore).Init(), nullptr);

    return node;
}
} // namespace ge