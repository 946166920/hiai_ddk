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
using namespace hiai;
using testing::Test;
using testing::WithParamInterface;

using namespace testing;

struct DyInputOutputDesc {
    std::string name;
    int num;
};

struct DstOpDesc {
    vector<std::string> inputs; // 固定输入
    vector<struct DyInputOutputDesc> dyInputs; // 动态输入
    vector<std::string> opInputs; // 可选输入
};

struct SrcOpDesc {
    vector<std::string> outputs; // 固定输出
    vector<struct DyInputOutputDesc> dyOutputs; // 动态输出
    vector<std::string> opOutputs; // 可选输出
};

struct OperatorSetInputsTestPara {
    struct SrcOpDesc srcOpDesc;
    struct DstOpDesc dstOpDesc;
    std::string srcName;
    std::string dstName;
    bool expectSuccess;
};

struct OperatorSetInputsNoSrcNameTestPara {
    struct SrcOpDesc srcOpDesc;
    struct DstOpDesc dstOpDesc;
    std::string dstName;
    bool expectSuccess;
};

struct OperatorSetInputsByIndexTestPara {
    struct SrcOpDesc srcOpDesc;
    struct DstOpDesc dstOpDesc;
    int srcIndex;
    int dstIndex;
    bool expectSuccess;
};
struct OperatorSetDyInputsByIndexTestPara {
    struct SrcOpDesc srcOpDesc;
    struct DstOpDesc dstOpDesc;
    std::string dyInputName;
    int dyInputIndex;
    int srcIndex;
    bool expectSuccess;
};
vector<std::string> outputs; // 固定输出

class Test_ge_operator_setinputs : public testing::Test, public WithParamInterface<OperatorSetInputsTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_operator_setinputs_no_srcName :
    public testing::Test,
    public WithParamInterface<OperatorSetInputsNoSrcNameTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_operator_setinputs_by_index :
    public testing::Test,
    public WithParamInterface<OperatorSetInputsByIndexTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class Test_ge_operator_setinputs_control :
    public testing::Test,
    public WithParamInterface<OperatorSetInputsByIndexTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_operator_repeat_setinputs : public testing::Test {
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class Test_ge_operator_set_dyinputs :
    public testing::Test,
    public WithParamInterface<OperatorSetDyInputsByIndexTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class TestSetinputOperator : public Operator {
public:
    TestSetinputOperator(const string& name, const string& type) : Operator(name, type, 3)
    {
    }
    void TestInputRegister(const string& name)
    {
        InputRegister(name);
    }
    void TestDynamicInputRegister(const string& name, const unsigned int num)
    {
        DynamicInputRegister(name, num);
    }
    void TestOptionalInputRegister(const string& name)
    {
        OptionalInputRegister(name);
    }
    void TestOutputRegister(const string& name)
    {
        OutputRegister(name);
    }
    void TestDynamicOutputRegister(const string& name, const unsigned int num)
    {
        DynamicOutputRegister(name, num);
    }
};

void RegisterSrcOpOutput(TestSetinputOperator& op, struct SrcOpDesc& opDesc)
{
    TensorDesc tensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT);
    // 固定输出
    for (auto output : opDesc.outputs) {
        op.TestOutputRegister(output);
    }
    // 动态输出
    for (auto output : opDesc.dyOutputs) {
        op.TestDynamicOutputRegister(output.name, output.num);
        std::cout<<"output.name ="<<output.name<<std::endl;
        std::cout<<"output.num ="<<output.num<<std::endl;
    }
    // 可选输出
    for (auto output : opDesc.opOutputs) {
        // 代码是空实现 无注册;
    }
}

void RegisterDstOpInput(TestSetinputOperator& op, struct DstOpDesc& opDesc)
{
    for (auto input : opDesc.inputs) {
        op.TestInputRegister(input);
    }
    // 动态输入
    for (auto input : opDesc.dyInputs) {
        op.TestDynamicInputRegister(input.name, input.num);
        std::cout<<"input.name ="<<input.name<<std::endl;
        std::cout<<"input.num ="<<input.num<<std::endl;
    }
    // 可选输入
    for (auto input : opDesc.opInputs) {
        op.TestOptionalInputRegister(input);
    }
}
bool CheckTensorDescEqual(const TensorDesc& left, const TensorDesc& right)
{
    return ((left.GetDataType() == right.GetDataType()) && (left.GetFormat() == right.GetFormat()) &&
        (left.GetShape() == right.GetShape()));
}

TEST_P(Test_ge_operator_setinputs, Test_Setintput)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);
    dstOp.SetInput(param.dstName, srcOp, param.srcName);

    if (param.expectSuccess) {
        // std::map<std::string, std::vector<OpAnchor>> outputLinks_{};
        // std::map<std::string, OpAnchor> inputLink_{};
        // using OpAnchor = std::pair<std::weak_ptr<OperatorImpl>, int32_t>;
        auto it = dstOp.GetImpl()->GetAllInputLinks().find(param.dstName);
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc() == srcOp.GetImpl()->GetOpDesc());
        EXPECT_TRUE(inAnchor.second == srcOp.GetImpl()->GetOpDesc()->GetOutputIndexByName(param.srcName));
        auto outAnchorVector = srcOp.GetImpl()->GetAllOutputLinks().find(param.srcName);
        EXPECT_TRUE(outAnchorVector->second.size() != 0);
    } else {
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(param.dstName) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}
TEST_P(Test_ge_operator_setinputs_no_srcName, Test_Setintput_no_srcName)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);
    dstOp.SetInput(param.dstName, srcOp);
    if (param.expectSuccess) {
        auto it  = dstOp.GetImpl()->GetAllInputLinks().find(param.dstName);
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc() == srcOp.GetImpl()->GetOpDesc());
        EXPECT_TRUE(inAnchor.second == 0);
        auto outAnchorVector  = srcOp.GetImpl()->GetAllOutputLinks().find(srcOp.GetImpl()->GetOpDesc()->GetOutputNameByIndex(0));
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    } else {
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(param.dstName) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}
TEST_P(Test_ge_operator_setinputs_by_index, Test_Setintput_by_index)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);
    dstOp.SetInput(param.dstIndex, srcOp, param.srcIndex);
    if (param.expectSuccess) {
        auto it  = dstOp.GetImpl()->GetAllInputLinks().find(dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex));
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc()->GetName() == srcOp.GetImpl()->GetOpDesc()->GetName());
        EXPECT_TRUE(inAnchor.second == param.srcIndex);
        auto outAnchorVector = srcOp.GetImpl()->GetAllOutputLinks().find(srcOp.GetImpl()->GetOpDesc()->GetOutputNameByIndex(param.srcIndex));
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    } else {
        std::cout<<"dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)"<<dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)<<std::endl;
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}

TEST_P(Test_ge_operator_setinputs_by_index, Test_Setintput_by_OpAnchor)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);
    dstOp.SetInput(dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex), srcOp.GetOutput(param.srcIndex));
    if (param.expectSuccess) {
        auto it  = dstOp.GetImpl()->GetAllInputLinks().find(dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex));
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc()->GetName() == srcOp.GetImpl()->GetOpDesc()->GetName());
        EXPECT_TRUE(inAnchor.second == param.srcIndex);
        auto outAnchorVector = srcOp.GetImpl()->GetAllOutputLinks().find(srcOp.GetImpl()->GetOpDesc()->GetOutputNameByIndex(param.srcIndex));
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    } else {
        std::cout<<"dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)"<<dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)<<std::endl;
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex)) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}

TEST_P(Test_ge_operator_set_dyinputs, Test_SetDyintput_by_OpAnchor)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);

    dstOp.SetDynamicInput(param.dyInputName, param.dyInputIndex, srcOp.GetOutput(param.srcIndex));
    if (param.expectSuccess) {
        auto it  = dstOp.GetImpl()->GetAllInputLinks().find(param.dyInputName + std::to_string(param.dyInputIndex));
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc()->GetName() == srcOp.GetImpl()->GetOpDesc()->GetName());
        EXPECT_TRUE(inAnchor.second == param.srcIndex);
        auto outAnchorVector = srcOp.GetImpl()->GetAllOutputLinks().find(srcOp.GetImpl()->GetOpDesc()->GetOutputNameByIndex(param.srcIndex));
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    } else {
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(param.dyInputName + std::to_string(param.dyInputIndex)) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}

TEST_P(Test_ge_operator_setinputs_control, Test_Setintput)
{
    auto param = GetParam();
    TestSetinputOperator srcOp("srcName", "srcType");
    RegisterSrcOpOutput(srcOp, param.srcOpDesc);
    TestSetinputOperator dstOp("dstName", "dstType");
    RegisterDstOpInput(dstOp, param.dstOpDesc);
    dstOp.SetInput(param.dstIndex, srcOp, param.srcIndex);
    string dstName = dstOp.GetImpl()->GetOpDesc()->GetInputNameByIndex(param.dstIndex);
    if (dstName.empty()) {
        dstName = std::to_string(param.dstIndex);
    }
    string srcName = srcOp.GetImpl()->GetOpDesc()->GetOutputNameByIndex(param.srcIndex);
    if (srcName.empty()) {
        srcName = std::to_string(param.dstIndex);
    }
    if (param.expectSuccess) {
        auto it  = dstOp.GetImpl()->GetAllInputLinks().find(dstName);
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc()->GetName() == srcOp.GetImpl()->GetOpDesc()->GetName());
        EXPECT_TRUE(inAnchor.second == param.srcIndex);
        auto outAnchorVector = srcOp.GetImpl()->GetAllOutputLinks().find(srcName);
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    } else {
        EXPECT_TRUE(dstOp.GetImpl()->GetAllInputLinks().find(dstName) == dstOp.GetImpl()->GetAllInputLinks().end());
    }
}
TEST_F(Test_ge_operator_repeat_setinputs, Test_repeat_Setintput)
{
    TestSetinputOperator firstSrcOp("firstSrcName", "srcType");
    firstSrcOp.TestOutputRegister("output1");
    TestSetinputOperator secondSrcOp("secondSrcName", "srcType");
    secondSrcOp.TestOutputRegister("output2");

    TestSetinputOperator dstOp("dstName", "dstType");
    dstOp.TestInputRegister("input");
    dstOp.SetInput(0, firstSrcOp, 0);
    {
        auto it = dstOp.GetImpl()->GetAllInputLinks().find("input");
        EXPECT_TRUE(it != dstOp.GetImpl()->GetAllInputLinks().end());
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc() == firstSrcOp.GetImpl()->GetOpDesc());
        EXPECT_TRUE(inAnchor.second == firstSrcOp.GetImpl()->GetOpDesc()->GetOutputIndexByName("output1"));
        auto outAnchorVector = firstSrcOp.GetImpl()->GetAllOutputLinks().find("output1");
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    }

    dstOp.SetInput(0, firstSrcOp, 0);
    {
        auto it = dstOp.GetImpl()->GetAllInputLinks().find("input");
        EXPECT_TRUE(it != dstOp.GetImpl()->GetAllInputLinks().end());
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc() == firstSrcOp.GetImpl()->GetOpDesc());
        EXPECT_TRUE(inAnchor.second == firstSrcOp.GetImpl()->GetOpDesc()->GetOutputIndexByName("output1"));
        auto outAnchorVector = firstSrcOp.GetImpl()->GetAllOutputLinks().find("output1");
        EXPECT_EQ(outAnchorVector->second.size(), 1);
    }
    // repeat set inputs by other src op and  index
    dstOp.SetInput(0, secondSrcOp, 0);
    {
        auto it = dstOp.GetImpl()->GetAllInputLinks().find("input");
        EXPECT_TRUE(it != dstOp.GetImpl()->GetAllInputLinks().end());
        auto inAnchor = it->second;
        EXPECT_TRUE(inAnchor.first.lock()->GetOpDesc() == firstSrcOp.GetImpl()->GetOpDesc());
        EXPECT_TRUE(inAnchor.second == firstSrcOp.GetImpl()->GetOpDesc()->GetOutputIndexByName("output1"));
        auto outAnchorVector = firstSrcOp.GetImpl()->GetAllOutputLinks().find("output1");
        EXPECT_EQ(outAnchorVector->second.size(), 1);
        EXPECT_TRUE(secondSrcOp.GetImpl()->GetAllOutputLinks().find("output2") == secondSrcOp.GetImpl()->GetAllOutputLinks().end());
    }
}
/*
 * 测试用例名称 : Test_ge_operator_setinputs
 * 测试用例描述 :
 *            设置算子输入(无效输入名字/index，有效输入名/index，无效输出名字/index，有效输出名/index，
 *             单输入，单输出，多输入，多输出，控制边)
 * 预置条件 : 构造Operator
 * 操作步骤 : 1. 构造算子节点
 *           2. 设置输入
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_Operator_invaild_dstname, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
                 .srcName = "datay",
                 .dstName = "x1",
                 .expectSuccess = false},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
            .srcName = "datay",
            .dstName = "",
            .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_invaild_srcName, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
                 .srcName = "out",
                 .dstName = "x",
                 .expectSuccess = false},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
            .srcName = "",
            .dstName = "x",
            .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_no_output, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .srcName = "out",
        .dstName = "x",
        .expectSuccess = false}));
INSTANTIATE_TEST_CASE_P(Test_Operator_no_inputs, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"out"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {}, .dyInputs = {}, .opInputs = {}},
        .srcName = "out",
        .dstName = "x",
        .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_single_input_output, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
                 .srcName = "datay",
                 .dstName = "x",
                 .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {}, .dyOutputs = {{"datay", 1}}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {}, .dyInputs = {{"x", 1}}, .opInputs = {}},
            .srcName = "datay1",
            .dstName = "x1",
            .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {}, .dyInputs = {}, .opInputs = {"x"}},
            .srcName = "datay",
            .dstName = "x",
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_muti_input, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x", "y"}, .dyInputs = {}, .opInputs = {}},
                 .srcName = "datay",
                 .dstName = "y",
                 .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {"y"}},
            .srcName = "datay",
            .dstName = "x",
            .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {{"y", 2}}, .opInputs = {"z"}},
            .srcName = "datay",
            .dstName = "z",
            .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {{"y", 2}}, .opInputs = {"z"}},
            .srcName = "datay",
            .dstName = "y1",
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_muti_output, Test_ge_operator_setinputs,
    ::Values(OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datax", "datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
                 .srcName = "datay",
                 .dstName = "x",
                 .expectSuccess = true},
        OperatorSetInputsTestPara{.srcOpDesc = {.outputs = {"datax"}, .dyOutputs = {{"datay", 2}}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
            .srcName = "datay1",
            .dstName = "x",
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_single_input_output, Test_ge_operator_setinputs_no_srcName,
    ::Values(OperatorSetInputsNoSrcNameTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .dstName = "x",
        .expectSuccess = true}));
INSTANTIATE_TEST_CASE_P(Test_Operator_muti_input, Test_ge_operator_setinputs_no_srcName,
    ::Values(OperatorSetInputsNoSrcNameTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x", "y"}, .dyInputs = {}, .opInputs = {}},
        .dstName = "y",
        .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_multi_output, Test_ge_operator_setinputs_no_srcName,
    ::Values(OperatorSetInputsNoSrcNameTestPara{
        .srcOpDesc = {.outputs = {"datax", "datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .dstName = "x",
        .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_single_input_output, Test_ge_operator_setinputs_by_index,
    ::Values(OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .srcIndex = 0,
        .dstIndex = 0,
        .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_invalid_srcindex, Test_ge_operator_setinputs_by_index,
    ::Values(OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .srcIndex = 2,
        .dstIndex = 0,
        .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_invalid_dstindex, Test_ge_operator_setinputs_by_index,
    ::Values(OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .srcIndex = 0,
        .dstIndex = 2,
        .expectSuccess = false}));

INSTANTIATE_TEST_CASE_P(Test_Operator_muti_input, Test_ge_operator_setinputs_by_index,
    ::Values(OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
                 .dstOpDesc = {.inputs = {"x", "y"}, .dyInputs = {}, .opInputs = {}},
                 .srcIndex = 0,
                 .dstIndex = 1,
                 .expectSuccess = true},
        OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {"y"}},
            .srcIndex = 0,
            .dstIndex = 1,
            .expectSuccess = true},
        OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {{"y", 2}}, .opInputs = {"z"}},
            .srcIndex = 0,
            .dstIndex = 1,
            .expectSuccess = true},
        OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {{"y", 2}}, .opInputs = {"z"}},
            .srcIndex = 0,
            .dstIndex = 2,
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_muti_output, Test_ge_operator_setinputs_by_index,
    ::Values(
        OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datax", "datay"}, .dyOutputs = {}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
            .srcIndex = 1,
            .dstIndex = 0,
            .expectSuccess = true},
        OperatorSetInputsByIndexTestPara{
            .srcOpDesc = {.outputs = {"datax"}, .dyOutputs = {{"datay", 2}}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
            .srcIndex = 2,
            .dstIndex = 0,
            .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_control_edge, Test_ge_operator_setinputs_control,
    ::Values(OperatorSetInputsByIndexTestPara{.srcOpDesc = {.outputs = {"datay"}, .dyOutputs = {}, .opOutputs = {}},
        .dstOpDesc = {.inputs = {"x"}, .dyInputs = {}, .opInputs = {}},
        .srcIndex = -1,
        .dstIndex = -1,
        .expectSuccess = true}));

INSTANTIATE_TEST_CASE_P(Test_Operator_set_dyinputs_by_opanchor, Test_ge_operator_set_dyinputs,
    ::Values(OperatorSetDyInputsByIndexTestPara{
            .srcOpDesc = {.outputs = {}, .dyOutputs = {{"datay", 2}}, .opOutputs = {}},
            .dstOpDesc = {.inputs = {}, .dyInputs = {{"x", 2}}, .opInputs = {}},
            .dyInputName = "x",
            .dyInputIndex = 1,
            .srcIndex = 0,
            .expectSuccess = true}));