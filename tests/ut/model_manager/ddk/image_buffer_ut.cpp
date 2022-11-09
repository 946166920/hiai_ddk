/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: image_buffer Unit Test
 */
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include <limits.h>
#include "framework/tensor/v2/aipp_tensor_v2.h"
#include "framework/tensor/v2/image_buffer_v2.h"

using namespace std;
using namespace hiai;

class imageBufferUTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }
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
    const uint32_t numberZero = 0;
    const uint32_t numberMaxInt = INT_MAX;
    const int32_t rotation = 2;
    ImageFormatV2 format = ImageFormatV2::YUV420SP;
    ImageFormatV2 formatInvalid = ImageFormatV2::INVALID;
    ImageColorSpaceV2 colorSpace = ImageColorSpaceV2::BT_601_NARROW;
};

/**
 * 测试用例描述：HIAI_CreateImageBuffer成功
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBuffer_succ_001)
{
    printf("---AI_CreateImageBuffer_succ_001 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(b, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->Data() != nullptr);
    EXPECT_TRUE(imageBuffer->Size() != 0);
}

/**
 * 测试用例描述：_CreateImageBuffer失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBuffer_fail_001)
{
    printf("---AI_CreateImageBuffer_fail_001 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(numberZero, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：_CreateImageBuffer失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBuffer_fail_002)
{
    printf("---AI_CreateImageBuffer_fail_002 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(numberMaxInt, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageBuffer失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBuffer_fail_003)
{
    printf("---AI_CreateImageBuffer_fail_003 start----\n");
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferV2(b, h, w, formatInvalid, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageBufferFromHandle成功
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBufferFromHandle_succ_001)
{
    printf("---AI_CreateImageBufferFromHandle_succ_001 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferFromHandleV2(handle, b, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->Data() != nullptr);
    EXPECT_TRUE(imageBuffer->Size() != 0);
}

/**
 * 测试用例描述：AI_CreateImageBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBufferFromHandle_fail_001)
{
    printf("---AI_CreateImageBufferFromHandle_fail_001 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferFromHandleV2(handle, numberZero, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBufferFromHandle_fail_002)
{
    printf("---AI_CreateImageBufferFromHandle_fail_002 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferFromHandleV2(handle, numberMaxInt, h, w, format, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：_CreateImageBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, HIAI_CreateImageBufferFromHandle_fail_003)
{
    printf("---AI_CreateImageBufferFromHandle_fail_003 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageBufferV2> imageBuffer = nullptr;
    HIAI_CreateImageBufferFromHandleV2(handle, b, h, w, formatInvalid, colorSpace, rotation, imageBuffer);
    EXPECT_TRUE(imageBuffer == nullptr);
}
