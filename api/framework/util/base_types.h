/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: base types
 */
#ifndef HIAI_FRAMEWOKR_UTIL_BASE_TYPES_H
#define HIAI_FRAMEWOKR_UTIL_BASE_TYPES_H

#include <cstdint>

namespace hiai {
enum class DataType {
    UINT8,
    FLOAT32,
    FLOAT16,
    INT32,
    INT8, //lint !e578
    INT16,
    BOOL,
    INT64,
    UINT32,
    DOUBLE,
    RESERVED //lint !e578
};

enum class Format {
    NCHW = 0, /* NCHW */
    NHWC, /* NHWC */
    ND, /* Nd Tensor */
    RESERVED //lint !e578
};

struct NativeHandle {
    int fd;
    int size;
    int offset;
};

static constexpr uint32_t MEM_ALLOC_ALIGN = 512;
}; // namespace hiai
#endif
