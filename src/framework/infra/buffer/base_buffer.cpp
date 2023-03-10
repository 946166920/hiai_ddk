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
#include "infra/base/base_buffer.h"
#include <new>
#include "securec.h"
#include "framework/infra/log/log.h"

namespace hiai {
BaseBuffer::BaseBuffer()
{
}

BaseBuffer::BaseBuffer(const BaseBuffer& other)
{
    if (!other.isOwner_) {
        isOwner_ = other.isOwner_;
        data_ = other.data_;
        size_ = other.size_;
    } else {
        isOwner_ = false;
        data_ = nullptr;
        size_ = 0;
        CopyFrom(other.data_, other.size_);
    }
}

BaseBuffer::BaseBuffer(uint8_t* data, size_t size, bool shouldOwn) : isOwner_(shouldOwn), data_(data), size_(size)
{
}

BaseBuffer::BaseBuffer(uint8_t* data, size_t size, std::function<void (uint8_t*)> freeFunc)
    : data_(data), size_(size), freeFunc_(freeFunc)
{
    isOwner_ = true;
}

BaseBuffer::~BaseBuffer()
{
    Clear();
}

bool BaseBuffer::Init(size_t size, uint8_t defaultVal)
{
    if (size == 0) {
        return false;
    }

    Clear();

    data_ = new (std::nothrow) uint8_t[size];
    if (data_ == nullptr) {
        FMK_LOGE("new failed.");
        return false;
    }
    if (memset_s(data_, size, defaultVal, size) != EOK) {
        FMK_LOGE("buffer memset failed.");
        delete[] data_;
        data_ = nullptr;
        return false;
    }

    size_ = size;
    isOwner_ = true;
    return true;
}

bool BaseBuffer::CopyFrom(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }

    Clear();

    data_ = new (std::nothrow) uint8_t[size];
    if (data_ == nullptr) {
        FMK_LOGE("new failed.");
        return false;
    }
    if (memcpy_s(data_, size, data, size) != EOK) {
        FMK_LOGE("buffer memset failed.");
        delete[] data_;
        data_ = nullptr;
        return false;
    }

    size_ = size;
    isOwner_ = true;
    return true;
}

void BaseBuffer::SetData(uint8_t* data, size_t size, bool shouldOwn)
{
    Clear();

    data_ = data;
    size_ = size;
    isOwner_ = shouldOwn;
}

const uint8_t* BaseBuffer::GetData() const
{
    return data_;
}

uint8_t* BaseBuffer::MutableData()
{
    return data_;
}

size_t BaseBuffer::GetSize() const
{
    return size_;
}

void BaseBuffer::Clear()
{
    if (!isOwner_ || data_ == nullptr) {
        return;
    }

    if (freeFunc_ != nullptr) {
        freeFunc_(data_);
    } else {
        delete[] data_;
    }

    data_ = nullptr;
    size_ = 0;
}
} // namespace hiai
