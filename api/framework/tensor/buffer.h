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
