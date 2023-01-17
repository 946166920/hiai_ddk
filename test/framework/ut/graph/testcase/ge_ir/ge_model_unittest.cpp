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

#include "graph/attr_value.h"
#include "graph/buffer.h"

#define protected public
#define private public
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "graph/attributes_holder.h"
#include "graph/model.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/op/op_desc.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/core/node/node_spec.h"
#include "graph/op/all_ops.h"
#include "graph/operator.h"
#undef private
#undef protected
#include "graph/op/control_flow_defs.h"
#include "graph/op/internal_defs.h"
#include "operator_impl.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace ge;
using namespace testing;
using namespace std;
using testing::Test;
using testing::WithParamInterface;
using GraphBuilderFn = std::function<void(ge::Graph&)>;
namespace {
struct Attr {
    string name;
    AttrValue value;
};

struct SubDesc {
    string name;
    GraphBuilderFn v;
};

struct EdgeDesc {
    string srcOpName;
    string srcTensorName;
    string dstOpName;
    string dstTensorName;
};

struct OperatorDesc {
    string name;
    string type;
    vector<Attr> attrs;
    vector<string> inputs;
    vector<string> outputs;
    vector<SubDesc> subFuncs;
};

struct GraphTestPara {
    vector<struct OperatorDesc> opList;
    vector<struct EdgeDesc> edgeList;
    vector<string> inputs;
    vector<string> outputs;
};

struct ModelTestPara {
    string name;
    string version;
    GraphTestPara graph;
};

class Test_Model : public testing::Test, public WithParamInterface<ModelTestPara> {
public:
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

class RegisterOperator : public Operator {
public:
    RegisterOperator(const string& name, const string& type) : Operator(name, type, 3)
    {
    }
    void InputsRegister(const string& name)
    {
        InputRegister(name);
    }

    void OutputsRegister(const string& name)
    {
        OutputRegister(name);
    }
};

void GenerateThenGraph(Graph& graph)
{
    auto data = RegisterOperator("then_input", "Data");
    data.OutputsRegister("y");
    auto conv = RegisterOperator("conv", "Conv");
    conv.InputsRegister("x");
    conv.OutputsRegister("y");
    conv.SetInput("x", data, "y");
    std::vector<ge::Operator> inputs {data};
    std::vector<ge::Operator> outputs {data};
    graph.SetInputs(inputs).SetOutputs(outputs);
}

template <typename T>
bool Compare(const std::vector<T>& left, const std::vector<T>& right)
{
    if (left.size() != right.size()) {
        return false;
    }
    for (int i = 0; i < left.size(); i++) {
        if (!(left[i] == right[i])) {
            return false;
        }
    }
    return true;
}

bool CheckAttrValue(ge::AttrValue& value1, ge::AttrValue& value2)
{
    if (value1.GetValueType() != value2.GetValueType()) {
        return false;
    }

    switch (value1.GetValueType()) {
        case ge::AttrValue::ValueType::VT_STRING: {
            std::string s1 = value1.GetString();
            std::string s2 = value2.GetString();
            return s1 == s2;
        }
        case ge::AttrValue::ValueType::VT_FLOAT: {
            float f1 = value1.GetFloat();
            float f2 = value2.GetFloat();
            return f1 == f2;
        }
        case ge::AttrValue::ValueType::VT_BOOL: {
            bool b1 = value1.GetBool();
            bool b2 = value2.GetBool();
            return b1 == b2;
        }
        case ge::AttrValue::ValueType::VT_INT: {
            int64_t i1 = value1.GetInt();
            int64_t i2 = value2.GetInt();
            return i1 == i2;
        }
        case ge::AttrValue::ValueType::VT_TENSOR_DESC: {
            ge::TensorDesc td1 = value1.GetTensorDesc();
            ge::TensorDesc td2 = value2.GetTensorDesc();
            ;
            return (td1.GetDataType() == td2.GetDataType()) && (td1.GetFormat() == td2.GetFormat()) &&
                (td1.GetShape() == td2.GetShape());
        }
        case ge::AttrValue::ValueType::VT_TENSOR: {
            ge::TensorPtr t1 = value1.GetTensor();
            ge::TensorPtr t2 = value2.GetTensor();
            return t1->GetData().GetSize() == t2->GetData().GetSize();
        }
        case ge::AttrValue::ValueType::VT_GRAPH: {
            ge::ComputeGraphPtr g1 = value1.GetGraph();
            ge::ComputeGraphPtr g2 = value2.GetGraph();
            return g1->ROLE(GraphSpec).IsValid() == g2->ROLE(GraphSpec).IsValid();
        }
        case ge::AttrValue::ValueType::VT_BYTES: {
            ge::Buffer bt1 = value1.GetBuffer();
            ge::Buffer bt2 = value2.GetBuffer();
            return bt1.GetSize() == bt2.GetSize();
        }
        case ge::AttrValue::ValueType::VT_NAMED_ATTRS: {
            ge::AttrValue::NamedAttrs na1 = value1.GetNamedAttrs();
            ge::AttrValue::NamedAttrs na2 = value2.GetNamedAttrs();
            return na1.GetName() == na2.GetName();
        }
        case ge::AttrValue::ValueType::VT_LIST_STRING: {
            std::vector<std::string> ls1 = value1.GetStringList();
            std::vector<std::string> ls2 = value2.GetStringList();
            return Compare(ls1, ls2);
        }
        case ge::AttrValue::ValueType::VT_LIST_FLOAT: {
            std::vector<float> valuet1 = value1.GetFloatList();
            std::vector<float> valuet2 = value2.GetFloatList();
            return Compare(valuet1, valuet2);
        }
        case ge::AttrValue::ValueType::VT_LIST_BOOL: {
            std::vector<bool> valuet1 = value1.GetBoolList();
            std::vector<bool> valuet2 = value2.GetBoolList();
            return Compare(valuet1, valuet2);
        }
        case ge::AttrValue::ValueType::VT_LIST_INT: {
            std::vector<int64_t> valuet1 = value1.GetIntList();
            std::vector<int64_t> valuet2 = value2.GetIntList();
            return Compare(valuet1, valuet2);
        }
        case ge::AttrValue::ValueType::VT_LIST_TENSOR_DESC: {
            std::vector<ge::TensorDesc> valuet1 = value1.GetTensorDescList();
            std::vector<ge::TensorDesc> valuet2 = value2.GetTensorDescList();
            return valuet1.size() == valuet2.size();
        }
        case ge::AttrValue::ValueType::VT_LIST_TENSOR: {
            std::vector<ge::TensorPtr> valuet1 = value1.GetTensorList();
            std::vector<ge::TensorPtr> valuet2 = value2.GetTensorList();
            return valuet1.size() == valuet2.size();
        }
        case ge::AttrValue::ValueType::VT_NONE: {
            return true;
        }
        default:
            return false;
    }
    return false;
}

void buildGraph(const struct GraphTestPara& param, Graph& graph)
{
    auto operatorList = param.opList;
    if (operatorList.size() == 0) {
        return;
    }
    map<string, Operator> operatorMap;
    for (auto opDesc : operatorList) {
        RegisterOperator op(opDesc.name, opDesc.type);
        operatorMap[opDesc.name] = op;
        for (auto input : opDesc.inputs) {
            op.InputsRegister(input);
        }
        for (auto output : opDesc.outputs) {
            op.OutputsRegister(output);
        }
        for (auto attr : opDesc.attrs) {
            op.SetAttr(attr.name, std::move(attr.value));
        }

        if ((op.GetImpl()->GetOpDesc()->GetType() == hiai::op::If::TYPE) ||
            (op.GetImpl()->GetOpDesc()->GetType() == hiai::op::While::TYPE)) {
            for (auto func : opDesc.subFuncs) {
                op.SetGraphBuilder(func.name, func.v);
            }
        } else if (op.GetImpl()->GetOpDesc()->GetType() == hiai::op::Case::TYPE) {
            op.SubgraphCountRegister("branches", opDesc.subFuncs.size());
            for (auto i = 0; i < opDesc.subFuncs.size(); i++) {
                op.SetSubgraphBuilder(opDesc.subFuncs[i].name, i, opDesc.subFuncs[i].v);
            }
        }
    }
    vector<ge::Operator> inputs;
    vector<ge::Operator> outputs;

    for (auto input : param.inputs) {
        if (operatorMap.find(input) != operatorMap.end()) {
            inputs.push_back(operatorMap[input]);
        }
    }
    for (auto output : param.outputs) {
        if (operatorMap.find(output) != operatorMap.end()) {
            outputs.push_back(operatorMap[output]);
        }
    }
    for (auto inEdge : param.edgeList) {
        operatorMap[inEdge.dstOpName].SetInput(
            inEdge.dstTensorName, operatorMap[inEdge.srcOpName], inEdge.srcTensorName);
    }

    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);
}

TEST_P(Test_Model, Test_Serialize_UnSerialize)
{
    auto param = GetParam();
    string modelName = param.name;
    string modelVersion = param.version;

    Graph graph("graph_defalut");
    buildGraph(param.graph, graph);

    Model model(modelName, modelVersion);
    model.SetGraph(graph);

    if (param.graph.opList.empty()) {
        EXPECT_EQ(model.IsValid(), false);
        return;
    }

    // Model isValid
    EXPECT_EQ(model.IsValid(), true);

    // Test Serialize
    Buffer buffer;
    GraphErrCodeStatus serializeResult = model.Save(buffer);
    EXPECT_EQ(serializeResult, GRAPH_SUCCESS);

    // Test Unserialize model
    Model unserializeModel;
    GraphErrCodeStatus unSerializeResult =
        unserializeModel.Load(buffer.GetData(), static_cast<size_t>(buffer.GetSize()));
    EXPECT_EQ(serializeResult, GRAPH_SUCCESS);

    // Test Unserialize name
    EXPECT_EQ(unserializeModel.GetName(), modelName);

    // Test Unserialize graph
    ComputeGraphPtr unSerializeGraph = ge::GraphUtils::GetComputeGraph(unserializeModel.GetGraph());
    EXPECT_EQ(unSerializeGraph != nullptr, true);

    // Test Unserialize node && opdesc
    for (auto operatorDesc : param.graph.opList) {
        Node* node = unSerializeGraph->ROLE(GraphFinder).FindNode(operatorDesc.name);
        EXPECT_EQ(node != nullptr, true);
        for (auto attr : operatorDesc.attrs) {
            AttrValue getAttr;
            GraphErrCodeStatus getAttrRet = node->ROLE(NodeSpec).OpDesc().GetAttr(attr.name, getAttr);
            EXPECT_EQ(getAttrRet, GRAPH_SUCCESS);
            bool compare = CheckAttrValue(getAttr, attr.value);
            EXPECT_EQ(compare, true);
        }
        if (operatorDesc.subFuncs.size() > 0) {
            Node* subNode = unSerializeGraph->ROLE(GraphFinder).FindNode("then_input");
            EXPECT_EQ(node != nullptr, true);
        }
    }
}
/*
 * 测试用例名称:
 * Test_Model/Test_Model_Graph_Valid
 * 测试用例描述 : 有效模型序列化、反序列化（包含、不包含子图）
 * 预置条件 : 正常模型
 * 操作步骤: 1.构造Model
 *  2. 序列化
 *  3. 反序列化
 *  4. 结果比对
 * 预期结果 : 结果一致
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_Model_Graph_Valid, Test_Model,
    ::Values(ModelTestPara {.name = "model",
        .version = "deprecated",
        .graph = {{{"data", "Data", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::INT>(1))}}, {"x"}, {"y"},
                       {{"", nullptr}}},
                      {"const", "Const", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0))}}, {},
                          {"y"}, {{"", nullptr}}},
                      {"conv", "Conv", {{"key", AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))}}, {"x", "w"},
                          {"y"}, {{"", nullptr}}}},
            {{"data", "y", "conv", "x"}, {"const", "y", "conv", "w"}}, {"data"}, {"conv"}}}));

INSTANTIATE_TEST_CASE_P(Test_Model_SubGraph_Valid, Test_Model,
    ::Values(ModelTestPara {.name = "model",
        .version = "deprecated",
        .graph = {{{"data1", "Data", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::INT>(1))}}, {"x"}, {"y"},
                       {{"", nullptr}}},
                      {"data2", "Data", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0))}}, {"x"},
                          {"y"}, {{"", nullptr}}},
                      {"if", "If", {{"key", AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))}}, {"cond", "x"},
                          {"y"}, {{"then_branch", GenerateThenGraph}, {"else_branch", GenerateThenGraph}}}},
            {{"data1", "y", "if", "x"}, {"data2", "y", "if", "cond"}}, {"data1", "data2"}, {"if"}}}));

INSTANTIATE_TEST_CASE_P(Test_Model_While_SubGraph_Valid, Test_Model,
    ::Values(ModelTestPara {.name = "model",
        .version = "deprecated",
        .graph = {{{"data1", "Data", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::INT>(1))}}, {"x"}, {"y"},
                       {{"", nullptr}}},
                      {"data2", "Data", {{"key", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0))}}, {"x"},
                          {"y"}, {{"", nullptr}}},
                      {"while", "While", {{"key", AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))}}, {"x"},
                          {"y"}, {{"cond", GenerateThenGraph}, {"body", GenerateThenGraph}}}},
            {{"data1", "y", "while", "x"}, {"data2", "y", "while", "x"}}, {"data1", "data2"}, {"while"}}}));

INSTANTIATE_TEST_CASE_P(Test_Model_Case_SubGraph_Valid, Test_Model,
    ::Values(ModelTestPara { .name = "model",
        .version = "deprecated",
        .graph = {
            { { "data1", "Data", { { "key", AttrValue::CreateFrom(static_cast<AttrValue::INT>(1)) } }, { "x" }, { "y" },
                  { { "", nullptr } } },
                { "data2", "Data", { { "key", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0)) } }, { "x" },
                    { "y" }, { { "", nullptr } } },
                { "case", "Case", { },
                    { "branch_index", "dynamic_input_0" }, { "y" },
                    { { "branches", GenerateThenGraph }, { "branches", GenerateThenGraph } } } },
            { { "data1", "y", "case", "branch_index" }, { "data2", "y", "case", "dynamic_input_0" } },
            { "data1", "data2" }, { "case" } } }));
/*
 * 测试用例名称:
 * Test_Model/Test_Model_Graph_Valid
 * 测试用例描述 : 无效模型模型序列化、反序列化
 * 预置条件 : 无效模型
 * 操作步骤: 1.构造Model
 *  2. 序列化
 * 预期结果 : 序列化失败
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_Model_Graph_InValid, Test_Model,
    ::Values(ModelTestPara {.name = "model", .version = "deprecated", .graph = {}}));

}; // namespace