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

#include "model_builder/ir/aipp/converter/aipp_param_info_converter.h"

#include <list>
#include <vector>
#include <algorithm>
#include <map>

// api/framework
#include "model/built_model_aipp.h"
#include "tensor/image_format.h"

// inc
#include "framework/graph/utils/tensor_utils.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/common/hcs_types.h"

// src/framework/inc
#include "infra/math/fp16_t.h"

#include "model_builder/ir/aipp/infershape/aipp_infershape_util.h"
#include "model/aipp/aipp_input_converter.h"

using namespace hiai;

namespace {
enum AIPP_MODE {
    STATIC = 1,
    DYNAMIC = 2
};

#define SAVE_AIPP_ATTR(key, para, type, aippAttrs) do { \
    (aippAttrs).SetAttr(#key, AttrValue::CreateFrom(static_cast<type>(para))); \
} while (0)

uint16_t TransFloatToFp16(float src)
{
    fp16_t fp16Data = src;
    return fp16Data.val;
}

InputFormat ConvertImageFormat2ProtoFormat(ImageFormat format)
{
    std::map<ImageFormat, InputFormat> formatMap = {
        {ImageFormat::YUV420SP, InputFormat::YUV420SP_U8},
        {ImageFormat::XRGB8888, InputFormat::XRGB8888_U8},
        {ImageFormat::RGB888, InputFormat::RGB888_U8},
        {ImageFormat::YUV400, InputFormat::YUV400_U8},
        {ImageFormat::ARGB8888, InputFormat::ARGB8888_U8},
        {ImageFormat::YUYV, InputFormat::YUYV_U8},
        {ImageFormat::YUV422SP, InputFormat::YUV422SP_U8},
        {ImageFormat::AYUV444, InputFormat::AYUV444_U8},
        {ImageFormat::BGR888, InputFormat::BGR888_U8},
        {ImageFormat::YUV444SP, InputFormat::YUV444SP_U8},
        {ImageFormat::YVU444SP, InputFormat::YVU444SP_U8},
        {ImageFormat::INVALID, InputFormat::UNDEFINED},
    };

    std::map<ImageFormat, InputFormat>::const_iterator iter = formatMap.find(format);
    if (iter != formatMap.cend()) {
        return iter->second;
    }
    return InputFormat::UNDEFINED;
}

Status ConvertInputFormatAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    if (paramInfo.inputFormat != ImageFormat::INVALID) {
        SAVE_AIPP_ATTR(input_format, ConvertImageFormat2ProtoFormat(paramInfo.inputFormat), AttrValue::INT, aippAttrs);
    }
    return SUCCESS;
}

Status ConvertCropAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(crop, paramInfo.enableCrop, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(load_start_pos_w, paramInfo.cropPara.cropStartPosW, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(load_start_pos_h, paramInfo.cropPara.cropStartPosH, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(crop_size_w, paramInfo.cropPara.cropSizeW, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(crop_size_h, paramInfo.cropPara.cropSizeH, AttrValue::INT, aippAttrs);

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertResizeAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(resize, paramInfo.enableResize, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(resize_output_w, paramInfo.resizePara.resizeOutputSizeW, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(resize_output_h, paramInfo.resizePara.resizeOutputSizeH, AttrValue::INT, aippAttrs);

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertPaddingAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(padding, paramInfo.enablePadding, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(left_padding_size, paramInfo.paddingPara.paddingSizeLeft, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(right_padding_size, paramInfo.paddingPara.paddingSizeRight, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(top_padding_size, paramInfo.paddingPara.paddingSizeTop, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(bottom_padding_size, paramInfo.paddingPara.paddingSizeBottom, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_0, paramInfo.paddingPara.paddingValueChn0, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_1, paramInfo.paddingPara.paddingValueChn1, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_2, paramInfo.paddingPara.paddingValueChn2, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_3, paramInfo.paddingPara.paddingValueChn3, AttrValue::FLOAT, aippAttrs);

    SAVE_AIPP_ATTR(padding_value_chn_0_fp16, TransFloatToFp16(paramInfo.paddingPara.paddingValueChn0),
                   AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_1_fp16, TransFloatToFp16(paramInfo.paddingPara.paddingValueChn1),
                   AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_2_fp16, TransFloatToFp16(paramInfo.paddingPara.paddingValueChn2),
                   AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(padding_value_chn_3_fp16, TransFloatToFp16(paramInfo.paddingPara.paddingValueChn3),
                   AttrValue::INT, aippAttrs);
    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertChannelSwapAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(rbuv_swap_switch, paramInfo.channelSwapPara.rbuvSwapSwitch, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(ax_swap_switch, paramInfo.channelSwapPara.axSwapSwitch, AttrValue::BOOL, aippAttrs);

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertRotateAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    if (paramInfo.enableRotate) {
        SAVE_AIPP_ATTR(rotation, paramInfo.rotatePara.rotate, AttrValue::BOOL, aippAttrs);
        SAVE_AIPP_ATTR(rotation_angle, paramInfo.rotatePara.rotationAngle, AttrValue::FLOAT, aippAttrs);
    }

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertDtcAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(dtc_switch, paramInfo.enableDtc, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(mean_chn_0, paramInfo.dtcPara.pixelMeanChn0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(mean_chn_1, paramInfo.dtcPara.pixelMeanChn1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(mean_chn_2, paramInfo.dtcPara.pixelMeanChn2, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(mean_chn_3, paramInfo.dtcPara.pixelMeanChn3, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_0, paramInfo.dtcPara.pixelMinChn0, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_1, paramInfo.dtcPara.pixelMinChn1, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_2, paramInfo.dtcPara.pixelMinChn2, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_3, paramInfo.dtcPara.pixelMinChn3, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_0, paramInfo.dtcPara.pixelVarReciChn0, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_1, paramInfo.dtcPara.pixelVarReciChn1, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_2, paramInfo.dtcPara.pixelVarReciChn2, AttrValue::FLOAT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_3, paramInfo.dtcPara.pixelVarReciChn3, AttrValue::FLOAT, aippAttrs);

    SAVE_AIPP_ATTR(var_reci_chn_0_fp16,
        TransFloatToFp16(paramInfo.dtcPara.pixelVarReciChn0 > 0 ? paramInfo.dtcPara.pixelVarReciChn0 : 0),
        AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_1_fp16,
        TransFloatToFp16(paramInfo.dtcPara.pixelVarReciChn0 > 0 ? paramInfo.dtcPara.pixelVarReciChn1 : 0),
        AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_2_fp16,
        TransFloatToFp16(paramInfo.dtcPara.pixelVarReciChn0 > 0 ? paramInfo.dtcPara.pixelVarReciChn2 : 0),
        AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(var_reci_chn_3_fp16,
        TransFloatToFp16(paramInfo.dtcPara.pixelVarReciChn0 > 0 ? paramInfo.dtcPara.pixelVarReciChn3 : 0),
        AttrValue::INT, aippAttrs);

    SAVE_AIPP_ATTR(min_chn_0_fp16, TransFloatToFp16(paramInfo.dtcPara.pixelMinChn0), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_1_fp16, TransFloatToFp16(paramInfo.dtcPara.pixelMinChn1), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_2_fp16, TransFloatToFp16(paramInfo.dtcPara.pixelMinChn2), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(min_chn_3_fp16, TransFloatToFp16(paramInfo.dtcPara.pixelMinChn3), AttrValue::INT, aippAttrs);

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

Status ConvertCscAippParams(AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    SAVE_AIPP_ATTR(csc_switch, paramInfo.enableCsc, AttrValue::BOOL, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r0c0, paramInfo.cscMatrixPara.matrixR0C0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r0c1, paramInfo.cscMatrixPara.matrixR0C1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r0c2, paramInfo.cscMatrixPara.matrixR0C2, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r1c0, paramInfo.cscMatrixPara.matrixR1C0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r1c1, paramInfo.cscMatrixPara.matrixR1C1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r1c2, paramInfo.cscMatrixPara.matrixR1C2, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r2c0, paramInfo.cscMatrixPara.matrixR2C0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r2c1, paramInfo.cscMatrixPara.matrixR2C1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(matrix_r2c2, paramInfo.cscMatrixPara.matrixR2C2, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(output_bias_0, paramInfo.cscMatrixPara.outputBias0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(output_bias_1, paramInfo.cscMatrixPara.outputBias1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(output_bias_2, paramInfo.cscMatrixPara.outputBias2, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(input_bias_0, paramInfo.cscMatrixPara.inputBias0, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(input_bias_1, paramInfo.cscMatrixPara.inputBias1, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(input_bias_2, paramInfo.cscMatrixPara.inputBias2, AttrValue::INT, aippAttrs);

    return ConvertInputFormatAippParams(aippAttrs, paramInfo);
}

uint32_t GetMaxSrcImageSize(TensorDescPtr& inputDesc, Shape& inputShape)
{
    uint32_t maxSrcImageSize = 0;
    ge::TensorUtils::GetSize(inputDesc, maxSrcImageSize);
    if (maxSrcImageSize != 0) {
        return maxSrcImageSize;
    }

    maxSrcImageSize = 1;
    for (auto& dim : inputShape.GetDims()) {
        maxSrcImageSize *= dim;
    }
    return maxSrcImageSize;
}

Status SetInputMaxSize(const ge::Node& dataNode, AttrValue::NamedAttrs& aippAttrs)
{
    const ge::OpDesc& opDesc = dataNode.ROLE(NodeSpec).OpDesc();
    TensorDescPtr inputDesc = opDesc.MutableInputDesc(0);
    Shape inputShape = inputDesc->GetShape();
    SAVE_AIPP_ATTR(aipp_mode, DYNAMIC, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(max_src_image_size, GetMaxSrcImageSize(inputDesc, inputShape), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(src_image_size_w, inputShape.GetDim(NCHW_DIM_W), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(src_image_size_h, inputShape.GetDim(NCHW_DIM_H), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(support_rotation, false, AttrValue::BOOL, aippAttrs);
    return hiai::SUCCESS;
}

Status SetSrcImageSize(const ge::Node& dataNode, AttrValue::NamedAttrs& aippAttrs)
{
    const ge::OpDesc& opDesc = dataNode.ROLE(NodeSpec).OpDesc();
    Shape inputShape = opDesc.MutableInputDesc(0)->GetShape();
    SAVE_AIPP_ATTR(aipp_mode, STATIC, AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(src_image_size_w, inputShape.GetDim(NCHW_DIM_W), AttrValue::INT, aippAttrs);
    SAVE_AIPP_ATTR(src_image_size_h, inputShape.GetDim(NCHW_DIM_H), AttrValue::INT, aippAttrs);

    return hiai::SUCCESS;
}

hiai::Status ConvertCropParas(const ge::Node& node, hiai::AippParamInfo& paramInfo)
{
    auto para = GetAippParam<CropPara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.enableCrop = true;
    paramInfo.cropPara = *para;
    return hiai::SUCCESS;
}

hiai::Status ConvertChannelSwapParas(const ge::Node& node, hiai::AippParamInfo& paramInfo)
{
    auto para = GetAippParam<ChannelSwapPara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.channelSwapPara = *para;
    return hiai::SUCCESS;
}

enum ImageType {
    JPEG,
    BT_601_NARROW,
    BT_601_FULL,
    BT_709_NARROW
};

hiai::Status ToRGB(InputFormat inputFormat, hiai::ImageFormat targetFormat, ImageType type,
    vector<int32_t>& cscValues, vector<int32_t>& inputBiasValues)
{
    const int rSize = 4;
    const int gSize = 3;
    const int bSize = 3;

    HIAI_EXPECT_TRUE_R(inputFormat != InputFormat::XRGB8888_U8 && inputFormat != InputFormat::RGB888_U8 &&
        inputFormat != InputFormat::ARGB8888_U8 && inputFormat != InputFormat::YUV400_U8, hiai::PARAM_INVALID);

    const int DEFAULT_INPUT_BIAS_0 = 16;
    const int DEFAULT_INPUT_BIAS_1 = 128;
    const int DEFAULT_INPUT_BIAS_2 = 128;
    inputBiasValues[0] = type == JPEG ? 0 : DEFAULT_INPUT_BIAS_0;
    inputBiasValues[1] = DEFAULT_INPUT_BIAS_1;
    inputBiasValues[2] = DEFAULT_INPUT_BIAS_2;

    const static map<hiai::ImageFormat, vector<uint32_t>> rgbMap = {
        {hiai::ImageFormat::RGB888, {0, 1, 2}}, {hiai::ImageFormat::BGR888, {2, 1, 0}}};

    const static int YUV_TO_RGB[rSize][gSize][bSize] = {
        {{256, 0, 359}, {256, -88, -183}, {256, 454, 0}},
        {{298, 0, 409}, {298, -100, -208}, {298, 516, 0}}, {{256, 0, 359}, {256, -88, -183}, {256, 454, 0}},
        {{298, 0, 460}, {298, -55, -137}, {298, 541, 0}}
    };
    uint32_t index = 0;
    for (uint32_t idx : rgbMap.at(targetFormat)) {
        for (uint32_t j = 0; j < bSize; ++j) {
            cscValues[index++] = YUV_TO_RGB[type][idx][j];
        }
    }

    return hiai::SUCCESS;
}

hiai::Status ToYUV(InputFormat inputFormat, hiai::ImageFormat targetFormat, ImageType type,
    vector<int32_t>& cscValues, vector<int32_t>& outputBiasValues)
{
    const int ySize = 4;
    const int uSize = 3;
    const int vSize = 3;

    HIAI_EXPECT_TRUE_R(inputFormat != InputFormat::YUV420SP_U8 && inputFormat != InputFormat::YUV422SP_U8 &&
        inputFormat != InputFormat::YUYV_U8 && inputFormat != InputFormat::AYUV444_U8 &&
        inputFormat != InputFormat::YUV400_U8, hiai::PARAM_INVALID);

    const static map<hiai::ImageFormat, vector<uint32_t>> yuvMap = {
        {hiai::ImageFormat::YUV444SP, {0, 1, 2}}, {hiai::ImageFormat::YVU444SP, {0, 2, 1}}};

    const int DEFAULT_OUTPUT_BIAS_0 = 16;
    const int DEFAULT_OUTPUT_BIAS_1 = 128;
    const int DEFAULT_OUTPUT_BIAS_2 = 128;
    outputBiasValues[0] = type == JPEG ? 0 : DEFAULT_OUTPUT_BIAS_0;
    outputBiasValues[1] = DEFAULT_OUTPUT_BIAS_1;
    outputBiasValues[2] = DEFAULT_OUTPUT_BIAS_2;

    const static int RGB_TO_YUV[ySize][uSize][vSize] = {
        {{77, 150, 29}, {-43, -85, 128}, {128, -107, -21}},
        {{66, 129, 25}, {-38, -74, 112}, {112, -94, -18}}, {{77, 150, 29}, {-43, -85, 128}, {128, -107, -21}},
        {{47, 157, 16}, {-26, -87, 112}, {112, -102, -10}}
    };
    uint32_t index = 0;
    for (uint32_t idx : yuvMap.at(targetFormat)) {
        for (uint32_t j = 0; j < vSize; ++j) {
            cscValues[index++] = RGB_TO_YUV[type][idx][j];
        }
    }

    return hiai::SUCCESS;
}

hiai::Status SetCscParas(
    InputFormat inputFormat, hiai::ImageFormat targetFormat, ImageType type, AippParamInfo& paramInfo)
{
    vector<int32_t> cscValues(9, 0);
    vector<int32_t> inputBiasValues(3, 0);
    vector<int32_t> outputBiasValues(3, 0);

    if (targetFormat == hiai::ImageFormat::RGB888 || targetFormat == hiai::ImageFormat::BGR888) {
        if (ToRGB(inputFormat, targetFormat, type, cscValues, inputBiasValues) != hiai::SUCCESS) {
            return hiai::PARAM_INVALID;
        }
    } else if (targetFormat == hiai::ImageFormat::YUV444SP || targetFormat == hiai::ImageFormat::YVU444SP) {
        if (ToYUV(inputFormat, targetFormat, type, cscValues, outputBiasValues) != hiai::SUCCESS) {
            return hiai::PARAM_INVALID;
        }
    } else {
        if (inputFormat == InputFormat::YUV400_U8) { // GRAY
            paramInfo.enableCsc = false;
            return hiai::SUCCESS;
        } else if (inputFormat == InputFormat::YUV420SP_U8 || inputFormat == InputFormat::YUV422SP_U8 ||
            inputFormat == InputFormat::YUYV_U8 || inputFormat == InputFormat::AYUV444_U8) { // YUV images
            cscValues[0] = 256;
        } else { // RGB images
            cscValues[0] = 76;
            cscValues[1] = 150;
            cscValues[2] = 30;
        }
    }
    paramInfo.cscMatrixPara.outputFormat = targetFormat;
    paramInfo.cscMatrixPara.matrixR0C0 = cscValues[0];
    paramInfo.cscMatrixPara.matrixR0C1 = cscValues[1];
    paramInfo.cscMatrixPara.matrixR0C2 = cscValues[2];
    paramInfo.cscMatrixPara.matrixR1C0 = cscValues[3];
    paramInfo.cscMatrixPara.matrixR1C1 = cscValues[4];
    paramInfo.cscMatrixPara.matrixR1C2 = cscValues[5];
    paramInfo.cscMatrixPara.matrixR2C0 = cscValues[6];
    paramInfo.cscMatrixPara.matrixR2C1 = cscValues[7];
    paramInfo.cscMatrixPara.matrixR2C2 = cscValues[8];
    paramInfo.cscMatrixPara.inputBias0 = inputBiasValues[0];
    paramInfo.cscMatrixPara.inputBias1 = inputBiasValues[1];
    paramInfo.cscMatrixPara.inputBias2 = inputBiasValues[2];
    paramInfo.cscMatrixPara.outputBias0 = outputBiasValues[0];
    paramInfo.cscMatrixPara.outputBias1 = outputBiasValues[1];
    paramInfo.cscMatrixPara.outputBias2 = outputBiasValues[2];

    return hiai::SUCCESS;
}

hiai::Status ConvertCscParas(const ge::Node& node, hiai::AippParamInfo& paramInfo)
{
    hiai::CscMatrixPara* cscMatrixPara = GetAippParam<hiai::CscMatrixPara>(node);
    hiai::CscPara* cscPara = GetAippParam<hiai::CscPara>(node);

    if (cscMatrixPara != nullptr) {
        paramInfo.cscMatrixPara = *cscMatrixPara;
        paramInfo.enableCsc = true;
        return hiai::SUCCESS;
    }

    if (cscPara == nullptr) {
        return hiai::FAILED;
    }

    InputFormat inputFormat = ConvertImageFormat2ProtoFormat(cscPara->imageFormat);
    if (inputFormat >= InputFormat::YUV420SP_U8 && inputFormat <= InputFormat::AYUV444_U8 &&
        paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = cscPara->imageFormat;
    }
    paramInfo.enableCsc = true;
    paramInfo.cscPara = *cscPara;
    static std::vector<hiai::ImageFormat> validFormat = {hiai::ImageFormat::YVU444SP, hiai::ImageFormat::YUV444SP,
        hiai::ImageFormat::RGB888, hiai::ImageFormat::BGR888, hiai::ImageFormat::YUV400};
    if (find(validFormat.begin(), validFormat.end(), cscPara->outputFormat) == validFormat.end()) {
        FMK_LOGE("target format invalid");
        return hiai::FAILED;
    }
    return SetCscParas(inputFormat, cscPara->outputFormat, static_cast<ImageType>(cscPara->imageColorSpace), paramInfo);
}

hiai::Status ConvertResizeParas(const ge::Node& node, AippParamInfo& paramInfo)
{
    auto para = GetAippParam<ResizePara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.enableResize = true;
    paramInfo.resizePara = *para;
    return hiai::SUCCESS;
}

hiai::Status ConvertDtcParas(const ge::Node& node, AippParamInfo& paramInfo)
{
    auto para = GetAippParam<DtcPara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.enableDtc = true;
    paramInfo.dtcPara = *para;
    return hiai::SUCCESS;
}

hiai::Status ConvertRotationParas(const ge::Node& node, AippParamInfo& paramInfo)
{
    auto para = GetAippParam<RotatePara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.enableRotate = true;
    paramInfo.rotatePara = *para;
    return hiai::SUCCESS;
}

hiai::Status ConvertPaddingParas(const ge::Node& node, AippParamInfo& paramInfo)
{
    auto para = GetAippParam<PadPara>(node);
    if (para == nullptr) {
        return hiai::FAILED;
    }

    if (para->imageFormat != ImageFormat::INVALID && paramInfo.inputFormat == ImageFormat::INVALID) {
        paramInfo.inputFormat = para->imageFormat;
    }
    paramInfo.enablePadding = true;
    paramInfo.paddingPara = *para;
    return hiai::SUCCESS;
}
} // namespace

namespace hiai {
Status AippParamInfoConverter::ConvertAippParamInfo2Attr(
    AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo)
{
    using ConvertAippPramFunc = std::function<hiai::Status(AttrValue::NamedAttrs&, const hiai::AippParamInfo&)>;
    std::vector<ConvertAippPramFunc> convertFuncList{ConvertInputFormatAippParams, ConvertRotateAippParams,
        ConvertChannelSwapAippParams, ConvertResizeAippParams, ConvertCropAippParams, ConvertPaddingAippParams,
        ConvertDtcAippParams, ConvertCscAippParams};

    for (const auto& func : convertFuncList) {
        if (func(aippAttrs, paramInfo) != SUCCESS) {
            FMK_LOGE("ConvertCropAippParams failed");
            return FAILED;
        }
    }

    return SUCCESS;
}

Status AippParamInfoConverter::ConvertAippInputInfo2Attr(
    AttrValue::NamedAttrs& aippAttrs, const ge::Node& dataNode, bool hasDynamicAippFunc)
{
    if (hasDynamicAippFunc) {
        return SetInputMaxSize(dataNode, aippAttrs);
    }

    return SetSrcImageSize(dataNode, aippAttrs);
}

Status AippParamInfoConverter::ConvertConst2AippParamInfo(
    const ge::Node& node, const std::string& aippFuncType, AippParamInfo& paramInfo)
{
    using ConvertAippFuncParasFunc = std::function<hiai::Status(const ge::Node&, hiai::AippParamInfo&)>;
    static const std::map<std::string, ConvertAippFuncParasFunc> convertFunc{
        {AIPP_FUNC_TYPE[AIPP_FUNC_IMAGE_CROP_V2], ConvertCropParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2], ConvertChannelSwapParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IMAGE_COLOR_SPACE_CONVERTION_V2], ConvertCscParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IMAGE_RESIZE_V2], ConvertResizeParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IMAGE_DATA_TYPE_CONVERSION_V2], ConvertDtcParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IAMGE_ROTATION_V2], ConvertRotationParas},
        {AIPP_FUNC_TYPE[AIPP_FUNC_IAMGE_PADDING_V2], ConvertPaddingParas},
    };

    std::map<std::string, ConvertAippFuncParasFunc>::const_iterator it = convertFunc.find(aippFuncType);
    if (it == convertFunc.cend()) {
        FMK_LOGE("ConvertConst2AippParamInfo failed");
        return hiai::FAILED;
    }

    if (it->second(node, paramInfo) != hiai::SUCCESS) {
        FMK_LOGE("parse ImageCrop op failed");
        return hiai::FAILED;
    }

    return hiai::SUCCESS;
}
} // namespace hiai
