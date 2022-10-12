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

#ifndef GE_MODEL_BASE_BUFFER_H
#define GE_MODEL_BASE_BUFFER_H

#include <cstdlib>
#include <cstdint>
/*lint -e1536*/
/*lint -e1539*/
/*lint -e1555*/
namespace ge {
class BaseBuffer {
public:
    BaseBuffer() : data_(nullptr), tensorBuffer_(nullptr), size_(0), isSupportZeroCpy_(false)
    {
    }

    BaseBuffer(void* data, std::size_t size, bool isSupportZeroCpy = false)
        : data_(static_cast<uint8_t*>(data)), tensorBuffer_(nullptr), size_(size), isSupportZeroCpy_(isSupportZeroCpy)
    {
    }

    BaseBuffer(std::uint8_t* data, std::size_t size, bool isSupportZeroCpy = false)
        : data_(data), tensorBuffer_(nullptr), size_(size), isSupportZeroCpy_(isSupportZeroCpy)
    {
    }

    ~BaseBuffer() = default;

    BaseBuffer& operator=(const BaseBuffer& other)
    {
        if (&other != this) {
            data_ = other.data_;
            size_ = other.size_;
            tensorBuffer_ = other.tensorBuffer_;
            isSupportZeroCpy_ = other.isSupportZeroCpy_;
        }
        return *this;
    }

    inline const std::uint8_t* GetData() const
    {
        return data_;
    }

    inline std::uint8_t* GetData()
    {
        return data_;
    }

    inline const void* GetTensorBuffer() const
    {
        return tensorBuffer_;
    }

    inline void* GetTensorBuffer()
    {
        return tensorBuffer_;
    }

    inline std::size_t GetSize() const
    {
        return size_;
    }

    // for compatibility
    inline const std::uint8_t* data() const
    {
        return GetData();
    }

    inline std::uint8_t* data()
    {
        return GetData();
    }

    inline std::size_t size() const
    {
        return GetSize();
    }

    inline void SetTensorBuffer(void* tensorBuffer)
    {
        tensorBuffer_ = tensorBuffer;
    }

    inline void SetData(std::uint8_t* data)
    {
        data_ = data;
    }

    inline void SetData(void* data)
    {
        data_ = static_cast<uint8_t*>(data);
    }

    inline void SetSize(std::size_t size)
    {
        size_ = size;
    }

    inline bool IsSupportZeroCpy() const
    {
        return isSupportZeroCpy_;
    }

private:
    std::uint8_t* data_;
    void* tensorBuffer_;
    std::size_t size_;
    bool isSupportZeroCpy_;
};
} // namespace ge
#endif // GE_MODEL_BASE_BUFFER_H
