/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: aipp tensor buffer
 */
#ifndef FRAMEWORK_TENSOR_AIPP_TENSOR_BUFFER_H
#define FRAMEWORK_TENSOR_AIPP_TENSOR_BUFFER_H

#include <vector>
#include "buffer.h"
#include "tensor_api_export.h"
#include "base/error_types.h"
#include "nd_tensor_buffer.h"
#include "image_process_config_types.h"

namespace hiai {
class HIAI_TENSOR_API_EXPORT IAIPPPara : public IBuffer {
public:
    // common set/get interface
    virtual ~IAIPPPara() = default;
    virtual uint32_t GetBatchCount() = 0;
    virtual Status SetInputIndex(uint32_t inputIndex) = 0;

    virtual int32_t GetInputIndex() = 0;
    virtual Status SetInputAippIndex(uint32_t inputAippIndex) = 0;
    virtual int32_t GetInputAippIndex() = 0;
    virtual Status SetInputFormat(ImageFormat inputFormat) = 0;
    virtual ImageFormat GetInputFormat() = 0;
    virtual std::vector<int32_t> GetInputShape() = 0;
    virtual Status SetInputShape(std::vector<int32_t>& shape) = 0;
    virtual Status SetCscPara(ImageFormat targetFormat, ImageColorSpace colorSpace = ImageColorSpace::JPEG) = 0;
    virtual Status SetCscPara(CscMatrixPara cscPara) = 0;
    virtual CscMatrixPara GetCscPara() = 0;

    virtual Status SetChannelSwapPara(ChannelSwapPara&& channelSwapPara) = 0;
    virtual ChannelSwapPara GetChannelSwapPara() = 0; // end common interface

    // set/get interface for batch, if batchIndex == -1 means for all batch
    virtual Status SetCropPara(int32_t batchIndex, CropPara&& cropPara) = 0;
    virtual CropPara GetCropPara(int32_t batchIndex) = 0;

    virtual Status SetResizePara(int32_t batchIndex, ResizePara&& resizePara) = 0;
    virtual ResizePara GetResizePara(int32_t batchIndex) = 0;

    virtual Status SetPaddingPara(int32_t batchIndex, PadPara&& paddingPara) = 0;
    virtual PadPara GetPaddingPara(int32_t batchIndex) = 0;

    virtual Status SetDtcPara(int32_t batchIndex, DtcPara&& dtcPara) = 0;
    virtual DtcPara GetDtcPara(int32_t batchIndex) = 0;
    // end batch interface
};

HIAI_TENSOR_API_EXPORT std::shared_ptr<IAIPPPara> CreateAIPPPara(uint32_t batchCount = 1);
} // namespace hiai

#endif // FRAMEWORK_BUFFER_AIPP_TENSOR_BUFFER_H
