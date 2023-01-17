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
#include "array_op_transformer.h"

#include "infra/base/assertion.h"

#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/debug/ge_graph_attr_define.h"

#include "graph/op/array_defs.h"
#include "framework/graph/op/internal_nn_defs.h"
#include "graph/op/const_defs.h"
#include "graph/op/detection_defs.h"
#include "graph/compatible/cpt_array_defs.h"

namespace hiai {
const int THREE_INPUT = 3;
const std::string PAD_T_PADDINGS = "t_paddings";
const int MIRRORPAD_MODE_REFLECT = 1;
const int MIRRORPAD_MODE_SYMMETRIC = 2;

ge::GraphErrCodeStatus SelectConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;

    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    auto visitor = [&opDesc](ge::Node& inNode) {
        const auto& preDesc = inNode.ROLE(NodeSpec).OpDesc();
        if (preDesc.GetType() == hiai::op::Data::TYPE) {
            size_t dataDimNum = preDesc.GetInputDesc(0).GetShape().GetDimNum();
            if (dataDimNum == 1) {
                (void)ge::AttrUtils::SetInt(opDesc, "inferred_format", static_cast<int64_t>(ge::FORMAT_NCHW));
            }
        }
        return hiai::SUCCESS;
    };
    return node.ROLE(NodeWalker).ListInDataNodes(visitor);
}

ge::GraphErrCodeStatus GatherV2DConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    HIAI_EXPECT_EXEC(TransformTypeConverter(node, config, isOldToNew));

    auto visitor = [](ge::Node& inNode) {
        auto& opDesc = inNode.ROLE(NodeSpec).OpDesc();
        if (opDesc.GetType() == hiai::op::Const::TYPE) {
            (void)ge::AttrUtils::SetInt(opDesc, "output_format", static_cast<int64_t>(ge::FORMAT_ND));
        }
        return hiai::SUCCESS;
    };
    return node.ROLE(NodeWalker).ListInNodes(visitor);
}

ge::GraphErrCodeStatus ConcatDConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    HIAI_EXPECT_EXEC(TypeAndAttrConverter(node, config, isOldToNew));

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    uint32_t attrN = 0;
    HIAI_EXPECT_TRUE_R(ge::AttrUtils::GetInt(opDesc, hiai::op::ConcatD::N, attrN), ge::GRAPH_SUCCESS);

    std::size_t realInputSize = node.ROLE(NodeSpec).InEdgeSize();
    HIAI_EXPECT_TRUE_R(attrN >= realInputSize, ge::GRAPH_SUCCESS);

    int32_t concatAxis = -1;
    HIAI_EXPECT_TRUE_R(ge::AttrUtils::GetInt(opDesc, ge::op::Concat::axis, concatAxis), ge::GRAPH_SUCCESS);

    ge::TensorPtr tmpAxis = hiai::make_shared_nothrow<ge::Tensor>();
    HIAI_EXPECT_NOT_NULL(tmpAxis);

    tmpAxis->MutableTensorDesc().SetShape(ge::Shape({1}));
    tmpAxis->SetData(reinterpret_cast<uint8_t*>(&concatAxis), sizeof(ge::DT_INT32));
    tmpAxis->MutableTensorDesc().SetDataType(ge::DT_INT32);
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    weightsVec.push_back(tmpAxis);
    ge::OpDescUtils::SetWeights(node, weightsVec);
    std::vector<bool> isInputConst = opDesc.GetIsInputConst();
    isInputConst.push_back(true);
    opDesc.SetIsInputConst(isInputConst);

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus OneHotConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;

    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // T
    if (desc.HasAttr("T")) {
        return ge::GRAPH_SUCCESS;
    }

    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.empty()) {
        FMK_LOGE("graph is null.");
        return false;
    }
    GE_CHK_BOOL_RET_STATUS(weightVec.size() >= THREE_INPUT, ge::GRAPH_FAILED, "inputs at least 3 is constant!");
    int64_t dataType = static_cast<int64_t>(weightVec[weightVec.size() - 1]->GetTensorDesc().GetDataType());
    (void)ge::AttrUtils::SetInt(desc, "T", dataType);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus PadConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // mode
    int mode = 0;
    if (!ge::AttrUtils::GetInt(desc, hiai::op::PadV3::mode, mode)) {
        (void)ge::AttrUtils::SetInt(desc, hiai::op::PadV3::mode, mode);
    }
    // t_paddings
    int tPaddings = 3;
    if (!ge::AttrUtils::GetInt(desc, PAD_T_PADDINGS, tPaddings)) {
        (void)ge::AttrUtils::SetInt(desc, PAD_T_PADDINGS, tPaddings);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SplitVConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    int32_t weightSize = 2;
    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.size() != static_cast<uint32_t>(weightSize)) {
        FMK_LOGE("splitV get size_splits or split_dim failed.");
        return ge::GRAPH_FAILED;
    }

    GE_CHK_BOOL_RET_STATUS(
        weightVec[1]->GetTensorDesc().GetDataType() == ge::DT_INT32, ge::GRAPH_FAILED, "split_dim must be int32.");
    ge::TensorDesc splitDimTensorDesc(ge::Shape(), ge::DT_INT32);
    ge::TensorPtr splitDim = std::make_shared<ge::Tensor>(
        splitDimTensorDesc, static_cast<const uint8_t*>(weightVec[1]->GetData().GetData()), 1 * sizeof(int32_t));
    vector<ge::TensorPtr> weights;
    weights.push_back(weightVec[0]);
    weights.push_back(splitDim);
    ge::OpDescUtils::SetWeights(node, weights);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus MirrorPadOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // mode
    if (!desc.HasAttr(hiai::op::MirrorPad::mode)) {
        FMK_LOGE("mirrorPad hasn't mode.");
        return ge::GRAPH_FAILED;
    }
    // int -> str
    if (isOldToNew) {
        int mode = MIRRORPAD_MODE_REFLECT;
        if (!ge::AttrUtils::GetInt(desc, hiai::op::MirrorPad::mode, mode)) {
            return ge::GRAPH_SUCCESS;
        }
        GE_CHK_BOOL_RET_STATUS(desc.DelAttr(hiai::op::MirrorPad::mode) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "delete int attr %s to opdesc failed!", ATTR_MODE.c_str());
        if (mode == MIRRORPAD_MODE_REFLECT) {
            (void)ge::AttrUtils::SetStr(desc, hiai::op::MirrorPad::mode, "REFLECT");
            return ge::GRAPH_SUCCESS;
        } else if (mode == MIRRORPAD_MODE_SYMMETRIC) {
            (void)ge::AttrUtils::SetStr(desc, hiai::op::MirrorPad::mode, "SYMMETRIC");
            return ge::GRAPH_SUCCESS;
        }
    } else { // str->int
        std::string mode;
        if (!ge::AttrUtils::GetStr(desc, hiai::op::MirrorPad::mode, mode)) {
            return ge::GRAPH_SUCCESS;
        }
        GE_CHK_BOOL_RET_STATUS(desc.DelAttr(hiai::op::MirrorPad::mode) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "delete str attr %s to opdesc failed!", ATTR_MODE.c_str());
        if (mode == "REFLECT") {
            (void)ge::AttrUtils::SetInt(desc, hiai::op::MirrorPad::mode, MIRRORPAD_MODE_REFLECT);
            return ge::GRAPH_SUCCESS;
        } else if (mode == "SYMMETRIC") {
            (void)ge::AttrUtils::SetInt(desc, hiai::op::MirrorPad::mode, MIRRORPAD_MODE_SYMMETRIC);
            return ge::GRAPH_SUCCESS;
        }
    }
    FMK_LOGE("mode is invalid.");
    return ge::GRAPH_FAILED;
}

static ge::GraphErrCodeStatus ConvertLastConstInputToAttr(ge::Node& node)
{
    auto visitor = [](const ge::Tensor& weight, ge::OpDesc& opDesc) {
        ge::DataType dType = weight.GetTensorDesc().GetDataType();
        HIAI_EXPECT_TRUE_R(dType == ge::DT_INT32, ge::GRAPH_SUCCESS);

        const int32_t* weightData = reinterpret_cast<const int32_t*>(weight.GetData().GetData());
        HIAI_EXPECT_NOT_NULL(weightData);

        uint32_t count = weight.GetData().GetSize() / sizeof(int32_t);
        HIAI_EXPECT_TRUE_R(count == 1, ge::GRAPH_SUCCESS);

        HIAI_EXPECT_TRUE(
            ge::AttrUtils::SetInt(opDesc, hiai::op::ConcatD::concat_dim, static_cast<int64_t>(weightData[0])));

        return hiai::SUCCESS;
    };
    auto& nodeSpec = node.ROLE(NodeSpec);
    std::size_t inputSize = nodeSpec.InEdgeSize();

    return node.ROLE(NodeCompatibler).TransConstInputToAttr(inputSize - 1, visitor);
}

static ge::GraphErrCodeStatus SurplusInputNodeRemoveForConcatOp(ge::Node& node)
{
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    uint32_t attrN = 1;
    HIAI_EXPECT_TRUE_R(ge::AttrUtils::GetInt(opDesc, hiai::op::ConcatD::N, attrN), ge::GRAPH_SUCCESS);

    std::size_t inputSize = node.ROLE(NodeSpec).InEdgeSize();
    HIAI_EXPECT_TRUE_R(inputSize - attrN == 1, ge::GRAPH_SUCCESS);

    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    HIAI_EXPECT_TRUE_R(!weightsVec.empty(), ge::GRAPH_SUCCESS);

    ge::DataType dType = weightsVec[weightsVec.size() - 1]->GetTensorDesc().GetDataType();
    HIAI_EXPECT_TRUE_R(dType == ge::DT_INT32, ge::GRAPH_SUCCESS);

    return node.ROLE(NodeCompatibler).RemoveSpecificInput(inputSize - 1);
}

ge::GraphErrCodeStatus ConcatDOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    HIAI_EXPECT_TRUE(ge::AttrUtils::SetInt(opdesc, "Tidx", 3));

    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    bool isNeedRemoveReduntConstInput = opdesc.HasAttr(hiai::op::ConcatD::concat_dim);
    if (isNeedRemoveReduntConstInput) {
        return SurplusInputNodeRemoveForConcatOp(node);
    } else {
        return ConvertLastConstInputToAttr(node);
    }
}

ge::GraphErrCodeStatus GatherV2DOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (opDesc.GetInputsDescSize() == 3) {
        return node.ROLE(NodeCompatibler).RemoveSpecificInput(2);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SplitDOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (desc.GetInputsDescSize() != 2) {
        return ge::GRAPH_SUCCESS;
    }

    // attr split_dim
    if (desc.HasAttr(hiai::op::SplitD::split_dim)) {
        return node.ROLE(NodeCompatibler).RemoveSpecificInput(0);
    }

    auto visitor = [](const ge::Tensor& weight, ge::OpDesc& opDesc) {
        if (weight.GetTensorDesc().GetDataType() != ge::DT_INT32) {
            FMK_LOGE("axes must be int32.");
            return hiai::FAILURE;
        }

        size_t axisSize = weight.GetData().size() / sizeof(int32_t);
        if (axisSize != 1) {
            return hiai::FAILURE;
        }

        const int32_t* axis = reinterpret_cast<const int32_t*>(weight.GetData().data());
        if (axis == nullptr) {
            return hiai::FAILURE;
        }
        (void)ge::AttrUtils::SetInt(opDesc, hiai::op::SplitD::split_dim, static_cast<int64_t>(axis[0]));
        return hiai::SUCCESS;
    };

    return node.ROLE(NodeCompatibler).TransConstInputToAttr(0, visitor);
}

ge::GraphErrCodeStatus DepthToSpaceOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    std::string dataFormat;
    if (!ge::AttrUtils::GetStr(desc, hiai::op::DepthToSpace::data_format, dataFormat)) {
        return ge::GRAPH_SUCCESS;
    } else {
        GE_CHK_BOOL_RET_STATUS(
            DataFormatToEnum(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "data format to enum failed!");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ReshapeOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (opdesc.HasAttr("to_be_deleted")) {
        GE_CHK_BOOL_RET_STATUS(opdesc.DelAttr("to_be_deleted") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "delete ir attr to_be_deleted to opdesc failed!");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus PadOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // mode
    int mode = 0;
    if (!ge::AttrUtils::GetInt(desc, hiai::op::PadV3::mode, mode)) {
        (void)ge::AttrUtils::SetInt(desc, hiai::op::PadV3::mode, mode);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SplitVOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    int64_t axis = 0;
    if (ge::AttrUtils::GetInt(desc, ge::op::Split::axis, axis)) {
        return ge::GRAPH_SUCCESS;
    }
    if (desc.GetInputsDescSize() != 3) {
        return ge::GRAPH_SUCCESS;
    }
    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.size() != 2) {
        return ge::GRAPH_FAILED;
    }
    if (weightVec[1]->GetTensorDesc().GetDataType() != ge::DT_INT32) {
        FMK_LOGE("axis must be int32.");
        return ge::GRAPH_FAILED;
    }
    size_t axisSize = weightVec[1]->GetData().size() / sizeof(int32_t);
    if (axisSize != 1) {
        return ge::GRAPH_FAILED;
    }
    const int32_t* axislist = reinterpret_cast<const int32_t*>(weightVec[1]->GetData().data());
    if (axislist == nullptr) {
        return ge::GRAPH_FAILED;
    }
    (void)ge::AttrUtils::SetInt(desc, ge::op::Split::axis, static_cast<int64_t>(axislist[0]));
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FlattenOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
    Node* ssdNode = graph.ROLE(GraphFinder).FindNode([](Node& n) {
        return n.ROLE(NodeSpec).Type() == hiai::op::SSDDetectionOutput::TYPE;
    });
    if (ssdNode != nullptr) {
        return TransformTypeConverter(node, config, isOldToNew);
    }
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus GetStridedSliceV2ConstInputValue(
    ge::OpDesc& desc, const vector<ge::TensorPtr>& weightsVec, vector<int32_t>& constInputValue)
{
    if (weightsVec.size() < 3) {
        return ge::GRAPH_FAILED;
    }
    size_t inputSize = desc.GetInputDesc(0).GetShape().GetDimNum();
    const int32_t* constInput1 = reinterpret_cast<const int32_t*>(weightsVec[0]->GetData().GetData());
    const int32_t* constInput2 = reinterpret_cast<const int32_t*>(weightsVec[1]->GetData().GetData());
    const int32_t* constInput3 = reinterpret_cast<const int32_t*>(weightsVec[2]->GetData().GetData());
    size_t wSize1 = weightsVec[0]->GetData().size() / sizeof(int32_t);
    size_t wSize2 = weightsVec[1]->GetData().size() / sizeof(int32_t);
    size_t wSize3 = weightsVec[2]->GetData().size() / sizeof(int32_t);
    if ((inputSize != 0) && (wSize1 != inputSize || wSize2 != inputSize || wSize3 != inputSize)) {
        return ge::GRAPH_FAILED;
    }
    if (inputSize <= 0) {
        // inputSize maybe 0 （old ir graph， no infershape）
        return ge::GRAPH_SUCCESS;
    }
    if (constInput1 == nullptr || constInput2 == nullptr || constInput3 == nullptr) {
        return ge::GRAPH_FAILED;
    }
    // recover attr (start、end、axis、strides) of StridedSliceV2
    for (size_t i = 0; i < wSize1; i++) {
        if (constInput1[i] != 0) {
            constInputValue = {constInput1[i], constInput2[i], static_cast<int>(i), constInput3[i]};
            return ge::GRAPH_SUCCESS;
        }
    }
    auto dims = desc.GetInputDesc(0).GetShape().GetDims();
    for (size_t i = 0; i < wSize2; i++) {
        if (dims[i] != constInput2[i]) {
            constInputValue = {constInput1[i], constInput2[i], static_cast<int>(i), constInput3[i]};
            return ge::GRAPH_SUCCESS;
        }
    }
    return ge::GRAPH_SUCCESS; // cannot recover attr value(other scene)
}

static void ChangeStridedSliceV2ConstInputShape(
    ge::Node& node, vector<ge::TensorPtr>& weightsVec, const vector<int32_t>& constInputValue)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // change const inputdesc shape to 1-d tensor
    for (size_t i = 1; i < desc.GetInputsSize(); i++) {
        ge::TensorDescPtr inputDesc = desc.MutableInputDesc(i);
        inputDesc->SetShape(ge::Shape({1}));
    }
    // chang the data value of const node, 1-d tensor
    for (size_t i = 0; i < constInputValue.size(); i++) {
        if (i < 3) {
            ge::TensorDesc& weightDesc = weightsVec[i]->MutableTensorDesc();
            weightDesc.SetShape(ge::Shape({1}));
            weightsVec[i]->SetData(reinterpret_cast<const uint8_t*>(&constInputValue[i]), sizeof(int32_t));
        } else {
            ge::TensorDesc v2TensorDesc(ge::Shape({1}), ge::FORMAT_NCHW, ge::DT_INT32);
            ge::TensorPtr v2Tensor = ge::TensorPtr(new (std::nothrow) ge::Tensor(v2TensorDesc));
            v2Tensor->SetData(reinterpret_cast<const uint8_t*>(&constInputValue[i]), sizeof(int32_t));
            weightsVec.push_back(v2Tensor);
        }
    }
}

ge::GraphErrCodeStatus StridedSliceV2OMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    if (weightsVec.size() != 3) {
        return ge::GRAPH_SUCCESS;
    }

    ge::ComputeGraph& originGraph = node.ROLE(NodeSpec).OwnerComputeGraph();

    if (originGraph.HasAttr(SRC_MERGED_WEIGHT_ADDR) && originGraph.HasAttr(SRC_MERGED_WEIGHT_SIZE)) {
        HIAI_EXPECT_EXEC(SplitGraphMergedWeight(originGraph));
    }

    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    vector<int32_t> constInputValue;
    if (GetStridedSliceV2ConstInputValue(desc, weightsVec, constInputValue) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("get strided slice v2 const input value failed");
        return ge::GRAPH_FAILED;
    }
    if (constInputValue.empty()) {
        return ge::GRAPH_SUCCESS;
    }

    // change const inputdesc shape to 1-d tensor
    ChangeStridedSliceV2ConstInputShape(node, weightsVec, constInputValue);

    string strideName = weightsVec[2]->GetTensorDesc().GetName(); // name of stride weight tensor
    if (strideName.size() > 0) {
        strideName = "";
        weightsVec[2]->MutableTensorDesc().SetName(strideName);
    }
    (void)ge::OpDescUtils::SetWeights(node, weightsVec);
    vector<bool> inInputConst = desc.GetIsInputConst();
    inInputConst.push_back(true);
    desc.SetIsInputConst(inInputConst);
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai