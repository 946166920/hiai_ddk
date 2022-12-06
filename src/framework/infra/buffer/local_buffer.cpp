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
#include "local_buffer.h"

#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"
namespace hiai {

bool LocalBuffer::Init(size_t size)
{
    return buffer_.Init(size);
}

void LocalBuffer::SetData(void* data, size_t size, bool shouldOwn)
{
    buffer_.SetData((uint8_t*)data, size, shouldOwn);
}

std::shared_ptr<IBuffer> CreateLocalBuffer(size_t size)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (size == 0) {
        FMK_LOGE("invalid input para size is %zu.", size);
        return nullptr;
    }
    std::shared_ptr<LocalBuffer> buffer = make_shared_nothrow<LocalBuffer>();
    if (buffer == nullptr) {
        FMK_LOGE("buffer is nullptr.");
        return nullptr;
    }

    if (!buffer->Init(size)) {
        return nullptr;
    }
    return buffer;
}

std::shared_ptr<IBuffer> CreateLocalBuffer(void* data, size_t size, bool shouldOwn)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (data == nullptr || size == 0) {
        FMK_LOGE("invalid input para. data is nullptr or size is %zu.", size);
        return nullptr;
    }
    std::shared_ptr<LocalBuffer> buffer = make_shared_nothrow<LocalBuffer>();
    if (buffer == nullptr) {
        FMK_LOGE("malloc failed.");
        return nullptr;
    }

    buffer->SetData(data, size, shouldOwn);
    return buffer;
}

} // namespace hiai