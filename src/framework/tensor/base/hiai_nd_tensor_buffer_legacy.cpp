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
#include "hiai_nd_tensor_buffer_legacy.h"

#include <cstdlib>
#include <vector>
#include "securec.h"

#include "framework/infra/log/log.h"
#include "framework/c/hiai_nd_tensor_buffer.h"
#include "hiai_nd_tensor_buffer_util.h"

#include "util/hiai_foundation_dl_helper.h"

void HIAI_NDTensorBuffer_ReleaseTensorBuffer(HIAI_TensorBuffer** buffer)
{
    auto releaseFunc = (void (*)(HIAI_TensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_destroy");
    if (releaseFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return;
    }

    releaseFunc(*buffer);
    *buffer = nullptr;
}

void HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(HIAI_NDTensorBuffer** buffer)
{
    auto releaseFunc = (void (*)(HIAI_NDTensorBuffer**))HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_Destroy");
    if (releaseFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return;
    }
    releaseFunc(buffer);
    *buffer = nullptr;
}

static void* HIAI_NDTensorBuffer_GetDataFromTensorBuffer(HIAI_TensorBuffer* buffer)
{
    auto getDataFunc = (void* (*)(HIAI_TensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_getRawBuffer");
    if (getDataFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return nullptr;
    }

    return getDataFunc(buffer);
}

static int32_t HIAI_NDTensorBuffer_GetSizeFromTensorBuffer(HIAI_TensorBuffer* buffer)
{
    auto getSizeFunc = (int32_t(*)(HIAI_TensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_getBufferSize");
    if (getSizeFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return -1;
    }

    return getSizeFunc(buffer);
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromTensorBuffer(
    const HIAI_NDTensorDesc* desc, HIAI_TensorBuffer* buffer)
{
    if (buffer == nullptr) {
        FMK_LOGE("buffer is nullptr.");
        return nullptr;
    }

    int32_t size = HIAI_NDTensorBuffer_GetSizeFromTensorBuffer(buffer);
    if (size <= 0) {
        return nullptr;
    }
    void* data = HIAI_NDTensorBuffer_GetDataFromTensorBuffer(buffer);

    return HIAI_NDTensorBuffer_Create(desc, data, static_cast<size_t>(size), static_cast<void*>(buffer), true, true);
}

void* HIAI_NDTensorBuffer_GetDataFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer)
{
    auto getDataFunc = (void* (*)(HIAI_NDTensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetData");
    if (getDataFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return nullptr;
    }

    return getDataFunc(buffer);
}

size_t HIAI_NDTensorBuffer_GetSizeFromNDTensorBuffer(HIAI_NDTensorBuffer* buffer)
{
    auto getSizeFunc = (size_t(*)(HIAI_NDTensorBuffer*))HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetSize");
    if (getSizeFunc == nullptr) {
        FMK_LOGE("sym not found.");
        return 0;
    }

    return getSizeFunc(buffer);
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(
    const HIAI_NDTensorDesc* desc, HIAI_NDTensorBuffer* buffer)
{
    if (buffer == nullptr) {
        FMK_LOGE("buffer is nullptr.");
        return nullptr;
    }

    size_t size = HIAI_NDTensorBuffer_GetSizeFromNDTensorBuffer(buffer);
    if (size == 0) {
        return nullptr;
    }
    void* data = HIAI_NDTensorBuffer_GetDataFromNDTensorBuffer(buffer);

    return HIAI_NDTensorBuffer_Create(desc, data, size, static_cast<void*>(buffer), true, false);
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBufferWithFormat(
    const HIAI_NDTensorDesc* desc, HIAI_ImageFormat format, void* createFun)
{
    int32_t dims[] = {1, 1, 1, 1};
    if (HIAI_NDTensorDesc_GetDimNum(desc) != NCHW_DIM_NUM) {
        return nullptr;
    }
    for (size_t i = 0; i < NCHW_DIM_NUM; i++) {
        dims[i] = HIAI_NDTensorDesc_GetDim(desc, i);
    }

    HIAI_TensorBuffer* buffer =
        ((HIAI_TensorBuffer* (*)(int, int, int, HIAI_ImageFormat)) createFun)(dims[0], dims[2], dims[3], format);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBufferWithFormatV3(
    const HIAI_NDTensorDesc* desc, const HIAI_ImageFormat format, void* createFun)
{
    HIAI_NDTensorBuffer* buffer =
        ((HIAI_NDTensorBuffer* (*)(const HIAI_NDTensorDesc*, HIAI_ImageFormat)) createFun)(desc, format);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedNDBufferWithSize(
    const HIAI_NDTensorDesc* desc, size_t size, void* createFun)
{
    HIAI_NDTensorBuffer* buffer = ((HIAI_NDTensorBuffer* (*)(const HIAI_NDTensorDesc*, size_t)) createFun)(desc, size);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBufferWithSize(
    const HIAI_NDTensorDesc* desc, size_t size, void* createFun)
{
    int32_t dims[] = {1, 1, 1, 1};
    if (HIAI_NDTensorDesc_GetDimNum(desc) == NCHW_DIM_NUM) {
        for (size_t i = 0; i < NCHW_DIM_NUM; i++) {
            dims[i] = HIAI_NDTensorDesc_GetDim(desc, i);
        }
    } else {
        /* for nd senaria, put total dim num to C dimension */
        dims[1] = static_cast<int32_t>(HIAI_NDTensorDesc_GetTotalDimNum(desc));
    }

    HIAI_TensorBuffer* buffer =
        ((HIAI_TensorBuffer* (*)(int, int, int, int, int)) createFun)(dims[0], dims[1], dims[2], dims[3], size);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBufferV3(
    const HIAI_NDTensorDesc* desc, void* createFun)
{
    HIAI_NDTensorBuffer* buffer = ((HIAI_NDTensorBuffer* (*)(const HIAI_NDTensorDesc*)) createFun)(desc);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBufferV2(
    const HIAI_NDTensorDesc* desc, void* createFun)
{
    size_t totalByteSize = HIAI_NDTensorDesc_GetByteSize(desc);
    if (totalByteSize == 0) {
        FMK_LOGE("invalid desc.");
        return nullptr;
    }

    return HIAI_NDTensorBuffer_CreateSharedBufferWithSize(desc, totalByteSize, createFun);
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBuffer(const HIAI_NDTensorDesc* desc, void* createFun)
{
    if (HIAI_NDTensorDesc_GetDimNum(desc) != NCHW_DIM_NUM) {
        FMK_LOGE("only supoort 4 dims.");
        return nullptr;
    }

    int32_t dims[] = {1, 1, 1, 1};
    for (size_t i = 0; i < NCHW_DIM_NUM; i++) {
        dims[i] = HIAI_NDTensorDesc_GetDim(desc, i);
    }

    HIAI_TensorBuffer* buffer =
        ((HIAI_TensorBuffer* (*)(int, int, int, int)) createFun)(dims[0], dims[1], dims[2], dims[3]);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(&buffer);
    }
    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateSharedBuffer_WithDataType(
    const HIAI_NDTensorDesc* desc, void* createFun)
{
    if (HIAI_NDTensorDesc_GetDimNum(desc) != NCHW_DIM_NUM) {
        FMK_LOGE("only supoort 4 dims.");
        return nullptr;
    }

    int32_t dims[] = {1, 1, 1, 1};
    for (size_t i = 0; i < NCHW_DIM_NUM; i++) {
        dims[i] = HIAI_NDTensorDesc_GetDim(desc, i);
    }

    HIAI_TensorBuffer* buffer = ((HIAI_TensorBuffer* (*)(int, int, int, int, HIAI_DataType)) createFun)(
        dims[0], dims[1], dims[2], dims[3], HIAI_NDTensorDesc_GetDataType(desc));

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(&buffer);
    }
    return ndBuffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNDTensorDescLegacy(const HIAI_NDTensorDesc* desc)
{
    HIAI_NDTensorBuffer* buffer = nullptr;
    void* createV3Func = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_CreateFromNDTensorDesc");
    if (createV3Func != nullptr) {
        return HIAI_NDTensorBuffer_CreateSharedBufferV3(desc, createV3Func);
    }

    void* createV2Func = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_create_v2");
    if (createV2Func != nullptr) {
        buffer = HIAI_NDTensorBuffer_CreateSharedBufferV2(desc, createV2Func);
        if (buffer != nullptr) { // need try v1 if v2 failed
            return buffer;
        }
    }

    void* createWithDataTypeFunc = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_CreateWithDataType");
    if (createWithDataTypeFunc != nullptr) {
        buffer = HIAI_NDTensorBuffer_CreateSharedBuffer_WithDataType(desc, createWithDataTypeFunc);
        if (buffer != nullptr) {
            return buffer;
        }
    }

    void* createFunc = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_create");
    if (createFunc != nullptr && (HIAI_NDTensorDesc_GetDataType(desc) == HIAI_DATATYPE_FLOAT32)) {
        buffer = HIAI_NDTensorBuffer_CreateSharedBuffer(desc, createFunc);
        if (buffer != nullptr) {
            return buffer;
        }
    }

    return nullptr;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromSizeLegacy(const HIAI_NDTensorDesc* desc, size_t size)
{
    void* createV3Func = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_CreateFromSize");
    if (createV3Func != nullptr) {
        return HIAI_NDTensorBuffer_CreateSharedNDBufferWithSize(desc, size, createV3Func);
    }

    void* createV2Func = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_create_v2");
    if (createV2Func != nullptr) {
        return HIAI_NDTensorBuffer_CreateSharedBufferWithSize(desc, size, createV2Func);
    }

    return nullptr;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromFormatLegacy(
    const HIAI_NDTensorDesc* desc, size_t size, HIAI_ImageFormat format)
{
    HIAI_NDTensorBuffer* buffer = nullptr;
    void* createNDImageFunc = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_CreateFromFormat");
    if (createNDImageFunc != nullptr) {
        buffer = HIAI_NDTensorBuffer_CreateSharedBufferWithFormatV3(desc, format, createNDImageFunc);
        if (buffer != nullptr) {
            return buffer;
        }
    }

    void* createImageFunc = HIAI_Foundation_GetSymbol("HIAI_ImageBuffer_create");
    if (createImageFunc != nullptr) {
        buffer = HIAI_NDTensorBuffer_CreateSharedBufferWithFormat(desc, format, createImageFunc);
        if (buffer != nullptr) {
            return buffer;
        }
    }

    void* createV2Func = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_create_v2");
    if (createV2Func != nullptr) {
        return HIAI_NDTensorBuffer_CreateSharedBufferWithSize(desc, size, createV2Func);
    }

    return nullptr;
}

static int32_t HIAI_NDTensorBuffer_GetFdV3(void* buffer)
{
    void* getFdFunc = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetFd");
    if (getFdFunc == nullptr) {
        FMK_LOGE("symbol not find.");
        return -1;
    }

    int32_t fd = ((int32_t(*)(const HIAI_NDTensorBuffer*))getFdFunc)((const HIAI_NDTensorBuffer*)buffer);
    if (fd < 0) {
        FMK_LOGE("Get nd Fd failed. fd:[%d]", fd);
        return -1;
    }
    return fd;
}

static int32_t HIAI_NDTensorBuffer_GetFdV1(void* buffer)
{
    void* getFdFunc = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_getFdAndSize");
    if (getFdFunc == nullptr) {
        FMK_LOGE("symbol not find.");
        return -1;
    }

    std::vector<HIAI_TensorBuffer*> bufferVec {static_cast<HIAI_TensorBuffer*>(buffer)};
    std::vector<int> fdList;
    std::vector<int> sizeList;
    (void)((int (*)(HIAI_TensorBuffer*[], int, std::vector<int>&, std::vector<int>&))getFdFunc)(
        bufferVec.data(), bufferVec.size(), fdList, sizeList);
    if (fdList.size() != 1 || sizeList.size() != 1) {
        FMK_LOGE("get fd and size fail.");
        return -1;
    }
    FMK_LOGD("get fd and size succ. fd:%d, size%d.", fdList[0], sizeList[0]);
    return static_cast<int32_t>(fdList[0]);
}

int32_t HIAI_NDTensorBuffer_GetFdLegacy(void* buffer, bool isLegacy)
{
    if (isLegacy) {
        return HIAI_NDTensorBuffer_GetFdV1(buffer);
    } else {
        return HIAI_NDTensorBuffer_GetFdV3(buffer);
    }
}

static int32_t HIAI_NDTensorBuffer_GetOriginFdV3(void* buffer)
{
    void* getOriginFdFunc = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_GetOriginFd");
    if (getOriginFdFunc == nullptr) {
        FMK_LOGE("symbol not find.");
        return -1;
    }

    int32_t fd = ((int32_t(*)(const HIAI_NDTensorBuffer*))getOriginFdFunc)((const HIAI_NDTensorBuffer*)buffer);
    if (fd < 0) {
        FMK_LOGW("Get origin fd failed. fd:[%d]", fd);
        return -1;
    }
    return fd;
}

static int32_t HIAI_NDTensorBuffer_GetOriginFdV1(void* buffer)
{
    void* getOriginFdFunc = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_getOriginFd");
    if (getOriginFdFunc == nullptr) {
        FMK_LOGE("symbol not find.");
        return -1;
    }

    std::vector<HIAI_TensorBuffer*> bufferVec {static_cast<HIAI_TensorBuffer*>(buffer)};
    std::vector<int> fdList;
    int ret = ((int (*)(HIAI_TensorBuffer*[], int, std::vector<int>&))getOriginFdFunc)(
        bufferVec.data(), bufferVec.size(), fdList);
    if (ret != 0 || fdList.size() != 1) {
        FMK_LOGE("get origin fd fail.");
        return -1;
    }
    FMK_LOGD("get origin fd succ. fd:%d.", fdList[0]);
    return static_cast<int32_t>(fdList[0]);
}

int32_t HIAI_NDTensorBuffer_GetOriginFdLegacy(void* buffer, bool isLegacy)
{
    if (isLegacy) {
        return HIAI_NDTensorBuffer_GetOriginFdV1(buffer);
    } else {
        return HIAI_NDTensorBuffer_GetOriginFdV3(buffer);
    }
}

void HIAI_NDTensorBuffer_DestroyLegacy(void* buffer, bool isLegacy)
{
    if (isLegacy) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(reinterpret_cast<HIAI_TensorBuffer**>(&buffer));
    } else {
        HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(reinterpret_cast<HIAI_NDTensorBuffer**>(&buffer));
    }
}