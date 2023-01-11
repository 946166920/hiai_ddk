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
#include "graph/compatible/all_ops.h"
#include "graph/compatible/operator_reg.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_sorter.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node_spec.h"
#include "graph/graph.h"
#include "graph/model.h"
#include "framework/graph/core/op/op_desc.h"
#include "graph/operator.h"
#include "framework/graph/utils/graph_utils.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace ge;
using namespace std;
using testing::Test;
using testing::WithParamInterface;
using GraphPtr = std::shared_ptr<ge::Graph>;
using GraphOperatorMap = std::map<std::string, ge::Operator>;
using namespace testing;

struct GraphTestParam {
    GraphPtr graph;
    bool isGraphVaildExpect;
};

struct EdgeDesc {
    std::string srcOpName;
    std::string srcTensorName;
    std::string dstOpName;
    std::string dstTensorName;
};

struct OperatorDesc {
    std::string name;
    std::string type;
    vector<std::string> inputs; // 固定输入
    vector<std::string> outputs; // 固定输出
};
struct GraphSetInputsTestPara {
    vector<struct OperatorDesc> opList;
    vector<struct EdgeDesc> edgeList;
    vector<std::string> inputs;
    bool expectValidValue;
};

struct GraphSetOutputsTestPara {
    vector<struct OperatorDesc> opList;
    vector<struct EdgeDesc> edgeList;
    vector<std::string> inputs;
    vector<std::string> outputs;
    bool expectSetSuccess;
};

class Test_ge_graph_setinputs : public testing::Test, public WithParamInterface<GraphSetInputsTestPara> {
public:
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

class Test_ge_graph_setoutputs : public testing::Test, public WithParamInterface<GraphSetOutputsTestPara> {
public:
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};
class TestGraphSetInputOperator : public Operator {
public:
    TestGraphSetInputOperator(const string& name, const string& type) : Operator(name, type, 3)
    {
    }
    void TestInputRegister(const string& name)
    {
        InputRegister(name);
    }

    void TestOutputRegister(const string& name)
    {
        OutputRegister(name);
    }
};
/*
 * 测试用例名称   :
 * UTEST_Graph/add_op_and_check_success
 * 测试用例描述 : 构造graph,添加op 检查op是否添加成功
 * 预置条件 : graph, op
 * 操作步骤: 1.构造算子节点
 *  2. 构造graph
 *  3. 添加op 到graph图中
 *  4. 检查op是否添加成功
 * 预期结果 : success
 * 修改历史 :
 */
TEST(UTEST_Graph, add_op_and_check_success)
{
   ge::Operator conv1("conv1", "Convolution");
   ge::Operator conv2("conv2", "Convolution");
   ge::Graph graph("default_graph");
   graph.AddOp(conv1);
   graph.AddOp(conv2);

   std::vector<string> opName;
   EXPECT_EQ(graph.GetAllOpName(opName), GRAPH_SUCCESS);
   EXPECT_EQ(opName.size(), 2);
   EXPECT_EQ(graph.CheckOpByName("conv1"), GRAPH_SUCCESS);
   EXPECT_EQ(graph.CheckOpByName("conv2"), GRAPH_SUCCESS);
}

TEST_P(Test_ge_graph_setinputs, Test_NormalGraph)
{
    auto param = GetParam();
    auto& operatorList = param.opList;
    std::map<std::string, ge::Operator> operatorMap;
    for (auto opDesc : operatorList) {
        TestGraphSetInputOperator op(opDesc.name, opDesc.type);
        operatorMap[opDesc.name] = op;
        // 固定输入
        for (auto input : opDesc.inputs) {
            op.TestInputRegister(input);
        }

        // 固定输出
        for (auto output : opDesc.outputs) {
            op.TestOutputRegister(output);
        }
    }
    vector<ge::Operator> inputs;

    for (auto input : param.inputs) {
        if (operatorMap.find(input) != operatorMap.end()) {
            inputs.push_back(operatorMap[input]);
        } else {
            Operator op(input, "None");
            inputs.push_back(op);
        }
    }

    for (auto& inEdge : param.edgeList) {
        operatorMap[inEdge.dstOpName].SetInput(
            inEdge.dstTensorName, operatorMap[inEdge.srcOpName], inEdge.srcTensorName);
    }
    ge::Graph graph("graph_defalut");

    graph.SetInputs(inputs);
    ComputeGraphPtr computerGraph = ge::GraphUtils::GetComputeGraph(graph);
    GraphErrCodeStatus topResult = GRAPH_FAILED;
    if (computerGraph != nullptr) {
        topResult = computerGraph->ROLE(GraphSorter).SortNodesDFS();
    }
    if (param.expectValidValue) {
        EXPECT_EQ(topResult, GRAPH_SUCCESS);
    } else {
        EXPECT_EQ(topResult, GRAPH_FAILED);
    }
}

TEST_P(Test_ge_graph_setoutputs, Test_NormalGraph)
{
    auto param = GetParam();
    auto& operatorList = param.opList;
    std::map<std::string, ge::Operator> operatorMap;
    for (auto opDesc : operatorList) {
        TestGraphSetInputOperator op(opDesc.name, opDesc.type);
        operatorMap[opDesc.name] = op;
        // 固定输入
        for (auto input : opDesc.inputs) {
            op.TestInputRegister(input);
        }

        // 固定输出
        for (auto output : opDesc.outputs) {
            op.TestOutputRegister(output);
        }
    }
    vector<ge::Operator> inputs;
    vector<ge::Operator> outputs;

    for (auto input : param.inputs) {
        if (operatorMap.find(input) != operatorMap.end()) {
            inputs.push_back(operatorMap[input]);
        } else {
            Operator op(input, "None");
            inputs.push_back(op);
        }
    }
    for (auto output : param.outputs) {
        if (operatorMap.find(output) != operatorMap.end()) {
            outputs.push_back(operatorMap[output]);
        } else {
            TestGraphSetInputOperator op(output, "None");
            outputs.push_back(op);
        }
    }

    for (auto& inEdge : param.edgeList) {
        operatorMap[inEdge.dstOpName].SetInput(
            inEdge.dstTensorName, operatorMap[inEdge.srcOpName], inEdge.srcTensorName);
    }
    ge::Graph graph("graph_defalut");
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);
    ComputeGraphPtr computeGraph = ge::GraphUtils::GetComputeGraph(graph);
    vector<Node*> outNodes;
    auto visitor1 = [&param, &outNodes](Node& node) {
        outNodes.push_back(&node);
        return hiai::SUCCESS;
    };
    computeGraph->ROLE(GraphListWalker).WalkOutNodes(std::move(visitor1));
    if (param.expectSetSuccess) {
        EXPECT_TRUE(computeGraph != nullptr);
        EXPECT_EQ(param.outputs.size(), outNodes.size());
        auto visitor = [&param](Node& node) {
            EXPECT_NE(find(param.outputs.begin(), param.outputs.end(), node.ROLE(NodeSpec).OpDesc().GetName()),
                param.outputs.end());
            return hiai::SUCCESS;
        };
        EXPECT_EQ(computeGraph->ROLE(GraphListWalker).WalkOutNodes(std::move(visitor)), hiai::SUCCESS);
    } else {
        if (param.inputs.size() == 0) {
            EXPECT_TRUE(computeGraph == nullptr);
        } else {
            EXPECT_TRUE(param.outputs.size() != outNodes.size());
        }
    }
}
/*
 * 测试用例名称   :
 * Test_construct_graph/Test_ge_graph_setinputs.Test_NormalGraph/0
 * 测试用例描述 : 构造graph,设置网络输入节点
 * 预置条件 : 算子描述列表(单输入，空输入，常量输入，多输入)，算子边关系列表（孤立节点，环状图，正常图）
 * 操作步骤: 1.构造算子节点
 *  2. 设置算子输入 输出
 *  3 设置边关系
 *  4. 设置graph图输入
 * 预期结果 : graph is invaild or vaild
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_construct_graph, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .expectValidValue = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_empty_inputs, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {""},
        .expectValidValue = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_const_inputs, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"const1"},
        .expectValidValue = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_multi_inputs, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"data2", "Data", {"x"}, {"y"}},
                                        {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"data2", "y", "conv1", "w"}},
        .inputs = {"data1", "data2"},
        .expectValidValue = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_multi_branch, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"data2", "Data", {"x"}, {"y"}},
                                        {"sin1", "Sin", {"x"}, {"y"}}, {"sin2", "Sin", {"x"}, {"y"}}},
        .edgeList = {{"data1", "y", "sin1", "x"}, {"data2", "y", "sin2", "x"}},
        .inputs = {"data1", "data2"},
        .expectValidValue = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_circle_ops, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"data2", "Data", {"x"}, {"y"}}},
        .edgeList = {{"data1", "y", "data2", "x"}, {"data2", "y", "data1", "x"}},
        .inputs = {"data1"},
        .expectValidValue = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_circle2_ops, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{
        .opList = {{"data1", "Data", {"x"}, {"y"}}, {"data2", "Data", {"x"}, {"y"}}, {"data3", "Data", {"x"}, {"y"}}},
        .edgeList = {{"data1", "y", "data2", "x"}, {"data2", "y", "data3", "x"}, {"data3", "y", "data1", "x"}},
        .inputs = {"data1"},
        .expectValidValue = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_isolated_op, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"conv1", "Conv", {"x", "w"}, {"y"}}, {"conv2", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .expectValidValue = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_isolated_const, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"const2", "Const", {}, {"y"}}, {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .expectValidValue = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_graph_switch, Test_ge_graph_setinputs,
    ::Values(GraphSetInputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                        {"const2", "Const", {}, {"y"}}, {"matmul", "Matmul", {"x1", "x2"}, {"y"}},
                                        {"switch", "Switch", {"data", "pred"}, {"outputFalse", "outputTrue"}}},
        .edgeList = {{"data1", "y", "matmul", "x1"}, {"const1", "y", "switch", "data"},
            {"const2", "y", "switch", "pred"}, {"switch", "outputFalse", "matmul", "x2"}},
        .inputs = {"data1"},
        .expectValidValue = true}));

/*
 * 测试用例名称   :
 * Test_construct_graph/Test_ge_graph_setoutputs
 * 测试用例描述 : 构造graph,设置网络输出节点
 * 预置条件 : 算子描述列表(单输出，空输出，常量输出，多输出)，算子边关系列表（孤立节点，环状图，正常图）
 * 操作步骤: 1.构造算子节点
 *  2. 设置算子输入 输出
 *  3. 设置边关系
 *  4. 设置图输入
 *  5. 设置图输出
 * 预期结果 : graph is invaild or vaild
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_construct_graph, Test_ge_graph_setoutputs,
    ::Values(GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                         {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .outputs = {"conv1"},
        .expectSetSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_empty_outputs, Test_ge_graph_setoutputs,
    ::Values(GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                         {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .outputs = {},
        .expectSetSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_not_exist_outputs, Test_ge_graph_setoutputs,
    ::Values(GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                         {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .outputs = {"conv2"},
        .expectSetSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_output_const, Test_ge_graph_setoutputs,
    ::Values(GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                         {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .outputs = {"const1"},
        .expectSetSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_output_middle, Test_ge_graph_setoutputs,
    ::Values(GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"const1", "Const", {}, {"y"}},
                                         {"sin", "Sin", {"x"}, {"y"}}, {"conv1", "Conv", {"x", "w"}, {"y"}}},
        .edgeList = {{"data1", "y", "sin", "x"}, {"sin", "y", "conv1", "x"}, {"const1", "y", "conv1", "w"}},
        .inputs = {"data1"},
        .outputs = {"sin"},
        .expectSetSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_construct_output_multi, Test_ge_graph_setoutputs,
    ::Values(
        GraphSetOutputsTestPara{.opList = {{"data1", "Data", {"x"}, {"y"}}, {"split1", "SplitD", {"x"}, {"y1", "y2"}},
                                    {"split2", "SplitD", {"x"}, {"y1", "y2"}}},
            .edgeList = {{"data1", "y", "split1", "x"}, {"data1", "y", "split2", "x"}},
            .inputs = {"data1"},
            .outputs = {"split1", "split2"},
            .expectSetSuccess = true}));