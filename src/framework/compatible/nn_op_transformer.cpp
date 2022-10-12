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
#include "nn_op_transformer.h"
#include <set>

#include "infra/base/assertion.h"
#include "framework/graph/debug/ge_attr_define.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/edge/edge.h"

#include "graph/op/nn_defs.h"
#include "graph/op/array_defs.h"
#include "graph/op/detection_defs.h"
#include "framework/graph/op/internal_defs.h"
#include "graph/compatible/cpt_nn_defs.h"

namespace hiai {
const std::set<string> nextOpSet = {"Softmax", "FullyConnection", "ChannelAxpy", "Flatten", "FlattenV2", "SSDNormalize",
    "Normalize", "FSRDetectionOutput", "SSDPriorBox", "PriorBox", "Activation", "NetOutput"};
const int ACTIVATION_MODE_HARDSWISH = 16;
const int DEPTHWISE_CONV_MODE = 3;
const int DIM_DEFAULT_VALUE = 1;
const int NORMAL_TENSOR_SIZE = 4;
const int DIMNUM_THREE_SIZE = 3;

static ge::GraphErrCodeStatus DataFormatToString(ge::OpDesc& desc)
{
    // data_format int->string
    const std::map<int, string> attrDataFormatMap = {
        {0, "NCHW"},
        {1, "NHWC"},
    };
    int DataFormat;
    if (ge::AttrUtils::GetInt(desc, "format", DataFormat)) {
        GE_CHK_BOOL_RET_STATUS(attrDataFormatMap.count(DataFormat) > 0, ge::GRAPH_FAILED,
            "The pad mode must be NCHW or NHWC, now is %d.", DataFormat);
        (void)ge::AttrUtils::SetStr(desc, "data_format", attrDataFormatMap.at(DataFormat));
    }
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus PadModeToEnum(ge::OpDesc& desc)
{
    // pad_mode string ->int
    const std::map<string, int64_t> attrPadModeMap = {
        {"SPECIFIC", 4},
        {"VALID", 5},
        {"SAME", 6},
    };
    std::string padMode;
    if (ge::AttrUtils::GetStr(desc, "pad_mode", padMode)) {
        GE_CHK_BOOL_RET_STATUS(attrPadModeMap.count(padMode) > 0, ge::GRAPH_FAILED,
            "The method must be SPECIFIC, VALID or SAME, now is %s.", padMode.c_str());
        GE_CHK_BOOL_RET_STATUS(desc.DelAttr("pad_mode") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "Delete ir attr pad_mode to opdesc failed!");
        (void)ge::AttrUtils::SetInt(desc, "pad_mode", attrPadModeMap.at(padMode));
    }
    return ge::GRAPH_SUCCESS;
}

static ge::OpDescPtr CreateTansposeOp(std::string name)
{
    static uint32_t transpose_count = 1;
    string sstmp = name + std::to_string(transpose_count++);

    ge::OpDescPtr opDesc = hiai::make_shared_nothrow<ge::OpDesc>(sstmp, "Permute");
    GE_CHK_BOOL_EXEC(opDesc != nullptr, return nullptr, "opDesc is nullptr");

    opDesc->AddInputDesc(TensorDesc(Shape(), FORMAT_ND));
    opDesc->AddOutputDesc(TensorDesc(Shape(), FORMAT_ND));
    (void)ge::AttrUtils::SetListStr(opDesc, "original_op_names", {""});

    vector<bool> isInputConst;
    isInputConst.push_back(false);
    opDesc->SetIsInputConst(isInputConst);
    opDesc->SetAttr("version", ge::AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(5)));
    return opDesc;
}

static ge::GraphErrCodeStatus PadModeToString(ge::OpDesc& desc)
{
    // pad_mode int->string
    const std::map<int, string> attrPadModeMap = {
        {0, "SPECIFIC"},
        {1, "SPECIFIC"},
        {2, "SPECIFIC"},
        {3, "SPECIFIC"},
        {4, "SPECIFIC"},
        {5, "VALID"},
        {6, "SAME"},
    };
    int padMode;
    if (ge::AttrUtils::GetInt(desc, "pad_mode", padMode)) {
        GE_CHK_BOOL_RET_STATUS(attrPadModeMap.count(padMode) > 0, ge::GRAPH_FAILED,
            "The pad mode must be SPECIFIC, VALID or SAME, now is %d.", padMode);
        GE_CHK_BOOL_RET_STATUS(desc.DelAttr("pad_mode") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "Delete ir attr pad_mode to opdesc failed!");
        (void)ge::AttrUtils::SetStr(desc, "pad_mode", attrPadModeMap.at(padMode));
    }
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus SetConvPadmode(ge::OpDesc& desc)
{
    string padMode;
    if (desc.GetType() == hiai::op::Convolution::TYPE && ge::AttrUtils::GetStr(
        desc, hiai::op::Convolution::pad_mode, padMode) && padMode == "VALID") {
        vector<int64_t> padsList;
        (void)ge::AttrUtils::GetListInt(desc, hiai::op::Convolution::pads, padsList);
        for (int64_t padValue : padsList) {
            if (padValue != 0) {
                (void)ge::AttrUtils::SetStr(desc, hiai::op::Convolution::pad_mode, "SPECIFIC");
                (void)desc.DelAttr("pad_conv");
                break;
            }
        }
    }
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus BiasComplete(ge::Node& node)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (!desc.HasAttr("has_bias")) {
        vector<ge::TensorPtr> weights = ge::OpDescUtils::MutableWeights(node);
        if (desc.GetType() == hiai::op::ConvTranspose::TYPE) {
            if (weights.size() == 3 && weights[0] != nullptr &&
                weights[0]->GetTensorDesc().GetShape().GetDimNum() == 1 &&
                weights[0]->GetTensorDesc().GetDataType() == ge::DT_INT32) {
                (void)ge::AttrUtils::SetBool(desc, "has_bias", true);
            } else if (weights.size() == 2 && weights[0] != nullptr && weights[1] != nullptr &&
                ((weights[0]->GetTensorDesc().GetDataType() == ge::DT_FLOAT &&
                weights[1]->GetTensorDesc().GetDataType() == ge::DT_FLOAT) ||
                (weights[0]->GetTensorDesc().GetDataType() == ge::DT_FLOAT16 &&
                weights[1]->GetTensorDesc().GetDataType() == ge::DT_FLOAT16))) {
                (void)ge::AttrUtils::SetBool(desc, "has_bias", true);
            }
        } else if (desc.GetType() != hiai::op::ConvTranspose::TYPE && weights.size() == 2) { // 2代表weightsVec数组的长度
            (void)ge::AttrUtils::SetBool(desc, "has_bias", true);
        }
        bool hasBias = false;
        (void)ge::AttrUtils::GetBool(desc, "has_bias", hasBias);
        if (hasBias) {
            ge::TensorPtr bias = weights[weights.size() - 1];
            const ge::Shape& biasShape = bias->GetTensorDesc().GetShape();
            if (biasShape.GetDimNum() == 1) {
                vector<int64_t> shapeValue;
                shapeValue.push_back(1);
                shapeValue.push_back(biasShape.GetDim(0));
                shapeValue.push_back(1);
                shapeValue.push_back(1);
                bias->MutableTensorDesc().SetShape(ge::Shape(shapeValue));
            }
        }
    }
    if (!desc.HasAttr("has_bias_value") && desc.HasAttr("has_bias")) {
        bool hasBias = false;
        (void)ge::AttrUtils::GetBool(desc, "has_bias", hasBias);
        (void)ge::AttrUtils::SetBool(desc, "has_bias_value", hasBias);
    }
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus ReConnetLSTMOutput(ge::Node& node)
{
    // if current node has 7 inputs, swap the order of 2nd ~ 5th.
    if (node.ROLE(NodeSpec).OpDesc().GetInputsDescSize() == 7) {
        vector<Edge> handleEdges;
        auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
        auto visitor = [&graph, &handleEdges](Edge& edge) {
            if (edge.DstIdx() >= 2 && edge.DstIdx() <= 5) {
                handleEdges.push_back(edge);
                return graph.ROLE(GraphModifier).RemoveEdge(edge);
            }
            return hiai::SUCCESS;
        };
        (void)node.ROLE(NodeWalker).ListInDataEdges(visitor);

        HIAI_EXPECT_TRUE(handleEdges.size() == 4);
        for (int i = 0; i < 4; i++) {
            HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).AddEdge(handleEdges[i].Src(), handleEdges[(i + 3) % 4].Dst()));
        }
    }
    return ge::GRAPH_SUCCESS;
}

// 算子转换函数
ge::GraphErrCodeStatus HardSwishConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    if (config.dstOpType != "") {
        desc.SetType(config.dstOpType);
    }
    (void)ge::AttrUtils::SetInt(desc, hiai::op::Activation::mode, ACTIVATION_MODE_HARDSWISH);
    (void)ge::AttrUtils::SetFloat(desc, hiai::op::Activation::coef, 0.0);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SpaceToDepthConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // set format
    if (!desc.HasAttr("data_format") && !desc.HasAttr("format")) {
        (void)ge::AttrUtils::SetStr(desc, hiai::op::SpaceToDepth::data_format, "NHWC");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ToDynamicInputConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        int32_t inputNum = static_cast<int32_t>(desc.GetInputsSize());
        (void)ge::AttrUtils::SetInt(desc, hiai::op::Eltwise::N, inputNum);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus Conv2DTransposeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // get node version
    int versionIR = 0;
    (void)ge::AttrUtils::GetInt(desc, OP_VERSION, versionIR);

    if (isOldToNew || (versionIR == VESION_VALUE_DEFAULT)) {
        GE_CHK_BOOL_RET_STATUS(TypeAndAttrConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "type and attr update failed!");
    }
    if (!isOldToNew) {
        GE_CHK_BOOL_RET_STATUS(PadModeToEnum(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "pad mode to enum failed!");
    }

    if (!isOldToNew && versionIR == VESION_VALUE_DEFAULT) {
        // mode and algo need complte when new -> old
        if (desc.HasAttr("mode")) {
            int64_t mode = 0;
            // The old version does not support the new depthwise convolution template，so convert depthwise to conv
            if ((ge::AttrUtils::GetInt(desc, "mode", mode)) && (mode == DEPTHWISE_CONV_MODE)) {
                GE_CHK_BOOL_RET_STATUS(desc.DelAttr("mode") == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
                    "Delete ir attr mode to opdesc failed!");
                (void)ge::AttrUtils::SetInt(desc, "mode", 1);
            }
        }
        if (!desc.HasAttr("mode")) {
            (void)ge::AttrUtils::SetInt(desc, "mode", 1);
        }
        if (!desc.HasAttr("algo")) {
            (void)ge::AttrUtils::SetInt(desc, "algo", 0);
        }
    }
    GE_CHK_BOOL_RET_STATUS(ConstantOMConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "constant type convert failed!");
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FullyConnectionConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // type mapping
    GE_CHK_BOOL_RET_STATUS(TransformTypeConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "transfer type failed!");
    GE_CHK_BOOL_RET_STATUS(
        UpdateAttrConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "attr converter failed!");
    // add attr
    if (isOldToNew) {
        (void)ge::AttrUtils::SetBool(desc, hiai::op::FullyConnection::transpose, false);
        (void)ge::AttrUtils::SetInt(desc, hiai::op::FullyConnection::axis, 1);
    } else {
        // deleteAttr
        bool transpose = false;
        if (ge::AttrUtils::GetBool(desc, hiai::op::FullyConnection::transpose, transpose)) {
            GE_CHK_BOOL_RET_STATUS(desc.DelAttr(hiai::op::FullyConnection::transpose) == ge::GRAPH_SUCCESS,
                ge::GRAPH_FAILED, "Delete ir attr transpose to opdesc failed!");
        }
        int axis = 0;
        if (ge::AttrUtils::GetInt(desc, hiai::op::FullyConnection::axis, axis)) {
            GE_CHK_BOOL_RET_STATUS(desc.DelAttr(hiai::op::FullyConnection::axis) == ge::GRAPH_SUCCESS,
                ge::GRAPH_FAILED, "Delete ir attr axis to opdesc failed!");
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ScaleConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    bool scaleBiasTerm = false;
    (void)ge::AttrUtils::GetBool(desc, ge::op::Scale::bias_term, scaleBiasTerm);
    if (!isOldToNew && !desc.HasAttr(ge::op::Scale::has_bias_value)) {
        if (scaleBiasTerm) {
            (void)ge::AttrUtils::SetBool(desc, ge::op::Scale::has_bias_value, true);
        } else {
            (void)ge::AttrUtils::SetBool(desc, ge::op::Scale::has_bias_value, false);
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus InstanceNormConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // max_output_size
    if (desc.HasAttr("reduction_indices")) {
        return ge::GRAPH_SUCCESS;
    }
    (void)ge::AttrUtils::SetStr(desc, "norm_type", "InstanceNorm");
    vector<int32_t> reductionIndicese;
    (void)ge::AttrUtils::SetListInt(desc, "reduction_indices", reductionIndicese);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LayerNormConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // max_output_size
    if (desc.HasAttr("reduction_indices")) {
        return ge::GRAPH_SUCCESS;
    }
    (void)ge::AttrUtils::SetStr(desc, "norm_type", "LayerNorm");
    vector<int32_t> reductionIndicese;
    (void)ge::AttrUtils::SetListInt(desc, "reduction_indices", reductionIndicese);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ProposalConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    GE_CHK_STATUS_RET_NOLOG(UpdateAttrConverter(node, config, isOldToNew));
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // output_actual_rois_num
    bool outputActualRoisNnum = false;
    if (ge::AttrUtils::GetBool(desc, hiai::op::Proposal::output_actual_rois_num, outputActualRoisNnum)) {
        GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(outputActualRoisNnum, return ge::GRAPH_FAILED,
            "The current platform not support Multiple outputs of Proposal, node: %s, type: %s.",
            desc.GetName().c_str(), desc.GetType().c_str());
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LSTMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        // expose_hidden
        (void)ge::AttrUtils::SetBool(desc, hiai::op::LSTM::expose_hidden, false);
    } else {
        // optional input
        HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveIdleEndpoint());

        // output size
        const int newVersionLstmOutputSize = 3;
        if (desc.GetOutputsSize() == newVersionLstmOutputSize) {
            ge::TensorDesc descTemp = desc.GetOutputDesc(0);
            desc.ClearAllOutputsDesc();
            if (desc.AddOutputDesc(descTemp) != ge::GRAPH_SUCCESS) {
                FMK_LOGE("lstm add outputdesc fail.");
                return ge::GRAPH_FAILED;
            }
        }

        // num_output
        if (!desc.HasAttr("num_output")) {
            (void)ge::AttrUtils::SetInt(desc, "num_output", 1);
        }
        // num_layers
        if (!desc.HasAttr("num_layers")) {
            (void)ge::AttrUtils::SetInt(desc, "num_layers", 1);
        }
        // support_cont
        if (!desc.HasAttr("support_cont")) {
            (void)ge::AttrUtils::SetBool(desc, "support_cont", true);
        }
        // hidden_size
        vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
        if (!desc.HasAttr("hidden_size") && !weightsVec.empty() && weightsVec[0] != nullptr) {
            // w_x : Weight tensor, must be const op. Used between x and hidden units. Shape is [4 * hidden_size, X]
            vector<int64_t> wXDims = weightsVec[0]->GetTensorDesc().GetShape().GetDims();
            if (wXDims.size() >= 1) {
                int64_t hiddenSize = wXDims[0] / 4;
                (void)ge::AttrUtils::SetInt(desc, "hidden_size", hiddenSize);
            }
        }
    }
    return ge::GRAPH_SUCCESS;
}

static void BidirectionLSTMConverterOldToNew(ge::Node& node)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // cell_type
    (void)ge::AttrUtils::SetStr(desc, hiai::op::BidirectionLSTM::cell_type, hiai::op::LSTM::TYPE);
    // state_is_tuple
    (void)ge::AttrUtils::SetBool(desc, hiai::op::BidirectionLSTM::state_is_tuple, true);
    // activation
    if (!desc.HasAttr(hiai::op::BidirectionLSTM::activation)) {
        (void)ge::AttrUtils::SetInt(desc, hiai::op::BidirectionLSTM::activation, 0);
        if (desc.HasAttr("lstm_activate")) {
            int64_t activation = 0;
            (void)ge::AttrUtils::GetInt(desc, "lstm_activate", activation);
            (void)ge::AttrUtils::SetInt(desc, hiai::op::BidirectionLSTM::activation, activation);
        }
    }
}

ge::GraphErrCodeStatus BidirectionLSTMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        BidirectionLSTMConverterOldToNew(node);
        return ge::GRAPH_SUCCESS;
    }
    HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveIdleEndpoint());

    // lstm_time_major
    if (!desc.HasAttr("lstm_time_major")) {
        (void)ge::AttrUtils::SetBool(desc, "lstm_time_major", false);
    }
    // new:activation -> old:lstm_activate
    bool hasLstmActivate = desc.HasAttr("lstm_activate");
    bool hasActivate = desc.HasAttr(hiai::op::BidirectionLSTM::activation);
    if (!hasLstmActivate && hasActivate) {
        (void)ge::AttrUtils::SetInt(desc, "lstm_activate", 0);
        // convert activation data type from string to int.
        const std::map<string, int64_t> activationMap = {
            {"Sigmoid", 0}, {"Tanh", 1}, {"ReLU", 2}, {"ReLU1", 3}, {"ReLU6", 4}};
        string activation = "";
        int64_t activationInt = 0;
        if (ge::AttrUtils::GetStr(desc, hiai::op::BidirectionLSTM::activation, activation)) {
            auto it = activationMap.find(activation);
            if (it != activationMap.end()) {
                (void)desc.DelAttr(hiai::op::BidirectionLSTM::activation);
                (void)ge::AttrUtils::SetInt(desc, "lstm_activate", it->second);
            }
        } else if (ge::AttrUtils::GetInt(desc, hiai::op::BidirectionLSTM::activation, activationInt)) {
            (void)ge::AttrUtils::SetInt(desc, "lstm_activate", activationInt);
        }
    } else if (!hasLstmActivate) {
        (void)ge::AttrUtils::SetInt(desc, "lstm_activate", 0);
    }

    // hidden_size
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    if (!desc.HasAttr("hidden_size") && !weightsVec.empty() && weightsVec[0] != nullptr) {
        // w_fw : Forward weight tensor, must be const op. Two dimensions [x(t -1) + h(t -1), 4 * hidden-size].
        vector<int64_t> wFwDims = weightsVec[0]->GetTensorDesc().GetShape().GetDims();
        if (wFwDims.size() >= 2) {
            int64_t ht = wFwDims[1] / 4;
            (void)ge::AttrUtils::SetInt(desc, "ht", ht);
            (void)ge::AttrUtils::SetInt(desc, "hidden_size", ht);
        }
    }

    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus ConvOMConverterOldToNew(ge::Node& node)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();

    HIAI_EXPECT_EXEC(DataFormatToString(desc));
    HIAI_EXPECT_EXEC(PadModeToString(desc));
    HIAI_EXPECT_EXEC(SetConvPadmode(desc));

    if (desc.GetType() != hiai::op::Correlation::TYPE || !desc.HasAttr("original_op_names")) {
        return ge::GRAPH_SUCCESS;
    }

    if (node.ROLE(NodeSpec).InDataEdgeSize() != 2) {
        return ge::GRAPH_SUCCESS;
    }

    Node* secondInputNode = node.ROLE(NodeWalker).InDataNode(1);
    HIAI_EXPECT_NOT_NULL(secondInputNode);

    if (secondInputNode->ROLE(NodeSpec).Type() == hiai::op::Reshape::TYPE) {
        string name = "transpose_before_correlation_";
        auto permuteDesc = CreateTansposeOp(name);
        HIAI_EXPECT_TRUE(
            ge::AttrUtils::SetListInt(permuteDesc, hiai::op::Permute::order, vector<int64_t> {3, 2, 0, 1}));

        auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
        Node* addNode = graph.ROLE(GraphModifier).AddNode(permuteDesc);
        HIAI_EXPECT_NOT_NULL(addNode);
        HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).InsertBefore(node, 1, *addNode));
    }

    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus ConrreationOMConverterNewToOld(ge::Node& node)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();

    if (desc.GetType() == hiai::op::Correlation::TYPE && !desc.HasAttr("num_output")) {
        int64_t input2Num = desc.GetInputDesc(1).GetShape().GetDim(0);
        (void)ge::AttrUtils::SetInt(desc, "num_output", input2Num);
    }

    if (desc.GetType() != hiai::op::Correlation::TYPE || !desc.HasAttr("original_op_names")) {
        return ge::GRAPH_SUCCESS;
    }

    if (node.ROLE(NodeSpec).InDataEdgeSize() != 2) {
        return ge::GRAPH_SUCCESS;
    }

    Node* secondInputNode = node.ROLE(NodeWalker).InDataNode(1);
    HIAI_EXPECT_NOT_NULL(secondInputNode);

    if (secondInputNode->ROLE(NodeSpec).Type() == hiai::op::Permute::TYPE) {
        string name = "transpose_after_permute_";
        auto permuteDesc = CreateTansposeOp(name);
        HIAI_EXPECT_TRUE(
            ge::AttrUtils::SetListInt(permuteDesc, hiai::op::Permute::order, vector<int64_t> {0, 2, 3, 1}));

        auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
        Node* insertNode = graph.ROLE(GraphModifier).AddNode(permuteDesc);
        HIAI_EXPECT_NOT_NULL(insertNode);
        HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).InsertBefore(node, 1, *insertNode));

        // correlation新ir模型已经特殊处理，这里为ux10插入permute
        auto permuteDesc0 = CreateTansposeOp(name);
        HIAI_EXPECT_TRUE(
            ge::AttrUtils::SetListInt(permuteDesc0, hiai::op::Permute::order, vector<int64_t> {0, 1, 2, 3}));

        Node* insertNode0 = graph.ROLE(GraphModifier).AddNode(permuteDesc0);
        HIAI_EXPECT_NOT_NULL(insertNode0);
        HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).InsertBefore(node, 0, *insertNode0));
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ConvOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        if (ConvOMConverterOldToNew(node) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("convert conv om from old to new failed");
            return ge::GRAPH_FAILED;
        }
    } else {
        if (desc.GetType() == hiai::op::ConvolutionDepthwise::TYPE) {
            if (!desc.HasAttr("groups")) {
                (void)ge::AttrUtils::SetInt(desc, "groups", 1);
            }
        }
        GE_CHK_BOOL_RET_STATUS(
            DataFormatToEnum(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "data format to enum failed!");
        GE_CHK_BOOL_RET_STATUS(PadModeToEnum(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "pad mode to enum failed!");
        if (!desc.HasAttr(ge::op::ConvolutionDepthwise::mode)) {
            (void)ge::AttrUtils::SetInt(desc, ge::op::ConvolutionDepthwise::mode, 1);
        }

        GE_CHK_BOOL_RET_STATUS(BiasComplete(node) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "bias complete failed!");

        if (!desc.HasAttr(ge::op::ConvolutionDepthwise::algo)) {
            (void)ge::AttrUtils::SetInt(desc, ge::op::ConvolutionDepthwise::algo, 0);
        }

        if (ConrreationOMConverterNewToOld(node) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("convert conrreation om from new to old failed");
            return ge::GRAPH_FAILED;
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus BiasAddOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        GE_CHK_BOOL_RET_STATUS(
            DataFormatToString(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "data format to string failed!");
    } else {
        GE_CHK_BOOL_RET_STATUS(
            DataFormatToEnum(desc) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "data format to enum failed!");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ReLU6OMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    int32_t activationMode = 0;
    // DOMI_ACTIVATION_RELU6 --> DOMI_ACTIVATION_CLIPPED_RELU
    if (ge::AttrUtils::GetInt(opdesc, hiai::op::Activation::mode, activationMode) && activationMode == 14) {
        (void)ge::AttrUtils::SetInt(opdesc, hiai::op::Activation::mode, 3);
        (void)ge::AttrUtils::SetFloat(opdesc, hiai::op::Activation::coef, 6.0);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ScaleOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    size_t inputNum = opdesc.GetInputsSize();
    if (!opdesc.HasAttr(ge::op::Scale::has_bias_value)) {
        if (inputNum == 3) {
            (void)ge::AttrUtils::SetBool(opdesc, ge::op::Scale::has_bias_value, true);
        } else {
            (void)ge::AttrUtils::SetBool(opdesc, ge::op::Scale::has_bias_value, false);
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus PriorBoxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    GE_CHK_BOOL_RET_STATUS(TransformTypeConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "convert priorBox transfer type failed!");
    auto& desc = node.ROLE(NodeSpec).OpDesc();
    // min_size
    vector<float> minSize;
    vector<float> maxSize;
    vector<float> variance;
    vector<float> aspectRatio;
    (void)ge::AttrUtils::GetListFloat(desc, hiai::op::PriorBox::min_size, minSize);
    (void)ge::AttrUtils::GetListFloat(desc, hiai::op::PriorBox::max_size, maxSize);
    (void)ge::AttrUtils::GetListFloat(desc, hiai::op::PriorBox::variance, variance);
    (void)ge::AttrUtils::GetListFloat(desc, hiai::op::PriorBox::aspect_ratio, aspectRatio);
    (void)ge::AttrUtils::SetInt(desc, SSD_PRIOR_BOX_ATTR_MIN_SIZE_NUM, minSize.size());
    (void)ge::AttrUtils::SetInt(desc, SSD_PRIOR_BOX_ATTR_MAX_SIZE_NUM, maxSize.size());
    (void)ge::AttrUtils::SetInt(desc, SSD_PRIOR_BOX_ATTR_VARIANCE_NUM, variance.size());
    (void)ge::AttrUtils::SetInt(desc, SSD_PRIOR_BOX_ATTR_ASPECT_RATIO_NUM, aspectRatio.size());
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus PReluConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    GE_CHK_STATUS_RET_NOLOG(TransformTypeConverter(node, config, isOldToNew));

    auto& desc = node.ROLE(NodeSpec).OpDesc();

    // channel_shared
    if (!desc.HasAttr(PRELU_ATTR_CHANNEL_SHARED)) {
        (void)ge::AttrUtils::SetBool(desc, PRELU_ATTR_CHANNEL_SHARED, false);
    }
    // slope
    vector<ge::TensorPtr> slope = ge::OpDescUtils::MutableWeights(node);
    GE_CHK_BOOL_RET_STATUS(slope.size() == 1, ge::GRAPH_FAILED, "get slope failed");
    const ge::Shape& slopeShape = slope[0]->GetTensorDesc().GetShape();
    if (slopeShape.GetDimNum() == NORMAL_TENSOR_SIZE) {
        GE_CHK_BOOL_RET_STATUS(slopeShape.GetDim(0) == DIM_DEFAULT_VALUE && slopeShape.GetDim(2) == DIM_DEFAULT_VALUE &&
                slopeShape.GetDim(3) == DIM_DEFAULT_VALUE,
            ge::GRAPH_FAILED, "not supported slope shape (%jd, %jd, %jd, %jd), must be 1C11 or C11",
            slopeShape.GetDim(0), slopeShape.GetDim(1), slopeShape.GetDim(2), slopeShape.GetDim(3));
    } else if (slopeShape.GetDimNum() == DIMNUM_THREE_SIZE) {
        GE_CHK_BOOL_RET_STATUS(slopeShape.GetDim(1) == DIM_DEFAULT_VALUE && slopeShape.GetDim(2) == DIM_DEFAULT_VALUE,
            ge::GRAPH_FAILED, "not supported slope shape (%jd, %jd, %jd), must be 1C11 or C11", slopeShape.GetDim(0),
            slopeShape.GetDim(1), slopeShape.GetDim(2));
        // reshape slope
        vector<ge::TensorPtr> preluWeights;
        ge::Shape slopeReshape({1, slopeShape.GetDim(0), slopeShape.GetDim(1), slopeShape.GetDim(2)});
        ge::TensorDesc slopeTensorDesc(slopeReshape, ge::DT_FLOAT);
        ge::TensorPtr slopeTensor = std::make_shared<ge::Tensor>(slopeTensorDesc,
            reinterpret_cast<const uint8_t*>(slope[0]->GetData().GetData()), slopeShape.GetDim(0) * sizeof(float));
        preluWeights.push_back(slopeTensor);
        ge::OpDescUtils::SetWeights(node, preluWeights);
    } else {
        FMK_LOGE("slope shape is invalid.");
        return ge::GRAPH_FAILED;
    }

    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus ChangeConstShapeForLayerNorm(ge::Node& node)
{
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    if (weightsVec.size() < 1) {
        return ge::GRAPH_SUCCESS;
    }
    for (size_t i = 0; i < weightsVec.size(); ++i) {
        if (weightsVec[i] == nullptr) {
            continue;
        }
        ge::TensorDesc& weightDesc = weightsVec[i]->MutableTensorDesc();
        ge::Shape weightShape = weightDesc.GetShape();
        bool startFlag = false;
        vector<int64_t> shapeValue;
        for (int64_t dim : weightShape.GetDims()) {
            if (dim != 1) {
                startFlag = true;
            }
            if (!startFlag) {
                continue;
            }
            shapeValue.push_back(dim);
        }
        weightDesc.SetShape(ge::Shape(shapeValue));
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus LayerNormOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (node.ROLE(NodeSpec).IdleInputEndpoints().size() == 0) {
        return ge::GRAPH_SUCCESS;
    }

    HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveIdleEndpoint());

    // change const node shape, remove dim == 1
    if (ChangeConstShapeForLayerNorm(node) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("change const node shape Failed");
        return ge::GRAPH_FAILED;
    }

    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (opDesc.HasAttr(hiai::op::LayerNorm::begin_norm_axis) &&
        opDesc.HasAttr(hiai::op::LayerNorm::begin_params_axis)) {
        return ge::GRAPH_SUCCESS;
    }
    vector<int64_t> reductionIndices;
    (void)ge::AttrUtils::GetListInt(opDesc, "reduction_indices", reductionIndices);
    if (reductionIndices.size() < 1) {
        return ge::GRAPH_SUCCESS;
    }

    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    if (weightsVec.size() < 1) {
        FMK_LOGE("weightsVec is empty.");
        return ge::GRAPH_FAILED;
    }
    (void)ge::AttrUtils::SetInt(opDesc, hiai::op::LayerNorm::begin_norm_axis, reductionIndices[0]);
    vector<int64_t> inputDims = opDesc.GetInputDesc(0).GetShape().GetDims();
    vector<int64_t> scaleDims = weightsVec[0]->MutableTensorDesc().GetShape().GetDims();
    if (scaleDims.size() < 1) {
        return ge::GRAPH_SUCCESS;
    }
    for (size_t i = 0; i < inputDims.size(); i++) {
        if (inputDims[i] == scaleDims[0]) {
            (void)ge::AttrUtils::SetInt(opDesc, hiai::op::LayerNorm::begin_params_axis, static_cast<int64_t>(i));
            break;
        }
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus AlgoOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (opdesc.HasAttr(ATTR_ALGO) || isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (opdesc.GetType() == hiai::op::Softmax::TYPE) {
        (void)ge::AttrUtils::SetInt(opdesc, ge::op::Softmax::algo, 1);
    } else if (opdesc.GetType() == hiai::op::LogSoftmax::TYPE) {
        (void)ge::AttrUtils::SetInt(opdesc, ge::op::LogSoftmax::algo, 2);
    } else {
        (void)ge::AttrUtils::SetInt(opdesc, ATTR_ALGO, 0);
    }

    return ge::GRAPH_SUCCESS;
}

static void SetAttrForLSTM(ge::OpDesc& opdesc)
{
    const std::map<std::string, bool> attrToBoolMap = {
        {"fw_state_concat", false},
        {"bw_state_concat", false},
        {"fwbw_state_concat", false},
    };
    for (auto& pair : attrToBoolMap) {
        if (!opdesc.HasAttr(pair.first)) {
            (void)ge::AttrUtils::SetBool(opdesc, pair.first, pair.second);
        }
    }

    const std::map<std::string, int64_t> attrToValueMap = {
        {"seqlen_index", -1},
        {"x_index", 0},
        {"cont_index", 1},
        {"xstatic_index", 2},
        {"num_layers", 1},
        {"mode", 2},
        {"input_mode", 0},
        {"direction_mode", 0},
        {"input_data_layout", 4},
        {"output_data_layout", 4},
    };
    for (auto& pair : attrToValueMap) {
        if (!opdesc.HasAttr(pair.first)) {
            (void)ge::AttrUtils::SetInt(opdesc, pair.first, pair.second);
        }
    }
}

ge::GraphErrCodeStatus LSTMOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    bool supportCont = false;
    if (ge::AttrUtils::GetBool(opdesc, "support_cont", supportCont) && supportCont) {
        GE_CHK_STATUS_RET_NOLOG(TransformTypeConverter(node, config, isOldToNew));
    }
    if (!isOldToNew) {
        // hidden_size
        vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
        if (!opdesc.HasAttr("hidden_size") && !weightsVec.empty() && weightsVec[0] != nullptr) {
            // w_x : Weight tensor, must be const op. Used between x and hidden units. Shape is [4 * hidden_size, X]
            vector<int64_t> wFwDims = weightsVec[0]->GetTensorDesc().GetShape().GetDims();
            if (wFwDims.size() >= 2) {
                int64_t ht = wFwDims[0] / 4;
                (void)ge::AttrUtils::SetInt(opdesc, "hidden_size", ht);
            }
        }

        SetAttrForLSTM(opdesc);
        return ge::GRAPH_SUCCESS;
    }

    return ReConnetLSTMOutput(node);
}

ge::GraphErrCodeStatus ProposalOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    if (opdesc.HasAttr("top_size")) {
        return ge::GRAPH_SUCCESS;
    }
    (void)ge::AttrUtils::SetInt(opdesc, "top_size", opdesc.GetOutputsSize());
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FullyConnectionOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (opdesc.GetOutputDesc(0).GetShape().GetDimNum() != 4) {
        return ge::GRAPH_SUCCESS;
    }
    ge::TensorDesc yDesc = opdesc.GetOutputDesc(0);
    vector<int64_t> shapeValue = yDesc.GetShape().GetDims();
    if (shapeValue.empty()) {
        FMK_LOGE("dim is empty.");
        return ge::GRAPH_FAILED;
    }
    Node* outNode = node.ROLE(NodeWalker).OutDataNode(0, 0);
    HIAI_EXPECT_NOT_NULL(outNode);

    const string& opType = outNode->ROLE(NodeSpec).Type();
    if (nextOpSet.find(opType) != nextOpSet.end()) {
        shapeValue.pop_back();
        shapeValue.pop_back();

        yDesc.SetShape(ge::Shape(shapeValue));
        opdesc.UpdateOutputDesc(0, yDesc);
    }

    return ge::GRAPH_SUCCESS;
}
} // namespace hiai