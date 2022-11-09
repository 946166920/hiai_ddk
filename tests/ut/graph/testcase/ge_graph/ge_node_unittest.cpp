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

#include <gtest/gtest.h>
#define protected public
#include "graph/attr_value.h"
#include "graph/tensor.h"
#include "framework/graph/utils/tensor_utils.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/node_utils.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/endpoint.h"
#undef protected
#include <iostream>

using namespace std;
using namespace ge;

class ge_test_node : public testing::Test{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

void CheckOutNodes(const Node* node, const std::vector<Node*>& nodes)
{
    std::size_t i = 0;
    auto ret = node->ROLE(NodeWalker).ListOutDataNodes([&i, &nodes](Node& outnode) {
        EXPECT_EQ(&outnode, nodes.at(1));
        i++;
        return hiai::SUCCESS;
    });
    EXPECT_EQ(i, 1);
    EXPECT_EQ(ret, hiai::SUCCESS);
    i = 0;
    ret = node->ROLE(NodeWalker).ListOutCtrlNodes([&i, &nodes](Node& outnode) {
        EXPECT_EQ(&outnode, nodes.at(2));
        i++;
        return hiai::SUCCESS;
    });
    EXPECT_EQ(i, 1);
    EXPECT_EQ(ret, hiai::SUCCESS);
    i = 0;
    ret = node->ROLE(NodeWalker).ListOutNodes([&i, &nodes](Node& outnode) {
        i++;
        EXPECT_EQ(&outnode, nodes.at(i));
        return hiai::SUCCESS;
    });
    EXPECT_EQ(i, 2);
    EXPECT_EQ(ret, hiai::SUCCESS);
}

TEST_F(ge_test_node, node)
{
    OpDescPtr descPtr = std::make_shared<OpDesc>("name1", "type1");
    EXPECT_EQ(descPtr->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    OpDescPtr descPtr2 = std::make_shared<OpDesc>("name2", "type2");
    EXPECT_EQ(descPtr2->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    ComputeGraphPtr graphPtr = ge::ComputeGraph::Make("name");
    Node* n1 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n1, nullptr);
    Node* n2 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n2, nullptr);
    Node* n3 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n3, nullptr);
    Node* n4 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n4, nullptr);

    auto ret = graphPtr->ROLE(GraphModifier).AddEdge(Endpoint(*n3, 0), Endpoint(*n4, 0));
    EXPECT_EQ(ret, hiai::SUCCESS);

    EXPECT_EQ(n3->ROLE(NodeSpec).OwnerComputeGraphPtr(), graphPtr);
    EXPECT_EQ(n3->ROLE(NodeSpec).Name(), "name1");
    int i = 0;
    ret = n3->ROLE(NodeWalker).ListOutDataEdges([&i](Edge& edge) {
        EXPECT_EQ(edge.DstIdx(), i++);
        return hiai::SUCCESS;
    });
    EXPECT_EQ(ret, hiai::SUCCESS);
    i = 0;
    ret = n3->ROLE(NodeWalker).ListInDataEdges([&i](Edge& edge) {
        EXPECT_EQ(edge.DstIdx(), i++);
        return hiai::SUCCESS;
    });
    EXPECT_EQ(ret, hiai::SUCCESS);

    ret = n4->ROLE(NodeWalker).ListInDataNodes([&](Node& innode) {
        EXPECT_EQ(&innode, n3);
        return hiai::SUCCESS;
    });
    EXPECT_EQ(ret, hiai::SUCCESS);

    ret = n3->ROLE(NodeWalker).ListOutDataNodes([&](Node& outnode) {
        EXPECT_EQ(&outnode, n4);
        return hiai::SUCCESS;
    });
    EXPECT_EQ(ret, hiai::SUCCESS);

    EXPECT_EQ(graphPtr->ROLE(GraphModifier).AddEdge(Endpoint(*n1, 0), Endpoint(*n4, 1)), hiai::SUCCESS);
    n4->ROLE(NodeSpec).SetOwnerComputeGraph(graphPtr);
}

TEST_F(ge_test_node, out_nodes)
{
    OpDescPtr descPtr = std::make_shared<OpDesc>("name1", "type1");
    EXPECT_EQ(descPtr->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    ComputeGraphPtr graphPtr = ge::ComputeGraph::Make("name");
    std::vector<Node*> nodes;
    Node* n1 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n1, nullptr);
    nodes.push_back(n1);
    Node* n2 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n2, nullptr);
    nodes.push_back(n2);
    Node* n4 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n4, nullptr);
    nodes.push_back(n4);

    EXPECT_EQ(graphPtr->ROLE(GraphModifier).AddEdge(Endpoint(*n1, 0), Endpoint(*n2, 0)), hiai::SUCCESS);
    EXPECT_EQ(graphPtr->ROLE(GraphModifier).AddEdge(Endpoint(*n1, -1), Endpoint(*n4, -1)), hiai::SUCCESS);
    EXPECT_EQ(graphPtr->ROLE(GraphModifier).AddEdge(Endpoint(*n2, 0), Endpoint(*n4, 0)), hiai::SUCCESS);

    CheckOutNodes(n1, nodes);

    EXPECT_EQ(n1->ROLE(NodeSpec).OutEdgeSize(), 2);
    EXPECT_EQ(n1->ROLE(NodeSpec).OutDataEdgeSize(), 1);
    EXPECT_EQ(n1->ROLE(NodeSpec).OutCtrlEdgeSize(), 1);
    EXPECT_EQ(n4->ROLE(NodeSpec).InCtrlEdgeSize(), 1);
    EXPECT_EQ(n4->ROLE(NodeSpec).InDataEdgeSize(), 1);
    EXPECT_EQ(n4->ROLE(NodeSpec).InEdgeSize(), 2);
}

TEST_F(ge_test_node, update_opdesc)
{
    OpDescPtr descPtr = std::make_shared<OpDesc>("name1", "type1");
    EXPECT_EQ(descPtr->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    OpDescPtr descPtr2 = std::make_shared<OpDesc>("name2", "type2");
    EXPECT_EQ(descPtr2->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    ComputeGraphPtr graphPtr = ge::ComputeGraph::Make("name");
    Node* n1 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    ASSERT_NE(n1, nullptr);
}


