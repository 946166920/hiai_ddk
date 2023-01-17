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
#include "compatible/HiAiAippPara.h"

#include <set>
#include <map>
#include <math.h>
#include <vector>

#include "tensor/aipp/aipp_para_impl.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {

using namespace std;

AippPara::AippPara()
{
}

AippPara::~AippPara()
{
    if (aippParaBase_ != nullptr) {
        aippParaBase_.reset();
        aippParaBase_ = nullptr;
    }
}

AIStatus AippPara::Init(uint32_t batchCount)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr) {
        FMK_LOGE("AippPara is already inited!.");
        return AI_FAILED;
    }
    aippParaBase_ = CreateAIPPPara(batchCount);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("Init failed.");
        return AI_FAILED;
    }
    return AI_SUCCESS;
}

uint32_t AippPara::GetBatchCount()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return 0;
    }
    return aippParaBase_->GetBatchCount();
}

AIStatus AippPara::SetInputIndex(uint32_t inputIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetInputIndex(inputIndex);
}

int32_t AippPara::GetInputIndex()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return 0;
    }
    return aippParaBase_->GetInputIndex();
}

AIStatus AippPara::SetInputAippIndex(uint32_t inputAippIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetInputAippIndex(inputAippIndex);
}

int32_t AippPara::GetInputAippIndex()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return 0;
    }
    return aippParaBase_->GetInputAippIndex();
}

AIStatus AippPara::SetInputShape(AippInputShape inputShape)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    vector<int32_t> shape;
    shape.push_back(inputShape.srcImageSizeW);
    shape.push_back(inputShape.srcImageSizeH);
    return aippParaBase_->SetInputShape(shape);
}

AippInputShape AippPara::GetInputShape()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    AippInputShape inputShape;
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return inputShape;
    }
    vector<int32_t> shape = aippParaBase_->GetInputShape();
    if (shape.size() < 2) {
        FMK_LOGE("invalid shape size %zu", shape.size());
        return inputShape;
    }

    inputShape.srcImageSizeW = static_cast<uint32_t>(shape[0]);
    inputShape.srcImageSizeH = static_cast<uint32_t>(shape[1]);
    return inputShape;
}

enum CceAippInputFormat {
    CCE_YUV420SP_U8 = 1,
    CCE_XRGB8888_U8,
    CCE_NC1HWC0DI_FP16,
    CCE_NC1HWC0DI_S8,
    CCE_RGB888_U8,
    CCE_ARGB8888_U8,
    CCE_YUYV_U8,
    CCE_YUV422SP_U8,
    CCE_AYUV444_U8,
    CCE_YUV400_U8,
    CCE_RESERVED
};

AIStatus AippPara::SetInputFormat(AiTensorImage_Format inputFormat)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    // RGB888_U8 is not supported as a valid inputFormat on lite
    static std::map<AiTensorImage_Format, CceAippInputFormat> inputFormatMap = {
        {AiTensorImage_YUV420SP_U8, CCE_YUV420SP_U8},
        {AiTensorImage_XRGB8888_U8, CCE_XRGB8888_U8},
        {AiTensorImage_ARGB8888_U8, CCE_ARGB8888_U8},
        {AiTensorImage_YUYV_U8, CCE_YUYV_U8},
        {AiTensorImage_YUV422SP_U8, CCE_YUV422SP_U8},
        {AiTensorImage_AYUV444_U8, CCE_AYUV444_U8},
        {AiTensorImage_YUV400_U8, CCE_YUV400_U8},
        {AiTensorImage_RGB888_U8, CCE_RGB888_U8},
    };
    if (inputFormatMap.find(inputFormat) == inputFormatMap.end()) {
        FMK_LOGE("SetInputFormat failed, inputFormat: %d is not supported on Lite currently", inputFormat);
        return AI_FAILED;
    }

    return aippParaBase_->SetInputFormat(static_cast<ImageFormat>(inputFormat));
}

AiTensorImage_Format AippPara::GetInputFormat()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AiTensorImage_Format::AiTensorImage_INVALID;
    }
    ImageFormat imgFormat = aippParaBase_->GetInputFormat();
    return static_cast<AiTensorImage_Format>(imgFormat);
}

static CscMatrixPara ConvertCscPara2CscMatrixPara(AippCscPara& cscPara)
{
    CscMatrixPara cscMatrixPara;
    cscMatrixPara.matrixR0C0 = cscPara.matrixR0C0;
    cscMatrixPara.matrixR0C1 = cscPara.matrixR0C1;
    cscMatrixPara.matrixR0C2 = cscPara.matrixR0C2;
    cscMatrixPara.matrixR1C0 = cscPara.matrixR1C0;
    cscMatrixPara.matrixR1C1 = cscPara.matrixR1C1;
    cscMatrixPara.matrixR1C2 = cscPara.matrixR1C2;
    cscMatrixPara.matrixR2C0 = cscPara.matrixR2C0;
    cscMatrixPara.matrixR2C1 = cscPara.matrixR2C1;
    cscMatrixPara.matrixR2C2 = cscPara.matrixR2C2;
    cscMatrixPara.outputBias0 = cscPara.outputBias0;
    cscMatrixPara.outputBias1 = cscPara.outputBias1;
    cscMatrixPara.outputBias2 = cscPara.outputBias2;
    cscMatrixPara.inputBias0 = cscPara.inputBias0;
    cscMatrixPara.inputBias1 = cscPara.inputBias1;
    cscMatrixPara.inputBias2 = cscPara.inputBias2;
    return cscMatrixPara;
}

static AippCscPara ConvertCscMatrixPara2CscPara(CscMatrixPara& cscMatrixPara, bool cscSwitch)
{
    AippCscPara cscPara;
    cscPara.matrixR0C0 = cscMatrixPara.matrixR0C0;
    cscPara.matrixR0C1 = cscMatrixPara.matrixR0C1;
    cscPara.matrixR0C2 = cscMatrixPara.matrixR0C2;
    cscPara.matrixR1C0 = cscMatrixPara.matrixR1C0;
    cscPara.matrixR1C1 = cscMatrixPara.matrixR1C1;
    cscPara.matrixR1C2 = cscMatrixPara.matrixR1C2;
    cscPara.matrixR2C0 = cscMatrixPara.matrixR2C0;
    cscPara.matrixR2C1 = cscMatrixPara.matrixR2C1;
    cscPara.matrixR2C2 = cscMatrixPara.matrixR2C2;
    cscPara.outputBias0 = cscMatrixPara.outputBias0;
    cscPara.outputBias1 = cscMatrixPara.outputBias1;
    cscPara.outputBias2 = cscMatrixPara.outputBias2;
    cscPara.inputBias0 = cscMatrixPara.inputBias0;
    cscPara.inputBias1 = cscMatrixPara.inputBias1;
    cscPara.inputBias2 = cscMatrixPara.inputBias2;
    cscPara.switch_ = cscSwitch;

    return cscPara;
}

AIStatus AippPara::SetCscPara(AiTensorImage_Format targetFormat, ImageType imageType)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetCscPara(static_cast<ImageFormat>(targetFormat), static_cast<ImageColorSpace>(imageType));
}

AIStatus AippPara::SetCscPara(AippCscPara cscPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && cscPara.switch_) {
        return aippParaBase_->SetCscPara(ConvertCscPara2CscMatrixPara(cscPara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AippCscPara AippPara::GetCscPara()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    CscMatrixPara cscMatrixPara;
    bool cscSwitch = false;
    if (aippParaBase_ != nullptr) {
        cscMatrixPara = aippParaBase_->GetCscPara();
        auto aippParaImpl = std::dynamic_pointer_cast<AIPPParaImpl>(aippParaBase_);
        cscSwitch = aippParaImpl->GetEnableCsc();
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertCscMatrixPara2CscPara(cscMatrixPara, cscSwitch);
}

static ChannelSwapPara ConvertAippChannelSwapPara2ChannelSwapPara(AippChannelSwapPara& aippChannelSwapPara)
{
    ChannelSwapPara channelSwapPara;
    channelSwapPara.rbuvSwapSwitch = aippChannelSwapPara.rbuvSwapSwitch;
    channelSwapPara.axSwapSwitch = aippChannelSwapPara.axSwapSwitch;
    return channelSwapPara;
}

static AippChannelSwapPara ConvertChannelSwapPara2AippChannelSwapPara(ChannelSwapPara& channelSwapPara)
{
    AippChannelSwapPara aippChannelSwapPara;

    aippChannelSwapPara.rbuvSwapSwitch = channelSwapPara.rbuvSwapSwitch;
    aippChannelSwapPara.axSwapSwitch = channelSwapPara.axSwapSwitch;
    return aippChannelSwapPara;
}

AIStatus AippPara::SetChannelSwapPara(AippChannelSwapPara channelSwapPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetChannelSwapPara(ConvertAippChannelSwapPara2ChannelSwapPara(channelSwapPara));
}

AippChannelSwapPara AippPara::GetChannelSwapPara()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    ChannelSwapPara para;
    if (aippParaBase_ != nullptr) {
        para = aippParaBase_->GetChannelSwapPara();
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertChannelSwapPara2AippChannelSwapPara(para);
}

static CropPara ConvertAippCropPara2CropPara(AippCropPara& aippCropPara)
{
    CropPara cropPara;
    cropPara.cropStartPosW = aippCropPara.cropStartPosW;
    cropPara.cropStartPosH = aippCropPara.cropStartPosH;
    cropPara.cropSizeW = aippCropPara.cropSizeW;
    cropPara.cropSizeH = aippCropPara.cropSizeH;

    return cropPara;
}

static AippCropPara ConvertCropPara2AippCropPara(CropPara& cropPara, bool cropSwitch)
{
    AippCropPara aippCropPara;
    aippCropPara.switch_ = cropSwitch;
    aippCropPara.cropStartPosW = cropPara.cropStartPosW;
    aippCropPara.cropStartPosH = cropPara.cropStartPosH;
    aippCropPara.cropSizeW = cropPara.cropSizeW;
    aippCropPara.cropSizeH = cropPara.cropSizeH;

    return aippCropPara;
}

AIStatus AippPara::SetCropPara(uint32_t batchIndex, AippCropPara cropPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && cropPara.switch_) {
        return aippParaBase_->SetCropPara(batchIndex, ConvertAippCropPara2CropPara(cropPara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AIStatus AippPara::SetCropPara(AippCropPara cropPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && cropPara.switch_) {
        return aippParaBase_->SetCropPara(ConvertAippCropPara2CropPara(cropPara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AippCropPara AippPara::GetCropPara(uint32_t batchIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    CropPara para;
    bool cropSwitch = false;
    if (aippParaBase_ != nullptr) {
        para = aippParaBase_->GetCropPara(batchIndex);
        auto aippParaImpl = std::dynamic_pointer_cast<AIPPParaImpl>(aippParaBase_);
        cropSwitch = aippParaImpl->GetEnableCrop(batchIndex);
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertCropPara2AippCropPara(para, cropSwitch);
}

static ResizePara ConvertAippResizePara2ResizePara(AippResizePara& aippResizePara)
{
    ResizePara resizePara;
    resizePara.resizeOutputSizeW = aippResizePara.resizeOutputSizeW;
    resizePara.resizeOutputSizeH = aippResizePara.resizeOutputSizeH;

    return resizePara;
}

static AippResizePara ConvertResizePara2AippResizePara(ResizePara& resizePara, bool resizeSwitch)
{
    AippResizePara aippResizePara;

    aippResizePara.switch_ = resizeSwitch;
    aippResizePara.resizeOutputSizeW = resizePara.resizeOutputSizeW;
    aippResizePara.resizeOutputSizeH = resizePara.resizeOutputSizeH;

    return aippResizePara;
}

AIStatus AippPara::SetResizePara(uint32_t batchIndex, AippResizePara resizePara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && resizePara.switch_) {
        return aippParaBase_->SetResizePara(batchIndex, ConvertAippResizePara2ResizePara(resizePara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AIStatus AippPara::SetResizePara(AippResizePara resizePara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && resizePara.switch_) {
        return aippParaBase_->SetResizePara(ConvertAippResizePara2ResizePara(resizePara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AippResizePara AippPara::GetResizePara(uint32_t batchIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    ResizePara para;
    bool resizeSwitch = false;
    if (aippParaBase_ != nullptr) {
        para = aippParaBase_->GetResizePara(batchIndex);
        auto aippParaImpl = std::dynamic_pointer_cast<AIPPParaImpl>(aippParaBase_);
        resizeSwitch = aippParaImpl->GetEnableResize(batchIndex);
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertResizePara2AippResizePara(para, resizeSwitch);
}

static PadPara ConvertAippPaddingPara2PaddingPara(AippPaddingPara& aippPaddingPara)
{
    PadPara padPara;
    padPara.paddingSizeTop = aippPaddingPara.paddingSizeTop;
    padPara.paddingSizeBottom = aippPaddingPara.paddingSizeBottom;
    padPara.paddingSizeLeft = aippPaddingPara.paddingSizeLeft;
    padPara.paddingSizeRight = aippPaddingPara.paddingSizeRight;

    padPara.paddingValueChn0 = aippPaddingPara.paddingValueChn0;
    padPara.paddingValueChn1 = aippPaddingPara.paddingValueChn1;
    padPara.paddingValueChn2 = aippPaddingPara.paddingValueChn2;
    padPara.paddingValueChn3 = aippPaddingPara.paddingValueChn3;

    return padPara;
}

static AippPaddingPara ConvertPaddingPara2AippPaddingPara(PadPara& padPara, bool paddingSwitch)
{
    AippPaddingPara aippPaddingPara;

    aippPaddingPara.paddingSizeTop = padPara.paddingSizeTop;
    aippPaddingPara.paddingSizeBottom = padPara.paddingSizeBottom;
    aippPaddingPara.paddingSizeLeft = padPara.paddingSizeLeft;
    aippPaddingPara.paddingSizeRight = padPara.paddingSizeRight;

    aippPaddingPara.paddingValueChn0 = padPara.paddingValueChn0;
    aippPaddingPara.paddingValueChn1 = padPara.paddingValueChn1;
    aippPaddingPara.paddingValueChn2 = padPara.paddingValueChn2;
    aippPaddingPara.paddingValueChn3 = padPara.paddingValueChn3;
    aippPaddingPara.switch_ = paddingSwitch;

    return aippPaddingPara;
}

AIStatus AippPara::SetPaddingPara(uint32_t batchIndex, AippPaddingPara paddingPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && paddingPara.switch_) {
        return aippParaBase_->SetPaddingPara(batchIndex, ConvertAippPaddingPara2PaddingPara(paddingPara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AIStatus AippPara::SetPaddingPara(AippPaddingPara paddingPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ != nullptr && paddingPara.switch_) {
        return aippParaBase_->SetPaddingPara(ConvertAippPaddingPara2PaddingPara(paddingPara));
    }
    FMK_LOGE("AippPara is not inited or switch is false!.");
    return AI_FAILED;
}

AippPaddingPara AippPara::GetPaddingPara(uint32_t batchIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    PadPara para;
    bool paddingSwitch = false;
    if (aippParaBase_ != nullptr) {
        para = aippParaBase_->GetPaddingPara(batchIndex);
        auto aippParaImpl = std::dynamic_pointer_cast<AIPPParaImpl>(aippParaBase_);
        paddingSwitch = aippParaImpl->GetEnablePadding(batchIndex);
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertPaddingPara2AippPaddingPara(para, paddingSwitch);
}

static DtcPara ConvertAippDtcPara2DtcPara(AippDtcPara& aippDtcPara)
{
    DtcPara dtcPara;
    dtcPara.pixelMeanChn0 = aippDtcPara.pixelMeanChn0;
    dtcPara.pixelMeanChn1 = aippDtcPara.pixelMeanChn1;
    dtcPara.pixelMeanChn2 = aippDtcPara.pixelMeanChn2;
    dtcPara.pixelMeanChn3 = aippDtcPara.pixelMeanChn3;
    dtcPara.pixelMinChn0 = aippDtcPara.pixelMinChn0;
    dtcPara.pixelMinChn1 = aippDtcPara.pixelMinChn1;
    dtcPara.pixelMinChn2 = aippDtcPara.pixelMinChn2;
    dtcPara.pixelMinChn3 = aippDtcPara.pixelMinChn3;
    dtcPara.pixelVarReciChn0 = aippDtcPara.pixelVarReciChn0;
    dtcPara.pixelVarReciChn1 = aippDtcPara.pixelVarReciChn1;
    dtcPara.pixelVarReciChn2 = aippDtcPara.pixelVarReciChn2;
    dtcPara.pixelVarReciChn3 = aippDtcPara.pixelVarReciChn3;
    return dtcPara;
}

static AippDtcPara ConvertDtcPara2AippDtcPara(DtcPara& dtcPara)
{
    AippDtcPara aippDtcPara;

    aippDtcPara.pixelMeanChn0 = dtcPara.pixelMeanChn0;
    aippDtcPara.pixelMeanChn1 = dtcPara.pixelMeanChn1;
    aippDtcPara.pixelMeanChn2 = dtcPara.pixelMeanChn2;
    aippDtcPara.pixelMeanChn3 = dtcPara.pixelMeanChn3;
    aippDtcPara.pixelMinChn0 = dtcPara.pixelMinChn0;
    aippDtcPara.pixelMinChn1 = dtcPara.pixelMinChn1;
    aippDtcPara.pixelMinChn2 = dtcPara.pixelMinChn2;
    aippDtcPara.pixelMinChn3 = dtcPara.pixelMinChn3;
    aippDtcPara.pixelVarReciChn0 = dtcPara.pixelVarReciChn0;
    aippDtcPara.pixelVarReciChn1 = dtcPara.pixelVarReciChn1;
    aippDtcPara.pixelVarReciChn2 = dtcPara.pixelVarReciChn2;
    aippDtcPara.pixelVarReciChn3 = dtcPara.pixelVarReciChn3;
    return aippDtcPara;
}

AIStatus AippPara::SetDtcPara(uint32_t batchIndex, AippDtcPara dtcPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetDtcPara(batchIndex, ConvertAippDtcPara2DtcPara(dtcPara));
}

AIStatus AippPara::SetDtcPara(AippDtcPara dtcPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (aippParaBase_ == nullptr) {
        FMK_LOGE("AippPara is not inited!.");
        return AI_FAILED;
    }
    return aippParaBase_->SetDtcPara(ConvertAippDtcPara2DtcPara(dtcPara));
}

AippDtcPara AippPara::GetDtcPara(uint32_t batchIndex)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    DtcPara para;
    if (aippParaBase_ != nullptr) {
        para = aippParaBase_->GetDtcPara(batchIndex);
    } else {
        FMK_LOGE("AippPara is not inited!.");
    }

    return ConvertDtcPara2AippDtcPara(para);
}

std::shared_ptr<IAIPPPara> AippPara::GetAIPPPara()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return aippParaBase_;
}

void AippPara::SetAIPPPara(std::shared_ptr<IAIPPPara>& para)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    aippParaBase_ = para;
}

} // namespace hiai
