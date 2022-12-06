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
#ifndef FRAMEWORK_BUFFER_ND_TENSOR_BUFFER_IMPL_H
#define FRAMEWORK_BUFFER_ND_TENSOR_BUFFER_IMPL_H
#include "tensor/nd_tensor_buffer.h"
#include "tensor/tensor_api_export.h"
#include "framework/c/hiai_nd_tensor_buffer.h"

namespace hiai {

class NDTensorBufferImpl : public INDTensorBuffer {
public:
    NDTensorBufferImpl(HIAI_NDTensorBuffer* impl);
    NDTensorBufferImpl(HIAI_NDTensorBuffer* impl, const NDTensorDesc& desc);
    ~NDTensorBufferImpl() override;

    HIAI_NDTensorBuffer* GetImpl();

    void* GetData() override;

    size_t GetSize() const override;

    const NDTensorDesc& GetTensorDesc() const override
    {
        return desc_;
    }

private:
    NDTensorBufferImpl(const NDTensorBufferImpl&) = delete;
    NDTensorBufferImpl& operator=(const NDTensorBufferImpl&) = delete;

private:
    NDTensorDesc desc_;
    HIAI_NDTensorBuffer* impl_ {nullptr};
};

HIAI_NDTensorBuffer* CreateHIAINDTensorBuffer(const NDTensorDesc& tensorDesc, size_t dataSize, HIAI_ImageFormat format);
std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(
    const NDTensorDesc& tensorDesc, size_t dataSize, HIAI_ImageFormat format);
HIAI_NDTensorBuffer* CreateHIAINDTensorBuffer(const NDTensorDesc& tensorDesc, const NativeHandle& handle);
HIAI_TENSOR_API_EXPORT HIAI_NDTensorBuffer* GetRawBufferFromNDTensorBuffer(
    const std::shared_ptr<INDTensorBuffer>& buffer);
} // namespace hiai

#endif