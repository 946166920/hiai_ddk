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
#ifndef _HIAI_IMAGE_PROCESS_CONFIG_H_
#define _HIAI_IMAGE_PROCESS_CONFIG_H_
#include <cstdint>
#include "image_format.h"

namespace hiai {
struct CropPara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    uint32_t cropStartPosW = 0;
    uint32_t cropStartPosH = 0;
    uint32_t cropSizeW = 0;
    uint32_t cropSizeH = 0;
};

struct ResizePara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    uint32_t resizeOutputSizeW = 0;
    uint32_t resizeOutputSizeH = 0;
};

struct ChannelSwapPara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    bool rbuvSwapSwitch = false;
    bool axSwapSwitch = false;
};

struct CscMatrixPara {
    ImageFormat outputFormat = ImageFormat::RGB888;
    ImageColorSpace colorSpace = ImageColorSpace::JPEG;
    int32_t matrixR0C0 = 0;
    int32_t matrixR0C1 = 0;
    int32_t matrixR0C2 = 0;
    int32_t matrixR1C0 = 0;
    int32_t matrixR1C1 = 0;
    int32_t matrixR1C2 = 0;
    int32_t matrixR2C0 = 0;
    int32_t matrixR2C1 = 0;
    int32_t matrixR2C2 = 0;
    int32_t outputBias0 = 0;
    int32_t outputBias1 = 0;
    int32_t outputBias2 = 0;
    int32_t inputBias0 = 0;
    int32_t inputBias1 = 0;
    int32_t inputBias2 = 0;
};

struct CscPara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    ImageFormat outputFormat = ImageFormat::RGB888;
    ImageColorSpace imageColorSpace = ImageColorSpace::JPEG;
};

struct DtcPara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    int16_t pixelMeanChn0 = 0;
    int16_t pixelMeanChn1 = 0;
    int16_t pixelMeanChn2 = 0;
    int16_t pixelMeanChn3 = 0;
    float pixelMinChn0 = 0.0;
    float pixelMinChn1 = 0.0;
    float pixelMinChn2 = 0.0;
    float pixelMinChn3 = 0.0;
    float pixelVarReciChn0 = 1.0;
    float pixelVarReciChn1 = 1.0;
    float pixelVarReciChn2 = 1.0;
    float pixelVarReciChn3 = 1.0;
};

struct RotatePara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    float rotationAngle = 0;
    bool rotate = true;
};

struct PadPara {
    ImageFormat imageFormat = ImageFormat::INVALID;
    uint32_t paddingSizeTop = 0;
    uint32_t paddingSizeBottom = 0;
    uint32_t paddingSizeLeft = 0;
    uint32_t paddingSizeRight = 0;
    float paddingValueChn0  = 0;
    float paddingValueChn1  = 0;
    float paddingValueChn2  = 0;
    float paddingValueChn3  = 0;
};
}; // namespace hiai
#endif
