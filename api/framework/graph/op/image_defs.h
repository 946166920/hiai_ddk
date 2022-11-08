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
#ifndef INC_API_GRAPH_OP_IMAGE_DEFS_H
#define INC_API_GRAPH_OP_IMAGE_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Image data tensor.
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    input_format : input_format of original image, accepted format is in the range of
 *                   [YUV420SP_U8, YUV422SP_U8, AYUV444_U8, YUYV_U8, YUV400_U8, XRGB8888_U8, ARGB8888_U8]
 *    src_image_size_w : original image size width
 *    src_image_size_h : original image size height
 *    image_type : accepted type is in the range of [JPEG, BT_601_NARROW, BT_601_FULL, BT_709_NARROW], default as JPEG
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc desc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData imageData = hiai::op::ImageData("Placeholder");
 *    imageData.update_input_desc_x(desc);
 *    imageData.set_attr_input_format("ARGB8888_U8");
 *    imageData.set_attr_src_image_size_w(64);
 *    imageData.set_attr_src_image_size_h(64);
 */
REG_OP(ImageData)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.REQUIRED_ATTR(input_format, AttrValue::STR)
.REQUIRED_ATTR(src_image_size_w, AttrValue::INT)
.REQUIRED_ATTR(src_image_size_h, AttrValue::INT)
.ATTR(image_type, AttrValue::STR { "JPEG" })
.OP_END()

/*
 * Dynamic image data tensor.
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    max_src_image_size : max src image size
 *    image_type : accepted type is in the range of [JPEG, BT_601_NARROW, BT_601_FULL, BT_709_NARROW], default as JPEG
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc desc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::DynamicImageData imageData = hiai::op::DynamicImageData("Placeholder");
 *    imageData.update_input_desc_x(desc);
 *    imageData.set_attr_max_src_image_size(64);
 *    imageData.set_attr_image_type("JPEG");
 */
REG_OP(DynamicImageData)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.REQUIRED_ATTR(max_src_image_size, AttrValue::INT)
.ATTR(image_type, AttrValue::STR { "JPEG" })
.OP_END()

/*
 * Data tensor for dynamic aipp
 * <Input>
 *      x : Input tensor stored config data
 * <Output>
 *      y : Output tensor
 * <Attr>
 *      type : respresenting aipp function type. Support 7 types including
 *             hiai::op::ImageCropV2::TYPE
 *             hiai::op::ImageResizeV2::TYPE
 *             hiai::op::ImageChannelSwapV2::TYPE
 *             hiai::op::ImageDataTypeConvertionV2::TYPE
 *             hiai::op::ImageColorSpaceConvertionV2::TYPE
 *             hiai::op::ImageRotateV2::TYPE
 *             hiai::op::ImagePadV2::TYPE
 *      value : value for the elements of the output tensor.
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    hiai::CropPara para;
 *    para.inputFormat = 0;
 *    para.cropStartPosW = 1;
 *    para.cropStartPosH = 1;
 *    para.cropSizeW = 240;
 *    para.cropSizeH = 240;
 *    hiai::op::ConfigData cropConfig = CreateConfigData(crop, "cropConfig",
 *    hiai::op::ImageCropV2::TYPE);
 */
REG_OP(ConfigData)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.REQUIRED_ATTR(type, AttrValue::STR)
.ATTR(value, AttrValue::TENSOR(new (std::nothrow) Tensor(TensorDesc())))
.OP_END()

/*
 * Extract crop from the input image tensor.
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor.
 * <Attr>
 *    load_start_pos_h : Y-axis of top left corner
 *    load_start_pos_w : X-axis of top left corner
 *    crop_size_h : Crop height
 *    crop_size_w : Crop width
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("x");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imageCrop = hiai::op::ImageCrop("imageCrop")
 *                     .set_input_x(x)
 *                     .set_attr_crop_size_h(64)
 *                     .set_attr_crop_size_w(64);
 */
REG_OP(ImageCrop)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.ATTR(load_start_pos_h, AttrValue::INT { 0 })
.ATTR(load_start_pos_w, AttrValue::INT { 0 })
.REQUIRED_ATTR(crop_size_h, AttrValue::INT)
.REQUIRED_ATTR(crop_size_w, AttrValue::INT)
.OP_END()

/*
 * Change image channel before Color Space Convertion
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    rbuv_swap_switch : whether to change the channel of R&B or U&V
 *    ax_swap_switch : whether to change GBA to ARGB or YUVA to AYUV
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("x");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imageChannelSwap = hiai::op::ImageChannelSwap("imageChannelSwap")
 *                            .set_input_x(x)
 *                            .set_attr_rbuv_swap_switch(false)
 *                            .set_attr_ax_swap_switch(true);
 */
REG_OP(ImageChannelSwap)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.ATTR(rbuv_swap_switch, AttrValue::BOOL { false })
.ATTR(ax_swap_switch, AttrValue::BOOL { false })
.OP_END()

/*
 * Support convert from YUV420SP YUYV YUV422SP AYUV444 to RGB888 BGR888
 * Support convert from XRGB8888 ARGB8888 to YVU444SP YUV444SP YUV400
 * Support convert from YUV420SP YUV422SP YUYV AYUV444 to YUV400
 * YUV400 can not be input format
 * Set target_format, CSC_MATRIX and CSC_BIAS will be configured by system according to input_format,
 * image_type and target_format.
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    target_format : target_format after color space conversion, accepted format is in the range of
 *                    [YVU444SP_U8, YUV444SP_U8, RGB888_U8, BGR888_U8, YUV400_U8]
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("Placeholder");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imageColorSpaceConvertion = hiai::op::ImageColorSpaceConvertion("imageColorSpaceConvertion")
 *                                     .set_input_x(x)
 *                                     .set_attr_target_format("YVU444SP_U8");
 */
REG_OP(ImageColorSpaceConvertion)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.REQUIRED_ATTR(target_format, AttrValue::STR)
.OP_END()

/*
 * Resize the input image tensor, It use bilinear or nearest neighbor algorithm to support scale up and down
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    resize_output_h : height after resize
 *    resize_output_w : width after resize
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 32, 32}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("x");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imageResize = hiai::op::ImageResize("imageResize")
 *                      .set_input_x(x)
 *                      .set_attr_resize_output_h(32)
 *                      .set_attr_resize_output_w(32);
 */
REG_OP(ImageResize)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.REQUIRED_ATTR(resize_output_h, AttrValue::INT)
.REQUIRED_ATTR(resize_output_w, AttrValue::INT)
.OP_END()

/*
 * AI core support feature map data type are: int8 and fp16. But image RGB888 or YUV444 data are all uint8 data type.
 * So we need data type conversion.
 * uint8->int8: pixel_out_chx(i) = pixel_in_chx(i) - mean_chn_i
 * uint8->fp16: pixel_out_chx(i) = (pixel_in_chx(i) - mean_chn_i - min_chn_i) * var_reci_chn
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor.
 * <Attr>
 *    mean_chn_0 : Average value of channel 0
 *    mean_chn_1 : Average value of channel 1
 *    mean_chn_2 : Average value of channel 2
 *    mean_chn_3 : Average value of channel 3
 *    min_chn_0 : Minimum value of channel 0
 *    min_chn_1 : Minimum value of channel 1
 *    min_chn_2 : Minimum value of channel 2
 *    min_chn_3 : Minimum value of channel 3
 *    var_reci_chn_0 : Variance of channel 0
 *    var_reci_chn_1 : Variance of channel 1
 *    var_reci_chn_2 : Variance of channel 2
 *    var_reci_chn_3 : Variance of channel 3
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("x");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imageDataTypeConversion = hiai::op::ImageDataTypeConversion("imageDataTypeConversion")
 *                                   .set_input_x(x);
 */
REG_OP(ImageDataTypeConversion)
.INPUT(x, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.ATTR(mean_chn_0, AttrValue::INT { 0 })
.ATTR(mean_chn_1, AttrValue::INT { 0 })
.ATTR(mean_chn_2, AttrValue::INT { 0 })
.ATTR(mean_chn_3, AttrValue::INT { 0 })
.ATTR(min_chn_0, AttrValue::FLOAT { 0 })
.ATTR(min_chn_1, AttrValue::FLOAT { 0 })
.ATTR(min_chn_2, AttrValue::FLOAT { 0 })
.ATTR(min_chn_3, AttrValue::FLOAT { 0 })
.ATTR(var_reci_chn_0, AttrValue::FLOAT { 1.0 })
.ATTR(var_reci_chn_1, AttrValue::FLOAT { 1.0 })
.ATTR(var_reci_chn_2, AttrValue::FLOAT { 1.0 })
.ATTR(var_reci_chn_3, AttrValue::FLOAT { 1.0 })
.OP_END()

/*
 * Rotate the input image tensor, It use bilinear or nearest neighbor algorithm to support scale up and down
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    rotation_angle : rotate degree
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(ImageRotation)
.INPUT(x, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.REQUIRED_ATTR(rotation_angle, AttrValue::FLOAT)
.OP_END()

/*
 * Add padding to input image tensor
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor.
 * <Attr>
 *    left_padding_size : left padding size
 *    right_padding_size : right padding size
 *    top_padding_size : top padding size
 *    bottom_padding_size : bottom padding size
 *    padding_value_chn_0 : channel 0 padding value
 *    padding_value_chn_1 : channel 1 padding value
 *    padding_value_chn_2 : channel 2 padding value
 *    padding_value_chn_3 : channel 3 padding value
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::ImageData x = hiai::op::ImageData("x");
 *    x.update_input_desc_x(xDesc);
 *    x.set_attr_input_format("ARGB8888_U8");
 *    x.set_attr_src_image_size_w(64);
 *    x.set_attr_src_image_size_h(64);
 *
 *    auto imagePadding = hiai::op::ImagePadding("imagePadding")
 *                       .set_input_x(x)
 *                       .set_attr_left_padding_size(30)
 *                       .set_attr_right_padding_size(30)
 *                       .set_attr_top_padding_size(30)
 *                       .set_attr_bottom_padding_size(30)
 *                       .set_padding_value_chn_0(20.0)
 *                       .set_padding_value_chn_1(20.0)
 *                       .set_padding_value_chn_2(20.0)
 *                       .set_padding_value_chn_3(20.0);
 */
REG_OP(ImagePadding)
.INPUT(x, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.ATTR(left_padding_size, AttrValue::INT { 0 })
.ATTR(right_padding_size, AttrValue::INT { 0 })
.ATTR(top_padding_size, AttrValue::INT { 0 })
.ATTR(bottom_padding_size, AttrValue::INT { 0 })
.ATTR(padding_value_chn_0, AttrValue::FLOAT { 0 })
.ATTR(padding_value_chn_1, AttrValue::FLOAT { 0 })
.ATTR(padding_value_chn_2, AttrValue::FLOAT { 0 })
.ATTR(padding_value_chn_3, AttrValue::FLOAT { 0 })
.OP_END()


/*
 * Extract crop from the input image tensor.
 * <Input>
 *    x : the input tensor
 *    param : the crop param
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::CropPara crop;
 *    crop.cropStartPosW = 0;
 *    crop.cropStartPosH = 0;
 *    crop.cropSizeW = 64;
 *    crop.cropSizeH = 64;
 *
 *    static
 *    hiai::op::Const cropConst = CreateConfigConst(crop, "cropConst", hiai::op::ImageCropV2::TYPE);
 *    auto imageCropv2 = hiai::op::ImageCropV2("imageCropv2")
 *                       .set_input_x(x)
 *                       .set_input_param(cropConst);
 *
 *    dynamic
 *    hiai::op::ConfigData cropConfig = CreateConfigData(crop, "cropConfig", hiai::op::ImageCropV2::TYPE);
 *    auto imageCropv2 = hiai::op::ImageCropV2("imageCropv2")
 *                       .set_input_x(x)
 *                       .set_input_param(cropConfig);
 */
REG_OP(ImageCropV2)
.INPUT(x, TensorType({ DT_UINT8 }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.OP_END()

/*
 * Resize the input image tensor, It use bilinear or nearest neighbor algorithm to support scale up and down
 * <Input>
 *    x : the input tensor
 *    param : resize parameters
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::ResizePara resize;
 *    resize.resizeOutputSizeW = 64;
 *    resize.resizeOutputSizeW = 64;
 *
 *    static
 *    hiai::op::Const resizeConst = CreateConfigConst(resize, "resizeConst", hiai::op::ImageResizeV2::TYPE);
 *    auto imageResizev2= hiai::op::ImageResizeV2("imageResizev2")
 *                       .set_input_x(x)
 *                       .set_input_param(cropConst);
 *
 *    dynamic
 *    hiai::op::ConfigData resizeConfig = CreateConfigData(resize, "resizeConfig", hiai::op::ImageResizeV2::TYPE);
 *    auto imageResizev2 = hiai::op::ImageCropV2("imageResizev2")
 *                       .set_input_x(x)
 *                       .set_input_param(resizeConfig);
 */
REG_OP(ImageResizeV2)
.INPUT(x, TensorType({ DT_UINT8 }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.OP_END()

/*
 * Change image channel before Color Space Convertion
 * <Input>
 *    x : the input tensor
 *    param : channel swap parameters
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::ChannelSwapPara channelSwap;
 *    channelSwap.imageFormat = ImageFormat::RGB888;
 *    channelSwap.rbuvSwapSwitch = true;
 *    channelSwap.axSwapSwitch = true;
 *
 *    static
 *    hiai::op::Const swapConst = CreateConfigConst(channelSwap, "swapConst", hiai::op::ImageChannelSwapV2::TYPE);
 *    auto channelSwapV2 = hiai::op::ImageChannelSwapV2("channelSwapV2")
 *                       .set_input_x(x)
 *                       .set_input_param(swapConst);
 *
 *    dynamic
 *    hiai::op::ConfigData swapConfig = CreateConfigData(channelSwap, "swapConfig", hiai::op::ImageChannelSwapV2::TYPE);
 *    auto channelSwapV2 = hiai::op::ImageChannelSwapV2("channelSwapV2")
 *                       .set_input_x(x)
 *                       .set_input_param(swapConfig);
 */
REG_OP(ImageChannelSwapV2)
.INPUT(x, TensorType({ DT_UINT8 }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.OP_END()

/*
 * Support convert YUV444 to RGB888, RGB888 to YUV444.
 * Set target_format, CSC_MATRIX and CSC_BIAS will be configured by system according to input_format,
 * image_type and target_format.
 * <Input>
 *    x : the input tensor
 *    param : color space convertion parameters
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("Placeholder");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::CscPara cscPara;
 *    cscPara.outputFormat = ImageFormat::YUV444SP;
 *    cscPara.imageColorSpace = ImageColorSpace::JPEG;
 *
 *    static
 *    hiai::op::Const cscConst = CreateConfigConst(cscPara, "cscConst");
 *    auto colorSpaceConvertionV2 = hiai::op::ImageChannelSwapV2("channelSwapV2")
 *                       .set_input_x(x)
 *                       .set_input_param(cscConst);
 *
 *    dynamic
 *    hiai::op::ConfigData cscConfig =
 *         CreateConfigData(cscPara, "cscConfig", hiai::op::ImageColorSpaceConvertionV2::TYPE);
 *    auto colorSpaceConvertionV2 = hiai::op::ImageChannelSwapV2("channelSwapV2")
 *                       .set_input_x(x)
 *                       .set_input_param(cscConfig);
 */
REG_OP(ImageColorSpaceConvertionV2)
.INPUT(x, TensorType({ DT_UINT8 }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.OP_END()

/*
 * AI core support feature map data type are: int8 and fp16. But image RGB888 or YUV444 data are all uint8 data type.
 * So we need data type conversion.
 * uint8->int8: pixel_out_chx(i) = pixel_in_chx(i) - mean_chn_i
 * uint8->fp16: pixel_out_chx(i) = (pixel_in_chx(i) - mean_chn_i - min_chn_i) * var_reci_chn
 * <Input>
 *    x : the input tensor
 *    param : dataType convertion parameters
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::DtcPara dtcPara;
 *    dtcPara.pixelMeanChn0 = 0;
 *    dtcPara.pixelMeanChn1 = 1;
 *    dtcPara.pixelMeanChn2 = 2;
 *    dtcPara.pixelMeanChn3 = 3;
 *    dtcPara.pixelMinChn0 = 4.0f;
 *    dtcPara.pixelMinChn1 = 5.0f;
 *    dtcPara.pixelMinChn2 = 6.0f;
 *    dtcPara.pixelMinChn3 = 7.0f;
 *    dtcPara.pixelVarReciChn0 = 8.0f;
 *    dtcPara.pixelVarReciChn1 = 9.0f;
 *    dtcPara.pixelVarReciChn2 = 10.0f;
 *    dtcPara.pixelVarReciChn3 = 11.0f;
 *
 *    static
 *    hiai::op::Const dtcConst = CreateConfigConst(dtcPara, "dtcConst", hiai::op::ImageDataTypeConvertionV2::TYPE);
 *    auto dataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("dataTypeConvertionV2")
 *                       .set_input_x(x)
 *                       .set_input_param(dtcConst);
 *
 *    dynamic
 *    hiai::op::ConfigData dtcConfig =
 *         CreateConfigData(dtcPara, "dtcConfig", hiai::op::ImageDataTypeConvertionV2::TYPE);
 *    auto dataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("dataTypeConvertionV2")
 *                       .set_input_x(x)
 *                       .set_input_param(dtcConfig);
 */
REG_OP(ImageDataTypeConvertionV2)
.INPUT(x, TensorType({ DT_UINT8 }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OP_END()

/*
 * Rotate the input image tensor, It use bilinear or nearest neighbor algorithm to support scale up and down
 * <Input>
 *    x : the input tensor.
 *    param : rotate parameters, not supported dynamic currently
 * <Output>
 *    y : the output tensor
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::RotatePara rotatePara;
 *    rotatePara.rotationAngle = 90.0f;
 *    rotatePara.rotate = true;
 *
 *    static
 *    hiai::op::Const rotateConst = CreateConfigConst(rotatePara, "rotateConst", hiai::op::ImageRotateV2::TYPE);
 *    auto rotateV2 = hiai::op::ImageRotateV2("rotateV2")
 *                       .set_input_x(x)
 *                       .set_input_param(rotateConst);
 */
REG_OP(ImageRotateV2)
.INPUT(x, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OP_END()

/*
 * Add padding to input image tensor
 * <Input>
 *    x : the input tensor.
 *    param : padding parameters
 * <Output>
 *    y : the output tensor.
 * <Added in HiAI version>
 *    100.520.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 3, 64, 64}), FORMAT_NCHW, DT_UINT8);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    hiai::PadPara padPara;
 *    padPara.paddingSizeTop = 10;
 *    padPara.paddingSizeBottom = 10;
 *    padPara.paddingSizeLeft = 20;
 *    padPara.paddingSizeRight = 20;
 *
 *    static
 *    hiai::op::Const padConst = CreateConfigConst(padPara, "padConst", hiai::op::ImagePadV2::TYPE);
 *    auto padV2 = hiai::op::ImagePadV2("padV2")
 *                       .set_input_x(x)
 *                       .set_input_param(padConst);
 *
 *    dynamic
 *    hiai::op::ConfigData padConfig = CreateConfigData(padPara, "padConfig", hiai::op::ImagePadV2::TYPE);
 *    auto padV2 = hiai::op::ImagePadV2("padV2")
 *                       .set_input_x(x)
 *                       .set_input_param(padConfig);
 */
REG_OP(ImagePadV2)
.INPUT(x, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.INPUT(param, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OP_END()

/*
 * Extracts crops from the input image tensor and resizes them.
 * <Input>
 *    x : 4-D tensor
 *    boxes : 2-D tensor. boxes[1] must be equal to 4.
 *    box_index : 1-D tensor. The value of box_index[i] specifies the image that the i-th box refers to.
 *                box_index[0] must be equal to boxes[0].
 *    crop_size : weight and height.
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    extrapolation_value : Value for extrapolation, default 0.
 *    method : Sampling method for resizing either bilinear or nearest. Defaults to bilinear.
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc boxesTensorDesc(Shape({5, 4}), FORMAT_NCHW, DT_FLOAT);
 *    TensorPtr boxesTensor = std::make_shared<hiai::Tensor>(boxesTensorDesc);
 *    vector<float> boxesValue(5 * 4, 0.0);
 *    boxesTensor->SetData((uint8_t*)boxesValue.data(), 5 * 4 * sizeof(float));
 *    auto boxes = hiai::op::Const("boxes").set_attr_value(boxesTensor);
 *
 *    TensorDesc boxIndexTensorDesc(Shape({5}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr boxIndexTensor = std::make_shared<hiai::Tensor>(boxIndexTensorDesc);
 *    vector<int32_t> boxIndexValue(5, 0);
 *    boxIndexTensor->SetData((uint8_t*)boxIndexValue.data(), 5 * sizeof(int32_t));
 *    auto boxIndex = hiai::op::Const("boxIndex").set_attr_value(boxIndexTensor);
 *
 *    TensorDesc cropSizeTensorDesc(Shape({2}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr cropSizeTensor = std::make_shared<hiai::Tensor>(cropSizeTensorDesc);
 *    vector<int32_t> cropSizeValue = {7, 8};
 *    cropSizeTensor->SetData((uint8_t*)cropSizeValue.data(), 2 * sizeof(int32_t));
 *    auto cropSize = hiai::op::Const("cropSize").set_attr_value(cropSizeTensor);
 *
 *    auto cropAndResize = hiai::op::CropAndResize("cropAndResize")
 *                         .set_input_x(x)
 *                         .set_input_boxes(boxes)
 *                         .set_input_box_index(boxIndex)
 *                         .set_input_crop_size(cropSize)
 *                         .set_attr_extrapolation_value(0)
 *                         .set_attr_method("bilinear");
 */
REG_OP(CropAndResize)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(boxes, TensorType({ DT_FLOAT }))
.INPUT(box_index, TensorType({ DT_INT32 }))
.INPUT(crop_size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(extrapolation_value, AttrValue::FLOAT { 0 })
.ATTR(method, AttrValue::STR { "bilinear" })
.OP_END()

/*
 * Consumes an input tensor X and region of interests (rois) to apply pooling across each RoI.
 * <Input>
 *    features : 4-D feature map
 *    rois : 2-D tensor, regions of interest to pool over
 *    rois_n : the number of RoI
 *    batch_indices : 1-D tensor of shape with each element denoting the index of the corresponding image in the batch.
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    spatial_scale : A scaling factor that maps the raw image coordinates to the input feature map coordinates,
 *                    { spatial_scale }. Generally is{ 1.0f }.
 *    pooled_height : Pooled output y's height.
 *    pooled_width : Pooled output y's width.
 *    sample_num : Number of sampling points, default 0.
 *    roi_end_mode : Used to determine whether roi_end_h / roi_end_w is incremented by 1,
 *                   0: roi_end_h / roi_end_w does not add one, 1: roi_end_h / roi_end_w add one. Default is 1.
 *    mode : The pooling method. Two modes are supported: 'avg' and 'max'. Default is 'avg'.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(ROIAlignV2)
.INPUT(features, TensorType({ DT_FLOAT }))
.INPUT(rois, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(rois_n, TensorType({ DT_INT32 }))
.OPTIONAL_INPUT(batch_indices, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(spatial_scale, AttrValue::LIST_FLOAT)
.REQUIRED_ATTR(pooled_height, AttrValue::INT)
.REQUIRED_ATTR(pooled_width, AttrValue::INT)
.ATTR(sample_num, AttrValue::INT { 0 })
.ATTR(roi_end_mode, AttrValue::INT { 1 })
.ATTR(mode, AttrValue::STR { "avg" })
.OP_END()

/*
 * Resizes images to 'size' using bilinear interpolation.
 * <Input>
 *    x : 4-D tensor
 *    size : 1-D tensor with shape [height, width], must be const op.
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    align_corners : If true, the centers of the 4 corner pixels of the input and output tensors
 *                    are aligned, preserving the values at the corner pixels.
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc sizeTensorDesc(Shape({2}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr sizeTensor = std::make_shared<hiai::Tensor>(sizeTensorDesc);
 *    vector<int32_t> dataValue = {7, 8};
 *    sizeTensor->SetData((uint8_t*)dataValue.data(), 2 * sizeof(int32_t));
 *    auto size = hiai::op::Const("size").set_attr_value(sizeTensor);
 *
 *    auto resizeBilinear = hiai::op::ResizeBilinear("resizeBilinear")
 *                          .set_input_x(x)
 *                          .set_input_size(size)
 *                          .set_attr_align_corners(false);
 */
REG_OP(ResizeBilinear)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(align_corners, AttrValue::BOOL { false })
.OP_END()

/*
 * Resizes images to 'size' using bilinear interpolation.
 * <Input>
 *    x : 4-D tensor
 *    size : 1-D tensor with shape [height, width]
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    align_corners : If true, the centers of the 4 corner pixels of the input and output tensors
 *                    are aligned, preserving the values at the corner pixels.
 *    half_pixel_centers : If true, the align_corners must be false. and this attr change the mapping way
 *                         to src image, default value is false.
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc sizeTensorDesc(Shape({2}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr sizeTensor = std::make_shared<hiai::Tensor>(sizeTensorDesc);
 *    vector<int32_t> dataValue = {7, 8};
 *    sizeTensor->SetData((uint8_t*)dataValue.data(), 2 * sizeof(int32_t));
 *    auto size = hiai::op::Const("size").set_attr_value(sizeTensor);
 *
 *    auto resizeBilinearV2 = hiai::op::ResizeBilinearV2("resizeBilinearV2")
 *                           .set_input_x(x)
 *                           .set_input_size(size)
 *                           .set_attr_align_corners(false)
 *                           .set_attr_half_pixel_centers(true);
 */
REG_OP(ResizeBilinearV2)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(align_corners, AttrValue::BOOL { false })
.ATTR(half_pixel_centers, AttrValue::BOOL { false })
.OP_END()

/*
 * Resizes images to 'size' using nearest neighbor interpolation.
 * <Input>
 *    x : 4-D tensor
 *    size : 1-D of two elements
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    align_corners : If true, the centers of the 4 corner pixels of the input and output tensors are aligned,
 *                    preserving the values at the corner pixels. Defaults to false
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc sizeTensorDesc(Shape({2}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr sizeTensor = std::make_shared<hiai::Tensor>(sizeTensorDesc);
 *    vector<int32_t> dataValue = {7, 8};
 *    sizeTensor->SetData((uint8_t*)dataValue.data(), 2 * sizeof(int32_t));
 *    auto size = hiai::op::Const("size").set_attr_value(sizeTensor);
 *
 *    auto resizeNearestNeighbor = hiai::op::ResizeNearestNeighbor("resizeNearestNeighbor")
 *                                 .set_input_x(x)
 *                                 .set_input_size(size)
 *                                 .set_attr_align_corners(false);
 */
REG_OP(ResizeNearestNeighbor)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(align_corners, AttrValue::BOOL { false })
.OP_END()

/*
 * Resizes images to 'size' using nearest neighbor interpolation.
 * <Input>
 *    x : 4-D tensor
 *    size : 1-D of two elements
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    align_corners : If true, the centers of the 4 corner pixels of the input and output tensors are aligned,
 *                    preserving the values at the corner pixels. Defaults to false
 *    half_pixel_centers : If true, the align_corners must be false. and this attr change the mapping way
 *                         to src image, default value is false.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(ResizeNearestNeighborV2)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(align_corners, AttrValue::BOOL { false })
.ATTR(half_pixel_centers, AttrValue::BOOL { false })
.OP_END()

/*
 * The operator is used to adjust the shape according to the stride_h and stride_w.
 * <Input>
 *    x : The input tensor of 4-D.
 * <Output>
 *    y : The output tensor of 4-D.
 * <Attr>
 *    stride_h : Dimension-h amplification factor, must be greater than 0.
 *    stride_w : Dimension-w amplification factor, must be greater than 0.
 *    scale : Reserved attribute, not need to enter.
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    auto upsample = hiai::op::Upsample("upsample")
 *                    .set_input_x(x)
 *                    .set_attr_stride_h(2)
 *                    .set_attr_stride_w(3);
 */
REG_OP(Upsample)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(stride_h, AttrValue::INT { 1 })
.ATTR(stride_w, AttrValue::INT { 1 })
.ATTR(scale, AttrValue::FLOAT { 1.0 })
.OP_END()

/*
 * Interpolation operation to adjust image shape.
 * <Input>
 *    x : 4-D tensor with shape [batch, depth, height, width], and must be non const op.
 * <Output>
 *    y : the output tensor of 4-D.
 * <Attr>
 *    height : height of output, must be greater than 0.
 *    width : width of output, must be greater than 0.
 *    shrink_factor : shrink factor, must be greater than 0.
 *    zoom_factor : zoom factor, must be greater than 0.
 *    pad_begin : padding at begin of input, must be less than or equal to 0.
 *    pad_end : padding at end of input, must be less than or equal to 0.
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    shrink_factor:
 *    auto interp = hiai::op::Interp("interp")
 *                  .set_input_x(x)
 *                  .set_attr_shrink_factor(2)
 *                  .set_attr_pad_begin(0)
 *                  .set_attr_pad_end(-1);
 *
 *    zoom_factor:
 *    auto interp = hiai::op::Interp("interp")
 *                  .set_input_x(x)
 *                  .set_attr_zoom_factor(2)
 *                  .set_attr_pad_begin(0)
 *                  .set_attr_pad_end(-1);
 *
 *    shrink_factor/zoom_factor:
 *    auto interp = hiai::op::Interp("interp")
 *                  .set_input_x(x)
 *                  .set_attr_shrink_factor(2)
 *                  .set_attr_zoom_factor(3)
 *                  .set_attr_pad_begin(-1)
 *                  .set_attr_pad_end(-2);
 *
 *    height/width:
 *    auto interp = hiai::op::Interp("interp")
 *                  .set_input_x(x)
 *                  .set_attr_height(7)
 *                  .set_attr_width(8)
 *                  .set_attr_pad_begin(0)
 *                  .set_attr_pad_end(-2);
 */
REG_OP(Interp)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(height, AttrValue::INT { -1 })
.ATTR(width, AttrValue::INT { -1 })
.ATTR(shrink_factor, AttrValue::INT { -1 })
.ATTR(zoom_factor, AttrValue::INT { -1 })
.ATTR(pad_begin, AttrValue::INT { 0 })
.ATTR(pad_end, AttrValue::INT { 0 })
.OP_END()

/*
 * To crop ,elements of the first input are selected to fit the dimensions of the second input.
 * <Input>
 *    x : The tensor to be croped.
 *    size : The size of the input x to be croped.
 * <Output>
 *    y : The output tensor.
 * <Attr>
 *    axis : The Dimension of input which to be croped.
 *    offsets : The offsets of input x.
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc sizeDesc(Shape({3, 4, 5, 6}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data size = hiai::op::Data("size");
 *    size.update_input_desc_x(sizeDesc);
 *
 *    auto crop = hiai::op::Crop("crop")
 *                .set_input_x(x)
 *                .set_input_size(size)
 *                .set_attr_axis(0)
 *                .set_attr_offsets({1, 1});
 */
REG_OP(Crop)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8, DT_BOOL }))
.INPUT(size, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8, DT_BOOL }))
.ATTR(axis, AttrValue::INT { 2 })
.REQUIRED_ATTR(offsets, AttrValue::LIST_INT)
.OP_END()

/*
 * Performs non-maximum suppression (NMS) on the boxes according to their intersection-over-union (IoU).
 * NMS iteratively removes lower scoring boxes which have an IoU greater than iou_threshold
 * with another (higher scoring) box.
 * <Input>
 *    boxes : Non const op, should be 2-D tensor, dim[0] of boxes should be equal to dim[0] of scores,
 *            dim[1] of boxes should be 4.
 *    scores : Non const op, should be 1-D tensor, dim[0] of scores should be equal to dim[0] of boxes.
 * <Output>
 *    y : A 1-D integer tensor with the shape-value less than or equal to max_output_size.
 * <Attr>
 *    max_output_size : Meaning the maximum number of boxes to be selected by non max suppression.
 *    iou_threshold : Meaning the threshold for deciding whether boxes overlap too much with respect to IOU.
 *    score_threshold : The threshold for deciding when to remove boxes based on score.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(NonMaxSuppressionV3D)
.INPUT(boxes, TensorType({ DT_FLOAT }))
.INPUT(scores, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_INT32 }))
.REQUIRED_ATTR(max_output_size, AttrValue::INT)
.REQUIRED_ATTR(iou_threshold, AttrValue::FLOAT)
.REQUIRED_ATTR(score_threshold, AttrValue::FLOAT)
.OP_END()

/*
 * Performs non-maximum suppression (NMS) on the boxes according to their intersection-over-union (IoU).
 * NMS iteratively removes lower scoring boxes which have an IoU greater than iou_threshold
 * with another (higher scoring) box.
 * <Input>
 *    boxes : An input tensor with shape [num_batches, spatial_dimension, 4]. The single box data format is
 *            indicated by center_point_box.
 *    scores : An input tensor with shape [num_batches, num_classes, spatial_dimension]
 *    max_output_boxes_per_class : Integer representing the maximum number of boxes to be selected per batch per class.
 *                                 It is a scalar. Only positive numbers(excluding 0) are supported, 1 for default.
 *    iou_threshold : Float representing the threshold for deciding whether boxes overlap too much with respect
 *                    to IOU. It is scalar. Value range [0, 1].
 *    score_threshold : Float representing the threshold for deciding when to remove boxes based on score.
 *                      It is a scalar.
 * <Output>
 *    selected_indices : selected indices from the boxes tensor. [num_selected_indices, 3],
 *                       the selected index format is [batch_index, class_index, box_index].
 * <Attr>
 *    center_point_box : Integer indicate the format of the box data.
 *                       The default is 0. 0 - the box data is supplied as [y1, x1, y2, x2]
 *                       where (y1, x1) and (y2, x2) are the coordinates of any diagonal pair of box corners and the
 *                       coordinates can be provided as normalized (i.e., lying in the interval [0, 1]) or absolute.
 *                       Mostly used for TF models. 1 - the box data is supplied as [x_center, y_center, width, height]
 *                       Mostly used for Pytorch models.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(NonMaxSuppressionV6)
.INPUT(boxes, TensorType({ DT_FLOAT }))
.INPUT(scores, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(max_output_boxes_per_class, TensorType({ DT_INT32 }))
.OPTIONAL_INPUT(iou_threshold, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(score_threshold, TensorType({ DT_FLOAT }))
.OUTPUT(selected_indices, TensorType({ DT_INT32 }))
.ATTR(center_point_box, AttrValue::INT { 0 })
.OP_END()
} // namespace hiai
// clang-format on

#endif