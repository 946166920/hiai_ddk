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
#include "framework/c/hiai_shared_buffer.h"

#include "util/hiai_foundation_dl_helper.h"
#include "securec.h"
#include "framework/infra/log/log.h"

struct HIAI_SharedBuffer {
    void* impl;
};

HIAI_SharedBuffer* HIAI_SharedBuffer_Create(size_t size)
{
    (void)size;
    return NULL;
}

static void DestroySharedBuffer(void** buffer)
{
    static const char* destroyFuncName = "HIAI_ModelBuffer_destroy";
    void (*destroyFunc)(void*) = (void (*)(void*))HIAI_Foundation_GetSymbol(destroyFuncName);
    if (destroyFunc == NULL) {
        FMK_LOGE("sym %s not found.", destroyFuncName);
        return;
    }

    destroyFunc(*buffer);
    *buffer = NULL;
}

HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromBuffer(void* buffer, size_t size)
{
    if (buffer == NULL || size == 0) {
        return NULL;
    }

    static const char* createFromBufferFuncName = "HIAI_ModelBuffer_create_from_buffer";
    void* (*createFromBufferFunc)(const char*, void*, int, int) =
        (void* (*)(const char*, void*, int, int))HIAI_Foundation_GetSymbol(createFromBufferFuncName);
    if (createFromBufferFunc == NULL) {
        FMK_LOGE("sym %s not found.", createFromBufferFuncName);
        return NULL;
    }

    void* impl = createFromBufferFunc("share_buffer", buffer, (int)size, (int)0);
    if (impl == NULL) {
        FMK_LOGE("create shared buffer failed.");
        return NULL;
    }

    HIAI_SharedBuffer* sharedBuffer = (HIAI_SharedBuffer*)malloc(sizeof(HIAI_SharedBuffer));
    if (sharedBuffer == NULL) {
        FMK_LOGE("malloc failed.");
        goto FREE_BUFFER;
    }
    (void)memset_s(sharedBuffer, sizeof(HIAI_SharedBuffer), 0, sizeof(HIAI_SharedBuffer));
    sharedBuffer->impl = impl;
    return sharedBuffer;

FREE_BUFFER:
    DestroySharedBuffer(&impl);
    return NULL;
}

HIAI_SharedBuffer* HIAI_SharedBuffer_CreateFromFile(const char* fileName)
{
    if (fileName == NULL) {
        return NULL;
    }

    static const char* createFromFileFuncName = "HIAI_ModelBuffer_create_from_file";
    void* (*createFromFileFunc)(const char*, const char*, int) =
        (void* (*)(const char*, const char*, int))HIAI_Foundation_GetSymbol(createFromFileFuncName);
    if (createFromFileFunc == NULL) {
        FMK_LOGE("sym %s not found.", createFromFileFuncName);
        return NULL;
    }

    void* impl = createFromFileFunc("share_buffer", fileName, (int)0);
    if (impl == NULL) {
        FMK_LOGE("create shared buffer failed.");
        return NULL;
    }

    HIAI_SharedBuffer* sharedBuffer = (HIAI_SharedBuffer*)malloc(sizeof(HIAI_SharedBuffer));
    if (sharedBuffer == NULL) {
        FMK_LOGE("malloc failed.");
        goto FREE_BUFFER;
    }
    (void)memset_s(sharedBuffer, sizeof(HIAI_SharedBuffer), 0, sizeof(HIAI_SharedBuffer));
    sharedBuffer->impl = impl;
    return sharedBuffer;

FREE_BUFFER:
    DestroySharedBuffer(&impl);
    return NULL;
}

size_t HIAI_SharedBuffer_GetSize(const HIAI_SharedBuffer* buffer)
{
    if (buffer == NULL) {
        return 0;
    }

    static const char* getSizeFuncName = "HIAI_ModelBuffer_getSize";
    int32_t (*getSizeFunc)(void*) = (int32_t(*)(void*))HIAI_Foundation_GetSymbol(getSizeFuncName);
    if (getSizeFunc == NULL) {
        FMK_LOGE("sym %s not found.", getSizeFuncName);
        return 0;
    }
    int32_t ret = getSizeFunc(buffer->impl);
    return ret < 0 ? 0 : (size_t)ret;
}

void* HIAI_SharedBuffer_GetData(const HIAI_SharedBuffer* buffer)
{
    (void)buffer;
    return NULL;
}

void HIAI_ShardBuffer_Destroy(HIAI_SharedBuffer** buffer)
{
    if (buffer == NULL || *buffer == NULL) {
        return;
    }

    DestroySharedBuffer(&((*buffer)->impl));
    free(*buffer);
    *buffer = NULL;
}
