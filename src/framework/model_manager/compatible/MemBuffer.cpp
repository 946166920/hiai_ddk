/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: MemBuffer
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
