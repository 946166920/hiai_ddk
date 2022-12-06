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
