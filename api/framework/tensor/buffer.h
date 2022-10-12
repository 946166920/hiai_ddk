/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: buffer inferface
 */
#ifndef FRAMEWORK_TENSOR_BUFFER_H
#define FRAMEWORK_TENSOR_BUFFER_H
#include <cstddef>
#include <memory>

#include "tensor_api_export.h"

namespace hiai {
class IBuffer {
public:
    IBuffer() = default;
    virtual ~IBuffer() = default;

    virtual void* GetData() = 0;
    virtual size_t GetSize() const = 0;
};

// create empty buffer with [size] bytes long, buffer will release when IBuffer deconstruct
HIAI_TENSOR_API_EXPORT std::shared_ptr<IBuffer> CreateLocalBuffer(size_t size);

// create buffer from exist data, data must delete by user
HIAI_TENSOR_API_EXPORT std::shared_ptr<IBuffer> CreateLocalBuffer(void* data, size_t size, bool shouldOwn = false);
} // namespace hiai
#endif // FRAMEWORK_BUFFER_BUFFER_H
