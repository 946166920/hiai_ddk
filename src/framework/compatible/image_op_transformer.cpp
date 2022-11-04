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
#include "image_op_transformer.h"

#include "infra/base/assertion.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "graph/op/image_defs.h"
#include "graph/op/const_defs.h"
#include "graph/compatible/cpt_image_defs.h"

#include "ir_verify.h"

namespace hiai {
const int CROP_AND_RESIZE_CROP_DIM_ONE = 1;
const int CROP_AND_RESIZE_CROP_SIZE_TWO = 2;
const int INTERP_HEIGHT_DEFAULT_VALUE = -1;
const int INTERP_WIDTH_DEFAULT_VALUE = -1;

const std::string INTERP_OUTPUT_DIM_MODE = "output_dim_mode";

static ge::GraphErrCodeStatus CropAndResizeConvertConstOpToAttr(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (desc.HasAttr(ge::op::CropAndResize::crop_size_h) && desc.HasAttr(ge::op::CropAndResize::crop_size_w)) {
        return ge::GRAPH_SUCCESS;
    }

    size_t inputSize = node.ROLE(NodeSpec).InEdgeSize();
    HIAI_EXPECT_TRUE(inputSize == 4);

    Node* lastNode = node.ROLE(NodeWalker).InDataNode(3);
    HIAI_EXPECT_NOT_NULL(lastNode);

    HIAI_EXPECT_TRUE(lastNode->ROLE(NodeSpec).Type() == hiai::op::Const::TYPE);

    // new ir -> old ir
    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.empty()) {
        FMK_LOGE("Graph is null.");
        return ge::GRAPH_FAILED;
    }
    ge::TensorPtr cropSizeTensor = weightVec[weightVec.size() - 1];

    const ge::Shape& cropSizeShape = cropSizeTensor->GetTensorDesc().GetShape();
    GE_CHK_BOOL_RET_STATUS(static_cast<int32_t>(cropSizeShape.GetDimNum()) == CROP_AND_RESIZE_CROP_DIM_ONE,
        ge::GRAPH_FAILED, "Dim number of crop must be %d.", CROP_AND_RESIZE_CROP_DIM_ONE);
    GE_CHK_BOOL_RET_STATUS(cropSizeShape.GetDim(0) == CROP_AND_RESIZE_CROP_SIZE_TWO, ge::GRAPH_FAILED,
        "Shape of crop must be (%d).", CROP_AND_RESIZE_CROP_SIZE_TWO);
    GE_CHK_BOOL_RET_STATUS(
        cropSizeTensor->GetTensorDesc().GetDataType() == ge::DT_INT32, ge::GRAPH_FAILED, "crop_size must be int32.");
    // get data
    int32_t* sizeHW = reinterpret_cast<int32_t*>(const_cast<uint8_t*>(cropSizeTensor->GetData().GetData()));
    if (sizeHW == nullptr) {
        return ge::GRAPH_FAILED;
    }
    (void)ge::AttrUtils::SetInt(desc, ge::op::CropAndResize::crop_size_h, static_cast<int64_t>(*sizeHW));
    (void)ge::AttrUtils::SetInt(desc, ge::op::CropAndResize::crop_size_w, static_cast<int64_t>(*(sizeHW + 1)));

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus CropAndResizeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return CropAndResizeAttrConvertToConstOp(node);
    }
    // get node version
    int versionIR = 0;
    (void)ge::AttrUtils::GetInt(desc, OP_VERSION, versionIR);
    if (!isOldToNew && versionIR == VESION_VALUE_DEFAULT) {
        if (CropAndResizeConvertConstOpToAttr(node) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("const op to attr failed");
            return ge::GRAPH_FAILED;
        }
    }
    // method attr: str->int
    const std::map<string, int64_t> attrValueMap = {
        {"bilinear", 0},
        {"nearest", 1},
    };
    std::string methodCropAndResize;
    if (ge::AttrUtils::GetStr(desc, hiai::op::CropAndResize::method, methodCropAndResize)) {
        GE_CHK_BOOL_RET_STATUS(desc.DelAttr(hiai::op::CropAndResize::method) == ge::GRAPH_SUCCESS,
            ge::GRAPH_FAILED, "Delete ir attr method to opdesc failed!");
        GE_CHK_BOOL_RET_STATUS(attrValueMap.count(methodCropAndResize) > 0, ge::GRAPH_FAILED,
            "The method must be bilinear or nearest, now is %s.", methodCropAndResize.c_str());
        (void)ge::AttrUtils::SetInt(desc, hiai::op::CropAndResize::method, attrValueMap.at(methodCropAndResize));
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus InterpConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    int32_t height = 0;
    int32_t width = 0;
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::height, height);
    (void)ge::AttrUtils::GetInt(desc, hiai::op::Interp::width, width);
    // Change the values of height and width to 0 if they are -1
    if (height == INTERP_HEIGHT_DEFAULT_VALUE && width == INTERP_WIDTH_DEFAULT_VALUE) {
        (void)ge::AttrUtils::SetInt(desc, hiai::op::Interp::height, 0);
        (void)ge::AttrUtils::SetInt(desc, hiai::op::Interp::width, 0);
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ROIAlignConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    GE_CHK_STATUS_RET_NOLOG(TransformTypeConverter(node, config, isOldToNew));

    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    int32_t pooledHeight = 0;
    int32_t pooledWidth = 0;
    float spatialScale = 0;
    (void)ge::AttrUtils::GetInt(opdesc, hiai::op::ROIAlignV2::pooled_height, pooledHeight);
    (void)ge::AttrUtils::GetInt(opdesc, hiai::op::ROIAlignV2::pooled_width, pooledWidth);
    vector<float> spatialScaleVec;
    (void)ge::AttrUtils::GetListFloat(opdesc, hiai::op::ROIAlignV2::spatial_scale, spatialScaleVec);
    if (spatialScaleVec.empty()) {
        FMK_LOGE("get attr spatial_scale failed !");
        return ge::GRAPH_FAILED;
    }
    spatialScale = spatialScaleVec.front();
    GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr(hiai::op::ROIAlignV2::pooled_height) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "Delete ir attr pooled_height to opdesc failed!");
    GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr(hiai::op::ROIAlignV2::pooled_width) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "Delete ir attr pooled_width to opdesc failed!");
    GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr(hiai::op::ROIAlignV2::spatial_scale) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "Delete ir attr spatial_scale to opdesc failed!");
    (void)ge::AttrUtils::SetInt(opdesc, "pooled_h", pooledHeight);
    (void)ge::AttrUtils::SetInt(opdesc, "pooled_w", pooledWidth);
    (void)ge::AttrUtils::SetFloat(opdesc, "spatial_scale", spatialScale);

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus InterpOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (opdesc.HasAttr(INTERP_OUTPUT_DIM_MODE)) {
        return ge::GRAPH_SUCCESS;
    }
    if (!isOldToNew) {
        InterpVerify(node);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ResizeNearestNeighborOMConverter(ge::Node& node, const ConvertConfigInfo& config,
    bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew && opDesc.GetInputsDescSize() == 3) {
        return node.ROLE(NodeCompatibler).RemoveSpecificInput(1);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ROIAlignOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        GE_CHK_STATUS_RET_NOLOG(TransformTypeConverter(node, config, isOldToNew));
        int32_t pooled_height = 0;
        int32_t pooled_width = 0;
        int32_t sampling_num = 0;
        float spatial_scale = 0;
        (void)ge::AttrUtils::GetInt(opdesc, "pooled_h", pooled_height);
        (void)ge::AttrUtils::GetInt(opdesc, "pooled_w", pooled_width);
        GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr("pooled_h") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "Delete ir attr pooled_h to opdesc failed!");
        GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr("pooled_w") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "Delete ir attr pooled_w to opdesc failed!");
        (void)ge::AttrUtils::GetFloat(opdesc, hiai::op::ROIAlignV2::spatial_scale, spatial_scale);
        GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr(hiai::op::ROIAlignV2::spatial_scale) == ge::GRAPH_SUCCESS,
            ge::GRAPH_FAILED, "Delete ir attr pooled_h to opdesc failed!");
        (void)ge::AttrUtils::SetInt(opdesc, hiai::op::ROIAlignV2::pooled_height, pooled_height);
        (void)ge::AttrUtils::SetInt(opdesc, hiai::op::ROIAlignV2::pooled_width, pooled_width);
        vector<float> spatialScaleVec = {spatial_scale};
        (void)ge::AttrUtils::SetListFloat(opdesc, hiai::op::ROIAlignV2::spatial_scale, spatialScaleVec);
        (void)ge::AttrUtils::GetInt(opdesc, "sampling_ratio", sampling_num);
        GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr("sampling_ratio") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "Delete ir attr sampling_ratio to opdesc failed!");
        (void)ge::AttrUtils::SetInt(opdesc, hiai::op::ROIAlignV2::sample_num, sampling_num);
        return ge::GRAPH_SUCCESS;
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus NonMaxSuppressionOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        if (opDesc.GetInputsDescSize() == 4) {
            HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveSpecificInput(3));
            HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveSpecificInput(2));
        }
    } else {
        float scoreThreshold = 0.0;
        if (ge::AttrUtils::GetFloat(opDesc, op::NonMaxSuppressionV3D::score_threshold, scoreThreshold)) {
            vector<ge::TensorPtr> weights = ge::OpDescUtils::MutableWeights(node);
            if (weights.size() >= 3 && weights[2] != nullptr) {
                scoreThreshold = *(reinterpret_cast<float*>(const_cast<uint8_t*>(weights[2]->GetData().data())));
            } else {
                scoreThreshold = 0;
            }
            (void)ge::AttrUtils::SetFloat(opDesc, op::NonMaxSuppressionV3D::score_threshold, scoreThreshold);
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus CropAndResizeOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    return CropAndResizeAttrConvertToConstOp(node);
}

ge::GraphErrCodeStatus CropAndResizeAttrConvertToConstOp(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // 删除空anchor
    HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveIdleEndpoint());

    int32_t cropSizeH = 0;
    int32_t cropSizeW = 0;
    // 这两个if语句不能合并，否则只能获取到前面的一个
    // 因为&& 当第一个条件不成立，就会短路，后面的条件不会执行
    if (!ge::AttrUtils::GetInt(desc, ge::op::CropAndResize::crop_size_h, cropSizeH)) {
        FMK_LOGW("old ir get attr crop_size_h failed!");
        return ge::GRAPH_SUCCESS;
    }
    if (!ge::AttrUtils::GetInt(desc, ge::op::CropAndResize::crop_size_w, cropSizeW)) {
        FMK_LOGW("old ir get attr crop_size_w failed!");
        return ge::GRAPH_SUCCESS;
    }
    // convert attr to input crop_size
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    // construct weight
    ge::TensorDesc cropSizeDesc(ge::Shape({2}), ge::FORMAT_ND, ge::DT_INT32);
    ge::TensorPtr tensorCropSize = ge::TensorPtr(new (std::nothrow) ge::Tensor(cropSizeDesc));
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(
        tensorCropSize == nullptr, return ge::GRAPH_FAILED, "tensorCropSize construct failed.");
    vector<int32_t> dataValueCropSize = {cropSizeH, cropSizeW};
    tensorCropSize->SetData(reinterpret_cast<uint8_t*>(dataValueCropSize.data()), sizeof(int32_t) * 2);
    // update weight
    weightsVec.push_back(tensorCropSize);
    (void)ge::OpDescUtils::SetWeights(node, weightsVec);
    // delete attr
    (void)desc.DelAttr(ge::op::CropAndResize::crop_size_h);
    (void)desc.DelAttr(ge::op::CropAndResize::crop_size_w);
    // update is_input_const
    vector<bool> inInputConst = desc.GetIsInputConst();
    inInputConst.push_back(true);
    desc.SetIsInputConst(inInputConst);
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai