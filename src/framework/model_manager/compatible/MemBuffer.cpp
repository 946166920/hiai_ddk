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
#include "compatible/MemBuffer.h"
#include "framework/infra/log/log.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {

uint32_t MemBuffer::GetMemBufferSize()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return size_;
}

void MemBuffer::SetMemBufferSize(uint32_t size)
{
    size_ = size;
}

void* MemBuffer::GetMemBufferData()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return data_;
}

void MemBuffer::SetMemBufferData(void* data)
{
    data_ = data;
}

void MemBuffer::SetServerMem(void* serverMem)
{
    servermem_ = serverMem;
}

void MemBuffer::SetAppAllocFlag(bool isAppAlloc)
{
    isAppAlloc_ = isAppAlloc;
}

void* MemBuffer::GetServerMem()
{
    return servermem_;
}

bool MemBuffer::GetAppAllocFlag()
{
    return isAppAlloc_;
}

} // namespace hiai
