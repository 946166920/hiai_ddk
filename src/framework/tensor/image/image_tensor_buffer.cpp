/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: image buffer implementation
 */
#include "tensor/image_tensor_buffer.h"
#include "image_tensor_buffer_impl.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"

namespace hiai {

std::shared_ptr<IImageTensorBuffer> CreateImageTensorBuffer(int32_t b, int32_t h, int32_t w, ImageFormat format,
    ImageColorSpace colorSpace, int32_t rotation)
{
    ImageTensorBufferInfo bufferInfo;
    NDTensorDesc desc;
    HIAI_NDTensorBuffer* ndTensor = nullptr;

    Status ret = ImageBufferInit(b, h, w, format, bufferInfo, desc, &ndTensor);
    if (ret != SUCCESS) {
        FMK_LOGE("HIAI_CreateImageBuffer ImageBufferInit failed");
        return nullptr;
    }
    std::shared_ptr<ImageTensorBufferImpl> imageTensor = nullptr;
    imageTensor = make_shared_nothrow<ImageTensorBufferImpl>(bufferInfo, ndTensor, desc);
    if (imageTensor == nullptr) {
        FMK_LOGE("HIAI_CreateImageBuffer create imageTensor failed");
        return nullptr;
    }
    imageTensor->SetRotation(rotation);
    imageTensor->SetColorSpace(colorSpace);
    return std::static_pointer_cast<IImageTensorBuffer>(imageTensor);
}

std::shared_ptr<IImageTensorBuffer> CreateImageTensorBufferFromHandle(const NativeHandle& handle, int32_t b, int32_t h,
    int32_t w, ImageFormat format, ImageColorSpace colorSpace, int32_t rotation)
{
    ImageTensorBufferInfo bufferInfo;
    NDTensorDesc desc;
    HIAI_NDTensorBuffer* ndTensor = nullptr;

    Status ret = ImageBufferInit(b, h, w, format, handle, bufferInfo, desc, &ndTensor);
    if (ret != SUCCESS) {
        FMK_LOGE("HIAI_CreateImageBuffer ImageBufferInit failed");
        return nullptr;
    }
    std::shared_ptr<ImageTensorBufferImpl> imageTensor = nullptr;
    imageTensor = make_shared_nothrow<ImageTensorBufferImpl>(bufferInfo, ndTensor, desc);
    if (imageTensor == nullptr) {
        FMK_LOGE("HIAI_CreateImageBuffer create imageTensor failed");
        return nullptr;
    }
    imageTensor->SetRotation(rotation);
    imageTensor->SetColorSpace(colorSpace);
    return std::static_pointer_cast<IImageTensorBuffer>(imageTensor);
}
} // namespace hiai
