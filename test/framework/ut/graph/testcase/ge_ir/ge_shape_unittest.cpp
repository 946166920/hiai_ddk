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
#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace ge;
using namespace std;
using testing::Test;
// using testing::WithParamInterface;

using namespace testing;

class Test_ge_shape : public testing::Test {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
/*
 * 测试用例名称   :
 * Test_ge_shape/test_construct_shape
 * 测试用例描述 : 构造shape类，测试构造函数
 * 预置条件 :  dims信息{}, {1, 1, 3, 4}
 * 操作步骤: 1. 通过{}构造shape1，校验shape构造结果
 *  2. 通过{}构造shape2，校验shape构造结果
 *  3. 通过{}构造shape3，校验shape构造结果
 *  4. 通过shape1构造shape4，校验shape构造结果
 *  5. 通过shape2构造shape5，校验shape构造结果
 *  6. 通过shape3构造shape6，校验shape构造结果
 * 预期结果 : 校验shape dim，ShapeSize
 * 修改历史 :
 */

TEST_F(Test_ge_shape, test_construct_and_get_dim_success)
{
    std::vector<int64_t> emptyDims = {};
    std::vector<int64_t> dims1 = {1, 1, 3, 4};
    // construct from vector {}
    ge::Shape shape1(emptyDims);
    EXPECT_TRUE(shape1.GetDims() == emptyDims);
    EXPECT_TRUE(shape1.GetDimNum() == 0);
    EXPECT_TRUE(shape1.GetTotalDimNum() == 0);

    ge::Shape shape2(dims1);
    EXPECT_TRUE(shape2.GetDims() == dims1);
    EXPECT_TRUE(shape2.GetDimNum() == dims1.size());
    EXPECT_TRUE(shape2.GetTotalDimNum() == 12);

    // construct from shape
    ge::Shape shape3(shape1);
    EXPECT_TRUE(shape3.GetDims() == shape1.GetDims());
    EXPECT_TRUE(shape3.GetDimNum() == shape1.GetDimNum());
    EXPECT_TRUE(shape3.GetTotalDimNum() == shape1.GetTotalDimNum());

    ge::Shape shape4(shape2);
    EXPECT_TRUE(shape4.GetDims() == shape2.GetDims());
    EXPECT_TRUE(shape4.GetDimNum() == shape2.GetDimNum());
    EXPECT_TRUE(shape4.GetTotalDimNum() == shape2.GetTotalDimNum());
}
/*
 * 测试用例名称   :
 * Test_ge_shape/test_construct_shape
 * 测试用例描述 : 构造shape类，测试构造函数
 * 预置条件 :  dims信息{0}, {-1, -1, -3, -4}
 * 操作步骤: 1. 通过{0}构造shape1，校验shape构造结果
 *  2. 通过{-1, -1, -3, -4}构造shape2，校验shape构造结果
 *  4. 通过shape1构造shape4，校验shape构造结果
 *  5. 通过shape2构造shape5，校验shape构造结果
 * 预期结果 : 校验shape 的dim个数，以及ShapeSize
 * 修改历史 :
 */

TEST_F(Test_ge_shape, test_construct_with_invalid_dims_and_get_dim)
{
    std::vector<int64_t> dims1 = {0};
    std::vector<int64_t> dims2 = {-1, -1, -3, -4};
    // construct from vector
    ge::Shape shape1(dims1);
    EXPECT_TRUE(shape1.GetDims() == dims1);
    EXPECT_TRUE(shape1.GetDimNum() == dims1.size());
    EXPECT_TRUE(shape1.GetTotalDimNum() == 0);
    ge::Shape shape2(dims2);
    EXPECT_TRUE(shape2.GetDims() == dims2);
    EXPECT_TRUE(shape2.GetDimNum() == dims2.size());
    EXPECT_TRUE(shape2.GetTotalDimNum() == 0);

    // construct from shape
    ge::Shape shape3(shape1);
    EXPECT_TRUE(shape3.GetDims() == shape1.GetDims());
    EXPECT_TRUE(shape3.GetDimNum() == shape1.GetDimNum());
    EXPECT_TRUE(shape3.GetTotalDimNum() == shape1.GetTotalDimNum());

    ge::Shape shape4(shape2);
    EXPECT_TRUE(shape4.GetDims() == shape2.GetDims());
    EXPECT_TRUE(shape4.GetDimNum() == shape2.GetDimNum());
    EXPECT_TRUE(shape4.GetTotalDimNum() == shape2.GetTotalDimNum());
}

/*
 * 测试用例名称   :
 * Test_ge_shape/test_set_dims_success
 * 测试用例描述 : 构造shape类，测试setDims函数
 * 预置条件 : 成功构造shape类 dims{1, 1, 3, 4};
 * 操作步骤: 1. 设置有效index(0)和dim值，返回值成功（GRAPH_SUCCESS）
 * 预期结果 : 返回GRAPH_SUCCESS，dim值设置正确
 * 修改历史 :
 */
TEST_F(Test_ge_shape, test_set_dims_success)
{
    std::vector<int64_t> dims = {1, 1, 3, 4};
    ge::Shape shape(dims);
    // vaild index
    ge::GraphErrCodeStatus ret = shape.SetDim(0, 5);
    EXPECT_EQ(ret, ge::GRAPH_SUCCESS);
    EXPECT_EQ(shape.GetDim(0), 5);
}

/*
 * 测试用例名称   :
 * Test_ge_shape/test_set_dims_invaild_index
 * 测试用例描述 : 构造shape类，测试setDims函数
 * 预置条件 : 成功构造shape类 dims{1, 1, 3, 4};
 * 操作步骤: 1. 设置无效index(-1)和dim值，返回值(GRAPH_FAILED)
 *  2. 设置无效index(超过dims范围)和dim值，返回值(GRAPH_FAILED)
 * 预期结果 : GRAPH_FAILED
 * 修改历史 :
 */
TEST_F(Test_ge_shape, test_set_dims_invaild_index)
{
    std::vector<int64_t> dims = {1, 1, 3, 4};
    ge::Shape shape(dims);
    // invaild index
    ge::GraphErrCodeStatus ret = shape.SetDim(6, 5);
    EXPECT_EQ(ret, ge::GRAPH_FAILED);
    // invaild index
    ret = shape.SetDim(-1, 5);
    EXPECT_EQ(ret, ge::GRAPH_FAILED);
}
