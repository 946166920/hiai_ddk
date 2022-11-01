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
#include "aipp_infershape_util.h"

#include "tensor/aipp_para.h"
#include "tensor/image_format.h"

#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"

#include "infra/base/assertion.h"

namespace ge {
const int32_t NCHW_DIM_C = 1;
const int32_t NCHW_DIM_H = 2;
const int32_t NCHW_DIM_W = 3;
const int32_t DEFAULT_CHANNEL = 3;

GraphErrCodeStatus ShapeNotChange(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);
    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(DataType::DT_UINT8);

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AippInfershapeUtil::DataShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    return ShapeNotChange(node, outputShapeVec, outputTypeVec);
}

GraphErrCodeStatus AippInfershapeUtil::CastTInferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    return ShapeNotChange(node, outputShapeVec, outputTypeVec);
}

GraphErrCodeStatus AippInfershapeUtil::ImageCropV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);
    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    hiai::CropPara* para = GetAippParam<hiai::CropPara>(*(node.ROLE(NodeWalker).InDataNode(1)));
    if (para == nullptr) {
        return GRAPH_FAILED;
    }
    outputShape.SetDim(NCHW_DIM_W, para->cropSizeW);
    outputShape.SetDim(NCHW_DIM_H, para->cropSizeH);

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(opDesc.MutableInputDesc(0)->GetDataType());

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AippInfershapeUtil::ImageResizeV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);
    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    hiai::ResizePara* para = GetAippParam<hiai::ResizePara>(*(node.ROLE(NodeWalker).InDataNode(1)));
    if (para == nullptr) {
        return GRAPH_FAILED;
    }
    outputShape.SetDim(NCHW_DIM_W, para->resizeOutputSizeW);
    outputShape.SetDim(NCHW_DIM_H, para->resizeOutputSizeH);

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(DataType::DT_UINT8);

    return GRAPH_SUCCESS;
}

inline uint32_t GetCscChannel(hiai::CscMatrixPara* cscMatrixPara, hiai::CscPara* cscPara)
{
    std::map<hiai::ImageFormat, uint32_t> channelMap = {
        {hiai::ImageFormat::YUV400, 1},
        {hiai::ImageFormat::YVU444SP, 4},
        {hiai::ImageFormat::YUV444SP, 4},
        {hiai::ImageFormat::RGB888, 3},
        {hiai::ImageFormat::BGR888, 3},
    };
    if (cscMatrixPara == nullptr && cscPara == nullptr) {
        return DEFAULT_CHANNEL;
    }

    std::map<hiai::ImageFormat, uint32_t>::const_iterator iter = channelMap.cend();
    if (cscMatrixPara != nullptr) {
        iter = channelMap.find(cscMatrixPara->outputFormat);
    } else if (cscPara != nullptr) {
        iter = channelMap.find(cscPara->outputFormat);
    }
    if (iter != channelMap.cend()) {
        return iter->second;
    }

    return DEFAULT_CHANNEL;
}

GraphErrCodeStatus AippInfershapeUtil::ImageColorSpaceConvertionV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    hiai::CscMatrixPara* cscMatrixPara = GetAippParam<hiai::CscMatrixPara>(*(node.ROLE(NodeWalker).InDataNode(1)));
    hiai::CscPara* cscPara = GetAippParam<hiai::CscPara>(*(node.ROLE(NodeWalker).InDataNode(1)));

    if (cscMatrixPara == nullptr && cscPara == nullptr) {
        return GRAPH_FAILED;
    }

    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);

    uint32_t channel = GetCscChannel(cscMatrixPara, cscPara);

    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    outputShape.SetDim(NCHW_DIM_C, channel);

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(DataType::DT_UINT8);

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AippInfershapeUtil::ImageRotateV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);
    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    hiai::RotatePara* para = GetAippParam<hiai::RotatePara>(*(node.ROLE(NodeWalker).InDataNode(1)));
    if (para == nullptr) {
        return GRAPH_FAILED;
    }
    int64_t inDimH = outputShape.GetDim(NCHW_DIM_H);
    int64_t inDimW = outputShape.GetDim(NCHW_DIM_W);
    if (static_cast<int>(para->rotationAngle) == 90 || static_cast<int>(para->rotationAngle) == 270) {
        outputShape.SetDim(NCHW_DIM_W, inDimH);
        outputShape.SetDim(NCHW_DIM_H, inDimW);
    }

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(DataType::DT_UINT8);

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AippInfershapeUtil::ImagePadV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_NOT_NULL_R(opDesc.MutableInputDesc(0), GRAPH_FAILED);
    Shape outputShape = opDesc.MutableInputDesc(0)->GetShape();

    hiai::PadPara* para = GetAippParam<hiai::PadPara>(*(node.ROLE(NodeWalker).InDataNode(1)));
    if (para == nullptr) {
        return GRAPH_FAILED;
    }
    outputShape.SetDim(NCHW_DIM_W, outputShape.GetDim(NCHW_DIM_W) + para->paddingSizeLeft + para->paddingSizeRight);
    outputShape.SetDim(NCHW_DIM_H, outputShape.GetDim(NCHW_DIM_H) + para->paddingSizeTop + para->paddingSizeBottom);

    outputShapeVec.push_back(outputShape);
    outputTypeVec.push_back(DataType::DT_UINT8);

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AippInfershapeUtil::ImageDataTypeConvertionV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    return ShapeNotChange(node, outputShapeVec, outputTypeVec);
}

GraphErrCodeStatus AippInfershapeUtil::ImageChannelSwapV2InferShape(
    const Node& node, std::vector<Shape>& outputShapeVec, std::vector<DataType>& outputTypeVec)
{
    return ShapeNotChange(node, outputShapeVec, outputTypeVec);
}
} // namespace ge
