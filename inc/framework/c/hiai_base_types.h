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
#ifndef FRAMEWORK_C_HIAI_BASE_TYPES_H
#define FRAMEWORK_C_HIAI_BASE_TYPES_H
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HIAI_DATATYPE_UINT8 = 0,
    HIAI_DATATYPE_FLOAT32 = 1,
    HIAI_DATATYPE_FLOAT16 = 2,
    HIAI_DATATYPE_INT32 = 3,
    HIAI_DATATYPE_INT8 = 4,
    HIAI_DATATYPE_INT16 = 5,
    HIAI_DATATYPE_BOOL = 6,
    HIAI_DATATYPE_INT64 = 7,
    HIAI_DATATYPE_UINT32 = 8,
    HIAI_DATATYPE_DOUBLE = 9,
    HIAI_DATATYPE_SIZE
} HIAI_DataType;

typedef enum {
    HIAI_YUV420SP_U8 = 0,
    HIAI_XRGB8888_U8,
    HIAI_YUV400_U8,
    HIAI_ARGB8888_U8,
    HIAI_YUYV_U8,
    HIAI_YUV422SP_U8,
    HIAI_AYUV444_U8,
    HIAI_RGB888_U8,
    HIAI_IMAGE_FORMAT_INVALID,
} HIAI_ImageFormat;

size_t HIAI_DataType_GetElementSize(HIAI_DataType type);

typedef enum {
    HIAI_FORMAT_NCHW = 0, /* NCHW */
    HIAI_FORMAT_NHWC, /* NHWC */
    HIAI_FORMAT_ND, /* Nd Tensor */
    HIAI_FORMAT_RESERVED = 255
} HIAI_Format;

static const uint32_t HIAI_MEM_ALLOC_ALIGN = 512;

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_BASE_TYPES_H
