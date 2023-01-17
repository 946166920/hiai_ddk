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
#include "nd_tensor_buffer_impl.h"
#include "framework/c/hiai_nd_tensor_buffer.h"
#include "hiai_nd_tensor_buffer_def.h"
#include "infra/base/securestl.h"
#include "securec.h"
#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {

typedef struct HIAI_TensorBuffer HIAI_TensorBuffer;

NDTensorBufferImpl::NDTensorBufferImpl(HIAI_MR_NDTensorBuffer* impl) : impl_(impl)
{
}

NDTensorBufferImpl::NDTensorBufferImpl(HIAI_MR_NDTensorBuffer* impl, const NDTensorDesc& desc)
    : desc_(desc), impl_(impl)
{
}

NDTensorBufferImpl::~NDTensorBufferImpl()
{
    if (impl_ != nullptr) {
        HIAI_MR_NDTensorBuffer_Destroy(&impl_);
    }
}

HIAI_MR_NDTensorBuffer* NDTensorBufferImpl::GetImpl()
{
    return impl_;
}

void* NDTensorBufferImpl::GetData()
{
    if (impl_ == nullptr) {
        return nullptr;
    }

    return HIAI_MR_NDTensorBuffer_GetData(impl_);
}

size_t NDTensorBufferImpl::GetSize() const
{
    if (impl_ == nullptr) {
        return 0;
    }

    return HIAI_MR_NDTensorBuffer_GetSize(impl_);
}

static std::shared_ptr<HIAI_NDTensorDesc> Convert2CTensorDesc(const NDTensorDesc& tensorDesc)
{
    if (tensorDesc.dims.empty()) {
        return nullptr;
    }
    return std::shared_ptr<HIAI_NDTensorDesc>(
        HIAI_NDTensorDesc_Create(tensorDesc.dims.data(), tensorDesc.dims.size(),
            static_cast<HIAI_DataType>(tensorDesc.dataType), static_cast<HIAI_Format>(tensorDesc.format)),
        [](HIAI_NDTensorDesc* p) { HIAI_NDTensorDesc_Destroy(&p); });
}

std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(const NDTensorDesc& tensorDesc)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::shared_ptr<HIAI_NDTensorDesc> cTensorDesc = Convert2CTensorDesc(tensorDesc);
    if (cTensorDesc == nullptr) {
        return nullptr;
    }
    auto tmpBuffer = HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(cTensorDesc.get());
    if (tmpBuffer == nullptr) {
        return nullptr;
    }
    return make_shared_nothrow<NDTensorBufferImpl>(tmpBuffer, tensorDesc);
}

std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(const NDTensorDesc& tensorDesc, const void* data, size_t dataSize)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::shared_ptr<HIAI_NDTensorDesc> cTensorDesc = Convert2CTensorDesc(tensorDesc);
    if (cTensorDesc == nullptr) {
        return nullptr;
    }
    auto tmpBuffer = HIAI_MR_NDTensorBuffer_CreateFromBuffer(cTensorDesc.get(), data, dataSize);
    if (tmpBuffer == nullptr) {
        return nullptr;
    }
    return make_shared_nothrow<NDTensorBufferImpl>(tmpBuffer, tensorDesc);
}

std::shared_ptr<INDTensorBuffer> CreateNDTensorBufferNoCopy(
    const NDTensorDesc& tensorDesc, const void* data, size_t dataSize)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::shared_ptr<HIAI_NDTensorDesc> cTensorDesc = Convert2CTensorDesc(tensorDesc);
    if (cTensorDesc == nullptr) {
        return nullptr;
    }
    auto tmpBuffer = HIAI_MR_NDTensorBuffer_CreateNoCopy(cTensorDesc.get(), data, dataSize);
    if (tmpBuffer == nullptr) {
        return nullptr;
    }
    return make_shared_nothrow<NDTensorBufferImpl>(tmpBuffer, tensorDesc);
}

static std::shared_ptr<HIAI_NativeHandle> Convert2CNativeHandle(const NativeHandle& handle)
{
    return std::shared_ptr<HIAI_NativeHandle>(HIAI_NativeHandle_Create(handle.fd, handle.size, handle.offset),
        [](HIAI_NativeHandle* p) { HIAI_NativeHandle_Destroy(&p); });
}

HIAI_MR_NDTensorBuffer* CreateHIAINDTensorBuffer(const NDTensorDesc& tensorDesc, const NativeHandle& handle)
{
    std::shared_ptr<HIAI_NDTensorDesc> cTensorDesc = Convert2CTensorDesc(tensorDesc);
    if (cTensorDesc == nullptr) {
        return nullptr;
    }

    std::shared_ptr<HIAI_NativeHandle> cHandle = Convert2CNativeHandle(handle);
    if (cHandle == nullptr) {
        return nullptr;
    }

    auto ndTensor = HIAI_MR_NDTensorBuffer_CreateFromNativeHandle(cTensorDesc.get(), cHandle.get());
    if (ndTensor == nullptr) {
        return nullptr;
    }
    return ndTensor;
}

std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(const NDTensorDesc& tensorDesc, const NativeHandle& handle)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    auto tmpBuffer = CreateHIAINDTensorBuffer(tensorDesc, handle);
    if (tmpBuffer == nullptr) {
        return nullptr;
    }
    return make_shared_nothrow<NDTensorBufferImpl>(tmpBuffer, tensorDesc);
}

HIAI_MR_NDTensorBuffer* GetRawBufferFromNDTensorBuffer(const std::shared_ptr<INDTensorBuffer>& buffer)
{
    std::shared_ptr<NDTensorBufferImpl> bufferImpl = std::dynamic_pointer_cast<NDTensorBufferImpl>(buffer);
    if (bufferImpl == nullptr) {
        FMK_LOGE("invalid buffer");
        return nullptr;
    }
    return bufferImpl->GetImpl();
}

HIAI_MR_NDTensorBuffer* CreateHIAINDTensorBuffer(
    const NDTensorDesc& tensorDesc, size_t dataSize, HIAI_ImageFormat format)
{
    std::shared_ptr<HIAI_NDTensorDesc> cTensorDesc = Convert2CTensorDesc(tensorDesc);
    if (cTensorDesc == nullptr) {
        FMK_LOGE("create nd tensor buffer from format: Convert2CTensorDesc failed.");
        return nullptr;
    }

    auto ndTensor = HIAI_MR_NDTensorBuffer_CreateFromFormat(cTensorDesc.get(), dataSize, format);
    if (ndTensor == nullptr) {
        return nullptr;
    }

    if (HIAI_MR_NDTensorBuffer_GetSize(ndTensor) != dataSize) {
        FMK_LOGE("mismatch buffer size.");
        HIAI_MR_NDTensorBuffer_Destroy(&ndTensor);
        return nullptr;
    }
    return ndTensor;
}

std::shared_ptr<INDTensorBuffer> CreateNDTensorBuffer(
    const NDTensorDesc& tensorDesc, size_t dataSize, HIAI_ImageFormat format)
{
    auto ndTensor = CreateHIAINDTensorBuffer(tensorDesc, dataSize, format);
    if (ndTensor == nullptr) {
        return nullptr;
    }

    return make_shared_nothrow<NDTensorBufferImpl>(ndTensor, tensorDesc);
}

} // namespace hiai