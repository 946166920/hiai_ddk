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
#include "ir_verify.h"
#include "transformer_utils.h"

#include "infra/base/assertion.h"
#include "graph/op/all_ops.h"
#include "framework/graph/op/internal_nn_defs.h"
#include "framework/graph/op/internal_defs.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/edge.h"

namespace hiai {
const int ACTIVATION_MODE_SIGMOID = 0;
const int ACTIVATION_MODE_GELU = 15;

const int L2_NORMALIZE_AXIS_VALUE = 1;

const int RESIZE_OUTPUT_DIM_BY_ZOOM_FACTOR = 0;
const int RESIZE_OUTPUT_DIM_BY_SHRINK_FACTOR = 1;
const int RESIZE_OUTPUT_DIM_EXPLICIT = 2;

const std::string INTERP_SHRINK_FACTOR = "shrink_factor";
const std::string INTERP_OUTPUT_DIM_MODE = "output_dim_mode";
const std::string INTERP_ZOOM_FACTOR = "zoom_factor";
const char* DEQUANTIZE_MIN_COMBINED_MODE = "MIN_COMBINED";

// 校验函数
ge::GraphErrCodeStatus DataVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    TensorDescPtr firstInputDesc = op.MutableInputDesc(0);
    HIAI_EXPECT_NOT_NULL(firstInputDesc);

    size_t realDimSize = firstInputDesc->GetShape().GetDimNum();
    ge::TensorUtils::SetRealDimCnt(*firstInputDesc, realDimSize);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FullyConnectionVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    bool transpose = false;
    (void)ge::AttrUtils::GetBool(op, hiai::op::FullyConnection::transpose, transpose);
    if (transpose) {
        FMK_LOGE("Fully connection transpose attr only support false");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ScaleVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr scale_from_blob
    bool scaleFromBlob = false;
    (void)ge::AttrUtils::GetBool(op, hiai::op::Scale::scale_from_blob, scaleFromBlob);
    if (scaleFromBlob) {
        FMK_LOGE("Scale scale_from_blob attr only support false");
        return ge::GRAPH_FAILED;
    }
    // num_axes
    int64_t numAxes = 1;
    (void)ge::AttrUtils::GetInt(op, hiai::op::Scale::num_axes, numAxes);
    if (numAxes != 1) {
        FMK_LOGE("Scale numAxes attr only support 1, now is %jd", numAxes);
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SizeVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr dtype
    int64_t dtype = 3;
    (void)ge::AttrUtils::GetInt(op, hiai::op::Size::dtype, dtype);
    if (dtype != static_cast<int64_t>(ge::DT_INT32)) {
        FMK_LOGE("Size dtype attr only support int32");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LRNVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr norm_region
    std::string normRegion = "ACROSS_CHANNELS";
    (void)ge::AttrUtils::GetStr(op, hiai::op::LRN::norm_region, normRegion);
    if (normRegion != "ACROSS_CHANNELS") {
        FMK_LOGE("LRN norm_region attr only support ACROSS_CHANNELS, now is %s", normRegion.c_str());
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus BNInferenceVerify(ge::Node& node)
{
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    if (weightsVec.size() == 4) {
        vector<ge::TensorPtr> weightsVecTmp;
        weightsVecTmp.push_back(weightsVec[2]);
        weightsVecTmp.push_back(weightsVec[3]);
        weightsVecTmp.push_back(weightsVec[0]);
        weightsVecTmp.push_back(weightsVec[1]);
        ge::OpDescUtils::SetWeights(node, weightsVecTmp);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus DepthwiseConv2DVerify(ge::Node& node)
{
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    // set attr groups def value
    (void)ge::AttrUtils::SetInt(opDesc, "groups", 1);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus EltwiseVerify(ge::Node& node)
{
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    // check input size
    if (opDesc.HasAttr(hiai::op::Eltwise::N)) {
        int inpuNum = 0;
        (void)ge::AttrUtils::GetInt(opDesc, hiai::op::Eltwise::N, inpuNum);
        GE_CHK_BOOL_RET_STATUS(static_cast<uint32_t>(inpuNum) == opDesc.GetInputsSize(), ge::GRAPH_FAILED,
            "The number of inputs must be equal to N!");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ShapeVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr dtype
    int64_t dtype = static_cast<int64_t>(ge::DT_INT32);
    (void)ge::AttrUtils::GetInt(op, hiai::op::Shape::dtype, dtype);
    if (dtype != static_cast<int64_t>(ge::DT_INT32)) {
        FMK_LOGE("Size dtype attr only support int32");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus DequantizeVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr mode
    std::string mode = DEQUANTIZE_MIN_COMBINED_MODE;
    (void)ge::AttrUtils::GetStr(op, hiai::op::Dequantize::mode, mode);
    if (mode != DEQUANTIZE_MIN_COMBINED_MODE) {
        FMK_LOGE("Dequantize mode attr only support MIN_COMBINED");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus QuantizeVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr mode
    std::string mode = DEQUANTIZE_MIN_COMBINED_MODE;
    (void)ge::AttrUtils::GetStr(op, hiai::op::Quantize::mode, mode);
    if (mode != DEQUANTIZE_MIN_COMBINED_MODE) {
        FMK_LOGE("Quantize mode attr only support MIN_COMBINED");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LayerNormVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr begin_norm_axis begin_params_axis
    int beginNormAxis = 0;
    int beginParamsAxis = 0;
    (void)ge::AttrUtils::GetInt(op, hiai::op::LayerNorm::begin_norm_axis, beginNormAxis);
    (void)ge::AttrUtils::GetInt(op, hiai::op::LayerNorm::begin_params_axis, beginParamsAxis);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ProposalVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr output_actual_rois_num
    bool outputActualRoisNum = false;
    (void)ge::AttrUtils::GetBool(op, hiai::op::Proposal::output_actual_rois_num, outputActualRoisNum);
    if (!outputActualRoisNum) {
        FMK_LOGE("Proposal output_actual_rois_num attr only support true");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LSTMVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    bool exposeHidden = false;
    if (ge::AttrUtils::GetBool(op, hiai::op::LSTM::expose_hidden, exposeHidden) && exposeHidden) {
        FMK_LOGE("LSTM expose_hidden attr only support 'false'.");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus BidirectionLSTMVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    bool stateIsTuple = false;
    if (ge::AttrUtils::GetBool(op, hiai::op::BidirectionLSTM::state_is_tuple, stateIsTuple) && !stateIsTuple) {
        FMK_LOGE("BidirectionLSTM state_is_tuple attr only support 'true'.");
        return ge::GRAPH_FAILED;
    }
    std::string cellType = "LSTM";
    (void)ge::AttrUtils::GetStr(op, hiai::op::BidirectionLSTM::cell_type, cellType);
    if (cellType != "LSTM") {
        FMK_LOGE("BidirectionLSTM cell_type attr only support 'LSTM'. Current value is '%s'", cellType.c_str());
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus DepthToSpaceVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // attr data_format
    std::string dataMode = "NHWC";
    (void)ge::AttrUtils::GetStr(op, hiai::op::DepthToSpace::data_format, dataMode);
    if (dataMode != "NHWC") {
        FMK_LOGE("DepthToSpace mode attr only support NHWC");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ActivationVerify(ge::Node& node)
{
    ge::OpDesc& op = node.ROLE(NodeSpec).OpDesc();
    // mode
    int64_t modeValue = 0;
    if (!ge::AttrUtils::GetInt(op, hiai::op::Activation::mode, modeValue)) {
        FMK_LOGE("Activation mode attr is null");
        return ge::GRAPH_FAILED;
    }

    if (!(modeValue >= ACTIVATION_MODE_SIGMOID && modeValue <= ACTIVATION_MODE_GELU)) {
        FMK_LOGE("attr mode must be in range[%d, %d]", ACTIVATION_MODE_SIGMOID, ACTIVATION_MODE_GELU);
        return ge::GRAPH_FAILED;
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus L2NormalizeVerify(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    vector<int64_t> axisList;
    (void)ge::AttrUtils::GetListInt(desc, hiai::op::L2Normalize::axis, axisList);
    if (axisList.size() == 0) {
        FMK_LOGE("get attr axis failed");
        return ge::GRAPH_FAILED;
    }
    if (axisList[0] != L2_NORMALIZE_AXIS_VALUE) {
        FMK_LOGE("axis value must be 1");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus NonMaxSuppressionV3DVerify(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (desc.HasAttr(op::NonMaxSuppressionV3D::iou_threshold) &&
        desc.HasAttr(op::NonMaxSuppressionV3D::score_threshold) &&
        desc.HasAttr(op::NonMaxSuppressionV3D::max_output_size)) {
        return ge::GRAPH_SUCCESS;
    }
    FMK_LOGE("get attr iou_threshold or score_threshold or max_output_size failed");
    return ge::GRAPH_FAILED;
}

ge::GraphErrCodeStatus InterpVerify(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    int32_t shrinkFactor = 0;
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::shrink_factor, shrinkFactor);
    if (shrinkFactor > 0) {
        (void)ge::AttrUtils::SetInt(desc, INTERP_OUTPUT_DIM_MODE, RESIZE_OUTPUT_DIM_BY_SHRINK_FACTOR);
        return ge::GRAPH_SUCCESS;
    }

    int32_t zoomFactor = 0;
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::zoom_factor, zoomFactor);
    if (zoomFactor > 0) {
        (void)ge::AttrUtils::SetInt(desc, INTERP_OUTPUT_DIM_MODE, RESIZE_OUTPUT_DIM_BY_ZOOM_FACTOR);
        return ge::GRAPH_SUCCESS;
    }

    int32_t height = 0;
    int32_t width = 0;
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::height, height);
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::width, width);
    if (height > 0 && width > 0) {
        (void)ge::AttrUtils::SetInt(desc, INTERP_OUTPUT_DIM_MODE, RESIZE_OUTPUT_DIM_EXPLICIT);
        return ge::GRAPH_SUCCESS;
    }
    FMK_LOGE("Can only support one mode: 'height/width' or 'zoom_factor' or 'shrink_factor");
    return ge::GRAPH_FAILED;
}

ge::GraphErrCodeStatus ConvTransposeVerify(ge::Node& node)
{
    const auto& firstEdge = node.ROLE(NodeWalker).InDataEdge(0);
    if (firstEdge.Exist()) {
        return ge::GRAPH_SUCCESS;
    }

    const auto& secondEdge = node.ROLE(NodeWalker).InDataEdge(1);
    HIAI_EXPECT_TRUE(secondEdge.Exist());
    const auto& thirdEdge = node.ROLE(NodeWalker).InDataEdge(2);
    HIAI_EXPECT_TRUE(thirdEdge.Exist());

    auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveEdge(secondEdge.Value()));
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveEdge(thirdEdge.Value()));

    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).AddEdge(thirdEdge.Value().Src(), secondEdge.Value().Dst()));
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).AddEdge(secondEdge.Value().Src(), thirdEdge.Value().Dst()));

    HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveIdleEndpoint());
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus MaxUnpool2DVerify(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    // attr data_format
    std::string dataMode = "NCHW";
    (void)ge::AttrUtils::GetStr(desc, hiai::op::MaxUnpool2D::data_format, dataMode);
    if (dataMode != "NCHW") {
        FMK_LOGE("MaxUnpool2D attr data_format only support NCHW");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus MaxPoolWithArgmaxV2Verify(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    // attr dtype
    int64_t dtype = 3;
    (void)ge::AttrUtils::GetInt(desc, hiai::op::MaxPoolWithArgmaxV2::dtype, dtype);
    if (dtype != static_cast<int64_t>(ge::DT_INT32) && dtype != static_cast<int64_t>(ge::DT_INT64)) {
        FMK_LOGE("MaxPool with argmaxV2 attr dtype only support int32 or int64");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai
