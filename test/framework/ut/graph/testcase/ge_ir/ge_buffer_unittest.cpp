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
using testing::WithParamInterface;

using namespace testing;

struct BufferConstructPara {
    size_t size;
    uint8_t defaultValue;
};
struct BufferCopyFromTestPara {
    uint8_t* data;
    size_t size;
    bool expectSuccess;
};
class Test_ge_buffer_construct : public testing::Test, public WithParamInterface<BufferConstructPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class Test_ge_buffer_copy_from : public testing::Test, public WithParamInterface<BufferCopyFromTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
TEST_P(Test_ge_buffer_construct, Test_buffer_construct)
{
    auto param = GetParam();
    ge::Buffer buffer(param.size, param.defaultValue);
    EXPECT_TRUE(buffer.GetSize() == param.size);
    std::uint8_t* data = buffer.MutableData();
    for (int i = 0; i < buffer.GetSize(); i++) {
        EXPECT_TRUE(*(data + i) == param.defaultValue);
    }
}
TEST_P(Test_ge_buffer_construct, Test_buffer_construct_no_defalutvalue)
{
    auto param = GetParam();
    ge::Buffer buffer(param.size);
    EXPECT_TRUE(buffer.GetSize() == param.size);
    std::uint8_t* data = buffer.MutableData();
    for (int i = 0; i < buffer.GetSize(); i++) {
        EXPECT_TRUE(*(data + i) == 0);
    }
}

TEST_P(Test_ge_buffer_construct, Test_buffer_ClearBuffer)
{
    auto param = GetParam();
    ge::Buffer buffer(param.size, param.defaultValue);
    EXPECT_TRUE(buffer.GetSize() == param.size);
    buffer.Clear();
    EXPECT_TRUE(buffer.GetSize() == 0);
}
TEST_P(Test_ge_buffer_copy_from, Test_buffer_copy_from)
{
    auto param = GetParam();
    ge::Buffer buffer;
    ge::Buffer::CopyFrom(param.data, param.size, buffer);
    if (param.expectSuccess) {
        EXPECT_TRUE(buffer.GetSize() == param.size);
        if (param.size != 0) {
            EXPECT_TRUE(0 == memcmp(buffer.MutableData(), param.data, buffer.GetSize()));
        }
    } else {
        EXPECT_TRUE(buffer.GetSize() == 0);
    }
}
uint8_t bufferData[2] = {1, 1};

/*
 * 测试用例名称   :
 * Test_ge_buffer_construct
 * 测试用例描述 : Buffer构造
 * 预置条件 : 输入Buffer size 默认value
 * 操作步骤: 1.Buffer构造
 *  2. 查询size和默认值
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_construct_buffer, Test_ge_buffer_construct,
    ::Values(
        BufferConstructPara{
            .size = 100,
            .defaultValue = 1,
        },
        BufferConstructPara{
            .size = 0,
            .defaultValue = 0,
        },
        BufferConstructPara{
            .size = 256,
            .defaultValue = 100,
        }));
/*
 * 测试用例名称   :
 * Test_ge_buffer_copy_from
 * 测试用例描述 : Buffer构造
 * 预置条件 : 输入Buffer data，size
 * 操作步骤: 1.设置属性
 *  2. 查询size和默认值
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_ge_buffer_copy_from_success, Test_ge_buffer_copy_from,
    ::Values(BufferCopyFromTestPara{
        .data = &bufferData[0],
        .size = 2,
        .expectSuccess = true,
    }));

INSTANTIATE_TEST_CASE_P(Test_ge_buffer_copy_from_0_size, Test_ge_buffer_copy_from,
    ::Values(BufferCopyFromTestPara{
        .data = &bufferData[0],
        .size = 0,
        .expectSuccess = false,
    }));
INSTANTIATE_TEST_CASE_P(Test_ge_buffer_copy_from_nullptr, Test_ge_buffer_copy_from,
    ::Values(BufferCopyFromTestPara{
        .data = nullptr,
        .size = 2,
        .expectSuccess = false,
    }));