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
#include "hiai_nd_tensor_buffer_util.h"

#include "securec.h"
#include "framework/infra/log/log.h"

#include "hiai_nd_tensor_buffer_def.h"

HIAI_MR_NDTensorBuffer* HIAI_MR_NDTensorBuffer_Create(
    const HIAI_NDTensorDesc* desc, void* data, size_t size, void* handle, bool isOwner, bool isLegacy)
{
    HIAI_NDTensorBufferV2* ndBuffer = (HIAI_NDTensorBufferV2*)malloc(sizeof(HIAI_NDTensorBufferV2));
    MALLOC_NULL_CHECK_RET_VALUE(ndBuffer, NULL);

    ndBuffer->desc = HIAI_NDTensorDesc_Clone(desc);
    if (ndBuffer->desc == NULL) {
        free(ndBuffer);
        return NULL;
    }

    ndBuffer->size = size;
    ndBuffer->data = data;
    ndBuffer->handle = handle;
    ndBuffer->owner = isOwner;
    ndBuffer->isLegacy = isLegacy;

    return (HIAI_MR_NDTensorBuffer*)ndBuffer;
}
