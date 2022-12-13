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
#include <queue>
#include <map>
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_serializer.h"

#include <utility>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <functional>

#define private public
#define protected public
#include "graph/model.h"
#include "graph/attr_value.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/tensor_utils.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/utils/op_desc_utils.h"
#undef private
#undef protected

#include "proto/ge_ir.pb.h"

using namespace ge;
using namespace std;

using std::vector;
using std::string;

void LinkEdge(const ComputeGraph& graph, Node& srcNode, int32_t srcIndex, Node& dstNode, int32_t dstIndex)
{
    EXPECT_EQ(graph.ROLE(GraphModifier).AddEdge(Endpoint(srcNode, srcIndex), Endpoint(dstNode, dstIndex)),
        hiai::SUCCESS);
}

Node* CreateNode(OpDescPtr op, ComputeGraph& ownerGraph)
{
    Node* node = ownerGraph.ROLE(GraphModifier).AddNode(op);
    EXPECT_NE(node, nullptr);
    return node;
}

void CompareShape(const vector<int64_t>& shape1, const vector<int64_t>& shape2)
{
    EXPECT_EQ(shape1.size(), shape2.size());
    if (shape1.size() == shape2.size()) {
        for (int i = 0; i < shape1.size(); i++) {
            EXPECT_EQ(shape1[i], shape2[i]);
        }
    }
}

template<typename T>
void CompareList(const vector<T>& val1, const vector<T>& val2)
{
    EXPECT_EQ(val1.size(), val2.size());
    if (val1.size() == val2.size()) {
        for (int i = 0; i < val1.size(); i++) {
            EXPECT_EQ(val1[i], val2[i]);
        }
    }
}

static bool NamedAttrsSimpleCmp(const AttrValue& left, const AttrValue& right)
{
    AttrValue::NamedAttrs val1, val2;
    val1 = left.GetNamedAttrs();
    val2 = right.GetNamedAttrs();
    if (val1.GetName() != val2.GetName()) {
        return false;
    }
    auto attrs1 = val1.GetAllAttrs();
    auto attrs2 = val2.GetAllAttrs();
    if (attrs1.size() != attrs1.size()) {
        return false;
    }

    for (auto it: attrs1) {
        auto it2 = attrs2.find(it.first);
        if (it2 == attrs2.end()) { // simple Check
            return false;
        }
        if(it.second.GetValueType() != it2->second.GetValueType()) {
            return false;
        }
        switch (it.second.GetValueType()) {
            case AttrValue::VT_INT:{
                int64_t i1 = 0, i2 = 0;
                i1 = it.second.GetInt();
                i2 = it2->second.GetInt();
                if(i1 != i2) {
                    return false;
                }
            }
            case AttrValue::VT_FLOAT:{
                AttrValue::FLOAT i1 = 0, i2 = 0;
                i1 = it.second.GetFloat();
                i2 = it2->second.GetFloat();
                if(i1 != i2) {
                    return false;
                }
            }
            case AttrValue::VT_STRING:{
                string i1, i2;
                i1 = it.second.GetString();
                i2 = it2->second.GetString();
                if(i1 != i2) {
                    return false;
                }
            }
            case AttrValue::VT_BOOL:{
                bool i1 = false, i2 = false;
                i1 = it.second.GetBool();
                i2 = it2->second.GetBool();
                if(i1 != i2) {
                    return false;
                }
            }
        }
    }
    return true;
}

static AttrValue::NamedAttrs CreateNamedAttrs(const string& name, std::map<string, AttrValue> map)
{
    AttrValue::NamedAttrs namedAttrs;
    namedAttrs.SetName(name);
    for(auto it :map) {
        namedAttrs.SetAttr(it.first, it.second);
    }
    return namedAttrs;
}

static std::vector<Node*> GetAllNodes(const ComputeGraph& graph)
{
    std::vector<Node*> nodes;
    auto ret = graph.ROLE(GraphListWalker).WalkAllNodes([&nodes](Node& node) {
        nodes.push_back(&node);
        return hiai::SUCCESS;
    });
    EXPECT_EQ(ret, hiai::SUCCESS);
    return nodes;
}

static void CheckAllNodesInGraph(const ComputeGraph& graph)
{
    auto nodes = GetAllNodes(graph);
    ASSERT_EQ(nodes.size(), 4);
    auto node1 = nodes.at(0);
    EXPECT_EQ(node1->ROLE(NodeSpec).Name(), "test");
    EXPECT_EQ(node1->ROLE(NodeWalker).InDataNode(0), nullptr);
    EXPECT_EQ(nodes.at(1)->ROLE(NodeSpec).Name(), "test2");
    {
        std::size_t idx = 0;
        auto ret = node1->ROLE(NodeWalker).ListInCtrlNodes([&idx, &nodes](Node& node) {
            idx++;
            EXPECT_EQ(&node, nodes.at(idx));
            return hiai::SUCCESS;
        });
        ASSERT_EQ(idx, 2);
        EXPECT_EQ(ret, hiai::SUCCESS);
    }
    {
        std::size_t idx = 0;
        auto ret = node1->ROLE(NodeWalker).ListOutCtrlNodes([&idx, &nodes](Node& node) {
            idx++;
            EXPECT_EQ(&node, nodes.at(3));
            return hiai::SUCCESS;
        });
        ASSERT_EQ(idx, 1);
        EXPECT_EQ(ret, hiai::SUCCESS);
    }
    {
        std::size_t idx = 0;
        auto ret = graph.ROLE(GraphListWalker).WalkInNodes([&idx, &nodes](Node& inputNode) {
            idx++;
            EXPECT_EQ(&inputNode, nodes.at(idx));
            return hiai::SUCCESS;
        });
        ASSERT_EQ(idx, 2);
        EXPECT_EQ(ret, hiai::SUCCESS);
    }
    {
        std::size_t idx = 0;
        auto ret = graph.ROLE(GraphListWalker).WalkOutNodes([&idx, &nodes](Node& outputNode) {
            idx++;
            EXPECT_EQ(&outputNode, nodes.at(3));
            return hiai::SUCCESS;
        });
        ASSERT_EQ(idx, 1);
        EXPECT_EQ(ret, hiai::SUCCESS);
    }
}

static void CheckSubGraphNodes(const ComputeGraph& graph, const std::string& name)
{
    auto subNodes = GetAllNodes(graph);
    ASSERT_EQ(subNodes.size(), 1);
    auto subNode = subNodes.at(0);
    EXPECT_EQ(subNode->ROLE(NodeSpec).Name(), name);
}

TEST(UTEST_ge_model_serialize, simple)
{
    Model model("model_name", "custom version3.0");
    model.SetAttr("model_key1", AttrValue::CreateFrom(static_cast<int64_t>(123)));
    model.SetAttr("model_key2", AttrValue::CreateFrom(static_cast<float>(456.78f)));
    model.SetAttr("model_key3", AttrValue::CreateFrom(string("abcd")));
    std::vector<int64_t> intV = {123, 456};
    model.SetAttr("model_key4", AttrValue::CreateFrom(intV));
    std::vector<float> fV = {456.78f, 998.90f};
    model.SetAttr("model_key5", AttrValue::CreateFrom(fV));
    std::vector<std::string> sV = {"abcd", "happy"};
    model.SetAttr("model_key6", AttrValue::CreateFrom(sV));
    model.SetAttr("model_key7", AttrValue::CreateFrom(false));
    std::vector<bool> bV = {true, false};
    model.SetAttr("model_key8", AttrValue::CreateFrom(bV));

    auto computeGraph = ge::ComputeGraph::Make("graph_name");

    // input
    auto inputOp = std::make_shared<OpDesc>("input", "Input");
    inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
    auto input = CreateNode(inputOp, *computeGraph);
    // w1
    auto w1Op = std::make_shared<OpDesc>("w1", "ConstOp");
    w1Op->AddOutputDesc(TensorDesc(Shape({12, 2, 64, 64, 16}), FORMAT_NC1HWC0, DT_FLOAT16));
    auto w1 = CreateNode(w1Op, *computeGraph);

    // node1
    auto node1Op = std::make_shared<OpDesc>("node1", "Conv2D");
    node1Op->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
    node1Op->AddInputDesc(TensorDesc(Shape({12, 2, 64, 64, 16}), FORMAT_NC1HWC0, DT_FLOAT16));
    node1Op->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
    auto node1 = CreateNode(node1Op, *computeGraph);

    // Attr set
    node1Op->SetAttr("node_key1", AttrValue::CreateFrom(Buffer(10)));

    auto namedAttrs1 = AttrValue::CreateFrom(
            CreateNamedAttrs("my_name", {{"int_val", AttrValue::CreateFrom(
                            static_cast<int64_t>(123))},
                   {"str_val", AttrValue::CreateFrom(string("abc"))},
                   {"float_val", AttrValue::CreateFrom(static_cast<float>(345.345))}}));

    node1Op->SetAttr("node_key3", std::move(namedAttrs1));
    // tensor
    auto tensorData1 = "qwertyui";
    auto tensor1 = std::make_shared<Tensor>(TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_INT8), (uint8_t*) tensorData1,
                                            8);
    auto tensorData2 = "asdfqwertyui";
    auto tensor2 = std::make_shared<Tensor>(TensorDesc(Shape({3, 2, 2}), FORMAT_ND, DT_UINT8), (uint8_t*) tensorData2,
                                            12);
    auto tensorData3 = "ghjkasdfqwertyui";
    auto tensor3 = std::make_shared<Tensor>(TensorDesc(Shape({4, 2, 2}), FORMAT_ND, DT_UINT16), (uint8_t*) tensorData3,
                                            16);
    node1Op->SetAttr("node_key5", AttrValue::CreateFrom(tensor1));
    std::vector<TensorPtr> tensorV = {tensor2, tensor3};
    node1Op->SetAttr("node_key6", AttrValue::CreateFrom(tensorV));

    auto tensorDesc = TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_INT16);
    TensorUtils::SetSize(tensorDesc, 100);
    node1Op->SetAttr("node_key7", AttrValue::CreateFrom(tensorDesc));
    std::vector<TensorDesc> tensorDesvV = {TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_INT32), TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_UINT32),
             TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_INT64), TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_UINT64),
             TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_BOOL),
             TensorDesc(Shape({2, 2, 2}), FORMAT_NCHW, DT_DOUBLE)};
    node1Op->SetAttr("node_key8", AttrValue::CreateFrom(tensorDesvV));

    LinkEdge(*computeGraph, *input, 0, *node1, 0);
    LinkEdge(*computeGraph, *w1, 0, *node1, 1);

    Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
    model.SetGraph(graph);

    Buffer buffer;
    ASSERT_EQ(model.Save(buffer), GRAPH_SUCCESS);
    EXPECT_TRUE(buffer.GetData() != nullptr);

    Model model2;
    ASSERT_EQ(model2.Load(buffer.GetData(), buffer.GetSize()), GRAPH_SUCCESS);
    EXPECT_EQ(model2.GetName(), "model_name");
    AttrValue::INT modelVal1;
    AttrUtils::GetInt(&model2, "model_key1", modelVal1);
    EXPECT_EQ(modelVal1, 123);

    AttrValue::FLOAT modelVal2;
    AttrUtils::GetFloat(&model2, "model_key2", modelVal2);
    EXPECT_EQ(modelVal2, (float) 456.78f);

    AttrValue::STR modelVal3;
    AttrUtils::GetStr(&model2, "model_key3", modelVal3);
    EXPECT_EQ(modelVal3, "abcd");

    AttrValue::LIST_INT modelVal4;
    AttrUtils::GetListInt(&model2, "model_key4", modelVal4);
    CompareList(modelVal4, {123, 456});

    AttrValue::LIST_FLOAT modelVal5;
    AttrUtils::GetListFloat(&model2, "model_key5", modelVal5);
    CompareList(modelVal5, {456.78f, 998.90f});

    AttrValue::LIST_STR modelVal6;
    AttrUtils::GetListStr(&model2, "model_key6", modelVal6);
    CompareList(modelVal6, {"abcd", "happy"});

    AttrValue::BOOL modelVal7;
    EXPECT_EQ(AttrUtils::GetBool(&model2, "model_key7", modelVal7), true);
    EXPECT_EQ(modelVal7, false);

    AttrValue::LIST_BOOL modelVal8;
    AttrUtils::GetListBool(&model2, "model_key8", modelVal8);
    CompareList(modelVal8, {true, false});

    auto graph2 = model2.GetGraph();
    const auto& sGraph = GraphUtils::GetComputeGraph(graph2);
    ASSERT_TRUE(sGraph != nullptr);
    auto sNodes = GetAllNodes(*sGraph);
    ASSERT_EQ(3, sNodes.size());

    auto sInput = sNodes.at(0);
    auto sW1 = sNodes.at(1);
    auto sNode1 = sNodes.at(2);
    {
        OpDesc& sOp = sInput->ROLE(NodeSpec).OpDesc();
        EXPECT_EQ(sOp.GetName(), "input");
        EXPECT_EQ(sOp.GetType(), "Input");
        auto sInputDesc = sOp.GetAllInputsDesc();
        ASSERT_EQ(sInputDesc.size(), 0);
        auto sOutputDesc = sOp.GetAllOutputsDesc();
        ASSERT_EQ(sOutputDesc.size(), 1);
        auto desc1 = sOutputDesc.at(0);
        EXPECT_EQ(desc1.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(desc1.GetDataType(), DT_FLOAT);
        CompareShape(desc1.GetShape().GetDims(), vector<int64_t>{12, 32, 64, 64});

        auto outDataNode = sInput->ROLE(NodeWalker).OutDataNode(0, 0);
        ASSERT_EQ(outDataNode, sNode1);
    }

    {
        OpDesc& sOp = sW1->ROLE(NodeSpec).OpDesc();
        EXPECT_EQ(sOp.GetName(), "w1");
        EXPECT_EQ(sOp.GetType(), "ConstOp");
        auto sInputDesc = sOp.GetAllInputsDesc();
        ASSERT_EQ(sInputDesc.size(), 0);
        auto sOutputDesc = sOp.GetAllOutputsDesc();
        ASSERT_EQ(sOutputDesc.size(), 1);
        auto desc1 = sOutputDesc.at(0);
        EXPECT_EQ(desc1.GetFormat(), FORMAT_NC1HWC0);
        EXPECT_EQ(desc1.GetDataType(), DT_FLOAT16);
        CompareShape(desc1.GetShape().GetDims(), vector<int64_t>{12, 2, 64, 64, 16});

        auto outDataNode = sW1->ROLE(NodeWalker).OutDataNode(0, 0);
        ASSERT_EQ(outDataNode, sNode1);
    }

    {
        OpDesc& sOp = sNode1->ROLE(NodeSpec).OpDesc();
        EXPECT_EQ(sOp.GetName(), "node1");
        EXPECT_EQ(sOp.GetType(), "Conv2D");
        auto sInputDesc = sOp.GetAllInputsDesc();
        ASSERT_EQ(sInputDesc.size(), 2);

        auto desc1 = sInputDesc.at(0);
        EXPECT_EQ(desc1.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(desc1.GetDataType(), DT_FLOAT);
        CompareShape(desc1.GetShape().GetDims(), vector<int64_t>{12, 32, 64, 64});

        auto desc2 = sInputDesc.at(1);
        EXPECT_EQ(desc2.GetFormat(), FORMAT_NC1HWC0);
        EXPECT_EQ(desc2.GetDataType(), DT_FLOAT16);
        CompareShape(desc2.GetShape().GetDims(), vector<int64_t>{12, 2, 64, 64, 16});

        auto sOutputDesc = sOp.GetAllOutputsDesc();
        ASSERT_EQ(sOutputDesc.size(), 1);
        auto desc3 = sOutputDesc.at(0);
        EXPECT_EQ(desc3.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(desc3.GetDataType(), DT_FLOAT);
        CompareShape(desc3.GetShape().GetDims(), vector<int64_t>{12, 32, 64, 64});

        auto outDataNode = sNode1->ROLE(NodeWalker).OutDataNode(0, 0);
        ASSERT_EQ(outDataNode, nullptr);

        // node attrs
        AttrValue::BYTES nodeVal1;
        AttrUtils::GetBytes(&sOp, "node_key1", nodeVal1);
        ASSERT_EQ(nodeVal1.GetSize(), 10);

        AttrValue s_namedAttrs;
        sOp.GetAttr("node_key3", s_namedAttrs);
        EXPECT_TRUE(NamedAttrsSimpleCmp(s_namedAttrs, namedAttrs1));

        TensorPtr sTensor;
        AttrUtils::GetTensor(&sOp, "node_key5", sTensor);
        ASSERT_TRUE(sTensor != nullptr);
        string str((char*)sTensor->GetData().GetData(), sTensor->GetData().GetSize());
        EXPECT_EQ(str, "qwertyui");

        vector<TensorPtr> s_listTensor;
        AttrUtils::GetListTensor(sOp, "node_key6", s_listTensor);
        ASSERT_EQ(s_listTensor.size(), 2);
        string str2((char*)s_listTensor[0]->GetData().GetData(), s_listTensor[0]->GetData().GetSize());
        EXPECT_EQ(str2, "asdfqwertyui");

        TensorDesc sTensorDesc;
        AttrUtils::GetTensorDesc(&sOp, "node_key7", sTensorDesc);
        EXPECT_EQ(sTensorDesc.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(sTensorDesc.GetDataType(), DT_INT16);
        uint32_t size = 0;
        TensorUtils::GetSize(sTensorDesc, size);
        EXPECT_EQ(size, 100);
    }
}

TEST(UTEST_ge_model_serialize, OpDesc)
{
    // node1Op
    auto node1Op = std::make_shared<OpDesc>("node1", "Conv2D");
    node1Op->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
    node1Op->AddInputDesc(TensorDesc(Shape({12, 2, 64, 64, 16}), FORMAT_NC1HWC0, DT_FLOAT16));
    node1Op->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));

    // Attr set
    node1Op->SetAttr("node_key1", AttrValue::CreateFrom(Buffer(10)));

    auto namedAttrs1 = AttrValue::CreateFrom(CreateNamedAttrs("my_name",
        {{"int_val", AttrValue::CreateFrom(static_cast<int64_t>(123))}, {"str_val", AttrValue::CreateFrom(string("abc"))},
            {"float_val", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(345.345))}}));
    node1Op->SetAttr("node_key3", std::move(namedAttrs1));

    ge::Buffer buffer;
    node1Op->Save(buffer);

    auto sOp = std::make_shared<OpDesc>();
    ASSERT_TRUE(sOp != nullptr);
    sOp->Load(buffer.GetData(), buffer.GetSize());

    {
        EXPECT_EQ(sOp->GetName(), "node1");
        EXPECT_EQ(sOp->GetType(), "Conv2D");
        auto sInputDesc = sOp->GetAllInputsDesc();
        ASSERT_EQ(sInputDesc.size(), 2);

        auto desc1 = sInputDesc.at(0);
        EXPECT_EQ(desc1.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(desc1.GetDataType(), DT_FLOAT);
        CompareShape(desc1.GetShape().GetDims(), vector<int64_t>{12, 32, 64, 64});

        auto desc2 = sInputDesc.at(1);
        EXPECT_EQ(desc2.GetFormat(), FORMAT_NC1HWC0);
        EXPECT_EQ(desc2.GetDataType(), DT_FLOAT16);
        CompareShape(desc2.GetShape().GetDims(), vector<int64_t>{12, 2, 64, 64, 16});

        auto sOutputDesc = sOp->GetAllOutputsDesc();
        ASSERT_EQ(sOutputDesc.size(), 1);
        auto desc3 = sOutputDesc.at(0);
        EXPECT_EQ(desc3.GetFormat(), FORMAT_NCHW);
        EXPECT_EQ(desc3.GetDataType(), DT_FLOAT);
        CompareShape(desc3.GetShape().GetDims(), vector<int64_t>{12, 32, 64, 64});

        // node attrs
        AttrValue::BYTES nodeVal1;
        AttrUtils::GetBytes(sOp, "node_key1", nodeVal1);
        ASSERT_EQ(nodeVal1.GetSize(), 10);

        AttrValue s_namedAttrs;
        sOp->GetAttr("node_key3", s_namedAttrs);
        EXPECT_TRUE(NamedAttrsSimpleCmp(s_namedAttrs, namedAttrs1));
    }
}

TEST(UTEST_ge_model_serialize, test_subGraph)
{
    Model model("model_name", "custom version3.0");
    {
        auto computeGraph = ge::ComputeGraph::Make("graph_name");
        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        auto subComputeGraph = ge::ComputeGraph::Make("sub_graph");
        // input
        auto subGraphInputOp = std::make_shared<OpDesc>("sub_graph_test", "TestOp2");
        subGraphInputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto subGraphInput = CreateNode(subGraphInputOp, *subComputeGraph);

        AttrUtils::SetGraph(inputOp, "sub_graph", subComputeGraph);
    }

    ge::Buffer buffer;
    model.Save(buffer);
    ASSERT_GE(buffer.GetSize(), 0);

    Model model2;
    model2.Load(buffer.GetData(), buffer.GetSize());
    ASSERT_TRUE(model2.GetGraph().IsValid());
    auto graph2 = GraphUtils::GetComputeGraph(model2.GetGraph());
    EXPECT_EQ(graph2->ROLE(GraphSpec).Name(), "graph_name");
    auto nodes2 = GetAllNodes(*graph2);
    ASSERT_EQ(nodes2.size(), 1);
    auto node2 = nodes2.at(0);
    EXPECT_EQ(node2->ROLE(NodeSpec).Name(), "test");
    OpDesc& node2Op = node2->ROLE(NodeSpec).OpDesc();
    EXPECT_EQ(node2Op.GetType(), "TestOp");
    auto node2InputDescs = node2Op.GetAllInputsDesc();
    ASSERT_EQ(node2InputDescs.size(), 1);
    auto node2InputDesc = node2InputDescs.at(0);

    ComputeGraphPtr subComputeGraph2;
    ASSERT_TRUE(AttrUtils::GetGraph(&node2Op, "sub_graph", subComputeGraph2));
    EXPECT_EQ(subComputeGraph2->ROLE(GraphSpec).Name(), "sub_graph");
    auto subNodes2 = GetAllNodes(*subComputeGraph2);
    ASSERT_EQ(subNodes2.size(), 1);
    auto subNode2 = subNodes2.at(0);
    EXPECT_EQ(subNode2->ROLE(NodeSpec).Name(), "sub_graph_test");
    ASSERT_EQ(subNode2->ROLE(NodeSpec).InDataEdgeSize(), 0);
    OpDesc& subNodeOp2 = subNode2->ROLE(NodeSpec).OpDesc();
    EXPECT_EQ(subNodeOp2.GetType(), "TestOp2");
    ASSERT_EQ(subNodeOp2.GetAllInputsDesc().size(), 1);
    auto subNode2InputDesc = subNodeOp2.GetAllInputsDesc().at(0);
    EXPECT_EQ(subNode2InputDesc.GetShape().GetDim(1), 32);
}

TEST(UTEST_ge_model_serialize, test_listSubGraph)
{
    Model model("model_name", "custom version3.0");
    {
        auto computeGraph = ge::ComputeGraph::Make("graph_name");
        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);
    }

    ge::Buffer buffer;
    model.Save(buffer);

    ASSERT_GE(buffer.GetSize(), 0);

    Model model2;
    model2.Load(buffer.GetData(), buffer.GetSize());
    ASSERT_TRUE(model2.GetGraph().IsValid());
    auto graph2 = GraphUtils::GetComputeGraph(model2.GetGraph());
    EXPECT_EQ(graph2->ROLE(GraphSpec).Name(), "graph_name");
    auto nodes2 = GetAllNodes(*graph2);
    ASSERT_EQ(nodes2.size(), 1);
}

TEST(UTEST_ge_model_serialize, test_Format)
{
    Model model("model_name", "custom version3.0");
    {
        auto computeGraph = ge::ComputeGraph::Make("graph_name");
        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NHWC, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_ND, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NC1HWC0, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_FRACTAL_Z, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NC1C0HWPAD, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NHWC1C0, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_FSR_NCHW, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_FRACTAL_DECONV, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_BN_WEIGHT, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_CHWN, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_FILTER_HWCK, DT_FLOAT));
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_FRACTAL_Z_C04, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        model.SetGraph(GraphUtils::CreateGraphFromComputeGraph(computeGraph));
    }
    ge::Buffer buffer;
    model.Save(buffer);
    ASSERT_GE(buffer.GetSize(), 0);
    Model model2;
    model2.Load(buffer.GetData(), buffer.GetSize());
    ASSERT_TRUE(model2.GetGraph().IsValid());

    auto graph = model2.GetGraph();
    ASSERT_TRUE(GraphUtils::GetComputeGraph(graph) != nullptr);
    ASSERT_EQ(GraphUtils::GetComputeGraph(graph)->ROLE(GraphSpec).NodesNum(), 1);

    auto nodes = GetAllNodes(*GraphUtils::GetComputeGraph(graph));
    OpDesc& op = nodes.at(0)->ROLE(NodeSpec).OpDesc();
    auto inputDescs = op.GetAllInputsDesc();
    ASSERT_EQ(inputDescs.size(), 13);
    EXPECT_EQ(inputDescs.at(0).GetFormat(), FORMAT_NCHW);
    EXPECT_EQ(inputDescs.at(1).GetFormat(), FORMAT_NHWC);
    EXPECT_EQ(inputDescs.at(2).GetFormat(), FORMAT_ND);
    EXPECT_EQ(inputDescs.at(3).GetFormat(), FORMAT_NC1HWC0);
    EXPECT_EQ(inputDescs.at(4).GetFormat(), FORMAT_FRACTAL_Z);
    EXPECT_EQ(inputDescs.at(5).GetFormat(), FORMAT_NC1C0HWPAD);
    EXPECT_EQ(inputDescs.at(6).GetFormat(), FORMAT_NHWC1C0);
    EXPECT_EQ(inputDescs.at(7).GetFormat(), FORMAT_FSR_NCHW);
    EXPECT_EQ(inputDescs.at(8).GetFormat(), FORMAT_FRACTAL_DECONV);
    EXPECT_EQ(inputDescs.at(9).GetFormat(), FORMAT_BN_WEIGHT);
    EXPECT_EQ(inputDescs.at(10).GetFormat(), FORMAT_CHWN);
    EXPECT_EQ(inputDescs.at(11).GetFormat(), FORMAT_FILTER_HWCK);
    EXPECT_EQ(inputDescs.at(12).GetFormat(), FORMAT_FRACTAL_Z_C04);
}

TEST(UTEST_ge_model_serialize, test_ControlEdge)
{
    Model model("model_name", "custom version3.0");
    {
        auto computeGraph = ge::ComputeGraph::Make("graph_name");
        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        // sink
        auto sinkOp = std::make_shared<OpDesc>("test2", "Sink");
        auto sink = CreateNode(sinkOp, *computeGraph);
        LinkEdge(*computeGraph, *sink, -1, *input, -1);
        // sink2
        auto sinkOp2 = std::make_shared<OpDesc>("test3", "Sink");
        auto sink2 = CreateNode(sinkOp2, *computeGraph);
        LinkEdge(*computeGraph, *sink2, -1, *input, -1);

        // dest
        auto destOp = std::make_shared<OpDesc>("test4", "Dest");
        auto dest = CreateNode(destOp, *computeGraph);
        LinkEdge(*computeGraph, *input, -1, *dest, -1);

        computeGraph->ROLE(GraphModifier).AddInput(*sink);
        computeGraph->ROLE(GraphModifier).AddInput(*sink2);
        computeGraph->ROLE(GraphModifier).AddOutput(*dest);

        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);
    }
    ge::Buffer buffer;
    model.Save(buffer);
    EXPECT_GE(buffer.GetSize(), 0);

    Model model2;
    model2.Load(buffer.GetData(), buffer.GetSize());
    ASSERT_TRUE(model2.GetGraph().IsValid());
    auto graph = GraphUtils::GetComputeGraph(model2.GetGraph());
    EXPECT_EQ(graph->ROLE(GraphSpec).Name(), "graph_name");

    CheckAllNodesInGraph(*graph);
}

TEST(UTEST_ge_model_serialize, test_SerializeGraph)
{
    auto computeGraph = ge::ComputeGraph::Make("graph_name");
    {
        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddInputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        // sink
        auto sinkOp = std::make_shared<OpDesc>("test2", "Sink");
        auto sink = CreateNode(sinkOp, *computeGraph);
        LinkEdge(*computeGraph, *sink, -1, *input, -1);

        // sink2
        auto sinkOp2 = std::make_shared<OpDesc>("test3", "Sink");
        auto sink2 = CreateNode(sinkOp2, *computeGraph);
        LinkEdge(*computeGraph, *sink2, -1, *input, -1);

        // dest
        auto destOp = std::make_shared<OpDesc>("test4", "Dest");
        auto dest = CreateNode(destOp, *computeGraph);
        LinkEdge(*computeGraph, *input, -1, *dest, -1);

        computeGraph->ROLE(GraphModifier).AddInput(*sink);
        computeGraph->ROLE(GraphModifier).AddInput(*sink2);
        computeGraph->ROLE(GraphModifier).AddOutput(*dest);
    }
    ge::Buffer buffer;
    computeGraph->ROLE(GraphSerializer).Save(buffer);
    EXPECT_GE(buffer.GetSize(), 0);

    auto graph = ge::ComputeGraph::Make("graph_name1");
    graph->ROLE(GraphSerializer).Load(buffer.GetData(), buffer.GetSize());
    ASSERT_TRUE(graph != nullptr);
    EXPECT_EQ(graph->ROLE(GraphSpec).Name(), "graph_name");

    CheckAllNodesInGraph(*graph);
}

TEST(UTEST_ge_model_serialize, test_invalid_Model)
{
    {// empty graph
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        ge::Buffer buffer;
        model.Save(buffer);
        EXPECT_EQ(buffer.GetSize(), 0);
    }
}

TEST(UTEST_ge_model_serialize, test_invalid_Graph_size)
{
    auto computeGraph = ge::ComputeGraph::Make("default");
    ge::Buffer buffer;
    computeGraph->ROLE(GraphSerializer).Save(buffer);
    EXPECT_EQ(buffer.GetSize(), 9);
}

TEST(UTEST_ge_model_serialize, test_invalid_TensorDesc)
{
    {// valid test
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::
        CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        ge::Buffer buffer;
        model.Save(buffer);
        EXPECT_GE(buffer.GetSize(), 0);
    }
    {// invalid format
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_RESERVED, DT_FLOAT)); // invalid format
        auto input = CreateNode(inputOp, *computeGraph);

        auto nodeListNew1 = GetAllNodes(*computeGraph);
        ASSERT_EQ(nodeListNew1.at(0)->ROLE(NodeSpec).OpDesc().GetAllOutputsDesc().size(), 1);


        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        ge::Buffer buffer;
        model.Save(buffer);
        ASSERT_GE(buffer.GetSize(), 0);
        Model model2;
        model2.Load(buffer.GetData(), buffer.GetSize());
        auto graphNew = GraphUtils::GetComputeGraph(model2.GetGraph());
        ASSERT_TRUE(graphNew != nullptr);
        auto nodeListNew = GetAllNodes(*graphNew);
        ASSERT_EQ(nodeListNew.size(), 1);
        OpDesc& opDescNew = nodeListNew.at(0)->ROLE(NodeSpec).OpDesc();
        auto outputDescListNew = opDescNew.GetAllOutputsDesc();
        ASSERT_EQ(outputDescListNew.size(), 0);
    }
    {// DT_UNDEFINED datatype
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_UNDEFINED));
        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);
        ge::Buffer buffer;
        model.Save(buffer);
        ASSERT_GE(buffer.GetSize(), 0);
        Model model2;
        model2.Load(buffer.GetData(), buffer.GetSize());
        auto graphNew = GraphUtils::GetComputeGraph(model2.GetGraph());
        ASSERT_TRUE(graphNew != nullptr);
        auto nodeListNew = GetAllNodes(*graphNew);
        ASSERT_EQ(nodeListNew.size(), 1);
        OpDesc& opDescNew = nodeListNew.at(0)->ROLE(NodeSpec).OpDesc();
        auto outputDescListNew = opDescNew.GetAllOutputsDesc();
        ASSERT_EQ(outputDescListNew.size(), 1);
        auto outputDescNew = outputDescListNew.at(0);
        EXPECT_EQ(outputDescNew.GetDataType(), DT_UNDEFINED);
        EXPECT_EQ(outputDescNew.GetFormat(), FORMAT_NCHW);
    }
}

TEST(UTEST_ge_model_serialize, test_invalid_Attrs)
{
    {// valid test
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));

          AttrValue::NamedAttrs namedAttrs;
          namedAttrs.SetAttr("key1", AttrValue::CreateFrom(static_cast<AttrValue::INT>(10)));
          AttrUtils::SetNamedAttrs(inputOp, "key", namedAttrs);

        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        ge::Buffer buffer;
        model.Save(buffer);
        EXPECT_GE(buffer.GetSize(), 0);
    }
    {// none type
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));

        AttrValue::NamedAttrs namedAttrs;
        EXPECT_EQ(namedAttrs.SetAttr("key1", AttrValue()), GRAPH_FAILED);
    }
    {// bytes attr len is 0
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));

        AttrValue::NamedAttrs namedAttrs;
        namedAttrs.SetAttr("key1", AttrValue::CreateFrom(AttrValue::BYTES(0)));
        AttrUtils::SetNamedAttrs(inputOp, "key", namedAttrs);

        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        ge::Buffer buffer;
        model.Save(buffer);
        EXPECT_GE(buffer.GetSize(), 0);

        Model model2;
        model2.Load(buffer.GetData(), buffer.GetSize());
    }
    {// invalid list bytes attr
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        // input
        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));

        AttrValue::NamedAttrs namedAttrs;
        AttrUtils::SetNamedAttrs(inputOp, "key", namedAttrs);

        auto input = CreateNode(inputOp, *computeGraph);
        Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
        model.SetGraph(graph);

        ge::Buffer buffer;
        model.Save(buffer);
        EXPECT_GE(buffer.GetSize(), 0);
    }
    {// invalid graph attr
        Model model("model_name", "custom version3.0");
        auto computeGraph = ge::ComputeGraph::Make("graph_name");

        auto inputOp = std::make_shared<OpDesc>("test", "TestOp");
        inputOp->AddOutputDesc(TensorDesc(Shape({12, 32, 64, 64}), FORMAT_NCHW, DT_FLOAT));
        AttrValue::NamedAttrs namedAttrs;
        ComputeGraphPtr nullComputeGraphPtr = nullptr;
        vector<ComputeGraphPtr> v = {nullComputeGraphPtr};

        AttrValue attr;
        attr.SetGraph(nullComputeGraphPtr);
        EXPECT_EQ(namedAttrs.SetAttr("key1", attr), GRAPH_FAILED);
        AttrValue value;
        EXPECT_EQ(namedAttrs.GetAttr("key1", value), GRAPH_FAILED);
        EXPECT_TRUE(value.IsEmpty());
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_TensorDesc)
{
    {   // valid
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.mutable_attr();

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_td();
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("NCHW");
        tensorDescAttr->set_dtype(hiai::proto::DataType::DT_INT8);

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
    }
    {   // invalid layout
        hiai::proto::ModelDef modeDeff;
        modeDeff.add_graph();
        auto attrs = modeDeff.mutable_attr();

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_td();
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("InvalidLayout");
        tensorDescAttr->set_dtype(hiai::proto::DataType::DT_INT8);

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());

        TensorDesc tensorDesc;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(model, "key1", tensorDesc));
        EXPECT_EQ(tensorDesc.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc.GetDataType(), DT_INT8);
        model.SetVersion(1);
        EXPECT_EQ(model.GetVersion(), 1);
        EXPECT_TRUE(model.Dump("") != 0);
    }
    {   // invalid datatype
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.mutable_attr();

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_td(); // tensor desc
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("NHWC");
        tensorDescAttr->set_dtype((hiai::proto::DataType) 100);

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        TensorDesc tensorDesc;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(model, "key1", tensorDesc));
        EXPECT_EQ(tensorDesc.GetFormat(), FORMAT_NHWC);
        EXPECT_EQ(tensorDesc.GetDataType(), DT_UNDEFINED);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_TensorDesc2)
{
        {   // invalid datatype
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.mutable_attr();

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_t()->mutable_desc(); // tensor
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("NHWC");
        tensorDescAttr->set_dtype((hiai::proto::DataType) 100);

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        TensorPtr tensor;
        EXPECT_TRUE(AttrUtils::GetTensor(model, "key1", tensor));
        ASSERT_TRUE(tensor != nullptr);
        auto tensorDesc = tensor->GetTensorDesc();
        EXPECT_EQ(tensorDesc.GetFormat(), FORMAT_NHWC);
        EXPECT_EQ(tensorDesc.GetDataType(), DT_UNDEFINED);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_TensorDesc1)
{
    {   // invalid attrmap
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->mutable_attr(); // graph attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_t()->mutable_desc(); // tensor
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("NCHW");
        tensorDescAttr->set_dtype(hiai::proto::DataType::DT_INT8);
        auto attrs1 = tensorDescAttr->mutable_attr();
        auto attr1 = (*attrs1)["key2"]; //empty attr

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        TensorPtr tensor;
        EXPECT_TRUE(AttrUtils::GetTensor(graph, "key1", tensor));
        ASSERT_TRUE(tensor != nullptr);
        auto tensorDesc = tensor->GetTensorDesc();
        AttrValue attrValue;
        EXPECT_EQ(tensorDesc.GetAttr("key2", attrValue), GRAPH_SUCCESS);
        EXPECT_EQ(attrValue.GetValueType(), AttrValue::VT_NONE);
    }
    {   // invalid attrmap2
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto tensorDescAttr = attrDef->mutable_t()->mutable_desc(); // tensor
        tensorDescAttr->set_has_out_attr(true);
        tensorDescAttr->set_layout("NCHW");
        tensorDescAttr->set_dtype(hiai::proto::DataType::DT_INT8);
        auto attrs1 = tensorDescAttr->mutable_attr();
        auto attr1 = (*attrs1)["key2"].mutable_list(); //empty list attr

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        TensorPtr tensor;
        EXPECT_TRUE(AttrUtils::GetTensor(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", tensor));
        ASSERT_TRUE(tensor != nullptr);
        auto tensorDesc = tensor->GetTensorDesc();
        AttrValue attrValue;
        EXPECT_EQ(tensorDesc.GetAttr("key2", attrValue), GRAPH_SUCCESS);
        EXPECT_EQ(attrValue.GetValueType(), AttrValue::VT_NONE);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_Attr1)
{
    {   // invalid graph
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto graphAttr = attrDef->mutable_g();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        ComputeGraphPtr graphAttrNew;
        EXPECT_TRUE(AttrUtils::GetGraph(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", graphAttrNew));
        ASSERT_TRUE(graphAttrNew != nullptr);
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(graphAttrNew, "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
    {   // invalid list graph
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        attrDef->mutable_list()->set_val_type(hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_GRAPH);
        auto graphAttr = attrDef->mutable_list()->add_g();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_Attr2)
{
    {   // invalid namedAttrs
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto graphAttr = attrDef->mutable_func();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        AttrValue::NAMED_ATTRS namedAttrs;
        EXPECT_TRUE(AttrUtils::GetNamedAttrs(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", namedAttrs));
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(namedAttrs, "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
    {   // invalid list namedAttrs
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        attrDef->mutable_list()->set_val_type(hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_NAMED_ATTRS);
        auto graphAttr = attrDef->mutable_list()->add_na();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_Attr3)
{
    {   // invalid tensorDesc
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto graphAttr = attrDef->mutable_td();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        TensorDesc tensorDesc;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", tensorDesc));
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(tensorDesc, "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
    {   // invalid list tensorDesc
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        attrDef->mutable_list()->set_val_type(hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_TENSOR_DESC);
        auto graphAttr = attrDef->mutable_list()->add_tf();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_Attr4)
{
    {   // invalid tensor
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        auto graphAttr = attrDef->mutable_t()->mutable_desc();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        TensorPtr tensor;
        EXPECT_TRUE(AttrUtils::GetTensor(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", tensor));
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(tensor->GetTensorDesc(), "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
    {   // invalid list tensor
        hiai::proto::ModelDef modeDeff;
        auto attrs = modeDeff.add_graph()->add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        attrDef->mutable_list()->set_val_type(hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_TENSOR);
        auto graphAttr = attrDef->mutable_list()->add_t()->mutable_desc();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(modeDeff.ByteSizeLong());
        modeDeff.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        auto graph = GraphUtils::GetComputeGraph(model.GetGraph());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        vector<TensorPtr> tensor;
        EXPECT_TRUE(AttrUtils::GetListTensor(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", tensor));
        ASSERT_EQ(tensor.size(), 1);
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(tensor.at(0)->GetTensorDesc(), "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
    {   // invalid list tensor
        hiai::proto::GraphDef graphDef;
        auto attrs = graphDef.add_op()->mutable_attr(); // node attr

        hiai::proto::AttrDef* attrDef = &(*attrs)["key1"];
        attrDef->mutable_list()->set_val_type(hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_TENSOR);
        auto graphAttr = attrDef->mutable_list()->add_t()->mutable_desc();
        auto attrsOfGraph = graphAttr->mutable_attr();
        auto tensorVal = (*attrsOfGraph)["key2"].mutable_td();
        tensorVal->set_has_out_attr(true);
        tensorVal->set_dtype(hiai::proto::DT_INT8);
        tensorVal->set_layout("invalidLayout");

        Buffer buffer(graphDef.ByteSizeLong());
        graphDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        auto graph = ge::ComputeGraph::Make("graph_name");
        graph->ROLE(GraphSerializer).Load(buffer.MutableData(), buffer.GetSize());
        ASSERT_TRUE(graph != nullptr);
        auto nodes = GetAllNodes(*graph);
        ASSERT_EQ(nodes.size(), 1);
        vector<TensorPtr> tensor;
        EXPECT_TRUE(AttrUtils::GetListTensor(&(nodes.at(0)->ROLE(NodeSpec).OpDesc()), "key1", tensor));
        ASSERT_EQ(tensor.size(), 1);
        TensorDesc tensorDesc1;
        EXPECT_TRUE(AttrUtils::GetTensorDesc(tensor.at(0)->GetTensorDesc(), "key2", tensorDesc1));
        EXPECT_EQ(tensorDesc1.GetFormat(), FORMAT_RESERVED);
        EXPECT_EQ(tensorDesc1.GetDataType(), DT_INT8);
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_InputOutput)
{
    // model invalid node input index
    {
        hiai::proto::ModelDef modelDef;
        auto testGraph = modelDef.add_graph();
        auto opDef0 = testGraph->add_op(); // node attr
        opDef0->set_name("test");

        auto opDef1 = testGraph->add_op(); // node attr
        opDef1->add_input("test:E");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // model invalid node input
    {
        hiai::proto::ModelDef modelDef;
        auto opDef = modelDef.add_graph()->add_op(); // node attr
        opDef->add_input("invalidNodeName:0");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // model invalid node control input
    {
        hiai::proto::ModelDef modelDef;
        auto testGraph = modelDef.add_graph();
        auto opDef = testGraph->add_op(); // node attr
        opDef->add_input("invalidNodeName:-1");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // model invalid graph input index
    {
        hiai::proto::ModelDef modelDef;
        auto testGraph = modelDef.add_graph();
        auto opDef = testGraph->add_op(); // node attr
        opDef->set_name("test");
        testGraph->add_input("test:E");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // model invalid graph input
    {
        hiai::proto::ModelDef modelDef;
        modelDef.add_graph()->add_input("invalidNodeName:0");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // model invalid graph input
    {
        hiai::proto::ModelDef modelDef;
        modelDef.add_graph()->add_output("invalidNodeName:0");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // graph invalid node input
    {
        hiai::proto::GraphDef graphDef;
        auto opDef = graphDef.add_op(); // node attr
        opDef->add_input("invalidNodeName:0");

        Buffer buffer(graphDef.ByteSizeLong());
        graphDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        auto graph = ge::ComputeGraph::Make("graph_name");

        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load(buffer.MutableData(), buffer.GetSize()));
    }
    // graph invalid node control input
    {
        hiai::proto::GraphDef graphDef;
        auto opDef = graphDef.add_op(); // node attr
        opDef->add_input("invalidNodeName:-1");

        Buffer buffer(graphDef.ByteSizeLong());
        graphDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        auto graph = ge::ComputeGraph::Make("graph_name");

        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load(buffer.MutableData(), buffer.GetSize()));
    }
    // graph invalid graph input
    {
        hiai::proto::GraphDef graphDef;
        graphDef.add_input("invalidNodeName:0");

        Buffer buffer(graphDef.ByteSizeLong());
        graphDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        auto graph = ge::ComputeGraph::Make("graph_name");

        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load(buffer.MutableData(), buffer.GetSize()));
    }
    // graph invalid graph output index
    {
        hiai::proto::ModelDef modelDef;
        auto testGraph = modelDef.add_graph();
        auto opDef = testGraph->add_op(); // node attr
        opDef->set_name("test");
        testGraph->add_output("test:E");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
    // graph invalid graph output
    {
        hiai::proto::GraphDef graphDef;
        graphDef.add_output("invalidNodeName:0");

        Buffer buffer(graphDef.ByteSizeLong());
        graphDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        auto graph = ge::ComputeGraph::Make("graph_name");

        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load(buffer.MutableData(), buffer.GetSize()));
    }
    // model invalid node input anchor
    {
        hiai::proto::ModelDef modelDef;
        auto graphDef = modelDef.add_graph();
        auto nodeDef1 = graphDef->add_op(); // node attr
        nodeDef1->set_name("node1");

        auto nodeDef2 = graphDef->add_op(); // node attr
        nodeDef2->add_input("node1:0");

        Buffer buffer(modelDef.ByteSizeLong());
        modelDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));

        Model model;
        model.Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(model.IsValid());
    }
}

TEST(UTEST_ge_model_unserialize, test_invalid_CodeBuffer)
{
    {
        char buffer[100] = "sdfasf";
        auto graph = ge::ComputeGraph::Make("graph_name");
        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load((uint8_t*)buffer, 100));
    }
    {
        char buffer[100] = "sdfasf";
        Model model;
        model.Load((uint8_t*)buffer, 100);
        EXPECT_FALSE(model.IsValid());
    }
    {
        char buffer[100] = "sdfasf";
        auto opDesc = std::make_shared<OpDesc>();
        EXPECT_TRUE(opDesc != nullptr);
        EXPECT_TRUE(opDesc->Load((uint8_t*)buffer, 100));
    }
    {
        auto graph = ge::ComputeGraph::Make("graph_name");
        EXPECT_FALSE(graph->ROLE(GraphSerializer).Load((uint8_t*) nullptr, 100));
    }
    {
        Model model;
        model.Load((uint8_t*) nullptr, 100);
        EXPECT_FALSE(model.IsValid());
    }
}

TEST(UTEST_ge_model_unserialize, InputDescCompleted)
{
    hiai::proto::OpDef opDef;
    auto temp = opDef.add_input_desc();
    temp->set_layout("NCHW");
    auto temp1 = opDef.add_input_desc();
    temp1->set_layout("NCHW");
    opDef.add_input("op0:0");
    opDef.add_input("op1:0");
    opDef.add_input("");
    // no need complete
    {
        opDef.set_has_out_attr(true);
        Buffer buffer(opDef.ByteSizeLong());
        opDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        auto opDesc = std::make_shared<OpDesc>();
        EXPECT_TRUE(opDesc != nullptr);
        opDesc->Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_TRUE(opDesc->GetInputsDescSize() == 2);
    }

    // no need complete
    {
        auto temp2 = opDef.add_input_desc();
        temp2->set_layout("NCHW");
        opDef.set_has_out_attr(true);
        Buffer buffer(opDef.ByteSizeLong());
        opDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        auto opDesc = std::make_shared<OpDesc>();
        EXPECT_TRUE(opDesc != nullptr);
        opDesc->Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_TRUE(opDesc->GetInputsDescSize() == 3);
    }

    // need complete
    {
        opDef.set_has_out_attr(true);
        opDef.set_type("LSTM");
        Buffer buffer(opDef.ByteSizeLong());
        opDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        auto opDesc = std::make_shared<OpDesc>();
        opDesc->Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_TRUE(opDesc->GetInputsDescSize() == 3);
    }
}

TEST(UTEST_ge_model_unserialize, InputsetlyoutIsVaild)
{
        hiai::proto::OpDef opDef;
        auto temp = opDef.add_input_desc();
        temp->set_layout("NCHW");
        auto temp1 = opDef.add_input_desc();
        temp1->set_layout("NCHW");
        auto temp2 = opDef.add_input_desc();
        temp2->set_layout("NCHW");
        opDef.add_input("op0:0");
        opDef.add_input("op1:0");
        opDef.add_input("");
        opDef.add_input("");
        opDef.set_has_out_attr(true);
        opDef.set_type("LSTM");
        Buffer buffer(opDef.ByteSizeLong());
        opDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        auto opDesc = std::make_shared<OpDesc>();
        opDesc->Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(opDesc->GetInputsDescSize() == 3);
}

TEST(UTEST_ge_model_unserialize, InputDescIsVaild)
{
    {
        hiai::proto::OpDef opDef;
        opDef.add_input_desc();
        opDef.add_input_desc();
        opDef.add_input("op0:0");
        opDef.add_input("op1:0");
        opDef.add_input("");
        opDef.add_input("");
        opDef.set_has_out_attr(true);
        opDef.set_type("LSTM");
        Buffer buffer(opDef.ByteSizeLong());
        opDef.SerializeToArray(buffer.MutableData(), static_cast<int>(buffer.GetSize()));
        auto opDesc = std::make_shared<OpDesc>();
        EXPECT_TRUE(opDesc != nullptr);
        opDesc->Load(buffer.MutableData(), buffer.GetSize());
        EXPECT_FALSE(opDesc->GetInputsDescSize() == 3);
    }

}
TEST(UTEST_ge_model_unserialize, ModelSaveFail)
{
    Model model("model_name", "custom version3.0");
    auto computeGraph = ge::ComputeGraph::Make("graph_name");
    // input
    auto inputOp = std::make_shared<OpDesc>("test", "ConstOp");
    inputOp->AddOutputDesc(TensorDesc(Shape({44, 1, 493, 824}), FORMAT_NCHW, DT_INT32));
    auto input = CreateNode(inputOp, *computeGraph);
    std::vector<float> data(44 * 493 * 824* 50, 0);
    TensorDesc tensorDesc(Shape({44, 50, 493, 824}), FORMAT_NCHW, DT_INT32);
    TensorPtr tensorConvFilter = make_shared<Tensor>(tensorDesc, (uint8_t*)data.data(), 893710400);
    std::vector<float> data1(44 * 493 * 824* 50, 0);
    TensorDesc tensorDesc1(Shape({44, 50, 493, 824}), FORMAT_NCHW, DT_INT32);
    TensorPtr tensorConvFilter1 = make_shared<Tensor>(tensorDesc1, (uint8_t*)data1.data(), 893710400);
    std::vector<float> data2(44 * 493 * 824* 50, 0);
    TensorDesc tensorDesc2(Shape({44, 50, 493, 824}), FORMAT_NCHW, DT_INT32);
    TensorPtr tensorConvFilter2 = make_shared<Tensor>(tensorDesc1, (uint8_t*)data2.data(), 893710400);
    vector<TensorPtr> tensorVec = {tensorConvFilter, tensorConvFilter1, tensorConvFilter2};
    OpDescUtils::SetWeights(*input, tensorVec);
    Graph graph = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
    model.SetGraph(graph);
    ge::Buffer buffer;
    model.Save(buffer);
    ASSERT_GE(buffer.GetSize(), 0);
}