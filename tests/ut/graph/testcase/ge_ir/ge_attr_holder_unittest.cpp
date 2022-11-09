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
#include "graph/attributes_holder.h"
#include "framework/graph/core/op/op_desc.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace ge;
using testing::Test;
using testing::WithParamInterface;

using namespace testing;
using namespace std;

enum Operation {
    SetAttr,
    GetAttr,
    HasAttr,
    DeleteAttr,
};

struct AttrHolderTestPara {
    AttrValue setValue;
    AttrValue::ValueType setType;
    string setName;
    Operation operation;
};

struct AttrMapHolderTestPara {
    map<string, AttrValue> setMapValue;
    string setName;
    Operation operation;
};

class Test_AttrHolder : public testing::Test, public WithParamInterface<AttrHolderTestPara> {
public:
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

class Test_AttrMapHolder : public testing::Test, public WithParamInterface<AttrMapHolderTestPara> {
public:
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

/* test utility functions */
template <typename T> void CheckVectorEqual(vector<T> v1, vector<T> v2)
{
    EXPECT_EQ(v1.size(), v2.size());
    for (unsigned i = 0; i < v1.size(); i++) {
        EXPECT_EQ(v1[i], v2[i]);
    }
}

bool checkResult(AttrValue::ValueType Type, AttrValue setValue, AttrValue getValue)
{
    switch (Type) {
        case AttrValue::ValueType::VT_STRING: {
          string setstr = setValue.GetString();
          string getstr = getValue.GetString();
          EXPECT_EQ(setstr, getstr);
          // new api will call: EXPECT_EQ(setValue.GetString(),
          // getValue.GetString());
          break;
        }
        case AttrValue::ValueType::VT_FLOAT: {
          float setstr = setValue.GetFloat();
          float getstr = getValue.GetFloat();
          EXPECT_EQ(setstr, getstr);
          break;
        }
        case AttrValue::ValueType::VT_INT: {
          int64_t setstr = setValue.GetInt();
          int64_t getstr = getValue.GetInt();
          EXPECT_EQ(setstr, getstr);
          break;
        }
        case AttrValue::ValueType::VT_TENSOR: {
          bool setstr = setValue.GetBool();
          bool getstr = getValue.GetBool();
          EXPECT_EQ(setstr, getstr);
          break;
        }
        case AttrValue::ValueType::VT_LIST_FLOAT: {
          vector<float> setstr = setValue.GetFloatList();
          vector<float> getstr = getValue.GetFloatList();
          CheckVectorEqual(setstr, getstr);
          break;
        }
        case AttrValue::ValueType::VT_LIST_INT: {
          vector<int64_t> setstr = setValue.GetIntList();
          vector<int64_t> getstr = getValue.GetIntList();
          CheckVectorEqual(setstr, getstr);
          break;
        }
        default:
            std::cout << "setvalue type error" << std::endl;
            return false;
    }
    return true;
}

TEST_P(Test_AttrHolder, Test_Attr_Holder)
{
    // AttrHolder is abstract calss
    OpDesc holder;
    auto param = GetParam();
    switch (param.operation) {
        case SetAttr: {
            GraphErrCodeStatus setStatus = holder.SetAttr(param.setName, param.setValue);
            EXPECT_EQ(setStatus, GRAPH_SUCCESS);
            break;
        }
        case GetAttr: {
            GraphErrCodeStatus setStatus = holder.SetAttr(param.setName, param.setValue);
            EXPECT_EQ(setStatus, GRAPH_SUCCESS);
            AttrValue getValue;
            GraphErrCodeStatus getStatus = holder.GetAttr(param.setName, getValue);
            EXPECT_EQ(getStatus, GRAPH_SUCCESS);
            bool compareRet = checkResult(param.setType, param.setValue, getValue);
            EXPECT_EQ(compareRet, true);
            break;
        }
        case HasAttr: {
            GraphErrCodeStatus setStatus = holder.SetAttr(param.setName, param.setValue);
            EXPECT_EQ(setStatus, GRAPH_SUCCESS);
            bool getRet = holder.HasAttr(param.setName);
            EXPECT_EQ(getRet, true);
            break;
        }
        case DeleteAttr: {
            GraphErrCodeStatus setStatus = holder.SetAttr(param.setName, param.setValue);
            EXPECT_EQ(setStatus, GRAPH_SUCCESS);
            GraphErrCodeStatus delStatus = holder.DelAttr(param.setName);
            EXPECT_EQ(delStatus, GRAPH_SUCCESS);
            bool getRet = holder.HasAttr(param.setName);
            EXPECT_EQ(getRet, false);
            break;
        }
        default:
            return;
    }
}
/*
 * 测试用例名称   :
 * Test_AttrHolder.Set_Int/Get_Int/Has_Int/DelInt
 * 测试用例描述 : AttrHolder设置属性，查询属性，删除属性
 * 预置条件 : AttrHolder类构造成功
 * 操作步骤: 1.设置属性
 *  2. 查询属性
 *  3. 删除属性
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(
    Test_Set_Int, Test_AttrHolder,
    ::Values(AttrHolderTestPara{
        .setValue = AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(1)),
        .setType = AttrValue::ValueType::VT_INT,
        .setName = "INT",
        .operation = SetAttr}));

INSTANTIATE_TEST_CASE_P(
    Test_Get_Int, Test_AttrHolder,
    ::Values(AttrHolderTestPara{
        .setValue = AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(1)),
        .setType = AttrValue::ValueType::VT_INT,
        .setName = "INT",
        .operation = GetAttr}));

INSTANTIATE_TEST_CASE_P(
    Test_Has_Int, Test_AttrHolder,
    ::Values(AttrHolderTestPara{
        .setValue = AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(1)),
        .setType = AttrValue::ValueType::VT_INT,
        .setName = "INT",
        .operation = HasAttr}));

INSTANTIATE_TEST_CASE_P(
    Test_DelInt, Test_AttrHolder,
    ::Values(AttrHolderTestPara{
        .setValue = AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(1)),
        .setType = AttrValue::ValueType::VT_INT,
        .setName = "INT",
        .operation = DeleteAttr}));