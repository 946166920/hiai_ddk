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
#include <limits.h>
#include "tensor/aipp_para.h"
#include "tensor/image_tensor_buffer.h"

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
    ImageFormat format = ImageFormat::YUV420SP;
    ImageFormat formatInvalid = ImageFormat::INVALID;
    ImageColorSpace colorSpace = ImageColorSpace::BT_601_NARROW;
};

/**
 * 测试用例描述：CreateImageTensorBuffer成功
 **/
TEST_F(imageBufferUTest, CreateImageTensorBuffer_succ_001)
{
    printf("---CreateImageTensorBuffer_succ_001 start----\n");
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBuffer(b, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer != nullptr);

    EXPECT_TRUE(imageBuffer->GetData() != nullptr);
    EXPECT_TRUE(imageBuffer->GetSize() != 0);
}

/**
 * 测试用例描述：CreateImageTensorBuffer失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBuffer_fail_001)
{
    printf("---CreateImageTensorBuffer_fail_001 start----\n");
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBuffer(numberZero, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageTensorBuffer失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBuffer_fail_002)
{
    printf("---CreateImageTensorBuffer_fail_002 start----\n");
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBuffer(numberMaxInt, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageTensorBuffer失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBuffer_fail_003)
{
    printf("---CreateImageTensorBuffer_fail_003 start----\n");
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBuffer(b, h, w, formatInvalid, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageTensorBufferFromHandle成功
 **/
TEST_F(imageBufferUTest, CreateImageTensorBufferFromHandle_succ_001)
{
    printf("---CreateImageTensorBufferFromHandle_succ_001 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBufferFromHandle(handle, b, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer != nullptr);
    EXPECT_TRUE(imageBuffer->GetData() != nullptr);
    EXPECT_TRUE(imageBuffer->GetSize() != 0);
}

/**
 * 测试用例描述：CreateImageTensorBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBufferFromHandle_fail_001)
{
    printf("---CreateImageTensorBufferFromHandle_fail_001 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBufferFromHandle(handle, numberZero, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageTensorBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBufferFromHandle_fail_002)
{
    printf("---CreateImageBufferFromHandle_fail_002 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBufferFromHandle(handle, numberMaxInt, h, w, format, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}

/**
 * 测试用例描述：CreateImageTensorBufferFromHandle失败
 **/
TEST_F(imageBufferUTest, CreateImageTensorBufferFromHandle_fail_003)
{
    printf("---CreateImageTensorBufferFromHandle_fail_003 start----\n");
    NativeHandle handle;
    handle.fd = 1;
    handle.size = b * h * w * sizeof(float);
    handle.offset = 0;
    std::shared_ptr<IImageTensorBuffer> imageBuffer =
    CreateImageTensorBufferFromHandle(handle, b, h, w, formatInvalid, colorSpace, rotation);
    EXPECT_TRUE(imageBuffer == nullptr);
}
