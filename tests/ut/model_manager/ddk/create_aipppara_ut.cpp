/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: aipppara Unit Test
 */
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include <limits.h>
#include <memory>

#include "tensor/aipp/hiai_tensor_aipp_para_def.h"
#include "tensor/aipp_para.h"
#include "tensor/image_tensor_buffer.h"
#include "framework/compatible/HiAiModelManagerType.h"

using namespace std;
using namespace hiai;

class CreateAippParaUTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }

    void SetUp()
    {
        aippPara = CreateAIPPPara(numberBatchCount);
        ASSERT_NE(nullptr, aippPara);
        const uint32_t n = 2;
        const uint32_t c = 2;
        const uint32_t h = 100;
        const uint32_t w = 200;
        const int32_t b = 3;
        const int32_t rotation = 2;
        ImageFormat format = ImageFormat::YUYV;
        ImageColorSpace colorSpace = ImageColorSpace::BT_601_NARROW;
        imageBuffer = CreateImageTensorBuffer(b, h, w, format, colorSpace, rotation);
        ASSERT_TRUE(imageBuffer != nullptr);
        ASSERT_TRUE(imageBuffer->GetData() != nullptr);
        ASSERT_TRUE(imageBuffer->GetSize() != 0);
    }

    void TearDown()
    {
        aippPara = nullptr;
    }

public:
    AIStatus status = false;
    uint32_t batchCount = 0;
    uint32_t inputAippIndex = 0;
    std::shared_ptr<IAIPPPara> aippPara = nullptr;
    std::shared_ptr<IImageTensorBuffer> imageBuffer = nullptr;
    const uint32_t numberBatchCount = 3;
};

/**
 * 测试用例描述：设置InputIndex
 **/
TEST_F(CreateAippParaUTest, AippPara_SetInputIndex)
{
    printf("---AippPara_SetInputIndex_succ_001 start----\n");
    batchCount = 2;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputIndex_succ_002 start----\n");
    batchCount = 1;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputIndex_succ_003 start----\n");
    batchCount = 0;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetBatchCount_succ_001 start----\n");
    uint32_t batchCountAipp = aippPara->GetBatchCount();
    EXPECT_TRUE(batchCountAipp == numberBatchCount);

    printf("---AippPara_SetInputIndex_succ_004 start----\n");
    batchCount = batchCountAipp - 2;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputIndex_succ_005 start----\n");
    batchCount = batchCountAipp - 1;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);
}

/**
 * 测试用例描述：获取InputAippIndex
 **/
TEST_F(CreateAippParaUTest, AippPara_GetInputIndex)
{
    printf("---AippPara_GetInputIndex_succ_001 start----\n");
    batchCount = 2;
    status = aippPara->SetInputIndex(batchCount);
    EXPECT_TRUE(status == AI_SUCCESS);
    uint32_t index = aippPara->GetInputIndex();
    EXPECT_TRUE(index == batchCount);
}

/**
 * 测试用例描述：设置InputAippIndex
 **/
TEST_F(CreateAippParaUTest, AippPara_SetInputAippIndex)
{
    printf("---AippPara_SetInputAippIndex_succ_001 start----\n");
    inputAippIndex = 2;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputAippIndex_succ_002 start----\n");
    inputAippIndex = 1;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputAippIndex_succ_003 start----\n");
    inputAippIndex = 0;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputAippIndex_succ_004 start----\n");
    uint32_t batchCountAipp = aippPara->GetBatchCount();
    EXPECT_TRUE(batchCountAipp == numberBatchCount);
    inputAippIndex = batchCountAipp - 2;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetInputAippIndex_succ_005 start----\n");
    inputAippIndex = batchCountAipp - 1;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);
}

/**
 * 测试用例描述：获取InputAippIndex
 **/
TEST_F(CreateAippParaUTest, AippPara_GetInputAippIndex)
{
    printf("---AippPara_GetInputAippIndex_succ_001 start----\n");
    inputAippIndex = 2;
    status = aippPara->SetInputAippIndex(inputAippIndex);
    EXPECT_TRUE(status == AI_SUCCESS);
    uint32_t index = aippPara->GetInputAippIndex();
    EXPECT_TRUE(index == inputAippIndex);
}

/**
 * 测试用例描述：根据AIPPCscPara设置CscPara，获取CscPara
 **/
TEST_F(CreateAippParaUTest, AippPara_SetCscPara_AIPPCscPara)
{
    printf("---AippPara_SetCscPara_AIPPCscPara_succ_001 start----\n");
    CscMatrixPara aippCscPara;
    aippCscPara.matrixR0C0 = 10;
    aippCscPara.matrixR0C1 = 11;
    aippCscPara.matrixR0C2 = 12;
    aippCscPara.matrixR1C0 = 20;
    aippCscPara.matrixR1C1 = 21;
    aippCscPara.matrixR1C2 = 22;
    aippCscPara.matrixR2C0 = 30;
    aippCscPara.matrixR2C1 = 31;
    aippCscPara.matrixR2C2 = 32;
    aippCscPara.outputBias0 = 40;
    aippCscPara.outputBias1 = 41;
    aippCscPara.outputBias2 = 42;
    aippCscPara.inputBias0 = 50;
    aippCscPara.inputBias1 = 51;
    aippCscPara.inputBias2 = 52;
    status = aippPara->SetCscPara(aippCscPara);
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetCscPara_succ_001 start----\n");
    CscMatrixPara para = aippPara->GetCscPara();
    EXPECT_TRUE(para.matrixR0C0 == aippCscPara.matrixR0C0);
    EXPECT_TRUE(para.matrixR0C1 == aippCscPara.matrixR0C1);
    EXPECT_TRUE(para.matrixR0C2 == aippCscPara.matrixR0C2);
    EXPECT_TRUE(para.matrixR1C0 == aippCscPara.matrixR1C0);
    EXPECT_TRUE(para.matrixR1C1 == aippCscPara.matrixR1C1);
    EXPECT_TRUE(para.matrixR1C2 == aippCscPara.matrixR1C2);
    EXPECT_TRUE(para.matrixR2C0 == aippCscPara.matrixR2C0);
    EXPECT_TRUE(para.matrixR2C1 == aippCscPara.matrixR2C1);
    EXPECT_TRUE(para.matrixR2C2 == aippCscPara.matrixR2C2);
    EXPECT_TRUE(para.outputBias0 == aippCscPara.outputBias0);
    EXPECT_TRUE(para.outputBias1 == aippCscPara.outputBias1);
    EXPECT_TRUE(para.outputBias2 == aippCscPara.outputBias2);
    EXPECT_TRUE(para.inputBias0 == aippCscPara.inputBias0);
    EXPECT_TRUE(para.inputBias1 == aippCscPara.inputBias1);
    EXPECT_TRUE(para.inputBias2 == aippCscPara.inputBias2);
}

/**
 * 测试用例描述：设置ChannelSwapPara，获取设置ChannelSwapPara
 **/
TEST_F(CreateAippParaUTest, AippPara_ChannelSwapPara)
{
    printf("---AippPara_SetChannelSwapPara_succ_001 start----\n");
    ChannelSwapPara channelSwapPara;
    channelSwapPara.rbuvSwapSwitch = true;
    channelSwapPara.axSwapSwitch = true;
    status = aippPara->SetChannelSwapPara(std::move(channelSwapPara));
    EXPECT_TRUE(status == AI_SUCCESS);
    printf("---AippPara_SetChannelSwapPara_succ_002 start----\n");
    channelSwapPara.rbuvSwapSwitch = false;
    channelSwapPara.axSwapSwitch = false;
    status = aippPara->SetChannelSwapPara(std::move(channelSwapPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetChannelSwapPara_succ_003 start----\n");
    channelSwapPara.rbuvSwapSwitch = true;
    channelSwapPara.axSwapSwitch = false;
    status = aippPara->SetChannelSwapPara(std::move(channelSwapPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetChannelSwapPara_succ_004 start----\n");
    channelSwapPara.rbuvSwapSwitch = false;
    channelSwapPara.axSwapSwitch = true;
    status = aippPara->SetChannelSwapPara(std::move(channelSwapPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetChannelSwapPara_succ_001 start----\n");
    ChannelSwapPara para = aippPara->GetChannelSwapPara();
    EXPECT_TRUE(para.rbuvSwapSwitch == channelSwapPara.rbuvSwapSwitch);
    EXPECT_TRUE(para.axSwapSwitch == channelSwapPara.axSwapSwitch);
}

/**
 * 测试用例描述：根据batchIndex设置PaddingPara，获取PaddingPara
 **/
TEST_F(CreateAippParaUTest, AippPara_SetPaddingPara_batchIndex)
{
    PadPara paddingPara;
    paddingPara.paddingSizeTop = 16;
    paddingPara.paddingSizeBottom = 16;
    paddingPara.paddingSizeLeft = 16;
    paddingPara.paddingSizeRight = 16;
    uint32_t batchIndex = 0;

    printf("---AippPara_SetPaddingPara_batchIndex_succ_001 start----\n");
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetPaddingPara_batchIndex_succ_001 start----\n");
    PadPara para = aippPara->GetPaddingPara(batchIndex);
    EXPECT_TRUE(para.paddingSizeTop == paddingPara.paddingSizeTop);
    EXPECT_TRUE(para.paddingSizeBottom == paddingPara.paddingSizeBottom);
    EXPECT_TRUE(para.paddingSizeLeft == paddingPara.paddingSizeLeft);
    EXPECT_TRUE(para.paddingSizeRight == paddingPara.paddingSizeRight);

    printf("---AippPara_SetPaddingPara_batchIndex_succ_002 start----\n");
    batchIndex = 1;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetPaddingPara_batchIndex_succ_003 start----\n");
    paddingPara.paddingSizeTop = UINT_MAX;
    paddingPara.paddingSizeBottom = UINT_MAX;
    paddingPara.paddingSizeLeft = UINT_MAX;
    paddingPara.paddingSizeRight = UINT_MAX;
    batchIndex = numberBatchCount - 2;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetPaddingPara_batchIndex_succ_004 start----\n");
    paddingPara.paddingSizeTop = UINT_MAX + 1;
    paddingPara.paddingSizeBottom = UINT_MAX + 1;
    paddingPara.paddingSizeLeft = UINT_MAX + 1;
    paddingPara.paddingSizeRight = UINT_MAX + 1;
    batchIndex = numberBatchCount - 1;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetPaddingPara_batchIndex_succ_005 start----\n");
    paddingPara.paddingSizeTop = 1;
    paddingPara.paddingSizeBottom = 1;
    paddingPara.paddingSizeLeft = 1;
    paddingPara.paddingSizeRight = 1;
    batchIndex = -1;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetPaddingPara_batchIndex_succ_006 start----\n");
    paddingPara.paddingSizeTop = 1;
    paddingPara.paddingSizeBottom = 1;
    paddingPara.paddingSizeLeft = 1;
    paddingPara.paddingSizeRight = 1;
    batchIndex = -1;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetPaddingPara_batchIndex_fail_001 start----\n");
    paddingPara.paddingSizeTop = 16;
    paddingPara.paddingSizeBottom = 16;
    paddingPara.paddingSizeLeft = 16;
    paddingPara.paddingSizeRight = 16;
    batchIndex = -2;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_FAILED);

    printf("---AippPara_SetPaddingPara_batchIndex_fail_002 start----\n");
    paddingPara.paddingSizeTop = 16;
    paddingPara.paddingSizeBottom = 16;
    paddingPara.paddingSizeLeft = 16;
    paddingPara.paddingSizeRight = 16;
    batchIndex = numberBatchCount;
    status = aippPara->SetPaddingPara(batchIndex, std::move(paddingPara));
    EXPECT_TRUE(status == AI_FAILED);
}

/**
 * 测试用例描述：根据batchIndex设置ResizePara，获取ResizePara
 **/
TEST_F(CreateAippParaUTest, AippPara_ResizePara_batchIndex)
{
    ResizePara resizePara;
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    uint32_t batchIndex = 2;

    printf("---AippPara_SetResizePara_batchIndex_succ_001 start----\n");
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetResizePara_batchIndex_succ_001 start----\n");
    ResizePara para = aippPara->GetResizePara(batchIndex);
    EXPECT_TRUE(para.resizeOutputSizeW == resizePara.resizeOutputSizeW);
    EXPECT_TRUE(para.resizeOutputSizeH == resizePara.resizeOutputSizeH);

    printf("---AippPara_SetResizePara_batchIndex_succ_002 start----\n");
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    batchIndex = -1;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetResizePara_batchIndex_succ_003 start----\n");
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    batchIndex = 0;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetResizePara_batchIndex_succ_004 start----\n");
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    batchIndex = numberBatchCount - 1;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetResizePara_batchIndex_fail_001 start----\n");
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    batchIndex = -2;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_FAILED);

    printf("---AippPara_SetResizePara_batchIndex_fail_002 start----\n");
    resizePara.resizeOutputSizeW = 300;
    resizePara.resizeOutputSizeH = 400;
    batchIndex = numberBatchCount;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_FAILED);
}

/**
 * 测试用例描述：根据输入的batchCount，init创建AippPara成功
 **/
TEST_F(CreateAippParaUTest, AippPara_SetCropPara_batchIndex)
{
    printf("---AippPara_SetCropPara_batchIndex_succ_001 start----\n");
    CropPara cropPara;
    cropPara.cropStartPosW = 100;
    cropPara.cropStartPosH = 100;
    cropPara.cropSizeW = 300;
    cropPara.cropSizeH = 500;
    uint32_t batchIndex = 2;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetCropPara_batchIndex_succ_001 start----\n");
    CropPara para = aippPara->GetCropPara(batchIndex);
    EXPECT_TRUE(para.cropStartPosW == cropPara.cropStartPosW);
    EXPECT_TRUE(para.cropStartPosH == cropPara.cropStartPosH);
    EXPECT_TRUE(para.cropSizeW == cropPara.cropSizeW);
    EXPECT_TRUE(para.cropSizeH == cropPara.cropSizeH);

    printf("---AippPara_SetCropPara_batchIndex_succ_002 start----\n");
    batchIndex = 2;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetCropPara_batchIndex_succ_003 start----\n");
    batchIndex = -1;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetCropPara_batchIndex_succ_004 start----\n");
    batchIndex = numberBatchCount - 1;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetCropPara_batchIndex_fail_001 start----\n");
    batchIndex = numberBatchCount;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_FAILED);

    printf("---AippPara_SetCropPara_batchIndex_fail_002 start----\n");
    batchIndex = -2;
    status = aippPara->SetCropPara(batchIndex, std::move(cropPara));
    EXPECT_TRUE(status == AI_FAILED);
}

/**
 * 测试用例描述：设置ResizePara，获取ResizePara
 **/
TEST_F(CreateAippParaUTest, AippPara_ResizePara)
{
    ResizePara resizePara;
    resizePara.resizeOutputSizeW = 100;
    resizePara.resizeOutputSizeH = 200;

    printf("---AippPara_SetResizePara_succ_001 start----\n");
    uint32_t batchIndex = 2;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetResizePara_succ_001 start----\n");
    ResizePara para = aippPara->GetResizePara(batchIndex);
    EXPECT_TRUE(para.resizeOutputSizeW == resizePara.resizeOutputSizeW);
    EXPECT_TRUE(para.resizeOutputSizeH == resizePara.resizeOutputSizeH);

    printf("---AippPara_SetResizePara_succ_002 start----\n");
    batchIndex = -1;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetResizePara_succ_003 start----\n");
    batchIndex = 0;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetResizePara_fail_001 start----\n");
    batchIndex = numberBatchCount;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_FAILED);

    printf("---AippPara_SetResizePara_fail_002 start----\n");
    batchIndex = -2;
    status = aippPara->SetResizePara(batchIndex, std::move(resizePara));
    EXPECT_TRUE(status == AI_FAILED);
}

/**
 * 测试用例描述：创建DtcPara
 **/
TEST_F(CreateAippParaUTest, AippPara_DtcPara)
{
    DtcPara dtcPara;
    dtcPara.pixelMeanChn0 = 30;
    dtcPara.pixelMeanChn1 = 31;
    dtcPara.pixelMeanChn2 = 32;
    dtcPara.pixelMeanChn3 = 33;
    dtcPara.pixelMinChn0 = 10;
    dtcPara.pixelMinChn1 = 11;
    dtcPara.pixelMinChn2 = 12;
    dtcPara.pixelMinChn3 = 13;
    dtcPara.pixelVarReciChn0 = 21.0;
    dtcPara.pixelVarReciChn1 = 21.1;
    dtcPara.pixelVarReciChn2 = 21.2;
    dtcPara.pixelVarReciChn3 = 21.3;

    printf("---AippPara_SetDtcPara_succ_002 start----\n");
    uint32_t batchIndex = 0;
    status = aippPara->SetDtcPara(batchIndex, std::move(dtcPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetDtcPara_succ_003 start----\n");
    batchIndex = -1;
    status = aippPara->SetDtcPara(batchIndex, std::move(dtcPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_SetDtcPara_succ_004 start----\n");
    batchIndex = numberBatchCount - 1;
    status = aippPara->SetDtcPara(batchIndex, std::move(dtcPara));
    EXPECT_TRUE(status == AI_SUCCESS);

    printf("---AippPara_GetDtcPara_succ_001 start-----\n");
    DtcPara para = aippPara->GetDtcPara(batchIndex);
    EXPECT_TRUE(para.pixelMeanChn0 == dtcPara.pixelMeanChn0);
    EXPECT_TRUE(para.pixelMeanChn1 == dtcPara.pixelMeanChn1);
    EXPECT_TRUE(para.pixelMeanChn2 == dtcPara.pixelMeanChn2);
    EXPECT_TRUE(para.pixelMeanChn3 == dtcPara.pixelMeanChn3);
    EXPECT_TRUE(para.pixelMinChn0 == static_cast<uint16_t>(dtcPara.pixelMinChn0) * 1.0f);
    EXPECT_TRUE(para.pixelMinChn1 == static_cast<uint16_t>(dtcPara.pixelMinChn1) * 1.0f);
    EXPECT_TRUE(para.pixelMinChn2 == static_cast<uint16_t>(dtcPara.pixelMinChn2) * 1.0f);
    EXPECT_TRUE(para.pixelMinChn3 == static_cast<uint16_t>(dtcPara.pixelMinChn3) * 1.0f);
    EXPECT_TRUE(para.pixelVarReciChn0 == static_cast<uint16_t>(dtcPara.pixelVarReciChn0) * 1.0f);
    EXPECT_TRUE(para.pixelVarReciChn1 == static_cast<uint16_t>(dtcPara.pixelVarReciChn1) * 1.0f);
    EXPECT_TRUE(para.pixelVarReciChn2 == static_cast<uint16_t>(dtcPara.pixelVarReciChn2) * 1.0f);
    EXPECT_TRUE(para.pixelVarReciChn3 == static_cast<uint16_t>(dtcPara.pixelVarReciChn3) * 1.0f);

    printf("---AippPara_SetDtcPara_fail_001 start----\n");
    batchIndex = numberBatchCount;
    status = aippPara->SetDtcPara(batchIndex, std::move(dtcPara));
    EXPECT_TRUE(status == AI_FAILED);

    printf("---AippPara_SetDtcPara_fail_002 start----\n");
    batchIndex = -2;
    status = aippPara->SetDtcPara(batchIndex, std::move(dtcPara));
    EXPECT_TRUE(status == AI_FAILED);
}
