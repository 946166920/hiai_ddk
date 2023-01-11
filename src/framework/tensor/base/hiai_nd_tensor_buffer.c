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
#include "framework/c/hiai_nd_tensor_buffer.h"

#include "securec.h"
#include "framework/infra/log/log.h"

#include "hiai_nd_tensor_buffer_local.h"
#include "hiai_nd_tensor_buffer_legacy.h"
#include "hiai_nd_tensor_buffer_util.h"

#include "util/hiai_foundation_dl_helper.h"

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_CreateNoCopy(
    const HIAI_NDTensorDesc* desc, const void* data, size_t dataSize)
{
    if (desc == NULL || data == NULL) {
        FMK_LOGE("desc or data is NULL.");
        return NULL;
    }

    if (dataSize == 0 || dataSize != HIAI_NDTensorDesc_GetByteSize(desc)) {
        FMK_LOGE("size is invalid");
        return NULL;
    }

    return HIAI_MR_NDTensorBuffer_Create(desc, (void*)data, dataSize, NULL, false, false);
}

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(const HIAI_NDTensorDesc* desc)
{
    if (desc == NULL) {
        FMK_LOGE("desc is NULL.");
        return NULL;
    }

    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU) {
        HIAI_MR_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNDTensorDescLegacy(desc);
        if (buffer != NULL) {
            return buffer;
        }
    }

    return HIAI_NDTensorBuffer_CreateLocalBufferFromNDTensorDesc(desc);
}

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_CreateFromSize(const HIAI_NDTensorDesc* desc, size_t size)
{
    if (desc == NULL) {
        FMK_LOGE("desc is NULL.");
        return NULL;
    }
    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU) {
        HIAI_MR_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSizeLegacy(desc, size);
        if (buffer != NULL) {
            return buffer;
        }
    }

    return HIAI_NDTensorBuffer_CreateLocalBufferFromSize(desc, size);
}

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_CreateFromFormat(
    const HIAI_NDTensorDesc* desc, size_t size, HIAI_ImageFormat format)
{
    if (desc == NULL || format >= HIAI_IMAGE_FORMAT_INVALID) {
        FMK_LOGE("desc or format is invalid.");
        return NULL;
    }

    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU) {
        HIAI_MR_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromFormatLegacy(desc, size, format);
        if (buffer != NULL) {
            return buffer;
        }
    }

    return HIAI_NDTensorBuffer_CreateLocalBufferFromSize(desc, size);
}

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_CreateFromBuffer(
    const HIAI_NDTensorDesc* desc, const void* data, size_t dataSize)
{
    HIAI_MR_NDTensorBuffer* buffer = HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(desc);
    if (buffer == NULL) {
        return NULL;
    }

    HIAI_NDTensorBufferV2* buffersV2 = (HIAI_NDTensorBufferV2*)buffer;

    if (buffersV2->size != dataSize) {
        FMK_LOGE("mismatch buffer size.");
        HIAI_MR_NDTensorBuffer_Destroy(&buffer);
        return NULL;
    }

    if (memcpy_s(buffersV2->data, buffersV2->size, data, dataSize) != 0) {
        FMK_LOGE("memcpy buffer failed.");
        HIAI_MR_NDTensorBuffer_Destroy(&buffer);
        return NULL;
    }
    return buffer;
}

HIAI_NDTensorDesc* HIAI_MR_NDTensorBuffer_GetNDTensorDesc(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return 0;
    }
    return ((HIAI_NDTensorBufferV2*)tensorBuffer)->desc;
}

size_t HIAI_MR_NDTensorBuffer_GetSize(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return 0;
    }
    return ((HIAI_NDTensorBufferV2*)tensorBuffer)->size;
}

void* HIAI_MR_NDTensorBuffer_GetData(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return NULL;
    }

    return ((HIAI_NDTensorBufferV2*)tensorBuffer)->data;
}

void* HIAI_MR_NDTensorBuffer_GetHandle(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return NULL;
    }
    return ((HIAI_NDTensorBufferV2*)tensorBuffer)->handle;
}

int32_t HIAI_MR_NDTensorBuffer_GetFd(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return -1;
    }

    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU) {
        return HIAI_NDTensorBuffer_GetFdLegacy(
            ((HIAI_NDTensorBufferV2*)tensorBuffer)->handle, ((HIAI_NDTensorBufferV2*)tensorBuffer)->isLegacy);
    }
    return -1;
}

int32_t HIAI_MR_NDTensorBuffer_GetOriginFd(const HIAI_MR_NDTensorBuffer* tensorBuffer)
{
    if (tensorBuffer == NULL) {
        return -1;
    }

    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU) {
        return HIAI_NDTensorBuffer_GetOriginFdLegacy(
            ((HIAI_NDTensorBufferV2*)tensorBuffer)->handle, ((HIAI_NDTensorBufferV2*)tensorBuffer)->isLegacy);
    }
    return -1;
}

void HIAI_MR_NDTensorBuffer_Destroy(HIAI_MR_NDTensorBuffer** tensorBuffer)
{
    if (tensorBuffer == NULL || *tensorBuffer == NULL) {
        return;
    }
    // 释放ndbuffer desc
    HIAI_NDTensorDesc* desc = HIAI_MR_NDTensorBuffer_GetNDTensorDesc(*tensorBuffer);
    HIAI_NDTensorDesc_Destroy(&desc);

    // 释放ndbuffer handle
    void* handle = ((HIAI_NDTensorBufferV2*)(*tensorBuffer))->handle;
    if (handle == NULL) {
        HIAI_NDTensorBuffer_ReleaseLocal(tensorBuffer);
    } else {
        HIAI_NDTensorBuffer_DestroyLegacy(((HIAI_NDTensorBufferV2*)(*tensorBuffer))->handle,
            ((HIAI_NDTensorBufferV2*)(*tensorBuffer))->isLegacy);
    }

    // 释放ndbuffer
    free(*tensorBuffer);
    *tensorBuffer = NULL;
}
