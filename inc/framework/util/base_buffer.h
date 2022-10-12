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
#ifndef HIAI_FRAMEWORK_UTIL_BUFFER_H
#define HIAI_FRAMEWORK_UTIL_BUFFER_H

#include <cstdint>
#include <cstdlib>

#include "util/util_api_export.h"

namespace hiai {
class HIAI_UTIL_API_EXPORT BaseBuffer {
public:
    BaseBuffer();

    //  attention: if other has ownership, this constructor will copy data
    BaseBuffer(const BaseBuffer& other);

    BaseBuffer(uint8_t* data, size_t size, bool shouldOwn = false);

    ~BaseBuffer();

    bool Init(size_t size, uint8_t defaultVal = 0);

    bool CopyFrom(const uint8_t* data, size_t size);

    void SetData(uint8_t* data, size_t size, bool shouldOwn = false);

    const uint8_t* GetData() const;

    uint8_t* MutableData();

    size_t GetSize() const;

    void Clear();

private:
    // forbiden copy assignment, use CopyFrom
    BaseBuffer& operator=(const BaseBuffer& other) = delete;
    BaseBuffer& operator==(BaseBuffer&& other) = delete;

private:
    bool isOwner_;
    uint8_t* data_;
    size_t size_;
};
} // namespace hiai
#endif // GE_BUFFER_H
