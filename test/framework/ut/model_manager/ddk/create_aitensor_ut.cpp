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
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include "framework/compatible/AiTensor.h"
#include "framework/compatible/HiAiModelManagerType.h"
#include <limits.h>
#include <map>
#include <iostream>

using namespace std;
using namespace hiai;
using namespace testing;

class CreateAiTensorUTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }
    void SetUp()
    {
        aiTensor = make_shared<AiTensor>();
        ASSERT_NE(nullptr, aiTensor);
        tensorDim = make_shared<TensorDimension>(n, c, h, w);
        ASSERT_NE(nullptr, tensorDim);
    }

    void TearDown()
    {
        tensorDim = nullptr;
        aiTensor = nullptr;
    }

public:
    shared_ptr<AiTensor> aiTensor = nullptr;
    shared_ptr<TensorDimension> tensorDim = nullptr;
    uint32_t n = 2;
    uint32_t c = 2;
    uint32_t h = 100;
    uint32_t w = 200;
    AIStatus status = false;
    const std::map<AiTensorImage_Format, uint32_t> sizeScaleMap = {
        {AiTensorImage_YUV420SP_U8, 3},
        {AiTensorImage_XRGB8888_U8, 4},
        {AiTensorImage_YUV400_U8, 1},
        {AiTensorImage_ARGB8888_U8, 4},
        {AiTensorImage_YUYV_U8, 2},
        {AiTensorImage_YUV422SP_U8, 2},
        {AiTensorImage_AYUV444_U8, 4},
        {AiTensorImage_RGB888_U8, 3},
        {AiTensorImage_BGR888_U8, 3},
    };
    const std::map<AiTensorImage_Format, uint32_t> channelMap = {
        {AiTensorImage_YUV420SP_U8, 3},
        {AiTensorImage_XRGB8888_U8, 4},
        {AiTensorImage_YUV400_U8, 1},
        {AiTensorImage_ARGB8888_U8, 4},
        {AiTensorImage_YUYV_U8, 3},
        {AiTensorImage_YUV422SP_U8, 3},
        {AiTensorImage_AYUV444_U8, 4},
        {AiTensorImage_RGB888_U8, 3},
        {AiTensorImage_BGR888_U8, 3},
        {AiTensorImage_YUV444SP_U8, 3},
        {AiTensorImage_YVU444SP_U8, 3},
        {AiTensorImage_INVALID, 255},
    };
};

uint32_t getDataTypeSize(hiai::HIAI_DataType dataType)
{
    if (dataType == hiai::HIAI_DATATYPE_UINT8) {
        return sizeof(uint8_t);
    } else if (dataType == hiai::HIAI_DATATYPE_FLOAT32) {
        return sizeof(float);
    } else if (dataType == hiai::HIAI_DATATYPE_FLOAT16) {
        return sizeof(float) / 2;
    } else if (dataType == hiai::HIAI_DATATYPE_INT32) {
        return sizeof(int32_t);
    } else if (dataType == hiai::HIAI_DATATYPE_INT8) {
        return sizeof(int8_t);
    } else if (dataType == hiai::HIAI_DATATYPE_INT16) {
        return sizeof(int16_t);
    } else if (dataType == hiai::HIAI_DATATYPE_BOOL) {
        return sizeof(bool);
    } else if (dataType == hiai::HIAI_DATATYPE_INT64) {
        return sizeof(int64_t);
    } else if (dataType == hiai::HIAI_DATATYPE_UINT32) {
        return sizeof(uint32_t);
    } else if (dataType == hiai::HIAI_DATATYPE_DOUBLE) {
        return sizeof(double);
    } else {
        return 0;
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_succ_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * 4
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_succ_001)
{
    printf("---InitAiTensor_Dim_succ_001 start----\n");
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_SUCCESS);
    EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
    EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * 4);
    TensorDimension td = aiTensor->GetTensorDimension();
    EXPECT_TRUE(td.GetNumber() == n);
    EXPECT_TRUE(td.GetChannel() == c);
    EXPECT_TRUE(td.GetHeight() == h);
    EXPECT_TRUE(td.GetWidth() == w);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_succ_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数下边值界内
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * 4
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_succ_002)
{
    printf("---InitAiTensor_Dim_succ_002 start----\n");
    n, c, h, w = 1, 1, 1, 2;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_SUCCESS);
    EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
    EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * 4);
    TensorDimension td = aiTensor->GetTensorDimension();
    EXPECT_TRUE(td.GetNumber() == n);
    EXPECT_TRUE(td.GetChannel() == c);
    EXPECT_TRUE(td.GetHeight() == h);
    EXPECT_TRUE(td.GetWidth() == w);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_succ_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数下边界值
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * 4
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_succ_003)
{
    printf("---InitAiTensor_Dim_succ_003 start----\n");
    n, c, h, w = 1, 1, 1, 1;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_SUCCESS);
    EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
    EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * 4);
    TensorDimension td = aiTensor->GetTensorDimension();
    EXPECT_TRUE(td.GetNumber() == n);
    EXPECT_TRUE(td.GetChannel() == c);
    EXPECT_TRUE(td.GetHeight() == h);
    EXPECT_TRUE(td.GetWidth() == w);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_succ_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * 4
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_succ_004)
{
    printf("---InitAiTensor_Dim_succ_004 start----\n");
    n, c, h, w = 1, 1, 1, UINT_MAX / 4 - 1;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_SUCCESS);
    EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
    EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * 4);
    TensorDimension td = aiTensor->GetTensorDimension();
    EXPECT_TRUE(td.GetNumber() == n);
    EXPECT_TRUE(td.GetChannel() == c);
    EXPECT_TRUE(td.GetHeight() == h);
    EXPECT_TRUE(td.GetWidth() == w);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_succ_005
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * 4
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_succ_005)
{
    printf("---InitAiTensor_Dim_succ_005 start----\n");
    n, c, h, w = 1, 1, 1, UINT_MAX / 4;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_SUCCESS);
    EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
    EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * 4);
    TensorDimension td = aiTensor->GetTensorDimension();
    EXPECT_TRUE(td.GetNumber() == n);
    EXPECT_TRUE(td.GetChannel() == c);
    EXPECT_TRUE(td.GetHeight() == h);
    EXPECT_TRUE(td.GetWidth() == w);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_fail_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数, TensorDimension为nullptr
 *           调用Iint接口
 * 预期结果 : 返回AI_INVALID_PARA
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_fail_001)
{
    printf("---InitAiTensor_Dim_fail_001 start----\n");
    status = aiTensor->Init(nullptr);
    EXPECT_TRUE(status == AI_INVALID_PARA);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_fail_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数. w= 0
 *           调用Iint接口
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_fail_002)
{
    printf("---InitAiTensor_Dim_fail_002 start----\n");
    tensorDim = make_shared<TensorDimension>(1, 1, 1, 0);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_fail_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数，size超过最大值
 *           调用Iint接口
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_fail_003)
{
    printf("---InitAiTensor_Dim_fail_003 start----\n");
    tensorDim = make_shared<TensorDimension>(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX / 4 + 1);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_fail_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数 w = -1
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_fail_004)
{
    printf("---InitAiTensor_Dim_fail_004 start----\n");
    tensorDim = make_shared<TensorDimension>(1, 1, 1, -1);
    status = aiTensor->Init(tensorDim.get());
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_HIAI_DataType_succ_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数 HIAI_DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_succ_001)
{
    printf("---InitAiTensor_Dim_HIAI_DataType_succ_001 start----\n");
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        status = aiTensor->Init(tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        std::cout << "GetSize: " << aiTensor->GetSize() <<endl;
        std::cout << "Size: " << n * c * h * w * getDataTypeSize(dataType) <<endl;
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }

        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_succ_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数下边界值内 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_succ_002)
{
    printf("---InitAiTensor_Dim_AI_DataType_succ_002 start----\n");
    n, c, h, w = 1, 1, 1, 2;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        status = aiTensor->Init(tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_succ_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 *           调用Iint接口
 * 操作步骤:  构造TensorDimension参数下边界值 _DataType参数
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_succ_003)
{
    printf("---InitAiTensor_Dim_AI_DataType_succ_003 start----\n");
    n, c, h, w = 1, 1, 1, 1;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        status = aiTensor->Init(tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_succ_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数上边界值内 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_succ_004)
{
    printf("---InitAiTensor_Dim_AI_DataType_succ_004 start----\n");
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        n, c, h, w = 1, 1, 1, INT_MAX / getDataTypeSize(dataType) - 1;
        tensorDim = make_shared<TensorDimension>(n, c, h, w);
        status = aiTensor->Init(tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_succ_005
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造TensorDimension参数上边界值 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_succ_005)
{
    printf("---InitAiTensor_Dim_AI_DataType_succ_005 start----\n");
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        n, c, h, w = 1, 1, 1, INT_MAX / getDataTypeSize(dataType);
        tensorDim = make_shared<TensorDimension>(n, c, h, w);
        status = aiTensor->Init(tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_fail_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造失败
 * 操作步骤:  构造TensorDimension参数 TensorDimension == nullptr
 *           调用Iint接口
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_fail_001)
{
    printf("---InitAiTensor_Dim_AI_DataType_fail_001 start----\n");
    hiai::HIAI_DataType dataType;
    dataType = hiai::HIAI_DataType(hiai::HIAI_DATATYPE_UINT8);
    status = aiTensor->Init(nullptr, dataType);
    EXPECT_TRUE(status == AI_INVALID_PARA);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_fail_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造失败
 * 操作步骤:  构造TensorDimension参数 w = 0
 *           调用Iint接口
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_fail_002)
{
    printf("---InitAiTensor_Dim_AI_DataType_fail_002 start----\n");
    tensorDim = make_shared<TensorDimension>(1, 1, 1, 0);
    hiai::HIAI_DataType dataType;
    dataType = hiai::HIAI_DataType(hiai::HIAI_DATATYPE_UINT8);
    status = aiTensor->Init(tensorDim.get(), dataType);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_Dim_DataType_fail_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造失败
 * 操作步骤:  构造TensorDimension参数 size超过最大值
 *           调用Iint接口
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_fail_003)
{
    printf("---InitAiTensor_Dim_AI_DataType_fail_003 start----\n");
    hiai::HIAI_DataType dataType;
    dataType = hiai::HIAI_DATATYPE_UINT8;
    tensorDim = make_shared<TensorDimension>(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX / getDataTypeSize(dataType) + 1);
    status = aiTensor->Init(tensorDim.get(), dataType);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_Dim__DataType_fail_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造失败
 * 操作步骤:  构造TensorDimension参数 _DataType参数不合法
 * 预期结果 : 返回AI_FAILED
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Dim_HIAI_DataType_fail_004)
{
    printf("---InitAiTensor_Dim_AI_DataType_fail_004 start----\n");
    hiai::HIAI_DataType dataType;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    dataType = hiai::HIAI_DataType(-1);
    status = aiTensor->Init(tensorDim.get(), dataType);
    EXPECT_TRUE(status == AI_INVALID_PARA);
}

/*
 * 测试用例名称   : InitAiTensor_Handle_Dim_DataType_succ_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造handle参数，TensorDimension参数 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_Handle_Dim_HIAI_DataType_succ_001)
{
    printf("---InitAiTensor_Handle_Dim_AI_DataType_succ_001 start----\n");
    hiai::HIAI_DataType dataType;
    NativeHandle handle;
    handle.fd = 1;
    handle.offset = 0;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        std::cout << "dataType" <<std::endl;
        std::cout << dataType << std::endl;
        handle.size = tensorDim->GetNumber() * tensorDim->GetChannel() * tensorDim->GetHeight() *
            tensorDim->GetWidth() * getDataTypeSize(dataType);
        status = aiTensor->Init(handle, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        std::cout << "aiTensor->GetSize():"<<aiTensor->GetSize() << std::endl;

        // std::cout << "getDataTypeSize(dataType):"<<getDataTypeSize(dataType) << std::endl;
        std::cout << n * c * h * w * getDataTypeSize(dataType) << std::endl;
        EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Handle_Dim_DataType_succ_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造handle参数，TensorDimension参数下边界值内 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Handle_Dim_HIAI_DataType_succ_002)
{
    printf("---InitAiTensor_Handle_Dim_AI_DataType_succ_002 start----\n");
    hiai::HIAI_DataType dataType;
    NativeHandle handle;
    n, c, h, w = 1, 1, 1, 2;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        handle.size = tensorDim->GetNumber() * tensorDim->GetChannel() * tensorDim->GetHeight() *
            tensorDim->GetWidth() * getDataTypeSize(dataType);
        status = aiTensor->Init(handle, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Handle_Dim_DataType_succ_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造handle参数，TensorDimension参数下边界值 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Handle_Dim_HIAI_DataType_succ_003)
{
    printf("---InitAiTensor_Handle_Dim_AI_DataType_succ_003 start----\n");
    hiai::HIAI_DataType dataType;
    NativeHandle handle;
    n, c, h, w = 1, 1, 1, 1;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        handle.size = tensorDim->GetNumber() * tensorDim->GetChannel() * tensorDim->GetHeight() *
            tensorDim->GetWidth() * getDataTypeSize(dataType);
        status = aiTensor->Init(handle, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Handle_Dim_DataType_succ_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造handle参数，TensorDimension参数上边界值内 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Handle_Dim_HIAI_DataType_succ_004)
{
    printf("---InitAiTensor_Handle_Dim_AI_DataType_succ_004 start----\n");
    hiai::HIAI_DataType dataType;
    NativeHandle handle;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        n, c, h, w = 1, 1, 1, INT_MAX / getDataTypeSize(dataType) - 1;
        tensorDim = make_shared<TensorDimension>(n, c, h, w);
        handle.size = tensorDim->GetNumber() * tensorDim->GetChannel() * tensorDim->GetHeight() *
            tensorDim->GetWidth() * getDataTypeSize(dataType);
        status = aiTensor->Init(handle, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_Handle_Dim_DataType_succ_005
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造handle参数，TensorDimension参数上边界值 _DataType参数
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */

TEST_F(CreateAiTensorUTest, InitAiTensor_Handle_Dim_HIAI_DataType_succ_005)
{
    printf("---InitAiTensor_Handle_Dim_AI_DataType_succ_005 start----\n");
    hiai::HIAI_DataType dataType;
    NativeHandle handle;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        dataType = hiai::HIAI_DataType(type);
        n, c, h, w = 1, 1, 1, INT_MAX / getDataTypeSize(dataType);
        tensorDim = make_shared<TensorDimension>(n, c, h, w);
        handle.size = tensorDim->GetNumber() * tensorDim->GetChannel() * tensorDim->GetHeight() *
            tensorDim->GetWidth() * getDataTypeSize(dataType);
        status = aiTensor->Init(handle, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_AiTensorImage_Format_succ_001
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造nhw参数和AiTensorImage_Format
 *           调用Init接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * iter->second * h * w
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_AiTensorImage_Format_succ_001)
{
    printf("---InitAiTensor_AiTensorImage_Format_succ_001 start----\n");
    hiai::AiTensorImage_Format imageFormat;
    for (int format = hiai::AiTensorImage_YUV420SP_U8; format <= hiai::AiTensorImage_ARGB8888_U8; format++) {
        imageFormat = hiai::AiTensorImage_Format(format);
        auto iter = sizeScaleMap.find(imageFormat);
        EXPECT_TRUE(iter != sizeScaleMap.end());
        status = aiTensor->Init(n, h, w, imageFormat);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        size_t size = static_cast<size_t>(n * iter->second * h * w);
        if (imageFormat == AiTensorImage_Format::AiTensorImage_YUV420SP_U8) {
            size = size / 2;
        }

        EXPECT_TRUE(aiTensor->GetSize() == size);
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == iter->second);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_AiTensorImage_Format_succ_002
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : 构造nhw参数下边界值内和AiTensorImage_Format
 *           调用Iint接口
 * 操作步骤:  根据nhwc和AiTensorImage_Format创建 Aitensor
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * iter->second * h * w
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_AiTensorImage_Format_succ_002)
{
    printf("---InitAiTensor_AiTensorImage_Format_succ_002 start----\n");
    hiai::AiTensorImage_Format imageFormat;
    n, h, w = 1, 1, 2;
    for (int format = hiai::AiTensorImage_YUV420SP_U8; format <= hiai::AiTensorImage_ARGB8888_U8; format++) {
        imageFormat = hiai::AiTensorImage_Format(format);
        auto iter = sizeScaleMap.find(imageFormat);
        EXPECT_TRUE(iter != sizeScaleMap.end());
        status = aiTensor->Init(n, h, w, imageFormat);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        size_t size = static_cast<size_t>(n * iter->second * h * w);
        if (imageFormat == AiTensorImage_Format::AiTensorImage_YUV420SP_U8) {
            size = size / 2;
        }
        EXPECT_TRUE(aiTensor->GetSize() == size);
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == iter->second);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_AiTensorImage_Format_succ_003
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造nhw参数下边界值和AiTensorImage_Format
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * iter->second * h * w
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_AiTensorImage_Format_succ_003)
{
    printf("---InitAiTensor_AiTensorImage_Format_succ_003 start----\n");
    hiai::AiTensorImage_Format imageFormat;
    n, h, w = 1, 1, 1;
    for (int format = hiai::AiTensorImage_YUV420SP_U8; format <= hiai::AiTensorImage_ARGB8888_U8; format++) {
        imageFormat = hiai::AiTensorImage_Format(format);
        auto iter = sizeScaleMap.find(imageFormat);
        EXPECT_TRUE(iter != sizeScaleMap.end());
        status = aiTensor->Init(n, h, w, imageFormat);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        size_t size = static_cast<size_t>(n * iter->second * h * w);
        if (imageFormat == AiTensorImage_Format::AiTensorImage_YUV420SP_U8) {
            size = size / 2;
        }
        EXPECT_TRUE(aiTensor->GetSize() == size);
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == iter->second);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_AiTensorImage_Format_succ_004
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  构造nhw参数上边界值内和AiTensorImage_Format
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * iter->second * h * w
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_AiTensorImage_Format_succ_004)
{
    printf("---InitAiTensor_AiTensorImage_Format_succ_004 start----\n");
    hiai::AiTensorImage_Format imageFormat;
    for (int format = hiai::AiTensorImage_YUV420SP_U8; format <= hiai::AiTensorImage_ARGB8888_U8; format++) {
        imageFormat = hiai::AiTensorImage_Format(format);
        auto iter = sizeScaleMap.find(imageFormat);
        EXPECT_TRUE(iter != sizeScaleMap.end());
        uint32_t channel = iter->second;
        n, h, w = 1, 1, (INT32_MAX - 1) / channel;
        status = aiTensor->Init(n, h, w, imageFormat);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        size_t size = static_cast<size_t>(n * iter->second * h * w);
        if (imageFormat == AiTensorImage_Format::AiTensorImage_YUV420SP_U8) {
            size = size / 2;
        }
        EXPECT_TRUE(aiTensor->GetSize() == size);
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == iter->second);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
 * 测试用例名称   : InitAiTensor_AiTensorImage_Format_succ_005
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : 构造nhw参数上边界值和AiTensorImage_Format
 *           调用Iint接口
 * 操作步骤:  根据nhw和AiTensorImage_Format创建 Aitensor
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetSize() == n * iter->second * h * w
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_AiTensorImage_Format_succ_005)
{
    printf("---InitAiTensor_AiTensorImage_Format_succ_005 start----\n");
    hiai::AiTensorImage_Format imageFormat;
    for (int format = hiai::AiTensorImage_YUV420SP_U8; format <= hiai::AiTensorImage_ARGB8888_U8; format++) {
        imageFormat = hiai::AiTensorImage_Format(format);
        auto iter = sizeScaleMap.find(imageFormat);
        EXPECT_TRUE(iter != sizeScaleMap.end());
        uint32_t channel = iter->second;
        n, h, w = 1, 1, INT32_MAX / channel;
        status = aiTensor->Init(n, h, w, imageFormat);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        size_t size = static_cast<size_t>(n * iter->second * h * w);
        if (imageFormat == AiTensorImage_Format::AiTensorImage_YUV420SP_U8) {
            size = size / 2;
        }
        EXPECT_TRUE(aiTensor->GetSize() == size);
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == iter->second);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
    }
}

/*
* 测试用例名称   : InitAiTensor_nhwc_imageformat_fail_001
* 测试用例描述：  创建AiTesnor
* 预置条件 : aiTensor构造失败
* 操作步骤:  构造nhw参数和AiTensorImage_Format,Format不合法
*           调用Iint接口
* 预期结果 : 返回AI_FAILED
* 修改历史 :
*/
TEST_F(CreateAiTensorUTest, InitAiTensor_nhwc_imageformat_fail_001)
{
    hiai::AiTensorImage_Format imageFormat;
    printf("---InitAiTensor_nhwc_imageformat_fail_001 start----\n");
    imageFormat = hiai::AiTensorImage_Format(255);
    status = aiTensor->Init(n, h, w, imageFormat);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
* 测试用例名称   : InitAiTensor_nhwc_imageformat_fail_002
* 测试用例描述：  创建AiTesnor
* 预置条件 : aiTensor构造失败
* 操作步骤:  构造nhw参数和AiTensorImage_Format,w=0
*           调用Iint接口
* 预期结果 : 返回AI_FAILED
* 修改历史 :
*/
TEST_F(CreateAiTensorUTest, InitAiTensor_nhwc_imageformat_fail_002)
{
    hiai::AiTensorImage_Format imageFormat;
    printf("---InitAiTensor_nhwc_imageformat_fail_002 start----\n");
    imageFormat = hiai::AiTensorImage_Format(hiai::AiTensorImage_YUV420SP_U8);
    status = aiTensor->Init(1, 1, 0, imageFormat);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
* 测试用例名称   : InitAiTensor_nhwc_imageformat_fail_003
* 测试用例描述：  创建AiTesnor
* 预置条件 : aiTensor构造失败
* 操作步骤:  构造nhw参数和AiTensorImage_Format,size超过最大值
*           调用Iint接口
* 预期结果 : 返回AI_FAILED
* 修改历史 :
*/
TEST_F(CreateAiTensorUTest, InitAiTensor_nhwc_imageformat_fail_003)
{
    hiai::AiTensorImage_Format imageFormat;
    printf("---InitAiTensor_nhwc_imageformat_fail_003 start----\n");
    imageFormat = hiai::AiTensorImage_Format(hiai::AiTensorImage_YUV420SP_U8);
    status = aiTensor->Init(1, 1, (uint32_t)(INT32_MAX) + 1, imageFormat);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
* 测试用例名称   : InitAiTensor_nhwc_imageformat_fail_004
* 测试用例描述：  创建AiTesnor
* 预置条件 : aiTensor构造失败
* 操作步骤:  构造nhw参数和AiTensorImage_Format,w=-1
*           调用Iint接口
* 预期结果 : 返回AI_FAILED
* 修改历史 :
*/
TEST_F(CreateAiTensorUTest, InitAiTensor_nhwc_imageformat_fail_004)
{
    hiai::AiTensorImage_Format imageFormat;
    printf("---InitAiTensor_nhwc_imageformat_fail_004 start----\n");
    imageFormat = hiai::AiTensorImage_Format(hiai::AiTensorImage_YUV420SP_U8);
    status = aiTensor->Init(1, 1, -1, imageFormat);
    EXPECT_TRUE(status == AI_FAILED);
}

/*
 * 测试用例名称   : InitAiTensor_NoCopy_succ
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  分配用户内存data 构造TensorDimension参数 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 *           aiTensor->GetBuffer() == data
 *           aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType)
 *           td.GetNumber() == n
 *           td.GetChannel() == c
 *           td.GetHeight() == h
 *           td.GetWidth() == w
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_NoCopy_succ)
{
    printf("---InitAiTensor_NoCopy_succ start----\n");
    n, c, h, w = 1, 3, 224, 224;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        void* data = (void*)malloc(1 * 3 * 224 * 224);
        dataType = hiai::HIAI_DataType(type);
        status = aiTensor->Init(data, tensorDim.get(), dataType);
        EXPECT_TRUE(status == AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() != nullptr);
        EXPECT_TRUE(aiTensor->GetBuffer() == data);
        if (aiTensor->GetSize() != n * c * h * w * sizeof(float)){
            EXPECT_TRUE(aiTensor->GetSize() == n * c * h * w * getDataTypeSize(dataType));
        }
        TensorDimension td = aiTensor->GetTensorDimension();
        EXPECT_TRUE(td.GetNumber() == n);
        EXPECT_TRUE(td.GetChannel() == c);
        EXPECT_TRUE(td.GetHeight() == h);
        EXPECT_TRUE(td.GetWidth() == w);
        free(data);
    }
}

/*
 * 测试用例名称   : InitAiTensor_NoCopy_failed
 * 测试用例描述：  创建AiTesnor
 * 预置条件 : aiTensor构造成功
 * 操作步骤:  分配用户内存data = nullptr 构造TensorDimension参数 _DataType参数
 *           调用Iint接口
 * 预期结果 : 返回success
 *           aiTensor->GetBuffer() != nullptr
 * 修改历史 :
 */
TEST_F(CreateAiTensorUTest, InitAiTensor_NoCopy_failed)
{
    printf("---InitAiTensor_NoCopy_failed start----\n");
    n, c, h, w = 1, 3, 224, 224;
    tensorDim = make_shared<TensorDimension>(n, c, h, w);
    hiai::HIAI_DataType dataType;
    for (int type = hiai::HIAI_DATATYPE_UINT8; type <= hiai::HIAI_DATATYPE_DOUBLE; type++) {
        void* data = nullptr;
        dataType = hiai::HIAI_DataType(type);
        status = aiTensor->Init(data, tensorDim.get(), dataType);
        EXPECT_TRUE(status != AI_SUCCESS);
        EXPECT_TRUE(aiTensor->GetBuffer() == nullptr);
    }
}