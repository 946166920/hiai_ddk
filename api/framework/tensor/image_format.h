/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: image tensor types
 */
#ifndef _HIAI_IMAGE_TENSOR_TYPES_H_
#define _HIAI_IMAGE_TENSOR_TYPES_H_

namespace hiai {
enum class ImageFormat {
    YUV420SP = 0,
    XRGB8888,
    YUV400,
    ARGB8888,
    YUYV,
    YUV422SP,
    AYUV444,
    RGB888,
    BGR888,
    YUV444SP,
    YVU444SP,
    INVALID = 255
};

enum class ImageColorSpace {
    JPEG,
    BT_601_NARROW,
    BT_601_FULL,
    BT_709_NARROW
};
}; // namespace hiai
#endif
