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
#include "framework/c/hiai_native_handle.h"
#include "securec.h"
#include "framework/infra/log/log.h"

struct HIAI_NativeHandle {
    int fd;
    int size;
    int offset;
};

HIAI_NativeHandle* HIAI_NativeHandle_Create(int fd, int size, int offset)
{
    HIAI_NativeHandle* handle = malloc(sizeof(HIAI_NativeHandle));
    MALLOC_NULL_CHECK_RET_VALUE(handle, NULL);
    (void)memset_s(handle, sizeof(HIAI_NativeHandle), 0, sizeof(HIAI_NativeHandle));

    handle->fd = fd;
    handle->size = size;
    handle->offset = offset;
    return handle;
}

void HIAI_NativeHandle_Destroy(HIAI_NativeHandle** handle)
{
    if (handle == NULL || *handle == NULL) {
        return;
    }

    free(*handle);
    *handle = NULL;
}

int HIAI_NativeHandle_GetFd(const HIAI_NativeHandle* handle)
{
    if (handle == NULL) {
        return -1;
    }
    return handle->fd;
}

int HIAI_NativeHandle_GetSize(const HIAI_NativeHandle* handle)
{
    if (handle == NULL) {
        return 0;
    }
    return handle->size;
}

int HIAI_NativeHandle_GetOffset(const HIAI_NativeHandle* handle)
{
    if (handle == NULL) {
        return 0;
    }
    return handle->offset;
}