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

#ifndef FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_PARAM_INFO_CONVERTER_H
#define FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_PARAM_INFO_CONVERTER_H

// api/framework
#include "graph/op/image_defs.h"
#include "graph/op/array_defs.h"

// inc/framework
#include "framework/common/fmk_error_codes.h"
#include "framework/graph/debug/ge_op_types.h"
#include "infra/base/assertion.h"

namespace ge {
class Node;
}

namespace hiai {
struct AippParamInfo;

const static std::vector<const char*> AIPP_FUNC_TYPE = {
    hiai::op::ImageCropV2::TYPE,
    hiai::op::ImageChannelSwapV2::TYPE,
    hiai::op::ImageColorSpaceConvertionV2::TYPE,
    hiai::op::ImageResizeV2::TYPE,
    hiai::op::ImageDataTypeConvertionV2::TYPE,
    hiai::op::ImageRotateV2::TYPE,
    hiai::op::ImagePadV2::TYPE,
};

enum class InputFormat {
    UNDEFINED = 0,
    YUV420SP_U8 = 1,
    XRGB8888_U8 = 2,
    RGB888_U8 = 3,
    YUV400_U8 = 4,
    ARGB8888_U8 = 5,
    YUYV_U8 = 6,
    YUV422SP_U8 = 7,
    AYUV444_U8 = 8,
    BGR888_U8 = 9,
    YUV444SP_U8 = 10,
    YVU444SP_U8 = 11
};

class AippParamInfoConverter {
public:
    static Status ConvertAippParamInfo2Attr(
        AttrValue::NamedAttrs& aippAttrs, const hiai::AippParamInfo& paramInfo);

    static Status ConvertAippInputInfo2Attr(
        AttrValue::NamedAttrs& aippAttrs, const ge::Node& dataNode, bool hasDynamicAippFunc);

    static Status ConvertConst2AippParamInfo(
        const ge::Node& node, const std::string& aippFuncType, AippParamInfo& paramInfo);
};
}; // namespace hiai

#endif // FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_PARAM_INFO_CONVERTER_H