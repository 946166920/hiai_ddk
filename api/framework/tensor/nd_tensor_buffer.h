/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: nd tensor buffer
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
