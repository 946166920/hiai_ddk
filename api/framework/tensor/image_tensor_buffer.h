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
#ifndef _HIAI_IMAGE_BUFFER_H_
#define _HIAI_IMAGE_BUFFER_H_

#include <memory>

#include "nd_tensor_buffer.h"
#include "buffer.h"
#include "tensor_api_export.h"
#include "image_format.h"

namespace hiai {
class HIAI_TENSOR_API_EXPORT IImageTensorBuffer : public INDTensorBuffer {
public:
    ~IImageTensorBuffer() override = default;
    virtual int32_t Batch() const = 0;
    virtual int32_t Height() const = 0;
    virtual int32_t Width() const = 0;
    virtual ImageFormat Format() const = 0;
    virtual ImageColorSpace ColorSpace() const = 0;
    virtual int32_t Rotation() const = 0;
};

HIAI_TENSOR_API_EXPORT std::shared_ptr<IImageTensorBuffer> CreateImageTensorBuffer(int32_t b, int32_t h, int32_t w,
    ImageFormat format, ImageColorSpace colorSpace, int32_t rotation);
HIAI_TENSOR_API_EXPORT std::shared_ptr<IImageTensorBuffer> CreateImageTensorBufferFromHandle(const NativeHandle& handle,
    int32_t b, int32_t h, int32_t w, ImageFormat format, ImageColorSpace colorSpace, int32_t rotation);
}; // namespace hiai
#endif
