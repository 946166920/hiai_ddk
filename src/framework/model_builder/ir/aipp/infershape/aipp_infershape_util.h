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

#ifndef FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_INFERSHAPE_UTIL_H
#define FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_INFERSHAPE_UTIL_H

#include <vector>

// api/framework
#include "graph/op/image_defs.h"
#include "graph/op/const_defs.h"
#include "graph/debug/ge_error_codes.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/debug/ge_log.h"

#define INFERSHAPE_API_EXPORT __attribute__((__visibility__("default")))

namespace ge {
using namespace std;
template <typename T> static T* GetAippParam(const Node& node)
{
    auto& nodeSpec = node.ROLE(NodeSpec);
    const auto& type = nodeSpec.Type();
    if (type == hiai::op::ConfigData::TYPE || type == hiai::op::Const::TYPE) {
        auto& opDesc = nodeSpec.OpDesc();
        ge::TensorPtr tensor;
        if (!ge::AttrUtils::GetTensor(opDesc, hiai::op::ConfigData::value, tensor)) {
            FMK_LOGE("GetTensor failed!");
            return nullptr;
        }
        if (tensor == nullptr) {
            FMK_LOGE("tensor is nullptr!");
            return nullptr;
        }
        ge::Buffer& buffer = tensor->MutableData();
        if (buffer.GetSize() != sizeof(T)) {
            FMK_LOGE("bufferSize: is not equal to funcSize!");
            return nullptr;
        }
        return static_cast<T*>(static_cast<void*>(buffer.MutableData()));
    }
    FMK_LOGE("node is invalid!");
    return nullptr;
}

using AippImageInferShapeFunc = GraphErrCodeStatus (*)(const Node&, std::vector<Shape>&, std::vector<DataType>&);

class INFERSHAPE_API_EXPORT AippInfershapeUtil {
public:
    static GraphErrCodeStatus DataShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageCropV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageResizeV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageColorSpaceConvertionV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageRotateV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImagePadV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageDataTypeConvertionV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus ImageChannelSwapV2InferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
    static GraphErrCodeStatus CastTInferShape(
        const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec);
};
} // namespace ge

#endif // FRAMEWORK_MODEL_BUILDER_IR_AIPP_AIPP_INFERSHAPE_UTIL_H