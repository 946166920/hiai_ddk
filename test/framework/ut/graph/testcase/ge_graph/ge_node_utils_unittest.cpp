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
#include "framework/graph/core/node/node.h"
#include "framework/graph/utils/node_utils.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/core/edge/edge.h"
#include <iostream>

using namespace std;
using namespace ge;


class ge_test_node_utils : public testing::Test {
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};


TEST_F(ge_test_node_utils, base)
{
    OpDescPtr descPtr = std::make_shared<OpDesc>("name1", "type1");
    ComputeGraphPtr graphPtr = ge::ComputeGraph::Make("1");
    Node* n = graphPtr->ROLE(GraphModifier).AddNode(descPtr);

    OpDescPtr descPtr1 = std::make_shared<OpDesc>("name1", "type1");
    EXPECT_EQ(descPtr1->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr1->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    OpDescPtr descPtr2 = std::make_shared<OpDesc>("name2", "type2");
    EXPECT_EQ(descPtr2->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    ComputeGraphPtr graphPtr1 = ge::ComputeGraph::Make("name");
    Node* n1 = graphPtr1->ROLE(GraphModifier).AddNode(descPtr1);
    Node* n2 = graphPtr1->ROLE(GraphModifier).AddNode(descPtr2);

    EXPECT_EQ(graphPtr1->ROLE(GraphModifier).AddEdge({*n1, 0}, {*n2, 0}), GRAPH_SUCCESS);
}
