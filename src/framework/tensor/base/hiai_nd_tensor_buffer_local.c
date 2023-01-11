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

#include "hiai_nd_tensor_buffer_local.h"

#include "securec.h"
#include "framework/infra/log/log.h"
#include "hiai_nd_tensor_buffer_util.h"

HIAI_MR_NDTensorBuffer* HIAI_NDTensorBuffer_CreateLocalBufferFromSize(const HIAI_NDTensorDesc* desc, size_t size)
{
    if (size == 0) {
        FMK_LOGE("size is invalid, malloc failed.");
        return NULL;
    }
    void* buffer = malloc(size);
    MALLOC_NULL_CHECK_RET_VALUE(buffer, NULL);

    (void)memset_s(buffer, size, 0, size);

    HIAI_MR_NDTensorBuffer* ndBuffer = HIAI_MR_NDTensorBuffer_Create(desc, buffer, size, NULL, true, false);
    if (ndBuffer == NULL) {
        FMK_LOGE("HIAI_NDTensorBuffer_Create failed.");
        free(buffer);
        return NULL;
    }
    return ndBuffer;
}

HIAI_MR_NDTensorBuffer* HIAI_NDTensorBuffer_CreateLocalBufferFromNDTensorDesc(const HIAI_NDTensorDesc* desc)
{
    size_t totalByteSize = HIAI_NDTensorDesc_GetByteSize(desc);
    return HIAI_NDTensorBuffer_CreateLocalBufferFromSize(desc, totalByteSize);
}

void HIAI_NDTensorBuffer_ReleaseLocal(HIAI_MR_NDTensorBuffer** buffer)
{
    // 释放ndbuffer data
    void* data = HIAI_MR_NDTensorBuffer_GetData(*buffer);
    if (((HIAI_NDTensorBufferV2*)(*buffer))->owner && data != NULL) {
        free(data);
    }
}