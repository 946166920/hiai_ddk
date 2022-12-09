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
#include "graph/attr_value.h"
#include "graph/tensor.h"
#include "framework/graph/utils/tensor_utils.h"
#include <iostream>
using namespace std;
using namespace ge;


class ge_test_tensor_utils : public testing::Test
{
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};


TEST_F(ge_test_tensor_utils, shape)
{
    TensorDesc tensorDesc;


    uint32_t s1 = 1;
    uint32_t s2 = 0;
    TensorUtils::SetSize(tensorDesc, s1);
    EXPECT_EQ(TensorUtils::GetSize(tensorDesc, s2), GRAPH_SUCCESS);
    EXPECT_EQ(s2, 1);
    TensorUtils::SetSize(tensorDesc, 2);
    EXPECT_EQ(TensorUtils::GetSize(tensorDesc, s2), GRAPH_SUCCESS);
    EXPECT_EQ(s2, 2);

    bool f1(true);
    bool f2(false);
    TensorUtils::SetReuseInput(tensorDesc, f1);
    EXPECT_EQ(TensorUtils::GetReuseInput(tensorDesc, f2), GRAPH_SUCCESS);
    EXPECT_EQ(f2, true);

    f1 = true;
    f2 = false;
    TensorUtils::SetOutputTensor(tensorDesc, f1);
    EXPECT_EQ(TensorUtils::GetOutputTensor(tensorDesc, f2), GRAPH_SUCCESS);
    EXPECT_EQ(f2, true);

    DeviceType d1(DeviceType::CPU);
    DeviceType d2(DeviceType::NPU);
    TensorUtils::SetDeviceType(tensorDesc, d1);
    EXPECT_EQ(TensorUtils::GetDeviceType(tensorDesc, d2), GRAPH_SUCCESS);
    EXPECT_EQ(d2, true);

    f1 = true;
    f2 = false;
    TensorUtils::SetInputTensor(tensorDesc, f1);
    EXPECT_EQ(TensorUtils::GetInputTensor(tensorDesc, f2), GRAPH_SUCCESS);
    EXPECT_EQ(f2, true);


    s1 = 1;
    s2 = 0;
    TensorUtils::SetRealDimCnt(tensorDesc, s1);
    EXPECT_EQ(TensorUtils::GetRealDimCnt(tensorDesc, s2), GRAPH_SUCCESS);
    EXPECT_EQ(s2, 1);

    s1 = 1;
    s2 = 0;
    TensorUtils::SetReuseInputIndex(tensorDesc, s1);
    EXPECT_EQ(TensorUtils::GetReuseInputIndex(tensorDesc, s2), GRAPH_SUCCESS);
    EXPECT_EQ(s2, 1);

    int64_t s3(1);
    int64_t s4(0);
    TensorUtils::SetDataOffset(tensorDesc, s3);
    EXPECT_EQ(TensorUtils::GetDataOffset(tensorDesc, s4), GRAPH_SUCCESS);
    EXPECT_EQ(s4, 1);
}
TEST_F(ge_test_tensor_utils, InputTensor)
{
    TensorDesc tensorDesc;
    bool flag = true;
    TensorDescPtr tensorDescptr = make_shared<ge::TensorDesc>();
    uint32_t cnt = 2;
    EXPECT_EQ(TensorUtils::GetRealDimCnt(tensorDescptr, cnt), GRAPH_SUCCESS);
    EXPECT_EQ(TensorUtils::GetRealDimCnt(nullptr, cnt), GRAPH_FAILED);
    EXPECT_EQ(TensorUtils::HasAlloffsetQuantizeInfo(nullptr), false);
    EXPECT_EQ(TensorUtils::HasAlloffsetQuantizeInfo(tensorDescptr), false);
    AllOffsetQuantizeInfo info;
    TensorUtils::GetAlloffsetQuantizeInfo(tensorDescptr, info);
}



