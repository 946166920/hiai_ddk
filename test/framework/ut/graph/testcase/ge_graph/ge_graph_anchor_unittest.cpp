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
#define private public
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/anchor_utils.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/edge.h"
#undef protected
#undef private

using namespace ge;
using namespace std;

class UTEST_ge_anchor : public testing::Test {
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

};

TEST_F(UTEST_ge_anchor, data_anchor_test)
{
    cout << "data anchor test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr_1 = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr_1);

    OpDescPtr in_op_ptr_2 = std::make_shared<OpDesc>("in_op_2", "float");
    in_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_2->AddInputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_2->AddOutputDesc("z", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr_2);
    InDataAnchorPtr in_data_anchor_x = in_owner_node_2->GetInDataAnchor(0);
    InDataAnchorPtr in_data_anchor_y = in_owner_node_2->GetInDataAnchor(1);
    InControlAnchorPtr in_control_anchor = in_owner_node_2->GetInControlAnchor();

    OpDescPtr out_op_ptr_1 = std::make_shared<OpDesc>("out_op_1", "float");
    out_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_1);
    OutDataAnchorPtr out_data_anchor_1 = out_owner_node_1->GetOutDataAnchor(0);

    OpDescPtr out_op_ptr_2 = std::make_shared<OpDesc>("out_op_2", "float");
    out_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_2);;
    OutDataAnchorPtr out_data_anchor_2 = out_owner_node_2->GetOutDataAnchor(0);

    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*out_owner_node_1, 0}, {*in_owner_node_1, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*out_owner_node_1, 0}, {*in_owner_node_2, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*out_owner_node_2, 0}, {*in_owner_node_2, 1}), GRAPH_SUCCESS);
    EXPECT_EQ(out_data_anchor_2->GetPeerAnchors().size(), 1);
    EXPECT_EQ(out_data_anchor_2->GetPeerInDataAnchors().size(), 1);
    EXPECT_EQ(out_data_anchor_1->GetPeerAnchors().size(), 2);
    EXPECT_NE(in_data_anchor_y->GetPeerOutAnchor(), nullptr);
    EXPECT_EQ(in_data_anchor_x->GetIdx(), 0);
    EXPECT_NE(in_data_anchor_y->GetOwnerNode(), nullptr);
    EXPECT_EQ(out_data_anchor_1->GetPeerInDataAnchors().size(), 2);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*in_owner_node_2, 1}, {*in_owner_node_2, 0}), GRAPH_FAILED);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*out_owner_node_2, 0}, {*in_owner_node_1, 0}), GRAPH_FAILED);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*out_owner_node_2, 0}, {*in_owner_node_2, 0}), GRAPH_FAILED);
    out_data_anchor_1->UnlinkAll();
    EXPECT_EQ(out_data_anchor_1->GetPeerInDataAnchors().size(), 0);
    out_data_anchor_2->UnlinkAll();
    EXPECT_EQ(out_data_anchor_2->GetPeerAnchors().size(), 0);

}

TEST_F(UTEST_ge_anchor, data_anchor_exception_test)
{
    cout << "data anchor exception test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    InDataAnchorPtr in_data_anchor = in_owner_node->GetInDataAnchor(0);

    OpDescPtr out_op_ptr_1 = std::make_shared<OpDesc>("out_op_1", "float");
    out_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_1);
    OutDataAnchorPtr out_data_anchor_1 = out_owner_node_1->GetOutDataAnchor(0);

    OpDescPtr out_op_ptr_2 = std::make_shared<OpDesc>("out_op_2", "float");
    out_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_2);;
    OutDataAnchorPtr out_data_anchor_2 = out_owner_node_2->GetOutDataAnchor(0);

    EXPECT_EQ(in_data_anchor->LinkFrom(nullptr), GRAPH_FAILED);
    EXPECT_EQ(out_data_anchor_2->LinkTo(InDataAnchorPtr(nullptr)), GRAPH_FAILED);
    EXPECT_EQ(in_data_anchor->Unlink(nullptr), GRAPH_FAILED);
    in_data_anchor->LinkFrom(out_data_anchor_1);
    EXPECT_EQ(out_data_anchor_2->LinkTo(in_data_anchor), GRAPH_FAILED);
    EXPECT_EQ(in_data_anchor->LinkFrom(out_data_anchor_2), GRAPH_FAILED);
    EXPECT_EQ(in_data_anchor->Unlink(out_data_anchor_2), GRAPH_FAILED);
    in_data_anchor->Unlink(out_data_anchor_1);
    EXPECT_EQ(in_data_anchor->GetPeerOutAnchor(), nullptr);
}


TEST_F(UTEST_ge_anchor, control_anchor_test)
{
    cout << "control anchor test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr_1 = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr_1);
    InControlAnchorPtr in_control_anchor_1 = in_owner_node_1->GetInControlAnchor();

    OpDescPtr in_op_ptr_2 = std::make_shared<OpDesc>("in_op_2", "float");
    in_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_2->AddInputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr_2->AddOutputDesc("z", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr_2);
    InControlAnchorPtr in_control_anchor_2 = in_owner_node_2->GetInControlAnchor();

    OpDescPtr out_op_ptr_1 = std::make_shared<OpDesc>("out_op_1", "float");
    out_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_1);
    OutControlAnchorPtr out_control_anchor_1 = out_owner_node_1->GetOutControlAnchor();

    OpDescPtr out_op_ptr_2 = std::make_shared<OpDesc>("out_op_2", "float");
    out_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_2);;
    OutControlAnchorPtr out_control_anchor_2 = out_owner_node_2->GetOutControlAnchor();


    EXPECT_EQ(in_control_anchor_1->LinkFrom(out_control_anchor_1), GRAPH_SUCCESS);
    EXPECT_EQ(out_control_anchor_1->LinkTo(in_control_anchor_2), GRAPH_SUCCESS);
    EXPECT_EQ(in_control_anchor_2->LinkFrom(out_control_anchor_2), GRAPH_SUCCESS);
    EXPECT_EQ(in_control_anchor_1->GetPeerAnchors().size(), 1);
    EXPECT_EQ(in_control_anchor_2->GetPeerOutControlAnchors().size(), 2);
    EXPECT_NE(in_control_anchor_2->GetOwnerNode(), nullptr);
    EXPECT_EQ(out_control_anchor_1->GetPeerInControlAnchors().size(), 2);

    EXPECT_EQ(in_control_anchor_1->Unlink(out_control_anchor_2), GRAPH_FAILED);
    EXPECT_EQ(out_control_anchor_2->Unlink(in_control_anchor_1), GRAPH_FAILED);
    EXPECT_EQ(in_control_anchor_1->Unlink(in_control_anchor_2), GRAPH_FAILED);

    EXPECT_EQ(out_control_anchor_2->Unlink(in_control_anchor_2), GRAPH_SUCCESS);
    EXPECT_EQ(in_control_anchor_2->GetPeerOutControlAnchors().size(), 1);
    EXPECT_EQ(out_control_anchor_1->GetPeerAnchors().size(), 2);
    out_control_anchor_1->UnlinkAll();
    EXPECT_EQ(out_control_anchor_1->GetPeerAnchors().size(), 0);
}

TEST_F(UTEST_ge_anchor, control_anchor_exception_test)
{
    cout << "control anchor exception test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* in_owner_node = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    InControlAnchorPtr in_control_anchor = in_owner_node->GetInControlAnchor();

    OpDescPtr out_op_ptr_1 = std::make_shared<OpDesc>("out_op_1", "float");
    out_op_ptr_1->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_1->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_1 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_1);
    OutControlAnchorPtr out_control_anchor_1 = out_owner_node_1->GetOutControlAnchor();

    OpDescPtr out_op_ptr_2 = std::make_shared<OpDesc>("out_op_2", "float");
    out_op_ptr_2->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    out_op_ptr_2->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* out_owner_node_2 = graph_ptr->ROLE(GraphModifier).AddNode(out_op_ptr_2);;
    OutControlAnchorPtr out_control_anchor_2 = out_owner_node_2->GetOutControlAnchor();

    EXPECT_EQ(in_control_anchor->LinkFrom(nullptr), GRAPH_FAILED);
    EXPECT_EQ(out_control_anchor_1->LinkTo(nullptr), GRAPH_FAILED);
    EXPECT_EQ(in_control_anchor->Unlink(nullptr), GRAPH_FAILED);
    in_control_anchor->LinkFrom(out_control_anchor_1);
    EXPECT_EQ(in_control_anchor->Unlink(out_control_anchor_2), GRAPH_FAILED);
    in_control_anchor->Unlink(out_control_anchor_1);
    EXPECT_EQ(in_control_anchor->GetPeerOutControlAnchors().size(), 0);
}

TEST_F(UTEST_ge_anchor, anchor_utils_test)
{
    cout << "anchor utils test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr relu_op_ptr = std::make_shared<OpDesc>("relu", "float");
    relu_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    relu_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* relu_node = graph_ptr->ROLE(GraphModifier).AddNode(relu_op_ptr);

    EXPECT_EQ(AnchorUtils::SetFormat(relu_node->GetInDataAnchor(0), FORMAT_NC1HWC0), GRAPH_SUCCESS);
    EXPECT_EQ(AnchorUtils::GetFormat(relu_node->GetInDataAnchor(0)), FORMAT_NC1HWC0);

    // exception
    EXPECT_EQ(AnchorUtils::SetFormat(relu_node->GetInDataAnchor(2), FORMAT_NCHW), GRAPH_FAILED);
    EXPECT_EQ(AnchorUtils::GetFormat(relu_node->GetInDataAnchor(2)), FORMAT_RESERVED);

    EXPECT_EQ(AnchorUtils::SetFormat(relu_node->GetOutDataAnchor(0), FORMAT_NC1HWC0), GRAPH_SUCCESS);
    EXPECT_EQ(AnchorUtils::GetFormat(relu_node->GetOutDataAnchor(0)), FORMAT_NC1HWC0);

    // exception
    EXPECT_EQ(AnchorUtils::SetFormat(relu_node->GetOutDataAnchor(0), FORMAT_RESERVED), GRAPH_FAILED);
    EXPECT_EQ(AnchorUtils::GetFormat(relu_node->GetOutDataAnchor(1)), FORMAT_RESERVED);
}

TEST_F(UTEST_ge_anchor, graph_utils_test)
{
    cout << "graph utils test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr conv_op_ptr = std::make_shared<OpDesc>("conv", "float");
    conv_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 16, 16, 16}), FORMAT_NCHW));
    conv_op_ptr->AddInputDesc("w", TensorDesc(Shape({32, 16, 3, 3}), FORMAT_FRACTAL_Z));
    conv_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* conv_node = graph_ptr->ROLE(GraphModifier).AddNode(conv_op_ptr);


    OpDescPtr bn_op_ptr = std::make_shared<OpDesc>("bn", "float");
    bn_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    bn_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* bn_node = graph_ptr->ROLE(GraphModifier).AddNode(bn_op_ptr);

    EXPECT_EQ(GraphUtils::AddEdge(nullptr, conv_node->GetInDataAnchor(0)), GRAPH_FAILED);
    EXPECT_EQ(GraphUtils::AddEdge(nullptr, FORMAT_NCHW, conv_node->GetInDataAnchor(0), FORMAT_NCHW), GRAPH_FAILED);

    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*conv_node, 0}, {*bn_node, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*conv_node, 0}, {*bn_node, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(GraphUtils::AddEdge(conv_node->GetOutDataAnchor(0), FORMAT_NC1HWC0, bn_node->GetInDataAnchor(0),
                                  FORMAT_NC1HWC0), GRAPH_SUCCESS);

    EXPECT_EQ(GraphUtils::AddEdge(OutControlAnchorPtr(nullptr), bn_node->GetInControlAnchor()), GRAPH_FAILED);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*conv_node, -1}, {*bn_node, -1}), GRAPH_SUCCESS);

    OpDescPtr relu_op_ptr = std::make_shared<OpDesc>("relu", "float");
    relu_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    relu_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* relu_node = graph_ptr->ROLE(GraphModifier).AddNode(relu_op_ptr);

    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*conv_node, -1}, {*bn_node, -1}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*conv_node, -1}, {*relu_node, -1}), GRAPH_FAILED);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).RemoveEdge({*conv_node, 0}, {*bn_node, 0}), GRAPH_SUCCESS);
}

TEST_F(UTEST_ge_anchor, data_anchor_replace_peer)
{
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr->AddInputDesc("x1", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddInputDesc("x2", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddInputDesc("x3", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y1", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y2", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y3", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* node1 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    Node* node2 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    Node* node3 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);

    OutDataAnchorPtr out_data_anchor = node1->GetOutDataAnchor(1);
    InDataAnchorPtr in_data_anchor = node2->GetInDataAnchor(1);
    EXPECT_EQ(out_data_anchor != nullptr, true);
    EXPECT_EQ(in_data_anchor != nullptr, true);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 1}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 2}), GRAPH_SUCCESS);

    int out_idx = 0;
    for(; out_idx < out_data_anchor->peerAnchors_.size(); out_idx++){
        if(out_data_anchor->peerAnchors_[out_idx].lock() == in_data_anchor)
        {
            break;
        }
    }
    EXPECT_EQ(out_idx, 1);

    int in_idx = 0;
    for(; in_idx < in_data_anchor->peerAnchors_.size(); in_idx++){
        if(in_data_anchor->peerAnchors_[in_idx].lock() == out_data_anchor)
        {
            break;
        }
    }
    EXPECT_EQ(in_idx, 0);

    graph_ptr->ROLE(GraphModifier).RemoveEdge({*node1, 1}, {*node2, 1});
    graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node3, 1});
    graph_ptr->ROLE(GraphModifier).AddEdge({*node3, 1}, {*node2, 1});

    int out_idx1 = 0;
    for(; out_idx1 < out_data_anchor->peerAnchors_.size(); out_idx1++){
        if(out_data_anchor->peerAnchors_[out_idx1].lock() == node3->GetInDataAnchor(1))
        {
            break;
        }
    }
    EXPECT_EQ(out_idx1, 2);

    int in_idx1 = 0;
    for(; in_idx1 < in_data_anchor->peerAnchors_.size(); in_idx1++){
        if(in_data_anchor->peerAnchors_[in_idx1].lock() == node3->GetOutDataAnchor(1))
        {
            break;
        }
    }
    EXPECT_EQ(in_idx1, in_idx);
}

TEST_F(UTEST_ge_anchor, graph_utils_insert_node)
{
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr in_op_ptr = std::make_shared<OpDesc>("in_op_1", "float");
    in_op_ptr->AddInputDesc("x1", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddInputDesc("x2", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddInputDesc("x3", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y1", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y2", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    in_op_ptr->AddOutputDesc("y3", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* node1 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    Node* node2 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);
    Node* node3 = graph_ptr->ROLE(GraphModifier).AddNode(in_op_ptr);

    OutDataAnchorPtr out_data_anchor = node1->GetOutDataAnchor(1);
    InDataAnchorPtr in_data_anchor = node2->GetInDataAnchor(1);
    EXPECT_EQ(out_data_anchor != nullptr, true);
    EXPECT_EQ(in_data_anchor != nullptr, true);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 0}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 1}), GRAPH_SUCCESS);
    EXPECT_EQ(graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node2, 2}), GRAPH_SUCCESS);

    int out_idx = 0;
    for(; out_idx < out_data_anchor->peerAnchors_.size(); out_idx++){
        if(out_data_anchor->peerAnchors_[out_idx].lock() == in_data_anchor)
        {
            break;
        }
    }
    EXPECT_EQ(out_idx, 1);

    int in_idx = 0;
    for(; in_idx < in_data_anchor->peerAnchors_.size(); in_idx++){
        if(in_data_anchor->peerAnchors_[in_idx].lock() == out_data_anchor)
        {
            break;
        }
    }
    EXPECT_EQ(in_idx, 0);

    graph_ptr->ROLE(GraphModifier).RemoveEdge({*node1, 1}, {*node2, 1});
    graph_ptr->ROLE(GraphModifier).AddEdge({*node1, 1}, {*node3, 0});
    graph_ptr->ROLE(GraphModifier).AddEdge({*node3, 1}, {*node2, 0});

    int out_idx1 = 0;
    for(; out_idx1 < out_data_anchor->peerAnchors_.size(); out_idx1++){
        if(out_data_anchor->peerAnchors_[out_idx1].lock() == node3->GetInDataAnchor(0))
        {
            break;
        }
    }
    EXPECT_EQ(out_idx1, 2);

    int in_idx1 = 0;
    for(; in_idx1 < in_data_anchor->peerAnchors_.size(); in_idx1++){
        if(in_data_anchor->peerAnchors_[in_idx1].lock() == node3->GetOutDataAnchor(0))
        {
            break;
        }
    }
    EXPECT_EQ(in_idx1, in_idx);

}
TEST_F(UTEST_ge_anchor, anchor_utils_test_Status)
{
    cout << "anchor utils test!" << endl;
    ComputeGraphPtr graph_ptr = ge::ComputeGraph::Make("graph");
    OpDescPtr relu_op_ptr = std::make_shared<OpDesc>("relu", "float");
    relu_op_ptr->AddInputDesc("x", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    relu_op_ptr->AddOutputDesc("y", TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW));
    Node* relu_node = graph_ptr->ROLE(GraphModifier).AddNode(relu_op_ptr);
    AnchorStatus tmp = AnchorUtils::GetStatus(relu_node->GetInDataAnchor(0));
    EXPECT_EQ(tmp, 0);
    EXPECT_EQ(AnchorUtils::SetStatus(relu_node->GetInDataAnchor(0), ge::AnchorStatus::ANCHOR_DATA), GRAPH_SUCCESS);
    EXPECT_EQ(AnchorUtils::SetStatus(relu_node->GetInDataAnchor(0), ge::AnchorStatus::ANCHOR_RESERVED), GRAPH_FAILED);
}