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

#define private public
#define protected public
#include "graph/attr_value.h"
#include "graph/tensor.h"
#include "framework/graph/utils/tensor_utils.h"
#undef private
#undef protected

#include <iostream>
using namespace std;
using namespace ge;


class ge_test_tensor : public testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};


TEST_F(ge_test_tensor, shape)
{
    Shape a;
    EXPECT_EQ(a.GetDim(0), 0);
    EXPECT_EQ(a.GetTotalDimNum(), 0);
    EXPECT_EQ(a.SetDim(0, 0), GRAPH_FAILED);

    vector<int64_t > vec({1,2,3,4});
    Shape b(vec);
    Shape c({1,2,3,4});
    EXPECT_EQ(c.GetDimNum(), 4);
    EXPECT_EQ(c.GetDim(2), 3);
    EXPECT_EQ(c.GetDim(5), 0);
    EXPECT_EQ(c.SetDim(10, 0), GRAPH_FAILED);

    EXPECT_EQ(c.SetDim(2, 2), GRAPH_SUCCESS);
    EXPECT_EQ(c.GetDim(2), 2);
    vector<int64_t > vec1 = c.GetDims();
    EXPECT_EQ(c.GetDim(0), vec1[0]);
    EXPECT_EQ(c.GetDim(1), vec1[1]);
    EXPECT_EQ(c.GetDim(2), vec1[2]);
    EXPECT_EQ(c.GetDim(3), vec1[3]);

    EXPECT_EQ(c.GetTotalDimNum(), 16);
}

TEST_F(ge_test_tensor, tensorDesc)
{
    TensorDesc a;
    Shape s({1,2,3,4});
    TensorDesc b(s);
    Shape s1 = b.GetShape();
    EXPECT_EQ(s1.GetDim(0), s.GetDim(0));
    b.MutableShape().SetDim(0, 2);
    EXPECT_EQ(b.GetShape().GetDim(0), 2);
    Shape s2({3, 2, 3, 4});
    b.SetShape(s2);
    EXPECT_EQ(b.GetShape().GetDim(0), 3);

    EXPECT_EQ(b.GetFormat(), FORMAT_NCHW);
    b.SetFormat(FORMAT_RESERVED);
    EXPECT_EQ(b.GetFormat(), FORMAT_RESERVED);

    EXPECT_EQ(b.GetDataType(), DT_FLOAT);
    b.SetDataType(DT_INT8);
    EXPECT_EQ(b.GetDataType(), DT_INT8);

    TensorDesc c;
    c.SetShape(Shape({1}));
    c.SetFormat(FORMAT_NCHW);
    c.SetShape(s);

    uint32_t size1 = 1;
    TensorUtils::SetSize(c, size1);

    TensorDesc e = c;
    uint32_t size2 = 0;
    EXPECT_EQ(TensorUtils::GetSize(e, size2), GRAPH_SUCCESS);
    EXPECT_EQ(size2, 1);

    TensorDesc f = c;
    size2 = 0;
    EXPECT_EQ(TensorUtils::GetSize(f, size2), GRAPH_SUCCESS);
    EXPECT_EQ(size2, 1);

    f.SetName("A");
    EXPECT_EQ("A", f.GetName());

}

TEST_F(ge_test_tensor, tensor)
{
    Shape s({1,2,3,4});
    TensorDesc tensorDesc(s);
    std::vector<uint8_t> data({1,2,3,4});
    Tensor a;
    Tensor b(tensorDesc);
    Tensor c(tensorDesc, Buffer(data.size()));
    Tensor d(tensorDesc, data.data(), data.size());

    Shape s1 = b.GetTensorDesc().GetShape();
    EXPECT_EQ(s1.GetDim(0), 1);
    EXPECT_EQ(b.GetTensorDesc().GetDataType(), DT_FLOAT);
    b.MutableTensorDesc().SetDataType(DT_INT8);
    EXPECT_EQ(b.GetTensorDesc().GetDataType(), DT_INT8);
    b.SetTensorDesc(tensorDesc);

    auto data1 = c.MutableData();
    c.SetData(data.data(), data.size());
    EXPECT_EQ(c.MutableData().GetData()[0], uint8_t(1));
    EXPECT_EQ(c.MutableData().GetData()[1], uint8_t(2));
    EXPECT_EQ(c.MutableData().GetData()[2], uint8_t(3));
    EXPECT_EQ(c.MutableData().GetData()[3], uint8_t(4));

    auto clone = d.Clone();
    EXPECT_EQ(clone != nullptr, true);
    ge::Buffer buffer;
    EXPECT_EQ(clone->MutableTensorDesc().Save(buffer), 0);
    EXPECT_EQ(clone->MutableTensorDesc().Load(buffer.data(), buffer.size()), 0);
}

TEST_F(ge_test_tensor, test_shape_copy_move)
{
    Shape shape(nullptr, false);
    EXPECT_EQ(shape.GetDimNum(), 0);

    Shape shape2 = shape;
    EXPECT_EQ(shape2.GetDimNum(), 0);

    Shape shape3({1,2,3});
    shape2 = shape3;
    EXPECT_EQ(shape2.GetDimNum(), 3);
    EXPECT_EQ(shape3.GetDimNum(), 3);
}

TEST_F(ge_test_tensor, test_TensorDesc_InvalidNull)
{
    TensorDesc tensorDesc(nullptr, false);
    EXPECT_EQ(tensorDesc.GetDataType(), DT_UNDEFINED);
    EXPECT_EQ(tensorDesc.GetFormat(), FORMAT_RESERVED);
    EXPECT_EQ(tensorDesc.MutableShape().shapeDef_, nullptr);

    TensorDesc tensorDesc2;
    EXPECT_EQ(tensorDesc2.GetDataType(), DT_FLOAT);
    EXPECT_EQ(tensorDesc2.GetFormat(), FORMAT_NCHW);

    tensorDesc2.SetDataType(DT_DUAL_SUB_INT8);
    EXPECT_EQ(tensorDesc2.GetDataType(), DT_DUAL_SUB_INT8);

    AllOffsetQuantizeInfo quantizeInfo;
    EXPECT_FALSE(TensorUtils::HasAlloffsetQuantizeInfo(tensorDesc2));
    EXPECT_EQ(TensorUtils::GetAlloffsetQuantizeInfo(tensorDesc2, quantizeInfo), GRAPH_FAILED);
    TensorUtils::SetAlloffsetQuantizeInfo(tensorDesc2, quantizeInfo);
    EXPECT_EQ(TensorUtils::GetAlloffsetQuantizeInfo(tensorDesc2, quantizeInfo), GRAPH_SUCCESS);
    EXPECT_TRUE(TensorUtils::HasAlloffsetQuantizeInfo(tensorDesc2));

    TensorUtils::SetWeightSize(tensorDesc, 100);
    EXPECT_EQ(TensorUtils::GetWeightSize(tensorDesc), 0);

}


TEST_F(ge_test_tensor, test_Tensor_InvalidNull)
{
    Tensor tensor(nullptr, false);
    EXPECT_EQ(tensor.GetData().GetSize(), 0);
    EXPECT_EQ(tensor.MutableData().GetSize(), 0);
    EXPECT_EQ(tensor.SetData(Buffer(100)), GRAPH_SUCCESS);

    TensorUtils::SetWeightSize(tensor.MutableTensorDesc(), 100);
    EXPECT_EQ(TensorUtils::GetWeightSize(tensor), 0);

    auto tensorPtr = std::make_shared<Tensor>(nullptr, false);
    TensorUtils::SetWeightSize(tensorPtr->MutableTensorDesc(), 100);
    EXPECT_EQ(TensorUtils::GetWeightSize(tensorPtr), 0);
}


TEST_F(ge_test_tensor, test_Tensor_Utils_WeightSize)
{
    Tensor tensor;
    EXPECT_EQ(tensor.GetData().GetSize(), 0);
    EXPECT_EQ(tensor.MutableData().GetSize(), 0);
    EXPECT_EQ(tensor.SetData(Buffer(100)), GRAPH_SUCCESS);

    TensorUtils::SetWeightSize(tensor.MutableTensorDesc(), 100);
    EXPECT_EQ(TensorUtils::GetWeightSize(tensor), 100);

    uint8_t buffer[100];
    EXPECT_TRUE(TensorUtils::GetWeightAddr(tensor, buffer) != nullptr);

    auto tensorPtr = std::make_shared<Tensor>();
    TensorUtils::SetWeightSize(tensorPtr->MutableTensorDesc(), 100);
    EXPECT_EQ(TensorUtils::GetWeightSize(tensorPtr), 100);

    Tensor tensor2(TensorDesc(), Buffer(100));
    EXPECT_EQ(tensor2.GetData().GetSize(), 100);
    EXPECT_EQ(tensor2.MutableData().GetSize(), 100);
}
