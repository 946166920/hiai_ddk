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
