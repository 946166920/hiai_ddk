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
#ifndef FRAMEWORK_BUFFER_SIMPLE_BUFFER_H
#define FRAMEWORK_BUFFER_SIMPLE_BUFFER_H
#include <memory>

#include "tensor/buffer.h"
#include "infra/base/base_buffer.h"

namespace hiai {

class LocalBuffer : public IBuffer {
public:
    LocalBuffer() = default;
    ~LocalBuffer() override = default;

    bool Init(size_t size);

    void SetData(void* data, size_t size, bool shouldOwn);

    inline void* GetData() override
    {
        return buffer_.MutableData();
    }

    inline size_t GetSize() const override
    {
        return buffer_.GetSize();
    }

private:
    BaseBuffer buffer_;
};

} // namespace hiai

#endif // FRAMEWORK_BUFFER_SIMPLE_BUFFER_H