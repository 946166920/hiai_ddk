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
#include "hiai_tensor_aipp_para_legacy.h"

#include "framework/c/hiai_tensor_aipp_para.h"
#include "framework/infra/log/log.h"

#include "util/hiai_foundation_dl_helper.h"

void* HIAI_TensorAippPara_CreateLegacy(uint32_t batchNum)
{
    static const char* createFuncName = "HIAI_TensorAipp_create";
    void* (*createFunc)(uint32_t) = (void* (*)(uint32_t))HIAI_Foundation_GetSymbol(createFuncName);
    if (createFunc == NULL) {
        FMK_LOGE("sym %s not found.", createFuncName);
        return NULL;
    }

    return createFunc(batchNum);
}

void* HIAI_TensorAippPara_GetRawBufferLegacy(const void* handle)
{
    static const char* getRawBufferFuncName = "HIAI_TensorAipp_getRawBuffer";
    void* (*getRawBufferFunc)(void*) = (void* (*)(void*))HIAI_Foundation_GetSymbol(getRawBufferFuncName);
    if (getRawBufferFunc == NULL) {
        FMK_LOGE("sym %s not found.", getRawBufferFuncName);
        return NULL;
    }

    return getRawBufferFunc((HIAI_MR_TensorAippPara*)handle);
}

int32_t HIAI_TensorAippPara_GetRawBufferSizeLegacy(const void* handle)
{
    int32_t size = 0;
    static const char* getRawBufferSizeFuncName = "HIAI_TensorAipp_getRawBufferSize";
    int32_t (*getRawBufferSizeFunc)(void*) = (int32_t(*)(void*))HIAI_Foundation_GetSymbol(getRawBufferSizeFuncName);
    if (getRawBufferSizeFunc != NULL) {
        size = getRawBufferSizeFunc((HIAI_MR_TensorAippPara*)handle);
    }

    if (size <= 0) {
        void* bufferPara = HIAI_TensorAippPara_GetRawBufferLegacy(handle);
        if (bufferPara == NULL) {
            FMK_LOGE("HIAI_TensorAippPara_GetRawBufferLegacy failed.");
            return 0;
        }
        HIAI_MR_TensorAippCommPara* commPara = (HIAI_MR_TensorAippCommPara*)(bufferPara);
        if (commPara == NULL) {
            FMK_LOGE("revert to HIAI_TensorAippCommPara failed.");
            return 0;
        }
        return sizeof(HIAI_MR_TensorAippCommPara) + sizeof(HIAI_MR_TensorAippBatchPara) * commPara->batchNum;
    }
    return size;
}

void* HIAI_TensorAippPara_GetHandleLegacy(const void* handle)
{
    return (void*)handle;
}

int32_t HIAI_TensorAippPara_GetInputIndexLegacy(const void* handle)
{
    if (handle == NULL) {
        return -1;
    }

    static const char* getInputIndexFuncName = "HIAI_TensorAipp_getInputIndex";
    int32_t (*getInutIndexFunc)(void*) = (int32_t(*)(void*))HIAI_Foundation_GetSymbol(getInputIndexFuncName);
    if (getInutIndexFunc == NULL) {
        FMK_LOGE("sym %s not found.", getInputIndexFuncName);
        return -1;
    }

    return getInutIndexFunc((HIAI_MR_TensorAippPara*)handle);
}

void HIAI_TensorAippPara_SetInputIndexLegacy(void* handle, uint32_t inputIndex)
{
    if (handle == NULL) {
        return;
    }

    static const char* setInputIndexFuncName = "HIAI_TensorAipp_setInputIndex";
    void (*setInutIndexFunc)(void*, uint32_t) =
        (void (*)(void*, uint32_t))HIAI_Foundation_GetSymbol(setInputIndexFuncName);
    if (setInutIndexFunc == NULL) {
        FMK_LOGE("sym %s not found.", setInputIndexFuncName);
        return;
    }

    setInutIndexFunc(handle, inputIndex);
}

int32_t HIAI_TensorAippPara_GetInputAippIndexLegacy(const void* handle)
{
    if (handle == NULL) {
        return -1;
    }

    static const char* getInputIndexFuncName = "HIAI_TensorAipp_getInputAippIndex";
    int32_t (*getInutIndexFunc)(void*) = (int32_t(*)(void*))HIAI_Foundation_GetSymbol(getInputIndexFuncName);
    if (getInutIndexFunc == NULL) {
        FMK_LOGE("sym %s not found.", getInputIndexFuncName);
        return -1;
    }

    return getInutIndexFunc((HIAI_MR_TensorAippPara*)handle);
}

void HIAI_TensorAippPara_SetInputAippIndexLegacy(void* handle, uint32_t inputAippIndex)
{
    if (handle == NULL) {
        return;
    }

    static const char* setInputIndexFuncName = "HIAI_TensorAipp_setInputAippIndex";
    void (*setInutIndexFunc)(void*, uint32_t) =
        (void (*)(void*, uint32_t))HIAI_Foundation_GetSymbol(setInputIndexFuncName);
    if (setInutIndexFunc == NULL) {
        FMK_LOGE("sym %s not found.", setInputIndexFuncName);
        return;
    }

    setInutIndexFunc(handle, inputAippIndex);
}

void HIAI_TensorAippPara_DestroyLegacy(void* handle)
{
    static const char* destroyFuncName = "HIAI_TensorAipp_destroy";
    void (*destroyFunc)(void*) = (void (*)(void*))HIAI_Foundation_GetSymbol(destroyFuncName);
    if (destroyFunc == NULL) {
        FMK_LOGE("sym %s not found.", destroyFuncName);
        return;
    }

    destroyFunc(handle);
}