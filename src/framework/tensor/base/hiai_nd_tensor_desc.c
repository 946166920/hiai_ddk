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
#include "framework/c/hiai_nd_tensor_desc.h"

#include <stdlib.h>

#include "framework/c/hiai_base_types.h"
#include "securec.h"
#include "framework/infra/log/log.h"

struct HIAI_NDTensorDesc {
    int32_t* dims;
    size_t dimNum;
    HIAI_DataType dataType;
    HIAI_Format format;
};

HIAI_NDTensorDesc* HIAI_NDTensorDesc_Create(
    const int32_t* dims, size_t dimNum, HIAI_DataType dataType, HIAI_Format format)
{
    if (dimNum == 0 || dimNum > (__UINT32_MAX__ / sizeof(int32_t))) {
        FMK_LOGE("dimNum is invalid.");
        return NULL;
    }
    HIAI_NDTensorDesc* desc = (HIAI_NDTensorDesc*)malloc(sizeof(HIAI_NDTensorDesc));
    MALLOC_NULL_CHECK_RET_VALUE(desc, NULL);
    (void)memset_s(desc, sizeof(HIAI_NDTensorDesc), 0, sizeof(HIAI_NDTensorDesc));

    size_t dimsSize = dimNum * sizeof(int32_t);
    desc->dims = malloc(dimsSize);
    if (desc->dims == NULL) {
        FMK_LOGE("malloc failed.");
        goto FREE_DESC;
    }

    if (memcpy_s(desc->dims, dimsSize, dims, dimsSize) != EOK) {
        FMK_LOGE("allocate memcpy_s failure.");
        free(desc->dims);
        goto FREE_DESC;
    }

    desc->dimNum = dimNum;
    desc->dataType = dataType;
    desc->format = format;
    return desc;

FREE_DESC:
    free(desc);
    return NULL;
}

HIAI_NDTensorDesc* HIAI_NDTensorDesc_Clone(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == NULL) {
        FMK_LOGE("tensorDesc is NULL.");
        return NULL;
    }
    return HIAI_NDTensorDesc_Create(tensorDesc->dims, tensorDesc->dimNum, tensorDesc->dataType, tensorDesc->format);
}

void HIAI_NDTensorDesc_Destroy(HIAI_NDTensorDesc** tensorDesc)
{
    if (tensorDesc == NULL || *tensorDesc == NULL) {
        return;
    }

    if ((*tensorDesc)->dims != NULL) {
        free((*tensorDesc)->dims);
    }

    free(*tensorDesc);
    *tensorDesc = NULL;
}

size_t HIAI_NDTensorDesc_GetDimNum(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == NULL) {
        return 0;
    }
    return tensorDesc->dimNum;
}

int32_t HIAI_NDTensorDesc_GetDim(const HIAI_NDTensorDesc* tensorDesc, size_t index)
{
    if (tensorDesc == NULL) {
        return -1;
    }

    if (index >= tensorDesc->dimNum) {
        FMK_LOGE("invalid index.");
        return -1;
    }

    return tensorDesc->dims[index];
}

HIAI_DataType HIAI_NDTensorDesc_GetDataType(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == NULL) {
        return 0;
    }
    return tensorDesc->dataType;
}

HIAI_Format HIAI_NDTensorDesc_GetFormat(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == NULL) {
        return 0;
    }
    return tensorDesc->format;
}

size_t HIAI_NDTensorDesc_GetTotalDimNum(const HIAI_NDTensorDesc* tensorDesc)
{
    if (tensorDesc == NULL) {
        return 0;
    }

    size_t totalSize = 1;
    for (size_t i = 0; i < tensorDesc->dimNum; i++) {
        int32_t dimValue = tensorDesc->dims[i];
        if (dimValue <= 0) {
            FMK_LOGE("invalid dim value.");
            return 0;
        }
        if (totalSize > SIZE_MAX / (uint32_t)(dimValue)) {
            FMK_LOGE("too large dimValue.");
            return 0;
        }
        totalSize *= (uint32_t)(dimValue);
    }

    return totalSize;
}

size_t HIAI_NDTensorDesc_GetByteSize(const HIAI_NDTensorDesc* tensorDesc)
{
    size_t totalDimNum = HIAI_NDTensorDesc_GetTotalDimNum(tensorDesc);
    if (totalDimNum == 0) {
        return 0;
    }

    if (tensorDesc->dataType < HIAI_DATATYPE_UINT8 || tensorDesc->dataType >= HIAI_DATATYPE_SIZE) {
        FMK_LOGE("invalid data type.");
        return 0;
    }

    if (totalDimNum > SIZE_MAX / HIAI_DataType_GetElementSize(tensorDesc->dataType)) {
        return 0;
    }
    return totalDimNum * HIAI_DataType_GetElementSize(tensorDesc->dataType);
}