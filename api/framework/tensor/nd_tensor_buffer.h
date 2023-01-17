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
#ifndef FRAMEWORK_TENSOR_ND_TENSOR_BUFFER_H
#define FRAMEWORK_TENSOR_ND_TENSOR_BUFFER_H

#include <utility>
#include <vector>
#include <memory>

#include "buffer.h"
#include "tensor_api_export.h"
#include "nd_tensor_desc.h"

#ifdef __cplusplus
extern "C" {
#endif

struct HIAI_NDTensorBuffer;

#ifdef __cplusplus
}
#endif

namespace hiai {
class INDTensorBuffer : public IBuffer {
public:
    virtual const NDTensorDesc& GetTensorDesc() const = 0;
};

HIAI_TENSOR_API_EXPORT std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(const NDTensorDesc& tensorDesc);
HIAI_TENSOR_API_EXPORT std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(
    const NDTensorDesc& tensorDesc, const void* data, size_t dataSize);
HIAI_TENSOR_API_EXPORT std::shared_ptr<INDTensorBuffer> CreateNDTensorBufferNoCopy(
    const NDTensorDesc& tensorDesc, const void* data, size_t dataSize);
HIAI_TENSOR_API_EXPORT std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(
    const NDTensorDesc& tensorDesc, const NativeHandle& handle);
} // namespace hiai
#endif // FRAMEWORK_BUFFER_ND_TENSOR_BUFFER_H
