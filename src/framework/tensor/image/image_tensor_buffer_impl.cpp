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
#include "image_tensor_buffer_impl.h"

#include <map>
#include "framework/infra/log/log.h"

namespace hiai {

static const std::map<ImageFormat, uint32_t> sizeScaleMap = {
    {ImageFormat::YUV420SP, 3},
    {ImageFormat::XRGB8888, 4},
    {ImageFormat::YUV400, 1},
    {ImageFormat::ARGB8888, 4},
    {ImageFormat::YUYV, 2},
    {ImageFormat::YUV422SP, 2},
    {ImageFormat::AYUV444, 4},
    {ImageFormat::RGB888, 3},
    {ImageFormat::BGR888, 3},
};
static const std::map<ImageFormat, int32_t> channelMap = {
    {ImageFormat::YUV420SP, 3},
    {ImageFormat::XRGB8888, 4},
    {ImageFormat::YUV400, 1},
    {ImageFormat::ARGB8888, 4},
    {ImageFormat::YUYV, 3},
    {ImageFormat::YUV422SP, 3},
    {ImageFormat::AYUV444, 4},
    {ImageFormat::RGB888, 3},
    {ImageFormat::BGR888, 3},
};

static Status GetChannelSizeForFormat(const ImageFormat format, int32_t& channel)
{
    auto iter = channelMap.find(format);
    if (iter == channelMap.end()) {
        FMK_LOGE("GetChannelSizeForFormat failed: ImageForamt %d is not supported.", format);
        return FAILURE;
    }
    channel = iter->second;
    return SUCCESS;
}

static Status GetSizeScaleForFormat(const ImageFormat format, uint32_t& sizeScale)
{
    auto iter = sizeScaleMap.find(format);
    if (iter == sizeScaleMap.end()) {
        FMK_LOGE("GetSizeScaleForFormat failed: ImageForamt %d is not supported.", format);
        return FAILURE;
    }
    sizeScale = iter->second;
    return SUCCESS;
}

static bool CheckInputOverflow(
    const uint32_t& number, const uint32_t& size, const uint32_t& h, const uint32_t& w, const uint32_t& formatSize)
{
    if (number > 0 && size > 0 && h > 0 && w > 0 && formatSize > 0) {
        if (number > (INT32_MAX / size / h / w / formatSize)) {
            return false;
        }
        return true;
    } else {
        return false;
    }
}

static Status InitTensorInfo(const int32_t b, const int32_t h, const int32_t w, const ImageFormat format,
    ImageTensorBufferInfo& bufferInfo, size_t& size)
{
    int32_t channel;
    if (GetChannelSizeForFormat(format, channel) != SUCCESS) {
        FMK_LOGE("GetChannelSizeForFormat failed");
        return FAILURE;
    }
    uint32_t sizeScale = 1;
    if (GetSizeScaleForFormat(format, sizeScale) != SUCCESS) {
        FMK_LOGE("GetSizeScaleForFormat failed");
        return FAILURE;
    }
    if (!CheckInputOverflow(b, sizeScale, h, w, 1)) {
        FMK_LOGE("CheckInputOveflow failed");
        return FAILURE;
    }

    size = static_cast<size_t>(b * h * w * sizeScale);
    if (format == ImageFormat::YUV420SP) {
        size = size / 2;
    }

    bufferInfo.batch = b;
    bufferInfo.height = h;
    bufferInfo.width = w;
    bufferInfo.format = format;
    bufferInfo.channel = channel;
    return SUCCESS;
}

Status ImageBufferInit(const int32_t b, const int32_t h, const int32_t w, const ImageFormat format,
    ImageTensorBufferInfo& bufferInfo, NDTensorDesc& desc, HIAI_MR_NDTensorBuffer** tensor)
{
    size_t size = 0;
    if (InitTensorInfo(b, h, w, format, bufferInfo, size) != SUCCESS) {
        FMK_LOGE("init tensor info failed.");
        return FAILURE;
    }

    desc.dims = {b, bufferInfo.channel, h, w};
    desc.dataType = DataType::UINT8;
    desc.format = hiai::Format::NCHW;

    HIAI_MR_NDTensorBuffer* ndTensor = CreateHIAINDTensorBuffer(desc, size, static_cast<HIAI_ImageFormat>(format));
    if (ndTensor == nullptr) {
        FMK_LOGE("ndTensor is nullptr");
        return FAILURE;
    }

    *tensor = ndTensor;

    return SUCCESS;
}

Status ImageBufferInit(const int32_t b, const int32_t h, const int32_t w, const ImageFormat format,
    const NativeHandle& handle, ImageTensorBufferInfo& bufferInfo, NDTensorDesc& desc, HIAI_MR_NDTensorBuffer** tensor)
{
    size_t size = 0;
    if (InitTensorInfo(b, h, w, format, bufferInfo, size) != SUCCESS) {
        FMK_LOGE("init tensor info failed.");
        return FAILURE;
    }

    desc.dims = {b, bufferInfo.channel, h, w};
    desc.dataType = DataType::UINT8;
    desc.format = hiai::Format::NCHW;

    HIAI_MR_NDTensorBuffer* ndTensor = CreateHIAINDTensorBuffer(desc, handle);
    if (ndTensor == nullptr) {
        FMK_LOGE("ndTensor is nullptr");
        return FAILURE;
    }

    *tensor = ndTensor;

    return SUCCESS;
}
} // namespace hiai