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
#include "framework/graph/core/op/op_desc.h"
#include "graph/op/array_defs.h"
#include "graph/tensor.h"
#include "graph/attr_value.h"
#undef protected
#undef private
using namespace std;
using namespace ge;

class ge_test_opdesc : public testing::Test {
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F(ge_test_opdesc, ge_test_opdesc_common)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    EXPECT_TRUE(opDesc);
    EXPECT_EQ(name, opDesc->GetName());
    EXPECT_EQ(type, opDesc->GetType());
    name = name + "_modify";
    type = type + "_modify";
    opDesc->SetName(name);
    opDesc->SetType(type);
    EXPECT_EQ(name, opDesc->GetName());
    EXPECT_EQ(type, opDesc->GetType());
}
TEST_F(ge_test_opdesc, ge_test_opdesc)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    bool ret = opDesc->InputIsSet("index");
    EXPECT_EQ(ret, false);
    opDesc->SetStreamId(2);
    int64_t stream_id = opDesc->GetId();
    EXPECT_EQ(stream_id, 0);
    vector<string> inputName{"conv1", "conv2"};
    opDesc->SetInputName(inputName);
    auto inputname_tmp = opDesc->GetInputName();
    EXPECT_EQ(inputname_tmp.size(), 2);
    opDesc->SetSrcName(inputName);
    vector<string> src_name = opDesc->GetSrcName();
    EXPECT_EQ(src_name.size(), 2);
    vector<int64_t> srcindex{1,2};
    opDesc->SetSrcIndex(srcindex);
    auto srcindex_tmp= opDesc->GetSrcIndex();
    EXPECT_EQ(srcindex_tmp.size(), 2);
    vector<int64_t> input{2,3};
    opDesc->SetInputOffset(input);
    size_t t = opDesc->GetInputOffsetSize();
    EXPECT_EQ(t,2);
    opDesc->ClearInputOffset();
    opDesc->AddInputOffset(3);
    int64_t value = opDesc->GetInputOffset(0);
    EXPECT_EQ(value, 3);
    opDesc->SetInputOffset(1,5);
    opDesc->AddInputOffset(3);
    auto input_offeset = opDesc->GetInputOffset();
    EXPECT_EQ(input_offeset.size(), 2);

}
TEST_F(ge_test_opdesc, ge_test_opdesc_index)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    vector<int64_t> output{1,2};
    opDesc->SetOutputOffset(output);
    const size_t t = opDesc->GetOutputOffsetSize();
    EXPECT_EQ(t, 2);
    opDesc->ClearOutputOffset();
    opDesc->AddOutputOffset(4);
    int64_t value = opDesc->GetOutputOffset(0);
    EXPECT_EQ(value, 4);
    opDesc->SetOutputOffset(2,6);
    auto out_offset = opDesc->GetOutputOffset();
    EXPECT_EQ(out_offset[0], 4);
    vector<string> dstName{"out1","out2"};
    opDesc->SetDstName(dstName);
    auto dstname_tmp = opDesc->GetDstName();
    EXPECT_EQ(dstname_tmp[0], "out1");
    vector<int64_t> dstIndex{3,5};
    opDesc->SetDstIndex(dstIndex);
    auto dstIndex_tmp = opDesc->GetDstIndex();
    EXPECT_EQ(dstIndex_tmp.size(), 2);
    opDesc->SetWorkspace(dstIndex);
    const size_t t1 = opDesc->GetWorkspaceSize();
    EXPECT_EQ(t1, 2);
    opDesc->ClearWorkspace();
    opDesc->AddWorkspace(6);
    value = opDesc->GetWorkspace(0);
    EXPECT_EQ(value, 6);
    opDesc->SetWorkspace(1,3);
    auto work_space = opDesc->GetWorkspace();
    EXPECT_EQ(work_space[0], 6);
    vector<uint32_t> workspaceBytes{10,26};
    opDesc->SetWorkspaceBytes(workspaceBytes);
    auto worke_space_bytes = opDesc->GetWorkspaceBytes();
    EXPECT_EQ(worke_space_bytes.size(), 2);
    const size_t t2 = opDesc->GetWorkspaceBytesSize();
    EXPECT_EQ(t2, 2);
    opDesc->AddWorkspaceBytes(3);
    opDesc->SetWorkspaceBytes(2, 9);
    uint32_t value_tmp = opDesc->GetWorkspaceBytes(2);
    EXPECT_EQ(value_tmp, 9);
    opDesc->ClearWorkspaceBytes();
}
TEST_F(ge_test_opdesc, ge_test_opdesc_const)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    vector<bool> isInputConst{true, false};
    opDesc->SetIsInputConst(isInputConst);
    size_t t = opDesc->GetIsInputConstSize();
    EXPECT_EQ(t, 2);
    bool flag = opDesc->IsInputConst(0);
    EXPECT_EQ(flag, true);
    opDesc->AddIsInputConst(false);
    auto input_const = opDesc->GetIsInputConst();
    EXPECT_EQ(input_const.size(), 3);
    opDesc->SetRunCL("clname1");
    const std::string name_cl = opDesc->GetRunCL();
    EXPECT_EQ(name_cl, "clname1");
}
TEST_F(ge_test_opdesc, ge_test_opdesc_inputs)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    EXPECT_TRUE(opDesc);
    TensorDesc tedesc1(Shape({1, 2, 3, 4}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_SUCCESS, opDesc->AddInputDesc(tedesc1));
    TensorDesc tedesc2(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_SUCCESS, opDesc->AddInputDesc("w", tedesc2));
    TensorDesc tedesc3(Shape({8, 9, 10, 11}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_FAILED, opDesc->AddInputDesc("w", tedesc3));

    TensorDesc tedesc4(Shape({1, 1, 1, 1}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(opDesc->UpdateInputDesc(1, tedesc4), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->UpdateInputDesc(4, tedesc4), GRAPH_FAILED);
    EXPECT_EQ(opDesc->UpdateInputDesc("w", tedesc4), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->UpdateInputDesc("weight", tedesc4), GRAPH_FAILED);

    TensorDesc getTe1 = opDesc->GetInputDesc(1);
    TensorDesc getTe2 = opDesc->GetInputDesc(4);
    TensorDesc getTe4 = opDesc->GetInputDesc("w");
    TensorDesc getTe3 = opDesc->GetInputDesc("weight");
    auto getTe1_ptr = opDesc->GetInputDescPtr(1);

    EXPECT_EQ(opDesc->GetInputNameByIndex(1), "w");
    EXPECT_EQ(opDesc->GetInputNameByIndex(2), "");

    auto vistor_in = opDesc->GetAllInputsDesc();
    EXPECT_EQ(vistor_in.size(), 2);

    auto input_size = opDesc->GetInputsSize();
    EXPECT_EQ(input_size, 2);
    bool ret = opDesc->InputIsSet("w");
    EXPECT_EQ(ret, true);
    auto names = opDesc->GetAllInputNames();
}

TEST_F(ge_test_opdesc, ge_test_opdesc_outputs)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    EXPECT_TRUE(opDesc);
    TensorDesc tedesc1(Shape({1, 2, 3, 4}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_SUCCESS, opDesc->AddOutputDesc(tedesc1));
    TensorDesc tedesc2(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_SUCCESS, opDesc->AddOutputDesc("w", tedesc2));
    TensorDesc tedesc3(Shape({8, 9, 10, 11}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(GRAPH_FAILED, opDesc->AddOutputDesc("w", tedesc3));

    TensorDesc tedesc4(Shape({1, 1, 1, 1}), FORMAT_NCHW, DT_FLOAT);
    EXPECT_EQ(opDesc->UpdateOutputDesc(1, tedesc4), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->UpdateOutputDesc(4, tedesc4), GRAPH_FAILED);
    EXPECT_EQ(opDesc->UpdateOutputDesc("w", tedesc4), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->UpdateOutputDesc("weight", tedesc4), GRAPH_FAILED);

    TensorDesc getTe1 = opDesc->GetOutputDesc(1);
    TensorDesc getTe2 = opDesc->GetOutputDesc(4);
    TensorDesc getTe4 = opDesc->GetOutputDesc("w");
    TensorDesc getTe3 = opDesc->GetOutputDesc("weight");

    auto getTe1ptr = opDesc->GetOutputDescPtr(1);

    vector<TensorDescPtr> vector_out = opDesc->GetOutputsDesc();
    opDesc->AddOutputDesc(vector_out);
    auto vistor_in = opDesc->GetAllOutputsDesc();
    auto vistor_in_ptr = opDesc->GetAllOutputsDescPtr();
    EXPECT_EQ(vistor_in.size(), 2);
    EXPECT_EQ(vistor_in_ptr.size(), 2);
    EXPECT_EQ(vector_out.size(), 2);
}

TEST_F(ge_test_opdesc, ge_test_opdesc_attrs)
{
    string name = "Conv2d";
    string type = "Data";
    OpDescPtr opDesc = std::shared_ptr<OpDesc>(new (std::nothrow) OpDesc(name, type));
    EXPECT_TRUE(opDesc);
    auto defautlAttrSize = opDesc->GetAllAttrs().size();

    static const string Pad = "pad";
    static const string BIAS = "bias";

    opDesc->SetAttr(Pad, AttrValue::CreateFrom(static_cast<int64_t>(6)));
    opDesc->SetAttr(BIAS, AttrValue::CreateFrom(static_cast<int64_t>(6)));

    AttrValue at;
    EXPECT_EQ(opDesc->GetAttr(Pad, at), GRAPH_SUCCESS);
    int64_t getatt = at.GetInt();
    EXPECT_EQ(getatt, 6);
    EXPECT_EQ(opDesc->GetAttr("xxx", at), GRAPH_FAILED);
    EXPECT_EQ(opDesc->GetAttr(BIAS, at), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->GetAttr("bia", at), GRAPH_FAILED);
    EXPECT_TRUE(opDesc->HasAttr(BIAS));
    EXPECT_FALSE(opDesc->HasAttr("xxx"));

    EXPECT_EQ(2, opDesc->GetAllAttrs().size() - defautlAttrSize);
    EXPECT_EQ(opDesc->DelAttr("xxx"), GRAPH_SUCCESS);
    EXPECT_EQ(opDesc->DelAttr(Pad), GRAPH_SUCCESS);
    EXPECT_EQ(1, opDesc->GetAllAttrs().size() - defautlAttrSize);

    // get set cl name
    opDesc->SetRunCL("NPUCL");
    EXPECT_EQ("NPUCL", opDesc->GetRunCL());

    opDesc->ClearAllInputsDesc();
    opDesc->ClearAllOutputsDesc();
}