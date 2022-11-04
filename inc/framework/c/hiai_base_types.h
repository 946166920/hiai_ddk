/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: base type
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
