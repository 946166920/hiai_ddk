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
#include "framework/c/hiai_tensor_aipp_para.h"

#include <pthread.h>
#include <stdlib.h>

#include "hiai_tensor_aipp_para_def.h"
#include "framework/infra/log/log.h"
#include "securec.h"
#include "hiai_tensor_aipp_para_local.h"
#include "hiai_tensor_aipp_para_legacy.h"
#include "util/hiai_foundation_dl_helper.h"

typedef struct HIAI_TensorAipp_FunctionDef {
    void* (*create)(uint32_t);
    void* (*getRawBuffer)(const void*);
    int32_t (*getRawBufferSize)(const void*);
    void* (*getHandle)(const void*);
    int32_t (*getInputIndex)(const void*);
    void (*setInputIndex)(void*, uint32_t);
    int32_t (*getInputAippIndex)(const void*);
    void (*setInputAippIndex)(void*, uint32_t);
    void (*destroy)(void*);
} HIAI_TensorAipp_FunctionDef;

static pthread_mutex_t g_functionDefInitMutex = PTHREAD_MUTEX_INITIALIZER;
static HIAI_TensorAipp_FunctionDef g_tensorAippFunctionDef;

static HIAI_TensorAipp_FunctionDef* GetTensorAippFunctionDef_Local(void)
{
    static int32_t isInited = 0;
    if (isInited == 0) {
        pthread_mutex_lock(&g_functionDefInitMutex);
        if (isInited == 0) {
            g_tensorAippFunctionDef.create = HIAI_TensorAippPara_CreateLocal;
            g_tensorAippFunctionDef.getRawBuffer = HIAI_TensorAippPara_GetRawBufferLocal;
            g_tensorAippFunctionDef.getRawBufferSize = HIAI_TensorAippPara_GetRawBufferSizeLocal;
            g_tensorAippFunctionDef.getHandle = HIAI_TensorAippPara_GetHandleLocal;
            g_tensorAippFunctionDef.getInputIndex = HIAI_TensorAippPara_GetInputIndexLocal;
            g_tensorAippFunctionDef.setInputIndex = HIAI_TensorAippPara_SetInputIndexLocal;
            g_tensorAippFunctionDef.getInputAippIndex = HIAI_TensorAippPara_GetInputAippIndexLocal;
            g_tensorAippFunctionDef.setInputAippIndex = HIAI_TensorAippPara_SetInputAippIndexLocal;
            g_tensorAippFunctionDef.destroy = HIAI_TensorAippPara_DestroyLocal;
            isInited = 1;
        }
        pthread_mutex_unlock(&g_functionDefInitMutex);
    }

    return &g_tensorAippFunctionDef;
}

static HIAI_TensorAipp_FunctionDef* GetTensorAippFunctionDef_Legacy(void)
{
    static int32_t isInited = 0;
    if (isInited == 0) {
        pthread_mutex_lock(&g_functionDefInitMutex);
        if (isInited == 0) {
            g_tensorAippFunctionDef.create = HIAI_TensorAippPara_CreateLegacy;
            g_tensorAippFunctionDef.getRawBuffer = HIAI_TensorAippPara_GetRawBufferLegacy;
            g_tensorAippFunctionDef.getRawBufferSize = HIAI_TensorAippPara_GetRawBufferSizeLegacy;
            g_tensorAippFunctionDef.getHandle = HIAI_TensorAippPara_GetHandleLegacy;
            g_tensorAippFunctionDef.getInputIndex = HIAI_TensorAippPara_GetInputIndexLegacy;
            g_tensorAippFunctionDef.setInputIndex = HIAI_TensorAippPara_SetInputIndexLegacy;
            g_tensorAippFunctionDef.getInputAippIndex = HIAI_TensorAippPara_GetInputAippIndexLegacy;
            g_tensorAippFunctionDef.setInputAippIndex = HIAI_TensorAippPara_SetInputAippIndexLegacy;
            g_tensorAippFunctionDef.destroy = HIAI_TensorAippPara_DestroyLegacy;
            isInited = 1;
        }
        pthread_mutex_unlock(&g_functionDefInitMutex);
    }

    return &g_tensorAippFunctionDef;
}

static HIAI_TensorAipp_FunctionDef* GetTensorAippFunctionDef(void)
{
    if (HIAI_Foundation_IsNpuSupport() == HIAI_SUPPORT_NPU &&
        HIAI_Foundation_GetSymbol("HIAI_TensorAipp_create") != NULL) {
        return GetTensorAippFunctionDef_Legacy();
    } else {
        return GetTensorAippFunctionDef_Local();
    }
}

HIAI_TensorAippPara* HIAI_TensorAippPara_Create(uint32_t batchNum)
{
    void* handle = GetTensorAippFunctionDef()->create(batchNum);
    if (handle == NULL) {
        handle = GetTensorAippFunctionDef_Local()->create(batchNum);
        if (handle == NULL) {
            return NULL;
        }
    }

    HIAI_TensorAippPara* aippPara = (HIAI_TensorAippPara*)malloc(sizeof(HIAI_TensorAippPara));
    if (aippPara == NULL) {
        FMK_LOGE("malloc failed.");
        goto FREE_HANDLE;
    }
    (void)memset_s(aippPara, sizeof(HIAI_TensorAippPara), 0, sizeof(HIAI_TensorAippPara));

    aippPara->handle = handle;
    aippPara->data = GetTensorAippFunctionDef()->getRawBuffer(handle);
    if (aippPara->data == NULL) {
        FMK_LOGE("craete invalid para.");
        goto FREE_PARA;
    }
    return aippPara;
FREE_PARA:
    free(aippPara);

FREE_HANDLE:
    GetTensorAippFunctionDef()->destroy(handle);
    return NULL;
}

HIAI_TensorAippPara* HIAI_TensorAippPara_CreateWithHandle(void* data, size_t size, void* handle)
{
    HIAI_TensorAippPara* aippPara = (HIAI_TensorAippPara*)malloc(sizeof(HIAI_TensorAippPara));
    if (aippPara == NULL) {
        FMK_LOGE("malloc HIAI_TensorAippPara failed.");
        return NULL;
    }

    aippPara->data = data;
    aippPara->size = size;
    aippPara->handle = handle;
    return aippPara;
}

void* HIAI_TensorAippPara_GetRawBuffer(HIAI_TensorAippPara* tensorAippPara)
{
    if (tensorAippPara == NULL) {
        return NULL;
    }
    return tensorAippPara->data;
}

int32_t HIAI_TensorAippPara_GetRawBufferSize(HIAI_TensorAippPara* tensorAippPara)
{
    if (tensorAippPara == NULL) {
        return 0;
    }
    return GetTensorAippFunctionDef()->getRawBufferSize(tensorAippPara->handle);
}

void* HIAI_TensorAippPara_GetHandle(HIAI_TensorAippPara* tensorAippPara)
{
    return tensorAippPara->handle;
}

int32_t HIAI_TensorAippPara_GetInputIndex(HIAI_TensorAippPara* tensorAippPara)
{
    if (tensorAippPara == NULL) {
        return -1;
    }

    return GetTensorAippFunctionDef()->getInputIndex(tensorAippPara->handle);
}

void HIAI_TensorAippPara_SetInputIndex(HIAI_TensorAippPara* tensorAippPara, uint32_t inputIndex)
{
    if (tensorAippPara == NULL) {
        return;
    }

    GetTensorAippFunctionDef()->setInputIndex(tensorAippPara->handle, inputIndex);
}

int32_t HIAI_TensorAippPara_GetInputAippIndex(HIAI_TensorAippPara* tensorAippPara)
{
    if (tensorAippPara == NULL) {
        return -1;
    }

    return GetTensorAippFunctionDef()->getInputAippIndex(tensorAippPara->handle);
}

void HIAI_TensorAippPara_SetInputAippIndex(HIAI_TensorAippPara* tensorAippPara, uint32_t inputAippIndex)
{
    if (tensorAippPara == NULL) {
        return;
    }

    GetTensorAippFunctionDef()->setInputAippIndex(tensorAippPara->handle, inputAippIndex);
}

void HIAI_TensorAippPara_Destroy(HIAI_TensorAippPara** aippParas)
{
    if (aippParas == NULL || *aippParas == NULL) {
        return;
    }

    GetTensorAippFunctionDef()->destroy((*aippParas)->handle);
    free(*aippParas);
    *aippParas = NULL;
}
