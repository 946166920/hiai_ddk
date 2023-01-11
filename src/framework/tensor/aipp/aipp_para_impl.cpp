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
#include "aipp_para_impl.h"

#include <map>
#include <set>
#include <string>

#include "framework/infra/log/log.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

namespace {

using namespace hiai;
using namespace std;
const float kFloat16Max = 65504;
const float kFloat16Lowest = -kFloat16Max;
const uint8_t maxBatchNum = 127;

const static int YUV_TO_RGB[4][3][3] = {{{256, 0, 359}, {256, -88, -183}, {256, 454, 0}},
    {{298, 0, 409}, {298, -100, -208}, {298, 516, 0}}, {{256, 0, 359}, {256, -88, -183}, {256, 454, 0}},
    {{298, 0, 460}, {298, -55, -137}, {298, 541, 0}}};
const static int RGB_TO_YUV[4][3][3] = {{{77, 150, 29}, {-43, -85, 128}, {128, -107, -21}},
    {{66, 129, 25}, {-38, -74, 112}, {112, -94, -18}}, {{77, 150, 29}, {-43, -85, 128}, {128, -107, -21}},
    {{47, 157, 16}, {-26, -87, 112}, {112, -102, -10}}};
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

static Status CheckBatchNum(uint8_t batchNum)
{
    if (batchNum < 1 || batchNum > maxBatchNum) {
        FMK_LOGE("batchNum is invalid,batchNum should in [1,%d],get:%d", maxBatchNum, batchNum);
        return FAILURE;
    }
    return SUCCESS;
}

static Status CheckBatchIndex(uint32_t batchIndex, uint32_t batchNum)
{
    if (batchIndex >= batchNum) {
        FMK_LOGE("batchIndex [%zu] is not supported.", batchIndex);
        return FAILURE;
    }
    return SUCCESS;
}

static _Float16 SaveUint16ToFp16(uint16_t num)
{
    _Float16* uintAddr = nullptr;
    uintAddr = reinterpret_cast<_Float16*>(&num);
    return *uintAddr;
}

static const string& FormatToStr(ImageFormat format)
{
    const static map<ImageFormat, string> strMap = {
        {ImageFormat::YUV420SP, "YUV420SP_U8"},
        {ImageFormat::XRGB8888, "XRGB8888_U8"},
        {ImageFormat::YUV400, "YUV400_U8"},
        {ImageFormat::ARGB8888, "ARGB8888_U8"},
        {ImageFormat::YUYV, "YUYV_U8"},
        {ImageFormat::YUV422SP, "YUV422SP_U8"},
        {ImageFormat::AYUV444, "AYUV444_U8"},
        {ImageFormat::RGB888, "RGB888_U8"},
        {ImageFormat::BGR888, "BGR888_U8"},
        {ImageFormat::YUV444SP, "YUV444SP_U8"},
        {ImageFormat::YVU444SP, "YVU444SP_U8"},
    };
    const static string undefinedStr = "undefined";
    if (strMap.find(format) != strMap.end()) {
        return strMap.at(format);
    }
    return undefinedStr;
}

static Status InitCscMatrixToRGB(ImageFormat inputFormat, ImageFormat targetFormat, vector<int32_t>& cscValues,
    vector<uint8_t>& inputBiasValues, ImageColorSpace imageType)
{
    if (inputFormat == ImageFormat::XRGB8888 || inputFormat == ImageFormat::RGB888 ||
        inputFormat == ImageFormat::ARGB8888 || inputFormat == ImageFormat::BGR888 ||
        inputFormat == ImageFormat::YUV400) {
        FMK_LOGE("Set SetCscPara failed, can not convert from %s image to %s by CSC", FormatToStr(inputFormat).c_str(),
            FormatToStr(targetFormat).c_str());
        return FAILURE;
    }
    const static map<ImageFormat, vector<uint32_t>> idxMap1 = {
        {ImageFormat::RGB888, {0, 1, 2}}, {ImageFormat::BGR888, {2, 1, 0}}};
    uint32_t step = 0;
    for (uint32_t idx : idxMap1.at(targetFormat)) {
        for (uint32_t j = 0; j < 3; ++j) {
            cscValues[step++] = YUV_TO_RGB[static_cast<uint32_t>(imageType)][idx][j];
        }
    }
    if (inputBiasValues.size() < 3) {
        FMK_LOGE("inputBiasValues size less than 3,get: %zu", inputBiasValues.size());
        return FAILURE;
    }
    inputBiasValues[0] = imageType == ImageColorSpace::JPEG ? 0 : 16;
    inputBiasValues[1] = 128;
    inputBiasValues[2] = 128;
    return SUCCESS;
}

static Status InitCscMatrixToYUV(ImageFormat inputFormat, ImageFormat targetFormat, vector<int32_t>& cscValues,
    vector<uint8_t>& outputBiasValues, ImageColorSpace imageType)
{
    if (inputFormat == ImageFormat::YUV420SP || inputFormat == ImageFormat::YUV422SP ||
        inputFormat == ImageFormat::YUYV || inputFormat == ImageFormat::AYUV444 || inputFormat == ImageFormat::YUV400) {
        FMK_LOGE("Set SetCscPara failed, can not convert from %s image to %s by CSC", FormatToStr(inputFormat).c_str(),
            FormatToStr(targetFormat).c_str());
        return FAILURE;
    }
    const static map<ImageFormat, vector<uint32_t>> idxMap2 = {
        {ImageFormat::YUV444SP, {0, 1, 2}}, {ImageFormat::YVU444SP, {0, 2, 1}}};
    uint32_t step = 0;
    for (uint32_t idx : idxMap2.at(targetFormat)) {
        for (uint32_t j = 0; j < 3; ++j) {
            cscValues[step++] = RGB_TO_YUV[static_cast<uint32_t>(imageType)][idx][j];
        }
    }
    if (outputBiasValues.size() < 3) {
        FMK_LOGE("outputBiasValues size less than 3,size:%zu", outputBiasValues.size());
        return FAILURE;
    }

    outputBiasValues[0] = imageType == ImageColorSpace::JPEG ? 0 : 16;
    outputBiasValues[1] = 128;
    outputBiasValues[2] = 128;
    return SUCCESS;
}

static Status InitCscMatrixToGray(ImageFormat inputFormat, vector<int32_t>& cscValues)
{
    if (inputFormat == ImageFormat::YUV400) {
        FMK_LOGE("Set SetCscPara failed, can not convert from %s image to YUV400_U8 by CSC",
            FormatToStr(inputFormat).c_str());
        return FAILURE;
    } else if (inputFormat == ImageFormat::YUV420SP || inputFormat == ImageFormat::YUV422SP ||
        inputFormat == ImageFormat::YUYV || inputFormat == ImageFormat::AYUV444) { // YUV images
        if (cscValues.empty()) {
            FMK_LOGE("cscValues can not be empty");
            return FAILURE;
        }
        cscValues[0] = 256;
    } else { // RGB images
        if (cscValues.size() < 3) {
            FMK_LOGE("cscValues size less than 3,get :%zu", cscValues.size());
            return FAILURE;
        }
        cscValues[0] = 76;
        cscValues[1] = 150;
        cscValues[2] = 30;
    }
    return SUCCESS;
}

static uint16_t SaveFp16ToUint16(_Float16 num)
{
    uint16_t* uintAddr = nullptr;
    uintAddr = reinterpret_cast<uint16_t*>(&num);
    return *uintAddr;
}

inline HIAI_MR_TensorAippBatchPara* GetBatchPara(HIAI_MR_TensorAippCommPara* commPara, uint32_t batchIndex)
{
    return reinterpret_cast<HIAI_MR_TensorAippBatchPara*>(reinterpret_cast<char*>(commPara) +
        sizeof(HIAI_MR_TensorAippCommPara) + sizeof(HIAI_MR_TensorAippBatchPara) * batchIndex);
}

static void UpdateCropPara(HIAI_MR_TensorAippCommPara* commPara, uint32_t batchIndex, const CropPara& cropPara)
{
    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(commPara, batchIndex);

    batchPara->cropSwitch = true;
    batchPara->cropStartPosW = cropPara.cropStartPosW;
    batchPara->cropStartPosH = cropPara.cropStartPosH;
    batchPara->cropSizeW = cropPara.cropSizeW;
    batchPara->cropSizeH = cropPara.cropSizeH;

    if (batchPara->scfSwitch != 0) {
        batchPara->scfInputSizeW = static_cast<int32_t>(batchPara->cropSizeW);
        batchPara->scfInputSizeH = static_cast<int32_t>(batchPara->cropSizeH);
    }
}

static void UpdateResizePara(HIAI_MR_TensorAippCommPara* commPara, uint32_t batchIndex, const ResizePara& resizePara)
{
    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(commPara, batchIndex);

    batchPara->scfSwitch = true;
    batchPara->scfInputSizeW =
        ((batchPara->cropSwitch != 0) ? static_cast<int32_t>(batchPara->cropSizeW) : commPara->srcImageSizeW);
    batchPara->scfInputSizeH =
        ((batchPara->cropSwitch != 0) ? static_cast<int32_t>(batchPara->cropSizeH) : commPara->srcImageSizeH);
    batchPara->scfOutputSizeW = resizePara.resizeOutputSizeW;
    batchPara->scfOutputSizeH = resizePara.resizeOutputSizeH;
}

static void UpdatePaddingPara(HIAI_MR_TensorAippCommPara* commPara, uint32_t batchIndex, const PadPara& paddingPara)
{
    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(commPara, batchIndex);

    batchPara->paddingSwitch = true;
    batchPara->paddingSizeTop = paddingPara.paddingSizeTop;
    batchPara->paddingSizeBottom = paddingPara.paddingSizeBottom;
    batchPara->paddingSizeLeft = paddingPara.paddingSizeLeft;
    batchPara->paddingSizeRight = paddingPara.paddingSizeRight;

    float value = std::max(std::min(paddingPara.paddingValueChn0, kFloat16Max), kFloat16Lowest);
    batchPara->paddingValueChn0 = SaveFp16ToUint16(value);
    value = std::max(std::min(paddingPara.paddingValueChn1, kFloat16Max), kFloat16Lowest);
    batchPara->paddingValueChn1 = SaveFp16ToUint16(value);
    value = std::max(std::min(paddingPara.paddingValueChn2, kFloat16Max), kFloat16Lowest);
    batchPara->paddingValueChn2 = SaveFp16ToUint16(value);
    value = std::max(std::min(paddingPara.paddingValueChn3, kFloat16Max), kFloat16Lowest);
    batchPara->paddingValueChn3 = SaveFp16ToUint16(value);
}

static void UpdateDtcPara(HIAI_MR_TensorAippCommPara* commPara, uint32_t batchIndex, const DtcPara& dtcPara)
{
    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(commPara, batchIndex);

    batchPara->dtcPixelMeanChn0 = dtcPara.pixelMeanChn0;
    batchPara->dtcPixelMeanChn1 = dtcPara.pixelMeanChn1;
    batchPara->dtcPixelMeanChn2 = dtcPara.pixelMeanChn2;
    batchPara->dtcPixelMeanChn3 = dtcPara.pixelMeanChn3;
    float value = std::max(std::min(dtcPara.pixelMinChn0, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelMinChn0 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelMinChn1, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelMinChn1 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelMinChn2, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelMinChn2 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelMinChn3, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelMinChn3 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelVarReciChn0, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelVarReciChn0 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelVarReciChn1, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelVarReciChn1 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelVarReciChn2, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelVarReciChn2 = SaveFp16ToUint16(value);
    value = std::max(std::min(dtcPara.pixelVarReciChn3, kFloat16Max), kFloat16Lowest);
    batchPara->dtcPixelVarReciChn3 = SaveFp16ToUint16(value);
}
} // namespace

namespace hiai {

HIAI_MR_TensorAippPara* AIPPParaBufferImpl::Init(uint32_t batchCount)
{
    return HIAI_MR_TensorAippPara_Create(batchCount);
}

Status AIPPParaBufferImpl::Release(HIAI_MR_TensorAippPara* paraBuff)
{
    HIAI_MR_TensorAippPara_Destroy(reinterpret_cast<HIAI_MR_TensorAippPara**>(&paraBuff));
    return SUCCESS;
}

void* AIPPParaBufferImpl::GetRawBuffer(HIAI_MR_TensorAippPara* paraBuff)
{
    return HIAI_MR_TensorAippPara_GetRawBuffer(paraBuff);
}

uint32_t AIPPParaBufferImpl::GetRawBufferSize(HIAI_MR_TensorAippPara* paraBuff)
{
    return HIAI_MR_TensorAippPara_GetRawBufferSize(paraBuff);
}

int AIPPParaBufferImpl::GetInputIndex(HIAI_MR_TensorAippPara* paraBuff)
{
    return HIAI_MR_TensorAippPara_GetInputIndex(paraBuff);
}

void AIPPParaBufferImpl::SetInputIndex(HIAI_MR_TensorAippPara* paraBuff, uint32_t inputIndex)
{
    HIAI_MR_TensorAippPara_SetInputIndex(paraBuff, inputIndex);
}

int AIPPParaBufferImpl::GetInputAippIndex(HIAI_MR_TensorAippPara* paraBuff)
{
    return HIAI_MR_TensorAippPara_GetInputAippIndex(paraBuff);
}

void AIPPParaBufferImpl::SetInputAippIndex(HIAI_MR_TensorAippPara* paraBuff, uint32_t inputAippIndex)
{
    HIAI_MR_TensorAippPara_SetInputAippIndex(paraBuff, inputAippIndex);
}

Status AIPPParaImpl::Init(uint32_t batchCount)
{
    if (paraBuff_ != nullptr) {
        FMK_LOGI("AIPPParaImpl is already inited!");
        return FAILURE;
    }

    HIAI_MR_TensorAippPara* paraBuff = AIPPParaBufferImpl::Init(batchCount);
    if (paraBuff == nullptr) {
        FMK_LOGE("Init error, AippPara is not inited!");
        return FAILURE;
    }
    // initial
    paraBuff_ = paraBuff;
    rawBuffer_ = AIPPParaBufferImpl::GetRawBuffer(paraBuff);
    if (InitAippPara(batchCount) != SUCCESS) {
        FMK_LOGE("Init error, InitAippPara is failed!");
        return FAILURE;
    }
    return SUCCESS;
}

Status AIPPParaImpl::Init(HIAI_MR_TensorAippPara* paraBuff)
{
    HIAI_EXPECT_TRUE(paraBuff_ == nullptr);
    HIAI_EXPECT_TRUE(rawBuffer_ == nullptr);
    HIAI_EXPECT_NOT_NULL(paraBuff);

    void* buffer = AIPPParaBufferImpl::GetRawBuffer(paraBuff);
    if (buffer == nullptr) {
        FMK_LOGE("Init error, paraBuff is not invailed!");
        return FAILURE;
    }

    paraBuff_ = paraBuff;
    rawBuffer_ = buffer;
    return SUCCESS;
}

AIPPParaImpl::~AIPPParaImpl()
{
    AIPPParaBufferImpl::Release(paraBuff_);

    paraBuff_ = nullptr;
    rawBuffer_ = nullptr;
}

TensorAippCommPara AIPPParaImpl::GetTensorAippCommPara()
{
    void* data = GetData();
    HIAI_EXPECT_NOT_NULL_R(data, std::make_pair(nullptr, 0));

    HIAI_MR_TensorAippCommPara* commPara = reinterpret_cast<HIAI_MR_TensorAippCommPara*>(data);
    HIAI_EXPECT_EXEC_R(CheckBatchNum(commPara->batchNum), std::make_pair(nullptr, 0));

    return std::make_pair(commPara, commPara->batchNum);
}

Status AIPPParaImpl::InitAippPara(uint32_t batchCount)
{
    if (batchCount < 1 || batchCount > 127) {
        return FAILURE;
    }

    void* data = GetData();
    HIAI_EXPECT_NOT_NULL(data);
    HIAI_MR_TensorAippCommPara* commPara = reinterpret_cast<HIAI_MR_TensorAippCommPara*>(data);
    HIAI_EXPECT_NOT_NULL(commPara);

    commPara->batchNum = batchCount;

    for (uint32_t batchIndex = 0; batchIndex < batchCount; ++batchIndex) {
        HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(commPara, batchIndex);
        batchPara->dtcPixelVarReciChn0 = SaveFp16ToUint16(1.0);
        batchPara->dtcPixelVarReciChn1 = SaveFp16ToUint16(1.0);
        batchPara->dtcPixelVarReciChn2 = SaveFp16ToUint16(1.0);
        batchPara->dtcPixelVarReciChn3 = SaveFp16ToUint16(1.0);
    }
    return SUCCESS;
}

void* AIPPParaImpl::GetData()
{
    return rawBuffer_;
}

size_t AIPPParaImpl::GetSize() const
{
    return AIPPParaBufferImpl::GetRawBufferSize(paraBuff_);
}

uint32_t AIPPParaImpl::GetBatchCount()
{
    return GetTensorAippCommPara().second;
}

Status AIPPParaImpl::SetInputIndex(uint32_t inputIndex)
{
    if (paraBuff_ == nullptr) {
        FMK_LOGE("SetInputIndex error, AippPara is not inited!");
        return FAILURE;
    }

    AIPPParaBufferImpl::SetInputIndex(paraBuff_, inputIndex);
    return SUCCESS;
}

int32_t AIPPParaImpl::GetInputIndex()
{
    if (paraBuff_ == nullptr) {
        FMK_LOGE("GetInputIndex error, AippPara is not inited!");
        return -1;
    }

    int32_t ret = AIPPParaBufferImpl::GetInputIndex(paraBuff_);
    if (ret == -1) {
        FMK_LOGE("GetInputIndex error, inner error occurred");
    }
    return ret;
}

Status AIPPParaImpl::SetInputAippIndex(uint32_t inputAippIndex)
{
    if (paraBuff_ == nullptr) {
        FMK_LOGE("SetInputAippIndex error, AippPara is not inited!");
        return FAILURE;
    }

    AIPPParaBufferImpl::SetInputAippIndex(paraBuff_, inputAippIndex);
    return SUCCESS;
}

int32_t AIPPParaImpl::GetInputAippIndex()
{
    if (paraBuff_ == nullptr) {
        FMK_LOGE("GetInputAippIndex error, AippPara is not inited!");
        return -1;
    }

    int32_t ret = AIPPParaBufferImpl::GetInputAippIndex(paraBuff_);
    if (ret == -1) {
        FMK_LOGE("GetInputAippIndex error, inner error occurred");
    }
    return ret;
}

Status AIPPParaImpl::SetCscPara(ImageFormat targetFormat, ImageColorSpace colorSpace)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL(para.first);

    ImageFormat inputFormat = GetInputFormat(para.first);
    if (inputFormat == ImageFormat::INVALID) {
        FMK_LOGE("inputFormat is Unknown, please set inputFormat first");
        return FAILURE;
    }

    std::set<ImageFormat> validFormat = {
        ImageFormat::YVU444SP, ImageFormat::YUV444SP, ImageFormat::RGB888, ImageFormat::BGR888, ImageFormat::YUV400};
    if (validFormat.find(targetFormat) == validFormat.end()) {
        FMK_LOGE(
            "targetFormat is invalid, valid targetFormat in range of [ YVU444SP, YUV444SP, RGB888, BGR888, YUV400 ]");
        return FAILURE;
    }

    vector<int32_t> cscValues(9, 0);
    vector<uint8_t> inputBiasValues(3, 0);
    vector<uint8_t> outputBiasValues(3, 0);
    Status ret = FAILURE;
    if (targetFormat == ImageFormat::RGB888 || targetFormat == ImageFormat::BGR888) {
        ret = InitCscMatrixToRGB(inputFormat, targetFormat, cscValues, inputBiasValues, colorSpace);
    } else if (targetFormat == ImageFormat::YUV444SP || targetFormat == ImageFormat::YVU444SP) {
        ret = InitCscMatrixToYUV(inputFormat, targetFormat, cscValues, outputBiasValues, colorSpace);
    } else {
        ret = InitCscMatrixToGray(inputFormat, cscValues);
    }
    if (ret != SUCCESS) {
        return ret;
    }

    para.first->cscSwitch = true;
    para.first->cscMatrixR0C0 = cscValues[0];
    para.first->cscMatrixR0C1 = cscValues[1];
    para.first->cscMatrixR0C2 = cscValues[2];
    para.first->cscMatrixR1C0 = cscValues[3];
    para.first->cscMatrixR1C1 = cscValues[4];
    para.first->cscMatrixR1C2 = cscValues[5];
    para.first->cscMatrixR2C0 = cscValues[6];
    para.first->cscMatrixR2C1 = cscValues[7];
    para.first->cscMatrixR2C2 = cscValues[8];
    para.first->cscInputBiasR0 = inputBiasValues[0];
    para.first->cscInputBiasR1 = inputBiasValues[1];
    para.first->cscInputBiasR2 = inputBiasValues[2];
    para.first->cscOutputBiasR0 = outputBiasValues[0];
    para.first->cscOutputBiasR1 = outputBiasValues[1];
    para.first->cscOutputBiasR2 = outputBiasValues[2];
    return SUCCESS;
}

Status AIPPParaImpl::SetCscPara(CscMatrixPara cscPara)
{
    HIAI_MR_TensorAippCommPara* commPara = GetTensorAippCommPara().first;
    HIAI_EXPECT_NOT_NULL(commPara);

    commPara->cscSwitch = true;
    commPara->cscMatrixR0C0 = cscPara.matrixR0C0;
    commPara->cscMatrixR0C1 = cscPara.matrixR0C1;
    commPara->cscMatrixR0C2 = cscPara.matrixR0C2;
    commPara->cscMatrixR1C0 = cscPara.matrixR1C0;
    commPara->cscMatrixR1C1 = cscPara.matrixR1C1;
    commPara->cscMatrixR1C2 = cscPara.matrixR1C2;
    commPara->cscMatrixR2C0 = cscPara.matrixR2C0;
    commPara->cscMatrixR2C1 = cscPara.matrixR2C1;
    commPara->cscMatrixR2C2 = cscPara.matrixR2C2;
    commPara->cscInputBiasR0 = static_cast<uint8_t>(cscPara.inputBias0);
    commPara->cscInputBiasR1 = static_cast<uint8_t>(cscPara.inputBias1);
    commPara->cscInputBiasR2 = static_cast<uint8_t>(cscPara.inputBias2);
    commPara->cscOutputBiasR0 = static_cast<uint8_t>(cscPara.outputBias0);
    commPara->cscOutputBiasR1 = static_cast<uint8_t>(cscPara.outputBias1);
    commPara->cscOutputBiasR2 = static_cast<uint8_t>(cscPara.outputBias2);
    return SUCCESS;
}

CscMatrixPara AIPPParaImpl::GetCscPara()
{
    CscMatrixPara cscPara;
    HIAI_MR_TensorAippCommPara* commPara = GetTensorAippCommPara().first;
    HIAI_EXPECT_NOT_NULL_R(commPara, cscPara);

    cscPara.matrixR0C0 = commPara->cscMatrixR0C0;
    cscPara.matrixR0C1 = commPara->cscMatrixR0C1;
    cscPara.matrixR0C2 = commPara->cscMatrixR0C2;
    cscPara.matrixR1C0 = commPara->cscMatrixR1C0;
    cscPara.matrixR1C1 = commPara->cscMatrixR1C1;
    cscPara.matrixR1C2 = commPara->cscMatrixR1C2;
    cscPara.matrixR2C0 = commPara->cscMatrixR2C0;
    cscPara.matrixR2C1 = commPara->cscMatrixR2C1;
    cscPara.matrixR2C2 = commPara->cscMatrixR2C2;
    cscPara.inputBias0 = static_cast<int32_t>(commPara->cscInputBiasR0);
    cscPara.inputBias1 = static_cast<int32_t>(commPara->cscInputBiasR1);
    cscPara.inputBias2 = static_cast<int32_t>(commPara->cscInputBiasR2);
    cscPara.outputBias0 = static_cast<int32_t>(commPara->cscOutputBiasR0);
    cscPara.outputBias1 = static_cast<int32_t>(commPara->cscOutputBiasR1);
    cscPara.outputBias2 = static_cast<int32_t>(commPara->cscOutputBiasR2);
    return cscPara;
}

Status AIPPParaImpl::SetChannelSwapPara(ChannelSwapPara&& channelSwapPara)
{
    HIAI_MR_TensorAippCommPara* commPara = GetTensorAippCommPara().first;
    HIAI_EXPECT_NOT_NULL(commPara);
    commPara->rbuvSwapSwitch = channelSwapPara.rbuvSwapSwitch;
    commPara->axSwapSwitch = channelSwapPara.axSwapSwitch;
    return SUCCESS;
}

ChannelSwapPara AIPPParaImpl::GetChannelSwapPara()
{
    ChannelSwapPara channelSwapPara;
    HIAI_MR_TensorAippCommPara* commPara = GetTensorAippCommPara().first;
    HIAI_EXPECT_NOT_NULL_R(commPara, channelSwapPara);
    channelSwapPara.rbuvSwapSwitch = commPara->rbuvSwapSwitch;
    channelSwapPara.axSwapSwitch = commPara->axSwapSwitch;
    return channelSwapPara;
}
// end common interface

template <typename T, typename UpdateParaFunc>
Status AIPPParaImpl::SetAippFuncPara(T&& funcPara, UpdateParaFunc updateParaFunc)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL(para.first);

    for (uint32_t index = 0; index < para.second; ++index) {
        updateParaFunc(para.first, index, funcPara);
    }
    return SUCCESS;
}

template <typename T, typename UpdateParaFunc>
Status AIPPParaImpl::SetAippFuncPara(uint32_t batchIndex, T&& funcPara, UpdateParaFunc updateParaFunc)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL(para.first);
    HIAI_EXPECT_EXEC(CheckBatchIndex(batchIndex, para.second));

    updateParaFunc(para.first, batchIndex, funcPara);
    return SUCCESS;
}

Status AIPPParaImpl::SetCropPara(CropPara&& cropPara)
{
    return SetAippFuncPara(cropPara, UpdateCropPara);
}

Status AIPPParaImpl::SetCropPara(uint32_t batchIndex, CropPara&& cropPara)
{
    return SetAippFuncPara(batchIndex, cropPara, UpdateCropPara);
}

CropPara AIPPParaImpl::GetCropPara(uint32_t batchIndex)
{
    CropPara cropPara;

    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, cropPara);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), cropPara);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    cropPara.cropStartPosW = batchPara->cropStartPosW;
    cropPara.cropStartPosH = batchPara->cropStartPosH;
    cropPara.cropSizeW = batchPara->cropSizeW;
    cropPara.cropSizeH = batchPara->cropSizeH;
    return cropPara;
}

Status AIPPParaImpl::SetResizePara(ResizePara&& resizePara)
{
    return SetAippFuncPara(resizePara, UpdateResizePara);
}

Status AIPPParaImpl::SetResizePara(uint32_t batchIndex, ResizePara&& resizePara)
{
    return SetAippFuncPara(batchIndex, resizePara, UpdateResizePara);
}

ResizePara AIPPParaImpl::GetResizePara(uint32_t batchIndex)
{
    ResizePara resizePara;

    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, resizePara);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), resizePara);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    resizePara.resizeOutputSizeW = batchPara->scfOutputSizeW;
    resizePara.resizeOutputSizeH = batchPara->scfOutputSizeH;
    return resizePara;
}

Status AIPPParaImpl::SetPaddingPara(PadPara&& paddingPara)
{
    return SetAippFuncPara(paddingPara, UpdatePaddingPara);
}

Status AIPPParaImpl::SetPaddingPara(uint32_t batchIndex, PadPara&& paddingPara)
{
    return SetAippFuncPara(batchIndex, paddingPara, UpdatePaddingPara);
}

PadPara AIPPParaImpl::GetPaddingPara(uint32_t batchIndex)
{
    PadPara padPara;

    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, padPara);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), padPara);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    padPara.paddingSizeTop = batchPara->paddingSizeTop;
    padPara.paddingSizeBottom = batchPara->paddingSizeBottom;
    padPara.paddingSizeLeft = batchPara->paddingSizeLeft;
    padPara.paddingSizeRight = batchPara->paddingSizeRight;

    padPara.paddingValueChn0 = SaveUint16ToFp16(batchPara->paddingValueChn0);
    padPara.paddingValueChn1 = SaveUint16ToFp16(batchPara->paddingValueChn1);
    padPara.paddingValueChn2 = SaveUint16ToFp16(batchPara->paddingValueChn2);
    padPara.paddingValueChn3 = SaveUint16ToFp16(batchPara->paddingValueChn3);
    return padPara;
}

Status AIPPParaImpl::SetDtcPara(DtcPara&& dtcPara)
{
    return SetAippFuncPara(dtcPara, UpdateDtcPara);
}

Status AIPPParaImpl::SetDtcPara(uint32_t batchIndex, DtcPara&& dtcPara)
{
    return SetAippFuncPara(batchIndex, dtcPara, UpdateDtcPara);
}

DtcPara AIPPParaImpl::GetDtcPara(uint32_t batchIndex)
{
    DtcPara dtcPara;

    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, dtcPara);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), dtcPara);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    dtcPara.pixelMeanChn0 = batchPara->dtcPixelMeanChn0;
    dtcPara.pixelMeanChn1 = batchPara->dtcPixelMeanChn1;
    dtcPara.pixelMeanChn2 = batchPara->dtcPixelMeanChn2;
    dtcPara.pixelMeanChn3 = batchPara->dtcPixelMeanChn3;
    dtcPara.pixelMinChn0 = SaveUint16ToFp16(batchPara->dtcPixelMinChn0);
    dtcPara.pixelMinChn1 = SaveUint16ToFp16(batchPara->dtcPixelMinChn1);
    dtcPara.pixelMinChn2 = SaveUint16ToFp16(batchPara->dtcPixelMinChn2);
    dtcPara.pixelMinChn3 = SaveUint16ToFp16(batchPara->dtcPixelMinChn3);
    dtcPara.pixelVarReciChn0 = SaveUint16ToFp16(batchPara->dtcPixelVarReciChn0);
    dtcPara.pixelVarReciChn1 = SaveUint16ToFp16(batchPara->dtcPixelVarReciChn1);
    dtcPara.pixelVarReciChn2 = SaveUint16ToFp16(batchPara->dtcPixelVarReciChn2);
    dtcPara.pixelVarReciChn3 = SaveUint16ToFp16(batchPara->dtcPixelVarReciChn3);
    return dtcPara;
}

ImageFormat AIPPParaImpl::GetInputFormat(HIAI_MR_TensorAippCommPara* commPara)
{
    HIAI_EXPECT_NOT_NULL_R(commPara, ImageFormat::INVALID);

    static const std::map<uint8_t, ImageFormat> cceToUserFormat = {
        {CCE_YUV420SP_U8, ImageFormat::YUV420SP},
        {CCE_XRGB8888_U8, ImageFormat::XRGB8888},
        {CCE_NC1HWC0DI_FP16, ImageFormat::INVALID},
        {CCE_NC1HWC0DI_S8, ImageFormat::INVALID},
        {CCE_RGB888_U8, ImageFormat::RGB888},
        {CCE_ARGB8888_U8, ImageFormat::ARGB8888},
        {CCE_YUYV_U8, ImageFormat::YUYV},
        {CCE_YUV422SP_U8, ImageFormat::YUV422SP},
        {CCE_AYUV444_U8, ImageFormat::AYUV444},
        {CCE_YUV400_U8, ImageFormat::YUV400},
    };
    if (cceToUserFormat.find(commPara->inputFormat) == cceToUserFormat.end()) {
        FMK_LOGE("GetInputFormat failed, inputFormat is unknown!");
        return ImageFormat::INVALID;
    }
    return cceToUserFormat.at(commPara->inputFormat);
}

Status AIPPParaImpl::SetInputFormat(ImageFormat inputFormat)
{
    static std::map<ImageFormat, CceAippInputFormat> inputFormatMap = {
        {ImageFormat::YUV420SP, CCE_YUV420SP_U8},
        {ImageFormat::XRGB8888, CCE_XRGB8888_U8},
        {ImageFormat::ARGB8888, CCE_ARGB8888_U8},
        {ImageFormat::YUYV, CCE_YUYV_U8},
        {ImageFormat::YUV422SP, CCE_YUV422SP_U8},
        {ImageFormat::AYUV444, CCE_AYUV444_U8},
        {ImageFormat::YUV400, CCE_YUV400_U8},
        {ImageFormat::RGB888, CCE_RGB888_U8},
    };
    if (inputFormatMap.find(inputFormat) == inputFormatMap.end()) {
        FMK_LOGE("SetInputFormat failed, inputFormat: %d is not supported on Lite currently", inputFormat);
        return FAILURE;
    }
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL(para.first);

    para.first->inputFormat = inputFormatMap[inputFormat];
    return SUCCESS;
}

ImageFormat AIPPParaImpl::GetInputFormat()
{
    auto para = GetTensorAippCommPara();
    return GetInputFormat(para.first);
}

std::vector<int32_t> AIPPParaImpl::GetInputShape()
{
    std::vector<int32_t> shape;
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, shape);

    shape = {para.first->srcImageSizeW, para.first->srcImageSizeH};
    return shape;
}

Status AIPPParaImpl::SetInputShape(std::vector<int32_t>& shape)
{
    if (shape.size() != 2) {
        FMK_LOGE("shape invalid!");
        return FAILURE;
    }

    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL(para.first);

    para.first->srcImageSizeW = shape[0];
    para.first->srcImageSizeH = shape[1];
    return SUCCESS;
}

Status AIPPParaImpl::GetAippParaBufferImpl(std::shared_ptr<AIPPParaBufferImpl>& aippParaImpl)
{
    aippParaImpl = make_shared_nothrow<AIPPParaBufferImpl>();
    if (aippParaImpl == nullptr) {
        return FAILURE;
    }
    return SUCCESS;
}

HIAI_MR_TensorAippPara* AIPPParaImpl::GetParaBuffer()
{
    return paraBuff_;
}

bool AIPPParaImpl::GetEnableCrop(uint32_t batchIndex)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, false);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), false);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    return batchPara->cropSwitch > 0;
}

bool AIPPParaImpl::GetEnableResize(uint32_t batchIndex)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, false);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), false);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    return batchPara->scfSwitch > 0;
}

bool AIPPParaImpl::GetEnableCsc()
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, false);
    return para.first->cscSwitch > 0;
}

bool AIPPParaImpl::GetEnablePadding(uint32_t batchIndex)
{
    auto para = GetTensorAippCommPara();
    HIAI_EXPECT_NOT_NULL_R(para.first, false);
    HIAI_EXPECT_EXEC_R(CheckBatchIndex(batchIndex, para.second), false);

    HIAI_MR_TensorAippBatchPara* batchPara = GetBatchPara(para.first, batchIndex);
    return batchPara->paddingSwitch > 0;
}

std::shared_ptr<IAIPPPara> CreateAIPPPara(uint32_t batchCount)
{
    shared_ptr<AIPPParaImpl> aippPara = make_shared_nothrow<AIPPParaImpl>();
    if (aippPara == nullptr) {
        FMK_LOGE("create aippParaImpl failed.");
        return nullptr;
    }
    auto status = aippPara->Init(batchCount);
    if (status != SUCCESS) {
        FMK_LOGE("create aippPara failed.");
        return nullptr;
    }
    return aippPara;
}
HIAI_MR_TensorAippPara* GetTensorAippParaFromAippPara(const std::shared_ptr<IAIPPPara>& aippPara)
{
    std::shared_ptr<AIPPParaImpl> aippParaImpl = std::dynamic_pointer_cast<AIPPParaImpl>(aippPara);
    if (aippParaImpl == nullptr) {
        FMK_LOGE("invalid aippPara");
        return nullptr;
    }
    return aippParaImpl->GetParaBuffer();
}

} // namespace hiai