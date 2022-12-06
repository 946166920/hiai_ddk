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

#include "util/base_types.h"
#include "hiai_nd_tensor_buffer_util.h"
#include "hiai_nd_tensor_buffer_legacy.h"
#include "framework/c/compatible/hiai_tensor_desc.h"

#include "util/hiai_foundation_dl_helper.h"

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandleV3(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle, void* createFun)
{
    using createNDTensorBufferFun = HIAI_NDTensorBuffer* (*)(const HIAI_NDTensorDesc*, const HIAI_NativeHandle*);
    auto create = reinterpret_cast<createNDTensorBufferFun>(createFun);
    HIAI_NDTensorBuffer* buffer = create(desc, handle);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromNDTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseNDTensorBuffer(&buffer);
    }

    return ndBuffer;
}

static HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandleV1(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle, void* createFun)
{
    hiai::NativeHandle h = {.fd = HIAI_NativeHandle_GetFd(handle),
        .size = HIAI_NativeHandle_GetSize(handle),
        .offset = HIAI_NativeHandle_GetOffset(handle)};

    HIAI_TensorDescription d;
    if (HIAI_NDTensorDesc_GetDimNum(desc) == NCHW_DIM_NUM) {
        d = {HIAI_NDTensorDesc_GetDim(desc, 0), HIAI_NDTensorDesc_GetDim(desc, 1), HIAI_NDTensorDesc_GetDim(desc, 2),
            HIAI_NDTensorDesc_GetDim(desc, 3), HIAI_NDTensorDesc_GetDataType(desc)};
    } else {
        d = {1, static_cast<int32_t>(HIAI_NDTensorDesc_GetTotalDimNum(desc)), 1, 1, HIAI_DATATYPE_FLOAT32};
    }

    using createTensorBufferFun = HIAI_TensorBuffer* (*)(HIAI_TensorDescription*, hiai::NativeHandle*);
    auto create = reinterpret_cast<createTensorBufferFun>(createFun);
    HIAI_TensorBuffer* buffer = create(&d, &h);

    HIAI_NDTensorBuffer* ndBuffer = HIAI_NDTensorBuffer_CreateFromTensorBuffer(desc, buffer);
    if (ndBuffer == nullptr) {
        HIAI_NDTensorBuffer_ReleaseTensorBuffer(&buffer);
    }

    return ndBuffer;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandleLegacy(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle)
{
    void* createFuncV3 = HIAI_Foundation_GetSymbol("HIAI_NDTensorBuffer_CreateFromNativeHandle");
    if (createFuncV3 != nullptr) {
        return HIAI_NDTensorBuffer_CreateFromNativeHandleV3(desc, handle, createFuncV3);
    }

    void* createFuncV1 = HIAI_Foundation_GetSymbol("HIAI_TensorBuffer_createTensorFromNativeHandle");
    if (createFuncV1 != nullptr) {
        return HIAI_NDTensorBuffer_CreateFromNativeHandleV1(desc, handle, createFuncV1);
    }
    return nullptr;
}

HIAI_NDTensorBuffer* HIAI_NDTensorBuffer_CreateFromNativeHandle(
    const HIAI_NDTensorDesc* desc, const HIAI_NativeHandle* handle)
{
    if (desc == nullptr || handle == nullptr) {
        return nullptr;
    }
    return HIAI_NDTensorBuffer_CreateFromNativeHandleLegacy(desc, handle);
}