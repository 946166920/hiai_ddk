/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: buffer.cpp
 */
#include "graph/buffer.h"
#include "securec.h"

namespace ge {
Buffer::Buffer() : Buffer(new (std::nothrow) std::string(), true)
{
}

Buffer::Buffer(std::string* buffer, bool isOwner) : buffer_(buffer), isOwner_(isOwner)
{
}

Buffer::Buffer(const Buffer& other) : Buffer()
{
    if (buffer_ != nullptr && other.buffer_ != nullptr) {
        *buffer_ = *other.buffer_;
    }
}

Buffer::Buffer(std::size_t size, std::uint8_t defaultVal) : Buffer()
{
    if (buffer_ != nullptr) {
        buffer_->resize(size, defaultVal);
    }
}

Buffer::~Buffer()
{
    if (isOwner_) {
        delete buffer_;
    }
    buffer_ = nullptr;
}

void Buffer::CopyFrom(const std::uint8_t* data, std::size_t bufferSize, Buffer& buffer)
{
    if (data != nullptr && bufferSize != 0 && buffer.buffer_ != nullptr) {
        *buffer.buffer_ = std::string(reinterpret_cast<const char*>(data), bufferSize);
    }
}

void Buffer::Resize(std::size_t bufferSize)
{
    if (buffer_ != nullptr) {
        buffer_->resize(bufferSize);
    }
}

void Buffer::Clear()
{
    if (buffer_ != nullptr) {
        buffer_->resize(0);
        buffer_->shrink_to_fit();
    }
}

Buffer& Buffer::operator=(const Buffer& other)
{
    if (&other == this) {
        return *this;
    }
    if (buffer_ != nullptr && other.buffer_ != nullptr) {
        Clear();
        *buffer_ = *other.buffer_;
    }
    return *this;
}

const std::uint8_t* Buffer::GetData() const
{
    if (buffer_ != nullptr && buffer_->size() != 0) {
        return reinterpret_cast<const std::uint8_t*>(buffer_->data());
    }
    return nullptr;
}

std::uint8_t* Buffer::MutableData()
{
    if (buffer_ != nullptr && buffer_->size() != 0) {
        return reinterpret_cast<std::uint8_t*>(const_cast<char*>(buffer_->data()));
    }
    return nullptr;
}

std::size_t Buffer::GetSize() const
{
    if (buffer_ != nullptr) {
        return buffer_->size();
    }
    return 0;
}

void Buffer::RefTo(const Buffer& buffer)
{
    buffer_ = buffer.buffer_;
    isOwner_ = buffer.isOwner_;
}
} // namespace ge
