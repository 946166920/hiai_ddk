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
#include "hiai_tensor_aipp_para_local.h"

#include <stdlib.h>

#include "framework/infra/log/log.h"
#include "securec.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/c/hiai_base_types.h"

typedef struct HIAI_TensorAippParaLocal {
    uint32_t inputIndex;
    uint32_t inputAippIndex;
    void* buffer;
    size_t size;
    void* handle;
} HIAI_TensorAippParaLocal;

void* HIAI_TensorAippPara_CreateLocal(uint32_t batchNum)
{
    static const uint32_t maxBatchNum = 128;
    if (batchNum >= maxBatchNum) {
        FMK_LOGE("too large batch num");
        return NULL;
    }

    size_t size = sizeof(HIAI_TensorAippCommPara) + sizeof(HIAI_TensorAippBatchPara) * batchNum;
    void* buffer = malloc(size);
    if (buffer == NULL) {
        FMK_LOGE("malloc failed.");
        return NULL;
    }
    (void)memset_s(buffer, size, 0, size);

    HIAI_TensorAippParaLocal* localPara = (HIAI_TensorAippParaLocal*)malloc(sizeof(HIAI_TensorAippParaLocal));
    if (localPara == NULL) {
        FMK_LOGE("malloc failed.");
        goto FREE_BUFFER;
    }
    (void)memset_s(localPara, sizeof(HIAI_TensorAippParaLocal), 0, sizeof(HIAI_TensorAippParaLocal));

    localPara->buffer = buffer;
    localPara->size = size;
    return localPara;

FREE_BUFFER:
    free(buffer);
    return NULL;
}

void* HIAI_TensorAippPara_GetRawBufferLocal(const void* handle)
{
    if (handle == NULL) {
        return NULL;
    }
    return ((HIAI_TensorAippParaLocal*)handle)->buffer;
}

int32_t HIAI_TensorAippPara_GetRawBufferSizeLocal(const void* handle)
{
    if (handle == NULL) {
        return 0;
    }
    return (int32_t)((HIAI_TensorAippParaLocal*)handle)->size;
}

void* HIAI_TensorAippPara_GetHandleLocal(const void* handle)
{
    (void)handle;
    return NULL;
}

int32_t HIAI_TensorAippPara_GetInputIndexLocal(const void* handle)
{
    if (handle == NULL) {
        return -1;
    }
    return (int32_t)((HIAI_TensorAippParaLocal*)handle)->inputIndex;
}

void HIAI_TensorAippPara_SetInputIndexLocal(void* handle, uint32_t inputIndex)
{
    if (handle == NULL) {
        return;
    }
    ((HIAI_TensorAippParaLocal*)handle)->inputIndex = inputIndex;
}

int32_t HIAI_TensorAippPara_GetInputAippIndexLocal(const void* handle)
{
    if (handle == NULL) {
        return -1;
    }
    return (int32_t)((HIAI_TensorAippParaLocal*)handle)->inputAippIndex;
}

void HIAI_TensorAippPara_SetInputAippIndexLocal(void* handle, uint32_t inputAippIndex)
{
    if (handle == NULL) {
        return;
    }
    ((HIAI_TensorAippParaLocal*)handle)->inputAippIndex = inputAippIndex;
}

void HIAI_TensorAippPara_DestroyLocal(void* handle)
{
    if (handle == NULL) {
        return;
    }
    HIAI_TensorAippParaLocal* para = (HIAI_TensorAippParaLocal*)handle;
    if (para->buffer != NULL) {
        free(para->buffer);
    }
    free(handle);
}