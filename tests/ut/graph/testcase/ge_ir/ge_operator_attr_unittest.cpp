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
#include "framework/graph/core/cgraph/graph_spec.h"
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

struct AttrDesc {
    std::string attrName;
    AttrValue::ValueType type;
    bool isRequired;
    ge::AttrValue defaultValue; // if is required attr, this value is no effect
};

struct OperatorSetAttrTestPara {
    struct AttrDesc attr;
    std::string name;
    ge::AttrValue value;
    ge::AttrValue expectValue;
};
ge::TensorPtr setTensorPtrVal = std::make_shared<Tensor>(TensorDesc(Shape({12, 32, 56, 56}), FORMAT_NCHW, DT_FLOAT));
ge::ComputeGraphPtr setGraphPtr = ge::ComputeGraph::Make("name");
ge::AttrValue graphAttr;
ge::AttrValue graphListAttr;
ge::AttrValue nameAttrs;

class Test_ge_operator_set_attr : public testing::Test, public WithParamInterface<OperatorSetAttrTestPara> {
public:
protected:
    void SetUp()
    {
        graphAttr.SetGraph(setGraphPtr);
    }

    void TearDown()
    {
    }
};
class TestOperator : public Operator {
public:
    TestOperator(const string& name, const string& type) : Operator(name, type, 3)
    {
    }
    void TestOptionalAttrRegister(const string& name, ge::AttrValue&& value)
    {
        OptionalAttrRegister(name, std::move(value));
    }
    void TestAttrRegister(const string& name, ge::AttrValue&& value)
    {
        AttrRegister(name, value.GetValueType());
    }
};

template <typename T> bool CompareValue(const std::vector<T>& left, const std::vector<T>& right)
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
bool CheckAttrValueEqual(ge::AttrValue& value1, ge::AttrValue& value2)
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
          return (td1.GetDataType() == td2.GetDataType()) &&
                 (td1.GetFormat() == td2.GetFormat()) &&
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
          return CompareValue(ls1, ls2);
        }
        case ge::AttrValue::ValueType::VT_LIST_FLOAT: {
          std::vector<float> valuet1 = value1.GetFloatList();
          std::vector<float> valuet2 = value2.GetFloatList();
          return CompareValue(valuet1, valuet2);
        }
        case ge::AttrValue::ValueType::VT_LIST_BOOL: {
          std::vector<bool> valuet1 = value1.GetBoolList();
          std::vector<bool> valuet2 = value2.GetBoolList();
          return CompareValue(valuet1, valuet2);
        }
        case ge::AttrValue::ValueType::VT_LIST_INT: {
          std::vector<int64_t> valuet1 = value1.GetIntList();
          std::vector<int64_t> valuet2 = value2.GetIntList();
          return CompareValue(valuet1, valuet2);
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
            std::cout << "default" << std::endl;
            return false;
    }
    return false;
}

TEST_P(Test_ge_operator_set_attr, Test_SetAttr)
{
    auto param = GetParam();
    TestOperator op("op_name", "op_type");
    if (param.attr.isRequired) {
        // 必选
        op.TestAttrRegister(param.attr.attrName, std::move(param.attr.defaultValue));

    } else {
        // 可选
        op.TestOptionalAttrRegister(param.attr.attrName, std::move(param.attr.defaultValue));
    }

    op.SetAttr(param.name, std::move(param.value));
    ge::AttrValue value;
    op.GetImpl()->GetOpDesc()->GetAttr(param.attr.attrName, value);
    EXPECT_TRUE(CheckAttrValueEqual(value, param.expectValue));
}

INSTANTIATE_TEST_CASE_P(Test_Operator_attr_name_misMatch, Test_ge_operator_set_attr,
    ::Values(
        // name为空 //
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "",
            .value = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
        // name不空但不匹配
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "xxxx",
            .value = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, true,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "xxxx",
            .value = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1})),
            .expectValue = AttrValue()}));

INSTANTIATE_TEST_CASE_P(Test_Operator_optinal_attr_misMatch_type, Test_ge_operator_set_attr,
    ::Values(
        // SET BOOL TO LIST_INT ATTR
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "pad",
            .value = AttrValue::CreateFrom(true),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
        // SET BOOL TO INT ATTR
        OperatorSetAttrTestPara{
            .attr = {"pad", AttrValue::ValueType::VT_INT, false, AttrValue::CreateFrom(static_cast<AttrValue::INT>(0))},
            .name = "pad",
            .value = AttrValue::CreateFrom(true),
            .expectValue = AttrValue::CreateFrom(static_cast<AttrValue::INT>(0))}));

INSTANTIATE_TEST_CASE_P(Test_Operator_required_attr_misMatch_type, Test_ge_operator_set_attr,
    ::Values(
        // SET LIST_FLOAT TO LIST_INT ATTR  FIXME:注册的AttrValue::ValueType
        // 当前没有使用到
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, true,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "pad",
            .value = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))},
        // SET BOOL TO LIST_INT ATTR
        OperatorSetAttrTestPara{.attr = {"pad", AttrValue::ValueType::VT_LIST_INT, true,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "pad",
            .value = AttrValue::CreateFrom(true),
            .expectValue = AttrValue::CreateFrom(true)},
        // SET BOOL TO INT ATTR
        OperatorSetAttrTestPara{
            .attr = {"pad", AttrValue::ValueType::VT_INT, true, AttrValue::CreateFrom(static_cast<AttrValue::INT>(0))},
            .name = "pad",
            .value = AttrValue::CreateFrom(true),
            .expectValue = AttrValue::CreateFrom(true)}));


static AttrValue::NamedAttrs CreateNamedAttrs(const string& name, std::map<string, AttrValue> map)
{
    AttrValue::NamedAttrs namedAttrs;
    namedAttrs.SetName(name);
    for (auto it : map) {
        namedAttrs.SetAttr(it.first, it.second);
    }
    return namedAttrs;
}

INSTANTIATE_TEST_CASE_P(Test_Operator_optinal_attr_Success, Test_ge_operator_set_attr,
    ::Values(
        // SET INT ATTR
        OperatorSetAttrTestPara{
            .attr = {"attrName", AttrValue::ValueType::VT_INT, false, AttrValue::CreateFrom(static_cast<AttrValue::INT>(0))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(static_cast<AttrValue::INT>(1)),
            .expectValue = AttrValue::CreateFrom(static_cast<AttrValue::INT>(1))},
        // SET FLOAT ATTR
        OperatorSetAttrTestPara{
            .attr = {"attrName", AttrValue::ValueType::VT_FLOAT, false, AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(0.0))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0)),
            .expectValue = AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(1.0))},
        // SET BOOL ATTR
        OperatorSetAttrTestPara{
            .attr = {"attrName", AttrValue::ValueType::VT_BOOL, false, AttrValue::CreateFrom(false)},
            .name = "attrName",
            .value = AttrValue::CreateFrom(true),
            .expectValue = AttrValue::CreateFrom(true)},
        // SET STR ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_STRING, false,
                                    AttrValue::CreateFrom(string("default"))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(string("new_value")),
            .expectValue = AttrValue::CreateFrom(string("new_value"))},
        // SET TENSORDESC ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_TENSOR_DESC, false,
                                    AttrValue::CreateFrom(TensorDesc())},
            .name = "attrName",
            .value = AttrValue::CreateFrom(
                TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT)),
            .expectValue = AttrValue::CreateFrom(
                TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT))},
        // SET TENSOR ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_TENSOR, false,
                                    AttrValue::CreateFrom(AttrValue::TENSOR(nullptr))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(setTensorPtrVal),
            .expectValue = AttrValue::CreateFrom(setTensorPtrVal)},
        // SET GRAPH ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_GRAPH, false,
                                    AttrValue()},
            .name = "attrName",
            .value = graphAttr,
            .expectValue = graphAttr},
        // SET BYTES ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_BYTES, false,
                                    AttrValue::CreateFrom(Buffer(100))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(Buffer(200)),
            .expectValue = AttrValue::CreateFrom(Buffer(200))},
        // SET NAMED_ATTRS ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_NAMED_ATTRS, false,
                                    AttrValue::CreateFrom(AttrValue::NamedAttrs())},
            .name = "attrName",
            .value = AttrValue::CreateFrom(CreateNamedAttrs("my_name",
                {{"int_val", AttrValue::CreateFrom(static_cast<int64_t>(123))}, {"str_val", AttrValue::CreateFrom(string("abc"))},
                    {"float_val", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(345.345))}})),
            .expectValue = AttrValue::CreateFrom(CreateNamedAttrs("my_name",
                {{"int_val", AttrValue::CreateFrom(static_cast<int64_t>(123))}, {"str_val", AttrValue::CreateFrom(string("abc"))},
                    {"float_val", AttrValue::CreateFrom(static_cast<AttrValue::FLOAT>(345.345))}}))}));

INSTANTIATE_TEST_CASE_P(Test_Operator_optinal_list_attr_Success, Test_ge_operator_set_attr,
    ::Values(
        // SET LIST_INT ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_INT, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1}))},
        // SET LIST_FLOAT ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_FLOAT, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_FLOAT({0.0, 0.0}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))},
        // SET LIST_BOOL ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_BOOL, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_BOOL({false}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_BOOL({true})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_BOOL({true}))},
        // SET LIST_STR ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_STRING, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_STR({"default"}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_STR({"newVaule"})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_STR({"newVaule"}))},
        // SET LIST_TENSORDESC ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_TENSOR_DESC, 
             false, AttrValue::CreateFrom(AttrValue::LIST_TENSOR_DESC({TensorDesc()}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_TENSOR_DESC({TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
              TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT)})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_TENSOR_DESC({TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
                    TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT)}))},
        // SET LIST_TENSOR ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_TENSOR, false,
                                    AttrValue::CreateFrom(AttrValue::LIST_TENSOR({nullptr}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_TENSOR({setTensorPtrVal})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_TENSOR({setTensorPtrVal}))},
        // SET LIST_GRAPH ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_GRAPH, false,
                                    AttrValue()},
            .name = "attrName",
            .value = graphListAttr,
            .expectValue = graphListAttr},
        // SET LIST_NAMED_ATTRS ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_NAMED_ATTRS, false,
                                    AttrValue()},
            .name = "attrName",
            .value = nameAttrs,
            .expectValue = nameAttrs}));

INSTANTIATE_TEST_CASE_P(Test_Operator_required_attr_Success, Test_ge_operator_set_attr,
    ::Values(
        // SET LIST_INT ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_INT, true,
                                    AttrValue::CreateFrom(AttrValue::LIST_INT({0, 0}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_INT({1, 1}))},
        // SET LIST_FLOAT ATTR
        OperatorSetAttrTestPara{.attr = {"attrName", AttrValue::ValueType::VT_LIST_FLOAT, true,
                                    AttrValue::CreateFrom(AttrValue::LIST_FLOAT({0.0, 0.0}))},
            .name = "attrName",
            .value = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0})),
            .expectValue = AttrValue::CreateFrom(AttrValue::LIST_FLOAT({1.0, 1.0}))}));
