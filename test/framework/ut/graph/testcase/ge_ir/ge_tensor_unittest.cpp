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

struct TensorConstructByBufferTestPara {
    ge::TensorDesc tensorDesc;
    ge::Buffer buffer;
    bool expectNoneZeroBuffer;
};
struct TensorConstructByDataAndSizeTestPara {
    ge::TensorDesc tensorDesc;
    uint8_t* data;
    size_t size;
    bool expectNoneZeroBuffer;
};
struct TensorSetTensorDescTestPara {
    ge::TensorDesc tensorDesc;
};
struct TensorSetDataByBufferTestPara {
    ge::Buffer buffer;
    bool expectNoneZeroBuffer;
};
struct TensorSetDataByDataAndSizeTestPara {
    uint8_t* data;
    size_t size;
    bool expectNoneZeroBuffer;
};

ge::Buffer bufferCopy;

class Test_ge_tensor_construct_buffer :
    public testing::Test,
    public WithParamInterface<TensorConstructByBufferTestPara> {
public:
protected:
    void SetUp()
    {
        ge::Buffer::CopyFrom((std::uint8_t*)nullptr, (std::size_t)0, bufferCopy);
    }

    void TearDown()
    {
    }
};

class Test_ge_tensor_construct_data_size :
    public testing::Test,
    public WithParamInterface<TensorConstructByDataAndSizeTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class Test_ge_tensor_set_tensordesc : public testing::Test, public WithParamInterface<TensorSetTensorDescTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_tensor_mutable_tensordesc : public testing::Test, public WithParamInterface<TensorSetTensorDescTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
class Test_ge_tensor_set_data_buffer : public testing::Test, public WithParamInterface<TensorSetDataByBufferTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_tensor_set_data_data_size :
    public testing::Test,
    public WithParamInterface<TensorSetDataByDataAndSizeTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};

class Test_ge_tensor_mutable_data : public testing::Test, public WithParamInterface<TensorSetDataByBufferTestPara> {
public:
protected:
    void SetUp()
    {
    }
    void TearDown()
    {
    }
};
TEST_P(Test_ge_tensor_construct_buffer, Test_tensor_construct)
{
    auto param = GetParam();
    ge::Tensor tensor(param.tensorDesc, param.buffer);
    ge::TensorDesc tensordesc = tensor.GetTensorDesc();
    EXPECT_TRUE(tensordesc.GetShape().GetDims() == param.tensorDesc.GetShape().GetDims());
    EXPECT_TRUE(tensordesc.GetFormat() == param.tensorDesc.GetFormat());
    EXPECT_TRUE(tensordesc.GetDataType() == param.tensorDesc.GetDataType());
    if (param.expectNoneZeroBuffer) {
        EXPECT_TRUE(tensor.GetData().GetSize() == param.buffer.GetSize());
        if (param.buffer.GetSize() != 0) {
            EXPECT_TRUE(0 == memcmp(tensor.GetData().GetData(), param.buffer.GetData(), tensor.GetData().GetSize()));
        }
    } else {
        EXPECT_TRUE(tensor.GetData().GetSize() == param.buffer.GetSize());
    }
}

TEST_P(Test_ge_tensor_construct_data_size, Test_tensor_construct)
{
    auto param = GetParam();
    ge::Tensor tensor(param.tensorDesc, param.data, param.size);
    ge::TensorDesc tensordesc = tensor.GetTensorDesc();
    EXPECT_TRUE(tensordesc.GetShape().GetDims() == param.tensorDesc.GetShape().GetDims());
    EXPECT_TRUE(tensordesc.GetFormat() == param.tensorDesc.GetFormat());
    EXPECT_TRUE(tensordesc.GetDataType() == param.tensorDesc.GetDataType());
    if (param.expectNoneZeroBuffer) {
        EXPECT_TRUE(tensor.GetData().GetSize() == param.size);
        if (param.size != 0) {
            EXPECT_TRUE(0 == memcmp(tensor.GetData().GetData(), param.data, tensor.GetData().GetSize()));
        }
    } else {
        EXPECT_TRUE(tensor.GetData().GetSize() == 0);
    }
}

TEST_P(Test_ge_tensor_set_tensordesc, Test_tensor_set_tensordesc)
{
    auto param = GetParam();
    ge::Tensor tensor;
    tensor.SetTensorDesc(param.tensorDesc);
    auto tensordesc = tensor.GetTensorDesc();
    EXPECT_TRUE(tensordesc.GetShape().GetDims() == param.tensorDesc.GetShape().GetDims());
    EXPECT_TRUE(tensordesc.GetFormat() == param.tensorDesc.GetFormat());
    EXPECT_TRUE(tensordesc.GetDataType() == param.tensorDesc.GetDataType());
}
TEST_P(Test_ge_tensor_mutable_tensordesc, Test_tensor_mutable_tensordesc)
{
    auto param = GetParam();
    ge::Tensor tensor;
    ge::TensorDesc& tmp = tensor.MutableTensorDesc();
    tmp.SetShape(param.tensorDesc.GetShape());
    tmp.SetFormat(param.tensorDesc.GetFormat());
    tmp.SetDataType(param.tensorDesc.GetDataType());
    auto tensordesc = tensor.GetTensorDesc();
    EXPECT_TRUE(tensordesc.GetShape().GetDims() == param.tensorDesc.GetShape().GetDims());
    EXPECT_TRUE(tensordesc.GetFormat() == param.tensorDesc.GetFormat());
    EXPECT_TRUE(tensordesc.GetDataType() == param.tensorDesc.GetDataType());
}
TEST_P(Test_ge_tensor_set_data_buffer, Test_tensor_set_data_buffer)
{
    auto param = GetParam();
    ge::Tensor tensor;
    tensor.SetData(param.buffer);
    if (param.expectNoneZeroBuffer) {
        EXPECT_TRUE(tensor.GetData().GetSize() == param.buffer.GetSize());
        if (param.buffer.GetSize() != 0) {
            EXPECT_TRUE(0 == memcmp(tensor.GetData().GetData(), param.buffer.GetData(), tensor.GetData().GetSize()));
        }
    } else {
        EXPECT_TRUE(tensor.GetData().GetSize() == 0);
    }
}

TEST_P(Test_ge_tensor_set_data_data_size, Test_tensor_set_data)
{
    auto param = GetParam();
    ge::Tensor tensor;
    tensor.SetData(param.data, param.size);
    if (param.expectNoneZeroBuffer) {
        EXPECT_TRUE(tensor.GetData().GetSize() == param.size);
        if (param.size != 0) {
            EXPECT_TRUE(0 == memcmp(tensor.GetData().GetData(), param.data, tensor.GetData().GetSize()));
        }
    } else {
        EXPECT_TRUE(tensor.GetData().GetSize() == 0);
    }
}

TEST_P(Test_ge_tensor_mutable_data, Test_tensor_mutable_data)
{
    auto param = GetParam();
    ge::Tensor tensor;
    tensor.SetData(param.buffer);
    if (param.expectNoneZeroBuffer) {
        EXPECT_TRUE(tensor.MutableData().GetSize() == param.buffer.GetSize());
        if (param.buffer.GetSize() != 0) {
            EXPECT_TRUE(
                0 == memcmp(tensor.MutableData().GetData(), param.buffer.GetData(), tensor.MutableData().GetSize()));
        }
    } else {
        EXPECT_TRUE(tensor.GetData().GetSize() == 0);
    }
}

uint8_t data[2] = {1, 1};

/*
 * 测试用例名称 : Test_ge_tensor
 * 测试用例描述 : 构造Tensor ，设置TensorDesc, 设置data
 * 预置条件 :
 * 操作步骤 : 1. 构造Tensor
 *           2. 设置TensorDesc
 *           3. 查询TensorDesc
 *           4. 设置data
 *           5. 查询data size
 * 预期结果 : Success or Failed
 * 修改历史 :
 */
INSTANTIATE_TEST_CASE_P(Test_construct_tensor, Test_ge_tensor_construct_buffer,
    ::Values(TensorConstructByBufferTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
                 .buffer = Buffer(100, 1),
                 .expectNoneZeroBuffer = true},
        TensorConstructByBufferTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
            .buffer = Buffer(static_cast<size_t>(0), 1),
            .expectNoneZeroBuffer = false},
        TensorConstructByBufferTestPara{.tensorDesc = TensorDesc(Shape({1, 32}), FORMAT_NCHW, DT_FLOAT),
            .buffer = bufferCopy,
            .expectNoneZeroBuffer = false}));

INSTANTIATE_TEST_CASE_P(Test_construct_tensor, Test_ge_tensor_construct_data_size,
    ::Values(TensorConstructByDataAndSizeTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
                 .data = &data[0],
                 .size = 2,
                 .expectNoneZeroBuffer = true},
        TensorConstructByDataAndSizeTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
            .data = &data[0],
            .size = 0,
            .expectNoneZeroBuffer = false},
        TensorConstructByDataAndSizeTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT),
            .data = nullptr,
            .size = 0,
            .expectNoneZeroBuffer = false}));

INSTANTIATE_TEST_CASE_P(Test_tensor_set_tensordesc, Test_ge_tensor_set_tensordesc,
    ::Values(TensorSetTensorDescTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT)}));

INSTANTIATE_TEST_CASE_P(Test_tensor_mutable_tensordesc, Test_ge_tensor_mutable_tensordesc,
    ::Values(TensorSetTensorDescTestPara{.tensorDesc = TensorDesc(Shape({1, 32, 8, 8}), FORMAT_NCHW, DT_FLOAT)}));

INSTANTIATE_TEST_CASE_P(Test_tensor_set_data, Test_ge_tensor_set_data_buffer,
    ::Values(TensorSetDataByBufferTestPara{.buffer = Buffer(100, 1), .expectNoneZeroBuffer = true},
        TensorSetDataByBufferTestPara{.buffer = Buffer(static_cast<size_t>(0), 1), .expectNoneZeroBuffer = false},
        TensorSetDataByBufferTestPara{.buffer = bufferCopy,
            .expectNoneZeroBuffer = false}));

INSTANTIATE_TEST_CASE_P(Test_tensor_set_data, Test_ge_tensor_set_data_data_size,
    ::Values(TensorSetDataByDataAndSizeTestPara{.data = &data[0], .size = 2, .expectNoneZeroBuffer = true},
        TensorSetDataByDataAndSizeTestPara{.data = &data[0], .size = 0, .expectNoneZeroBuffer = false},
        TensorSetDataByDataAndSizeTestPara{.data = nullptr, .size = 0, .expectNoneZeroBuffer = false}));

INSTANTIATE_TEST_CASE_P(Test_tensor_mutable_data, Test_ge_tensor_mutable_data,
    ::Values(TensorSetDataByBufferTestPara{.buffer = Buffer(100, 1), .expectNoneZeroBuffer = true},
        TensorSetDataByBufferTestPara{.buffer = Buffer(static_cast<size_t>(0), 1), .expectNoneZeroBuffer = false},
        TensorSetDataByBufferTestPara{.buffer = bufferCopy,
            .expectNoneZeroBuffer = false}));