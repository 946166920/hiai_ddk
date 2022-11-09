/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: aipp_tensor Unit Test
 */
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include "framework/tensor/v2/aipp_tensor_v2.h"
#include "framework/tensor/v2/image_buffer_v2.h"

using namespace std;
using namespace hiai;

class aippTensorUTest : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

public:
    const uint32_t n = 2;
    const uint32_t c = 2;
    const uint32_t h = 100;
    const uint32_t w = 200;
    const int32_t b = 3;
    const int32_t rotation = 2;
    ImageFormatV2 format = ImageFormatV2::YUV420SP;
    ImageColorSpaceV2 colorSpace = ImageColorSpaceV2::BT_601_NARROW;
};

/*
* 测试用例标题：_CreateAIPPara_succ_001
* 测试用例描述：测试_CreateAIPParaV2接口
* 预置条件：
* 操作步骤：
*           1. 调用_CreateImageBufferV2接口创建ImageBuffer
*           2. 调用_CreateAIPParaV2，通过ImageBuffer创建AippPara
* 预期结果：创建成功
*/
TEST_F(aippTensorUTest, HIAI_CreateAIPPara_succ_001)
{
    printf("---AI_CreateAIPPara_succ_001 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(b, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->Data() != nullptr);
    EXPECT_TRUE(imageBuffer->Size() != 0);

    std::shared_ptr<IAIPPParaV2> aippPara = nullptr;
    HIAI_CreateAIPParaV2(imageBuffer, aippPara);
    EXPECT_TRUE(aippPara != nullptr);
}

/*
* 测试用例标题：_CreateAIPPTensor_succ_001
* 测试用例描述：测试_CreateAIPPTensorV2接口
* 预置条件：
* 操作步骤：
*           1. 调用_CreateImageBufferV2接口创建ImageBuffer
*           2. 调用_CreateAIPParaV2，通过ImageBuffer创建AippPara
*           3. 调用_CreateAIPPTensorV2创建AIPPTensor
* 预期结果：创建成功
*/
TEST_F(aippTensorUTest, HIAI_CreateAIPPTensor_succ_001)
{
    printf("---AI_CreateAIPPTensor_succ_001 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(b, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->Data() != nullptr);
    EXPECT_TRUE(imageBuffer->Size() != 0);

    vector<std::shared_ptr<IAIPPParaV2>> aippParas;
    std::shared_ptr<IAIPPParaV2> aippPara = nullptr;
    HIAI_CreateAIPParaV2(imageBuffer, aippPara);
    ASSERT_TRUE(aippPara != nullptr);
    aippParas.push_back(aippPara);

    std::shared_ptr<IAIPPTensorV2> tensor = nullptr;
    HIAI_CreateAIPPTensorV2(imageBuffer, aippParas, tensor);
    EXPECT_TRUE(tensor != nullptr);
}

/*
* 测试用例标题：_CreateAIPPTensor_fail_001
* 测试用例描述：测试_CreateAIPPTensorV2接口
* 预置条件：
* 操作步骤：
*           1. 调用_CreateImageBufferV2接口创建ImageBuffer
*           2. 调用_CreateAIPParaV2，通过ImageBuffer创建AippPara
*           3. 调用_CreateAIPPTensorV2, 传入非法ImageBuffer, 创建AIPPTensor
* 预期结果：创建失败
*/
TEST_F(aippTensorUTest, HIAI_CreateAIPPTensor_fail_001)
{
    printf("---AI_CreateAIPPTensor_fail_001 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(b, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->Data() != nullptr);
    EXPECT_TRUE(imageBuffer->Size() != 0);

    vector<std::shared_ptr<IAIPPParaV2>> aippParas;
    std::shared_ptr<IAIPPParaV2> aippPara = nullptr;
    HIAI_CreateAIPParaV2(imageBuffer, aippPara);
    ASSERT_TRUE(aippPara != nullptr);
    aippParas.push_back(aippPara);

    std::shared_ptr<IAIPPTensorV2> tensor = nullptr;
    HIAI_CreateAIPPTensorV2(nullptr, aippParas, tensor);
    EXPECT_TRUE(tensor == nullptr);
}