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
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>

#include <iostream>
#include <cstdlib>
#include <dlfcn.h>

#include "tensor/aipp/aipp_para_impl.h"
#include "tensor/aipp_para.h"
#include "tensor/buffer.h"
#include "framework/c/hiai_tensor_aipp_para.h"

using namespace hiai;
using AIStatus = int32_t;
static const AIStatus AI_SUCCESS = 0;
static const AIStatus AI_FAILED = 1;
static const AIStatus AI_NOT_INIT = 2;
static const AIStatus AI_INVALID_PARA = 3;
static const AIStatus AI_INVALID_API = 7;
static const AIStatus AI_INVALID_POINTER = 8;

class AippPara_v2_ut : public testing::Test {
protected:
    static void SetUpTestCase()
    {
        std::cout << __func__ << std::endl;
    }

    static void TearDownTestCase()
    {
        std::cout << __func__ << std::endl;
    }

    void SetUp()
    {
        std::cout << __func__ << std::endl;
        InitAippPara();
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
        aippPara_ = nullptr;
        GlobalMockObject::verify();
    }

    void InitAippPara(uint32_t batchCount = 1)
    {
        aippPara_ = CreateAIPPPara(batchCount);
    }

private:
    std::shared_ptr<IAIPPPara> aippPara_ = nullptr;
};

/*
 * 测试用例标题：CreateAIPPPara_fail
 * 测试用例描述：测试CreateAIPPPara接口,通过指定Batch数初始化AippPara对象
 * 预置条件：batchCount为0
 * 操作步骤：
 *           1. 调用CreateAIPPPara接口获取
 *           2. 检查结果
 * 预期结果：
 *          1.CreateAIPPPara接口返回nullptr
 */
TEST_F(AippPara_v2_ut, CreateAIPPPara_fail)
{
    uint32_t batchCount = 0;
    std::shared_ptr<IAIPPPara> aippPara = nullptr;
    aippPara = CreateAIPPPara(batchCount);
    EXPECT_EQ(aippPara, nullptr);
}

/*
 * 测试用例标题：SetInputIndex_GetInputIndex_001
 * 测试用例描述：测试SetInputIndex接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputIndex接口设置输入index
 *           2. 调用GetInputIndex接口获取index
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputIndex接口返回AI_SUCCESS
 *          2.GetInputIndex接口返回结果与输入index一致
 */
TEST_F(AippPara_v2_ut, SetInputIndex_GetInputIndex_001)
{
    int32_t INPUT_INDEX = 0;
    AIStatus ret = aippPara_->SetInputIndex(INPUT_INDEX);
    EXPECT_EQ(AI_SUCCESS, ret);

    ret = aippPara_->GetInputIndex();
    EXPECT_EQ(INPUT_INDEX, ret);
}

/*
 * 测试用例标题：SetInputIndex_GetInputIndex_001
 * 测试用例描述：测试SetInputIndex接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初未始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputIndex接口设置输入index
 *           2. 调用GetInputIndex接口获取index
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputIndex接口返回AI_FAILED
 *          2.GetInputIndex接口返回结果与输入index不一致
 */
TEST_F(AippPara_v2_ut, SetInputIndex_GetInputIndex_fail_001)
{
    int32_t INPUT_INDEX = 1;
    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetInputIndex(INPUT_INDEX);
    EXPECT_EQ(AI_FAILED, ret);

    ret = aippPara_->GetInputIndex();
    EXPECT_NE(INPUT_INDEX, ret);
}

/*
 * 测试用例标题：GetBatchCount_001
 * 测试用例描述：测试GetBatchCount接口
 * 预置条件：
 * 操作步骤：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 *           2. 调用GetBatchCount接口
 *           3. 检查结果
 * 预期结果：
 *          1.GetBatchCount接口返回初始化时的Batch数
 */
TEST_F(AippPara_v2_ut, GetBatchCount_001)
{
    int32_t BATCH_COUTN = 2;
    InitAippPara(BATCH_COUTN);
    int batchCount = aippPara_->GetBatchCount();
    EXPECT_EQ(BATCH_COUTN, batchCount);
}

/*
 * 测试用例标题：SetInputAippIndex_GetInputAippIndex_001
 * 测试用例描述：测试SetInputAippIndex接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputAippIndex接口设置输入index
 *           2. 调用GetInputAippIndex接口获取index
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputAippIndex接口返回AI_SUCCESS
 *          2.GetInputAippIndex接口返回结果与输入index一致
 */
TEST_F(AippPara_v2_ut, SetInputAippIndex_GetInputAippIndex_001)
{
    int32_t INPUT_INDEX = 2;
    AIStatus ret = aippPara_->SetInputAippIndex(INPUT_INDEX);
    EXPECT_EQ(AI_SUCCESS, ret);

    ret = aippPara_->GetInputAippIndex();
    EXPECT_EQ(INPUT_INDEX, ret);
}

/*
 * 测试用例标题：SetInputAippIndex_GetInputAippIndex_fail_001
 * 测试用例描述：测试SetInputAippIndex接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初未始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputAippIndex接口设置输入index
 *           2. 调用GetInputAippIndex接口获取index
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputAippIndex接口返回AI_FAILED
 *          2.GetInputAippIndex接口返回结果与输入index不一致
 */
TEST_F(AippPara_v2_ut, SetInputAippIndex_GetInputAippIndex_fail_001)
{
    int32_t INPUT_INDEX = 2;
    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetInputAippIndex(INPUT_INDEX);
    EXPECT_EQ(AI_FAILED, ret);

    ret = aippPara->GetInputAippIndex();
    EXPECT_NE(INPUT_INDEX, ret);
}

/*
 * 测试用例标题：SetInputShape_GetInputShape_001
 * 测试用例描述：测试SetInputShape接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置输入AippInputShape
 *           2. 调用SetInputShape接口设置输入AippInputShape
 *           3. 调用GetInputShape接口获取AippInputShape
 *           4. 检查结果
 * 预期结果：
 *          1.SetInputAippIndex接口返回AI_SUCCESS
 *          2.GetInputAippIndex接口返回结果与输入AippInputShape一致
 */
TEST_F(AippPara_v2_ut, SetInputShape_GetInputShape_001)
{
    constexpr uint32_t IMG_WIDTH = 100;
    constexpr uint32_t IMG_HEIGHT = 200;
    std::vector<int32_t> inShape;
    inShape.push_back(IMG_WIDTH);
    inShape.push_back(IMG_HEIGHT);

    AIStatus ret = aippPara_->SetInputShape(inShape);
    EXPECT_EQ(AI_SUCCESS, ret);

    std::vector<int32_t> shape = aippPara_->GetInputShape();
    EXPECT_EQ(IMG_WIDTH, shape[0]);
    EXPECT_EQ(IMG_HEIGHT, shape[1]);
}

/*
 * 测试用例标题：SetInputShape_GetInputShape_fail_001
 * 测试用例描述：测试SetInputShape接口
 * 预置条件：
 *           1. 创建并通过指定Batch数未初始化AippPara对象
 * 操作步骤：
 *           1. 设置输入AippInputShape
 *           2. 调用SetInputShape接口设置输入AippInputShape
 *           3. 调用GetInputShape接口获取AippInputShape
 *           4. 检查结果
 * 预期结果：
 *          1.SetInputAippIndex接口返回AI_FAILED
 *          2.GetInputAippIndex接口返回结果与输入AippInputShape不一致
 */
TEST_F(AippPara_v2_ut, SetInputShape_GetInputShape_fail_001)
{
    constexpr uint32_t IMG_WIDTH = 100;
    constexpr uint32_t IMG_HEIGHT = 200;
    std::vector<int32_t> inShape;
    inShape.push_back(IMG_WIDTH);
    inShape.push_back(IMG_HEIGHT);

    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetInputShape(inShape);
    EXPECT_EQ(AI_FAILED, ret);

    std::vector<int32_t> shape = aippPara_->GetInputShape();
    EXPECT_NE(IMG_WIDTH, shape[0]);
    EXPECT_NE(IMG_HEIGHT, shape[1]);
}

/*
 * 测试用例标题：SetInputFormat_GetInputFormat_001
 * 测试用例描述：测试SetInputFormat接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputFormat接口设置输入图片类型
 *           2. 调用GetInputFormat接口获取输入图片类型
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputFormat接口返回AI_SUCCESS
 *          2.GetInputFormat接口返回结果与输入图片类型一致
 */
TEST_F(AippPara_v2_ut, SetInputFormat_GetInputFormat_001)
{
    AIStatus ret = aippPara_->SetInputFormat(ImageFormat::YUV420SP);
    EXPECT_EQ(AI_SUCCESS, ret);
    ImageFormat format = aippPara_->GetInputFormat();
    EXPECT_EQ(ImageFormat::YUV420SP, format);
}

/*
 * 测试用例标题：SetInputFormat_GetInputFormat_fail_001
 * 测试用例描述：测试SetInputFormat接口
 * 预置条件：ImageFormat非法
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputFormat接口设置输入图片类型
 *           2. 调用GetInputFormat接口获取输入图片类型
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputFormat接口返回AI_FAILED
 *          2.GetInputFormat接口返回结果与输入图片类型一致
 */
TEST_F(AippPara_v2_ut, SetInputFormat_GetInputFormat_fail_001)
{
    AIStatus ret = aippPara_->SetInputFormat(ImageFormat::INVALID);
    EXPECT_EQ(AI_FAILED, ret);
    ImageFormat format = aippPara_->GetInputFormat();
    EXPECT_EQ(ImageFormat::INVALID, format);
}

/*
 * 测试用例标题：SetInputFormat_GetInputFormat_fail_002
 * 测试用例描述：测试SetInputFormat接口
 * 预置条件：
 *           1. 创建并通过指定Batch数未初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetInputFormat接口设置输入图片类型
 *           2. 调用GetInputFormat接口获取输入图片类型
 *           3. 检查结果
 * 预期结果：
 *          1.SetInputFormat接口返回AI_FAILED
 *          2.GetInputFormat接口返回结果与输入图片类型不一致
 */
TEST_F(AippPara_v2_ut, SetInputFormat_GetInputFormat_fail_002)
{
    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetInputFormat(ImageFormat::YUV420SP);
    EXPECT_EQ(AI_FAILED, ret);
    ImageFormat format = aippPara->GetInputFormat();
    EXPECT_EQ(ImageFormat::INVALID, format);
}

/**
 * 测试用例描述：根据InputFormat，ImageType设置CscPara
 **/
TEST_F(AippPara_v2_ut, AippPara_SetCscPara_InputFormat_ImageType_01)
{
    ImageFormat inputFormat = ImageFormat::YUYV;
    ImageColorSpace imageType = ImageColorSpace::BT_601_NARROW;
    AIStatus ret = aippPara_->SetInputFormat(inputFormat);
    EXPECT_EQ(AI_SUCCESS, ret);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_succ_001 start----\n");
    inputFormat = ImageFormat::RGB888;
    imageType = ImageColorSpace::BT_601_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_EQ(AI_SUCCESS, ret);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_succ_002 start----\n");
    inputFormat = ImageFormat::BGR888;
    imageType = ImageColorSpace::BT_601_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_SUCCESS);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_succ_003 start----\n");
    inputFormat = ImageFormat::YUV400;
    imageType = ImageColorSpace::JPEG;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_SUCCESS);

    inputFormat = ImageFormat::XRGB8888;
    ret = aippPara_->SetInputFormat(inputFormat);
    ASSERT_TRUE(ret == AI_SUCCESS);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_succ_004 start----\n");
    inputFormat = ImageFormat::YUV444SP;
    imageType = ImageColorSpace::BT_601_FULL;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_SUCCESS);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_succ_005 start----\n");
    inputFormat = ImageFormat::YVU444SP;
    imageType = ImageColorSpace::BT_709_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_SUCCESS);
    printf("---AippPara_SetCscPara_InputFormat_ImageType_fail_001 start----\n");
    inputFormat = static_cast<ImageFormat>(-1);
    imageType = ImageColorSpace::BT_709_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_FAILED);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_fail_002 start----\n");
    inputFormat = ImageFormat::RGB888;
    imageType = ImageColorSpace::BT_709_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_FAILED);

    printf("---AippPara_SetCscPara_InputFormat_ImageType_fail_003 start----\n");
    inputFormat = ImageFormat::INVALID;
    imageType = ImageColorSpace::BT_601_NARROW;
    ret = aippPara_->SetCscPara(inputFormat, imageType);
    EXPECT_TRUE(ret == AI_FAILED);
}

/*
 * 测试用例标题：SetCscPara_With_Format_ColorSpace_GetCscPara_fail_001
 * 测试用例描述：测试SetInputFormat接口
 * 预置条件：ImageFormat 不在YVU444SP, YUV444SP, RGB888, BGR888, YUV400 范围内
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetCscPara接口设置csc转换参数
 *           2. 调用GetCscPara接口获取csc转换参数
 *           3. 检查结果
 * 预期结果：
 *          1.SetCscPara接口返回AI_FAILED
 */
TEST_F(AippPara_v2_ut, SetCscPara_With_Format_ColorSpace_GetCscPara_fail_001)
{
    AIStatus ret = aippPara_->SetCscPara(ImageFormat::RGB888, ImageColorSpace::JPEG);
    EXPECT_EQ(AI_FAILED, ret);
}

/*
 * 测试用例标题：SetCscPara_With_Format_ColorSpace_GetCscPara_fail_002
 * 测试用例描述：测试SetInputFormat接口
 * 预置条件：ImageFormat非法
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 调用SetCscPara接口设置csc转换参数
 *           2. 调用GetCscPara接口获取csc转换参数
 *           3. 检查结果
 * 预期结果：
 *          1.SetCscPara接口返回AI_FAILED
 */
TEST_F(AippPara_v2_ut, SetCscPara_With_Format_ColorSpace_GetCscPara_fail_002)
{
    AIStatus ret = aippPara_->SetCscPara(ImageFormat::INVALID, ImageColorSpace::JPEG);
    EXPECT_EQ(AI_FAILED, ret);
}

/*
 * 测试用例标题：SetChannelSwapPara_GetChannelSwapPara_001
 * 测试用例描述：测试SetChannelSwapPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置ChannelSwapPara
 *           2. 调用SetChannelSwapPara接口设置通道交换参数
 *           3. 调用GetChannelSwapPara接口获取通道交换参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetChannelSwapPara接口返回AI_SUCCESS
 *          2.GetChannelSwapPara接口返回结果与输入参数一致
 */
TEST_F(AippPara_v2_ut, SetChannelSwapPara_GetChannelSwapPara_001)
{
    ChannelSwapPara para;
    para.rbuvSwapSwitch = false;
    para.axSwapSwitch = true;

    AIStatus ret = aippPara_->SetChannelSwapPara(std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);

    ChannelSwapPara rpara = aippPara_->GetChannelSwapPara();
    EXPECT_EQ(para.rbuvSwapSwitch, rpara.rbuvSwapSwitch);
    EXPECT_EQ(para.axSwapSwitch, rpara.axSwapSwitch);
}

/*
 * 测试用例标题：SetChannelSwapPara_GetChannelSwapPara_fail_002
 * 测试用例描述：测试SetChannelSwapPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数未初始化AippPara对象
 * 操作步骤：
 *           1. 设置ChannelSwapPara
 *           2. 调用SetChannelSwapPara接口设置通道交换参数
 *           3. 调用GetChannelSwapPara接口获取通道交换参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetChannelSwapPara接口返回AI_FAILED
 *          2.GetChannelSwapPara接口返回结果与输入参数不一致
 */
TEST_F(AippPara_v2_ut, SetChannelSwapPara_GetChannelSwapPara_fail_002)
{
    ChannelSwapPara para;
    para.rbuvSwapSwitch = false;
    para.axSwapSwitch = true;

    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetChannelSwapPara(std::move(para));
    EXPECT_EQ(AI_FAILED, ret);

    ChannelSwapPara rpara = aippPara->GetChannelSwapPara();
    EXPECT_NE(para.axSwapSwitch, rpara.axSwapSwitch);
}

/*
 * 测试用例标题：SetCropPara_GetCropPara_001
 * 测试用例描述：测试SetCropPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置CropPara
 *           2. 调用SetCropPara接口设置CropPara参数
 *           3. 调用GetCropPara接口获取CropPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetCropPara接口返回AI_SUCCESS
 *          2.GetCropPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetCropPara_GetCropPara_001)
{
    CropPara para;
    para.cropStartPosW = 10;
    para.cropStartPosH = 10;
    para.cropSizeW = 100;
    para.cropSizeH = 200;

    AIStatus ret = aippPara_->SetCropPara(0, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    CropPara rpara = aippPara_->GetCropPara(0);

    EXPECT_EQ(para.cropStartPosW, rpara.cropStartPosW);
    EXPECT_EQ(para.cropStartPosH, rpara.cropStartPosH);
    EXPECT_EQ(para.cropSizeW, rpara.cropSizeW);
    EXPECT_EQ(para.cropSizeH, rpara.cropSizeH);
}

/*
 * 测试用例标题：SetCropPara_GetCropPara_002
 * 测试用例描述：测试SetCropPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置CropPara
 *           2. 调用SetCropPara接口设置CropPara参数
 *           3. 调用GetCropPara接口获取CropPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetCropPara接口返回AI_SUCCESS
 *          2.GetCropPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetCropPara_GetCropPara_002)
{
    InitAippPara(2);
    CropPara para;
    para.cropStartPosW = 10;
    para.cropStartPosH = 10;
    para.cropSizeW = 100;
    para.cropSizeH = 200;

    AIStatus ret = aippPara_->SetCropPara(1, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    CropPara rpara = aippPara_->GetCropPara(1);

    EXPECT_EQ(para.cropStartPosW, rpara.cropStartPosW);
    EXPECT_EQ(para.cropStartPosH, rpara.cropStartPosH);
    EXPECT_EQ(para.cropSizeW, rpara.cropSizeW);
    EXPECT_EQ(para.cropSizeH, rpara.cropSizeH);
}

/*
 * 测试用例标题：SetCropPara_GetCropPara_fail_002
 * 测试用例描述：测试SetCropPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数未初始化AippPara对象
 * 操作步骤：
 *           1. 设置CropPara
 *           2. 调用SetCropPara接口设置CropPara参数
 *           3. 调用GetCropPara接口获取CropPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetCropPara接口返回AI_SUCCESS
 *          2.GetCropPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetCropPara_GetCropPara_fail_002)
{
    CropPara para;
    para.cropStartPosW = 10;
    para.cropStartPosH = 10;
    para.cropSizeW = 100;
    para.cropSizeH = 200;

    std::shared_ptr<AIPPParaImpl> aippPara = std::make_shared<AIPPParaImpl>();
    AIStatus ret = aippPara->SetCropPara(0, std::move(para));
    EXPECT_EQ(AI_FAILED, ret);
    CropPara rpara = aippPara->GetCropPara(0);

    EXPECT_NE(para.cropStartPosW, rpara.cropStartPosW);
    EXPECT_NE(para.cropStartPosH, rpara.cropStartPosH);
    EXPECT_NE(para.cropSizeW, rpara.cropSizeW);
    EXPECT_NE(para.cropSizeH, rpara.cropSizeH);
}

/*
 * 测试用例标题：SetResizePara_GetResizePara_001
 * 测试用例描述：测试SetResizePara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置ResizePara
 *           2. 调用SetResizePara接口设置ResizePara参数
 *           3. 调用GetResizePara接口获取ResizePara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetResizePara接口返回AI_SUCCESS
 *          2.GetResizePara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetResizePara_GetResizePara_001)
{
    int32_t BATCH_COUTN = 0;
    ResizePara para;
    para.resizeOutputSizeW = 100;
    para.resizeOutputSizeH = 100;
    AIStatus ret = aippPara_->SetResizePara(BATCH_COUTN, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    ResizePara rpara = aippPara_->GetResizePara(BATCH_COUTN);

    EXPECT_EQ(para.resizeOutputSizeW, rpara.resizeOutputSizeW);
    EXPECT_EQ(para.resizeOutputSizeH, rpara.resizeOutputSizeH);
}

/*
 * 测试用例标题：SetResizePara_GetResizePara_01_OK
 * 测试用例描述：测试SetResizePara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置ResizePara
 *           2. 调用SetResizePara接口为特定下标的batch设置ResizePara参数
 *           3. 调用GetResizePara接口获取ResizePara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetResizePara接口返回AI_SUCCESS
 *          2.GetResizePara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetResizePara_GetResizePara_002)
{
    InitAippPara(2);
    ResizePara para;
    para.resizeOutputSizeW = 100;
    para.resizeOutputSizeH = 100;

    AIStatus ret = aippPara_->SetResizePara(1, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    ResizePara rpara = aippPara_->GetResizePara(1);

    EXPECT_EQ(para.resizeOutputSizeW, rpara.resizeOutputSizeW);
    EXPECT_EQ(para.resizeOutputSizeH, rpara.resizeOutputSizeH);
}

/*
 * 测试用例标题：SetPaddingPara_GetPaddingPara_001
 * 测试用例描述：测试SetPaddingPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置PadPara
 *           2. 调用SetResizePara接口设置PadPara参数
 *           3. 调用GetResizePara接口获取PadPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetPaddingPara接口返回AI_SUCCESS
 *          2.GetPaddingPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetPaddingPara_GetPaddingPara_001)
{
    PadPara para;
    para.paddingSizeTop = 10;
    para.paddingSizeBottom = 20;
    para.paddingSizeLeft = 30;
    para.paddingSizeRight = 40;
    para.paddingValueChn0 = 40.0;
    para.paddingValueChn1 = 50.0;
    para.paddingValueChn2 = 60.0;
    para.paddingValueChn3 = 70.0;

    int32_t INPUT_INDEX = 0;
    AIStatus ret = aippPara_->SetPaddingPara(INPUT_INDEX, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    PadPara rpara = aippPara_->GetPaddingPara(INPUT_INDEX);

    EXPECT_EQ(para.paddingSizeTop, rpara.paddingSizeTop);
    EXPECT_EQ(para.paddingSizeBottom, rpara.paddingSizeBottom);
    EXPECT_EQ(para.paddingSizeLeft, rpara.paddingSizeLeft);
    EXPECT_EQ(para.paddingSizeRight, rpara.paddingSizeRight);

    EXPECT_EQ(para.paddingValueChn0, rpara.paddingValueChn0);
    EXPECT_EQ(para.paddingValueChn1, rpara.paddingValueChn1);
    EXPECT_EQ(para.paddingValueChn2, rpara.paddingValueChn2);
    EXPECT_EQ(para.paddingValueChn3, rpara.paddingValueChn3);
}

/*
 * 测试用例标题：SetPaddingPara_GetPaddingPara_002
 * 测试用例描述：测试SetPaddingPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置PadPara
 *           2. 调用SetPaddingPara接口为特定下标的batch设置PadPara参数
 *           3. 调用GetPaddingPara接口获取PadPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetPaddingPara接口返回AI_SUCCESS
 *          2.GetPaddingPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetPaddingPara_GetPaddingPara_002)
{
    InitAippPara(2);
    PadPara para;
    para.paddingSizeTop = 10;
    para.paddingSizeBottom = 20;
    para.paddingSizeLeft = 30;
    para.paddingSizeRight = 40;
    para.paddingValueChn0 = 40.0;
    para.paddingValueChn1 = 50.0;
    para.paddingValueChn2 = 60.0;
    para.paddingValueChn3 = 70.0;

    AIStatus ret = aippPara_->SetPaddingPara(1, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    PadPara rpara = aippPara_->GetPaddingPara(1);

    EXPECT_EQ(para.paddingSizeTop, rpara.paddingSizeTop);
    EXPECT_EQ(para.paddingSizeBottom, rpara.paddingSizeBottom);
    EXPECT_EQ(para.paddingSizeLeft, rpara.paddingSizeLeft);
    EXPECT_EQ(para.paddingSizeRight, rpara.paddingSizeRight);

    EXPECT_EQ(para.paddingValueChn0, rpara.paddingValueChn0);
    EXPECT_EQ(para.paddingValueChn1, rpara.paddingValueChn1);
    EXPECT_EQ(para.paddingValueChn2, rpara.paddingValueChn2);
    EXPECT_EQ(para.paddingValueChn3, rpara.paddingValueChn3);
}

/*
 * 测试用例标题：SetDtcPara_GetDtcPara_001
 * 测试用例描述：测试SetDtcPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置DtcPara
 *           2. 调用SetDtcPara接口设置DtcPara参数
 *           3. 调用GetDtcPara接口获取DtcPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetDtcPara接口返回AI_SUCCESS
 *          2.GetDtcPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetDtcPara_GetDtcPara_001)
{
    DtcPara para;
    para.pixelMeanChn0 = 1;
    para.pixelMeanChn1 = 2;
    para.pixelMeanChn2 = 3;
    para.pixelMeanChn3 = 4;
    para.pixelMinChn0 = 5;
    para.pixelMinChn1 = 6;
    para.pixelMinChn2 = 7;
    para.pixelMinChn3 = 8;
    para.pixelVarReciChn0 = 1.0;
    para.pixelVarReciChn1 = 2.0;
    para.pixelVarReciChn2 = 3.0;
    para.pixelVarReciChn3 = 4.0;

    int32_t INPUT_INDEX = 0;

    AIStatus ret = aippPara_->SetDtcPara(INPUT_INDEX, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    DtcPara rpara = aippPara_->GetDtcPara(INPUT_INDEX);

    EXPECT_EQ(para.pixelMeanChn0, rpara.pixelMeanChn0);
    EXPECT_EQ(para.pixelMeanChn1, rpara.pixelMeanChn1);
    EXPECT_EQ(para.pixelMeanChn2, rpara.pixelMeanChn2);
    EXPECT_EQ(para.pixelMeanChn3, rpara.pixelMeanChn3);
    EXPECT_EQ(para.pixelMinChn0, rpara.pixelMinChn0);
    EXPECT_EQ(para.pixelMinChn1, rpara.pixelMinChn1);
    EXPECT_EQ(para.pixelMinChn2, rpara.pixelMinChn2);
    EXPECT_EQ(para.pixelMinChn3, rpara.pixelMinChn3);
    EXPECT_EQ(para.pixelVarReciChn0, rpara.pixelVarReciChn0);
    EXPECT_EQ(para.pixelVarReciChn1, rpara.pixelVarReciChn1);
    EXPECT_EQ(para.pixelVarReciChn2, rpara.pixelVarReciChn2);
    EXPECT_EQ(para.pixelVarReciChn3, rpara.pixelVarReciChn3);
}

/*
 * 测试用例标题：SetDtcPara_GetDtcPara_002
 * 测试用例描述：测试SetDtcPara接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           1. 设置DtcPara
 *           2. 调用SetDtcPara接口为特定下标的batch设置DtcPara参数
 *           3. 调用GetDtcPara接口获取DtcPara参数
 *           4. 检查结果
 * 预期结果：
 *          1.SetDtcPara接口返回AI_SUCCESS
 *          2.GetDtcPara接口返回结果与设置参数一致
 */
TEST_F(AippPara_v2_ut, SetDtcPara_GetDtcPara_002)
{
    InitAippPara(2);
    DtcPara para;
    para.pixelMeanChn0 = 1;
    para.pixelMeanChn1 = 2;
    para.pixelMeanChn2 = 3;
    para.pixelMeanChn3 = 4;
    para.pixelMinChn0 = 5;
    para.pixelMinChn1 = 6;
    para.pixelMinChn2 = 7;
    para.pixelMinChn3 = 8;
    para.pixelVarReciChn0 = 1.0;
    para.pixelVarReciChn1 = 2.0;
    para.pixelVarReciChn2 = 3.0;
    para.pixelVarReciChn3 = 4.0;

    AIStatus ret = aippPara_->SetDtcPara(1, std::move(para));
    EXPECT_EQ(AI_SUCCESS, ret);
    DtcPara rpara = aippPara_->GetDtcPara(1);

    EXPECT_EQ(para.pixelMeanChn0, rpara.pixelMeanChn0);
    EXPECT_EQ(para.pixelMeanChn1, rpara.pixelMeanChn1);
    EXPECT_EQ(para.pixelMeanChn2, rpara.pixelMeanChn2);
    EXPECT_EQ(para.pixelMeanChn3, rpara.pixelMeanChn3);
    EXPECT_EQ(para.pixelMinChn0, rpara.pixelMinChn0);
    EXPECT_EQ(para.pixelMinChn1, rpara.pixelMinChn1);
    EXPECT_EQ(para.pixelMinChn2, rpara.pixelMinChn2);
    EXPECT_EQ(para.pixelMinChn3, rpara.pixelMinChn3);
    EXPECT_EQ(para.pixelVarReciChn0, rpara.pixelVarReciChn0);
    EXPECT_EQ(para.pixelVarReciChn1, rpara.pixelVarReciChn1);
    EXPECT_EQ(para.pixelVarReciChn2, rpara.pixelVarReciChn2);
    EXPECT_EQ(para.pixelVarReciChn3, rpara.pixelVarReciChn3);
}

/*
 * 测试用例标题：GetData_001
 * 测试用例描述：测试GetData_001接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           2. 调用GetData接口获取data
 *           3. 检查结果
 * 预期结果：
 *          1.GetData接口返回不为nullptr
 */
TEST_F(AippPara_v2_ut, GetData_001)
{
    auto ret = aippPara_->GetData();
    EXPECT_NE(nullptr, ret);
}

/*
 * 测试用例标题：GetSize_001
 * 测试用例描述：测试GetSize_001接口
 * 预置条件：
 *           1. 创建并通过指定Batch数初始化AippPara对象
 * 操作步骤：
 *           2. 调用GetSize接口获取size
 *           3. 检查结果
 * 预期结果：
 *          1.GetSize接口返回不为0
 */
TEST_F(AippPara_v2_ut, GetSize_001)
{
    auto ret = aippPara_->GetSize();
    EXPECT_NE(0, ret);
}