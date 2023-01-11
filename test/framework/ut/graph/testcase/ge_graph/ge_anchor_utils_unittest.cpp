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
#include <iostream>
#define protected public
#include "framework/graph/core/node/node.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/anchor_utils.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/edge/edge.h"
#undef protected
using namespace ge;
using namespace std;

class UTEST_ge_anchor_utils : public testing::Test {
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

};

TEST_F(UTEST_ge_anchor_utils, InsertTransNode_InDataAnchor)
{
    ComputeGraphPtr graphPtr = ge::ComputeGraph::Make("name");
    OpDescPtr descPtr = std::make_shared<OpDesc>("name1", "type1");
    Node* n1 = graphPtr->ROLE(GraphModifier).AddNode(descPtr);
    NodePtr ptr = graphPtr->ROLE(GraphFinder).FindNodePtr(*n1);
    InDataAnchorPtr a1 = std::make_shared<InDataAnchor>(ptr, 0);

    EXPECT_EQ(AnchorUtils::SetFormat(a1, FORMAT_ND), GRAPH_SUCCESS);
    Format f1 = AnchorUtils::GetFormat(a1);
    EXPECT_EQ(f1, FORMAT_ND);

    InDataAnchorPtr a2 = std::make_shared<InDataAnchor>(ptr, 0);
    EXPECT_EQ(AnchorUtils::SetFormat(nullptr, FORMAT_ND), GRAPH_FAILED);
    Format f2 = AnchorUtils::GetFormat(nullptr);
    EXPECT_EQ(f2, FORMAT_RESERVED);

    //has control edge
    OpDescPtr descPtr1 = std::make_shared<OpDesc>("name1", "type1");
    EXPECT_EQ(descPtr1->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr1->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    OpDescPtr descPtr2 = std::make_shared<OpDesc>("name2", "type2");
    EXPECT_EQ(descPtr2->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddInputDesc("w", TensorDesc(Shape({1, 1, 1, 1}), FORMAT_NCHW)), GRAPH_SUCCESS);
    EXPECT_EQ(descPtr2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW)), GRAPH_SUCCESS);

    ComputeGraphPtr graphPtr1 = ge::ComputeGraph::Make("name");
    n1 = graphPtr1->ROLE(GraphModifier).AddNode(descPtr1);
    Node* n2 = graphPtr1->ROLE(GraphModifier).AddNode(descPtr2);

    EXPECT_EQ(graphPtr1->ROLE(GraphModifier).AddEdge({*n1, -1}, {*n2, -1}), GRAPH_SUCCESS);
    EXPECT_EQ(graphPtr1->ROLE(GraphModifier).AddEdge({*n1, 0}, {*n2, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graphPtr1->ROLE(GraphModifier).RemoveEdge(*n1, *n2), GRAPH_SUCCESS);
    std::vector<OpDescPtr> vecOpDesc{descPtr1, descPtr2};
    EXPECT_EQ(GraphUtils::InsertTransNode(*graphPtr, a2, vecOpDesc), 3);
}