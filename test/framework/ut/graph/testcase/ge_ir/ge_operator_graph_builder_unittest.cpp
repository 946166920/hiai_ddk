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
#include "graph/graph.h"
#include "graph/model.h"
#include "framework/graph/core/op/op_desc.h"
#include "graph/operator.h"
#include "framework/graph/utils/graph_utils.h"
#include "operator_impl.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace ge;
using namespace std;
using namespace hiai;
using testing::Test;
using testing::WithParamInterface;

using namespace testing;

struct OperatorSetGraphBuilderTestPara {
    vector<std::string> regGraphBranchName;
    std::string name;
    ge::GraphBuilderFn graphBuilderPtr;
    bool repeatSetGraphBuilder;
    ge::GraphBuilderFn repeatGraphBuilderPtr;
    bool expectSuccess;
};
class TestGraphBuilderOperator : public Operator {
public:
    TestGraphBuilderOperator(const string& name, const string& type) : Operator(name, type, 3)
    {
    }
    void TestAttrRegister(const string& name)
    {
        static ge::AttrValue attr = AttrValue::CreateFrom(string("default"));
        AttrRegister(name, attr.GetValueType());
    }
    void TestSetGraphBuilder(const string& name, const GraphBuilderFn& v)
    {
        SetGraphBuilder(name, v);
    }
};

class Test_ge_operator_set_graph_builder :
    public testing::Test,
    public WithParamInterface<OperatorSetGraphBuilderTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
TEST_P(Test_ge_operator_set_graph_builder, Test_SetGraphBuilder)
{
    auto param = GetParam();
    TestGraphBuilderOperator op("op_name", "op_type");
    // 必选输入
    for (auto graphBanchName : param.regGraphBranchName) {
        op.TestAttrRegister(graphBanchName);
    }

    op.TestSetGraphBuilder(param.name, param.graphBuilderPtr);
    if (param.repeatSetGraphBuilder) {
        op.TestSetGraphBuilder(param.name, param.repeatGraphBuilderPtr);
    }
    ge::AttrValue value;
    op.GetImpl()->GetOpDesc()->GetAttr(param.name, value);
    if (param.expectSuccess) {
        EXPECT_TRUE(value.GetValueType() == ge::AttrValue::ValueType::VT_STRING);
        std::string s1 = value.GetString();
        std::string graphName = "op_name_" + param.name;
        std::replace(graphName.begin(), graphName.end(), '/', '_');
        EXPECT_TRUE(s1 == graphName);
    } else {
    }
}
void BuildSubGraph(ge::Graph& graph)
{
}
/*
 * 测试用例名称   :
 * Test_ge_operator_set_graph_builder
 * 测试用例描述 : 设置GraphBuilder函数
 * 预置条件 : 构造Operator
 * 操作步骤: 1.构造算子节点
 *  2. 设置GraphBuilder函数
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_set_nullptr, Test_ge_operator_set_graph_builder,
    ::Values(OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
                 .name = "then",
                 .graphBuilderPtr = nullptr,
                 .repeatSetGraphBuilder = false,
                 .repeatGraphBuilderPtr = nullptr,
                 .expectSuccess = true},
        OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
            .name = "then",
            .graphBuilderPtr = BuildSubGraph,
            .repeatSetGraphBuilder = false,
            .repeatGraphBuilderPtr = nullptr,
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_repeat_set_mismatch_name, Test_ge_operator_set_graph_builder,
    ::Values(OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
                 .name = "else",
                 .graphBuilderPtr = nullptr,
                 .repeatSetGraphBuilder = false,
                 .repeatGraphBuilderPtr = nullptr,
                 .expectSuccess = true},
        OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
            .name = "else",
            .graphBuilderPtr = BuildSubGraph,
            .repeatSetGraphBuilder = false,
            .repeatGraphBuilderPtr = BuildSubGraph,
            .expectSuccess = true},
        OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
            .name = "",
            .graphBuilderPtr = BuildSubGraph,
            .repeatSetGraphBuilder = false,
            .repeatGraphBuilderPtr = BuildSubGraph,
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_repeat_set, Test_ge_operator_set_graph_builder,
    ::Values(OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
                 .name = "then",
                 .graphBuilderPtr = nullptr,
                 .repeatSetGraphBuilder = true,
                 .repeatGraphBuilderPtr = nullptr,
                 .expectSuccess = true},
        OperatorSetGraphBuilderTestPara{.regGraphBranchName = {"then"},
            .name = "then",
            .graphBuilderPtr = BuildSubGraph,
            .repeatSetGraphBuilder = true,
            .repeatGraphBuilderPtr = BuildSubGraph,
            .expectSuccess = true}));
