/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: image buffer base definition
 */
#ifndef TENSOR_IMAGE_BUFFER_IMPL_H
#define TENSOR_IMAGE_BUFFER_IMPL_H

#include "tensor/image_tensor_buffer.h"
#include "tensor/base/nd_tensor_buffer_impl.h"
#include "framework/infra/log/log.h"
#include "base/error_types.h"

namespace hiai {

struct ImageTensorBufferInfo {
    int32_t rotation {0};
    int32_t batch {0};
    int32_t height {0};
    int32_t width {0};
    int32_t channel {0};
    ImageFormat format {ImageFormat::INVALID};
    ImageColorSpace colorSpace {ImageColorSpace::BT_601_NARROW};
};

class ImageTensorBufferImpl : public IImageTensorBuffer, public NDTensorBufferImpl {
public:
    ImageTensorBufferImpl(ImageTensorBufferInfo& bufferInfo, HIAI_NDTensorBuffer* impl) : NDTensorBufferImpl(impl)
    {
        bufferInfo_.batch = bufferInfo.batch;
        bufferInfo_.channel = bufferInfo.channel;
        bufferInfo_.height = bufferInfo.height;
        bufferInfo_.rotation = bufferInfo.rotation;
        bufferInfo_.width = bufferInfo.width;
        bufferInfo_.format = bufferInfo.format;
    }

    ImageTensorBufferImpl(ImageTensorBufferInfo& bufferInfo, HIAI_NDTensorBuffer* impl, const NDTensorDesc& desc)
        : NDTensorBufferImpl(impl, desc)
    {
        bufferInfo_.batch = bufferInfo.batch;
        bufferInfo_.channel = bufferInfo.channel;
        bufferInfo_.height = bufferInfo.height;
        bufferInfo_.rotation = bufferInfo.rotation;
        bufferInfo_.width = bufferInfo.width;
        bufferInfo_.format = bufferInfo.format;
    }

    ~ImageTensorBufferImpl() override
    {
    }

    int32_t Batch() const override
    {
        return bufferInfo_.batch;
    }

    int32_t Height() const override
    {
        return bufferInfo_.height;
    }

    int32_t Width() const override
    {
        return bufferInfo_.width;
    }

    int32_t Channel() const
    {
        return bufferInfo_.channel;
    }

    ImageFormat Format() const override
    {
        return bufferInfo_.format;
    }

    void SetColorSpace(const ImageColorSpace colorSpace)
    {
        if (colorSpace < ImageColorSpace::BT_601_NARROW || colorSpace > ImageColorSpace::BT_709_NARROW) {
            FMK_LOGE("colorSpace invalid");
            return;
        }
        bufferInfo_.colorSpace = colorSpace;
    }

    ImageColorSpace ColorSpace() const override
    {
        return bufferInfo_.colorSpace;
    }

    int32_t Rotation() const override
    {
        return bufferInfo_.rotation;
    }

    void SetRotation(const int32_t rotation)
    {
        bufferInfo_.rotation = rotation;
    }

    void* GetData() override
    {
        return NDTensorBufferImpl::GetData();
    }

    size_t GetSize() const override
    {
        return NDTensorBufferImpl::GetSize();
    }

    const NDTensorDesc& GetTensorDesc() const override
    {
        return NDTensorBufferImpl::GetTensorDesc();
    }

private:
    ImageTensorBufferInfo bufferInfo_;
};

Status ImageBufferInit(const int32_t b, const int32_t h, const int32_t w, const ImageFormat format,
    ImageTensorBufferInfo& bufferInfo, NDTensorDesc& desc, HIAI_NDTensorBuffer** tensor);
Status ImageBufferInit(const int32_t b, const int32_t h, const int32_t w, const ImageFormat format,
    const NativeHandle& handle, ImageTensorBufferInfo& bufferInfo, NDTensorDesc& desc, HIAI_NDTensorBuffer** tensor);
} // namespace hiai
#endif // TENSOR_IMAGE_BUFFER_IMPL_H