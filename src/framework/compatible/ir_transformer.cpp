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

#include "framework/compatible/ir_transformer.h"
#include <algorithm>
#include "array_op_transformer.h"
#include "math_op_transformer.h"
#include "nn_op_transformer.h"
#include "detection_op_transformer.h"
#include "internal_op_transformer.h"
#include "image_op_transformer.h"
#include "control_flow_op_transformer.h"
#include "ir_verify.h"

#include "infra/base/assertion.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/util/rom_version_util.h"
#include "graph/op/const_defs.h"

namespace hiai {
// 版本号
const static std::string BASE_ROM_VERSION = "100.320";
const static std::string BASE_990C20_VERSION = "100.320.010";
const static std::string BASE_VERSION3_BASE = "100.330";
const static std::string BASE_UX11_VERSION = "100.333";
const static std::string BASE_BALC10_VERSION = "100.500";

using OP_CONVERT_FUNC = std::function<ge::GraphErrCodeStatus(ge::Node&, const ConvertConfigInfo&, bool)>;
using OP_VERIFY_FUNC = std::function<ge::GraphErrCodeStatus(ge::Node&)>;

struct ConvertConfig {
    // 算子映射函数
    OP_CONVERT_FUNC func;
    // 算子信息
    ConvertConfigInfo cfg;
};

// 算子版本 0 <-> 3 映射表
const static std::map<std::string, ConvertConfig> IR_DEF_CONVERT_MAP = {
    {"HardSwish", {HardSwishConverter, {"Activation", false, {}}}},
    {"Select", {SelectConverter, {"", false, {}}}},
    {"SpaceToDepth", {SpaceToDepthConverter, {"", true, {}}}},
    {"Eltwise", {ToDynamicInputConverter, {"", false, {}}}},
    {"BatchNorm", {TransformTypeConverter, {"BNInference", true, {}}}},
    {"BatchNormExt2", {TransformTypeConverter, {"BNInference", true, {}}}},
    {"BNInference", {TransformTypeConverter, {"BatchNorm", false, {}}}},
    {"Gather", {TransformTypeConverter, {"GatherV2D", true, {}}}},
    {"GatherV2D", {GatherV2DConverter, {"Gather", false, {}}}},
    {"ReduceProd", {TransformTypeConverter, {"ReduceProdD", true, {}}}},
    {"ReduceProdD", {TransformTypeConverter, {"ReduceProd", false, {}}}},
    {"ReduceAll", {TransformTypeConverter, {"ReduceAllD", true, {}}}},
    {"ReduceAllD", {TransformTypeConverter, {"ReduceAll", false, {}}}},
    {"ArgMax", {ArgMaxConverter, {"ArgMaxExt2", true, {}}}},
    {"ArgMaxExt2", {TransformTypeConverter, {"ArgMax", false, {}}}},
    {"RandomUniform", {TransformTypeConverter, {"RandomUniformNoSeed", true, {}}}},
    {"RandomUniformNoSeed", {TransformTypeConverter, {"RandomUniform", false, {}}}},
    {"Concat", {TypeAndAttrConverter, {"ConcatD", true, {{"axis", "concat_dim", "", ""}}}}},
    {"ConcatD", {ConcatDConverter, {"Concat", false, {{"axis", "concat_dim", "", ""}}}}},
    {"Softmax", {SetIntAttrDefValueConverter, {"", false, {{"algo", "", "1", ""}}}}},
    {"LogSoftmax", {SetIntAttrDefValueConverter, {"", false, {{"algo", "", "2", ""}}}}},
    {"Slice", {SetIntAttrDefValueConverter, {"", false, {{"axis", "", "0", ""}}}}},
    {"CropAndResize", {CropAndResizeConverter, {"", false, {{"method", "method", "0", "bilinear"}}}}},
    {"Deconvolution",
        {TypeAndAttrConverter,
            {"ConvTranspose", true,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"ConvTranspose",
        {Conv2DTransposeConverter,
            {"Deconvolution", false,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"Convolution",
        {Conv2DTransposeConverter,
            {"", false,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"ConvolutionDepthwise",
        {Conv2DTransposeConverter,
            {"", false,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"Correlation",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"Correlation_V2",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"pad", "pads", "", ""},
                    {"stride", "strides", "", ""},
                    {"dilation", "dilations", "", ""},
                    {"group", "groups", "", ""},
                }}}},
    {"SplitD",
        {TypeAndAttrConverter, {"Split", false, {{"axis", "split_dim", "", ""}, {"output_num", "num_split", "", ""}}}}},
    {"Split",
        {TypeAndAttrConverter, {"SplitD", true, {{"axis", "split_dim", "", ""}, {"output_num", "num_split", "", ""}}}}},
    {"LRN",
        {UpdateAttrConverter,
            {"", false,
                {{"lrn_localsize", "depth_radius", "5", "5"}, {"lrn_alpha", "alpha", "1.0", "1.0"},
                    {"lrn_beta", "beta", "0.5", "0.5"}, {"lrn_k", "bias", "1.0", "1.0"}}}}},
    {"Cast", {TypeAndAttrConverter, {"CastT", true, {{"SrcT", "src_dtype", "", ""}, {"DstT", "dst_dtype", "", ""}}}}},
    {"CastT", {TypeAndAttrConverter, {"Cast", false, {{"SrcT", "src_dtype", "", ""}, {"DstT", "dst_dtype", "", ""}}}}},
    {"Pooling", {TransformTypeConverter, {"PoolingD", true, {}}}},
    {"PoolingD", {TransformTypeConverter, {"Pooling", false, {}}}},
    {"FullConnection",
        {FullyConnectionConverter,
            {"FullyConnection", true,
                {
                    {"filter_quant_type", "w_quant_type", "", ""},
                    {"filter_quant_scales", "w_quant_scales", "", ""},
                }}}},
    {"FullyConnection",
        {FullyConnectionConverter,
            {"FullConnection", false,
                {
                    {"filter_quant_type", "w_quant_type", "", ""},
                    {"filter_quant_scales", "w_quant_scales", "", ""},
                }}}},
    {"Scale", {ScaleConverter, {"", false, {}}}},
    {"TopKV2", {TransformTypeConverter, {"TopK", true, {}}}},
    {"TopK", {TransformTypeConverter, {"TopKV2", false, {}}}},
    {"BatchMatMul",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"adj_x", "adj_x1", "", ""},
                    {"adj_y", "adj_x2", "", ""},
                }}}},
    {"MatMul",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"transposeX", "transpose_x1", "", ""},
                    {"transposeW", "transpose_x2", "", ""},
                }}}},
    {"Clip", {TransformTypeConverter, {"ClipByValue", true, {}}}},
    {"ClipByValue", {TransformTypeConverter, {"Clip", false, {}}}},
    {"OneHot", {OneHotConverter, {"", false, {}}}},
    {"PriorBox", {PriorBoxConverter, {"SSDPriorBox", false, {}}}},
    {"SSDPriorBox", {PriorBoxConverter, {"", false, {}}}},
    {"Normalize", {TransformTypeConverter, {"SSDNormalize", false, {}}}},
    {"L2Normalize", {L2NormalizeConverter, {"", false, {}}}},
    {"ReduceMax", {ReduceMaxConverter, {"", false, {}}}},
    {"ReduceMin", {ReduceMinConverter, {"", false, {}}}},
    {"NonMaxSuppression", {TransformTypeConverter, {"NonMaxSuppressionV3D", true, {}}}},
    {"NonMaxSuppressionV3D", {TransformTypeConverter, {"NonMaxSuppression", false, {}}}},
    {"InstanceNorm", {InstanceNormConverter, {"", false, {}}}},
    {"LayerNorm", {LayerNormConverter, {"", false, {}}}},
    {"Proposal",
        {ProposalConverter,
            {"", false,
                {
                    {"nms_thresh", "iou_thresh", "", ""},
                }}}},
    {"Pad", {PadConverter, {"", false, {}}}},
    {"PadV3", {PadV3Converter, {"Pad", false, {}}}},
    {"LSTM", {LSTMConverter, {"", false, {}}}},
    {"BidirectionLSTM", {BidirectionLSTMConverter, {"", false, {}}}},
    {"RNN", {BidirectionLSTMConverter, {"", false, {}}}},
    {"ChannelShuffle",
        {TypeAndAttrConverter,
            {"ShuffleChannelV2", true,
                {
                    {"num_group", "group", "", ""},
                }}}},
    {"ShuffleChannelV2",
        {TypeAndAttrConverter,
            {"ChannelShuffle", false,
                {
                    {"num_group", "group", "", ""},
                }}}},
    {"RandomNormal", {TransformTypeConverter, {"RandomNormalNoSeed", true, {}}}},
    {"RandomNormalNoSeed", {TransformTypeConverter, {"RandomNormal", false, {}}}},
    {"RandomShuffle", {TransformTypeConverter, {"RandomShuffleNoSeed", true, {}}}},
    {"RandomShuffleNoSeed", {TransformTypeConverter, {"RandomShuffle", false, {}}}},
    {"Upsample",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"scale_h", "stride_h", "", ""},
                    {"scale_w", "stride_w", "", ""},
                }}}},
    {"FSRDetectionOutput",
        {FSRDetectionOutputConverter,
            {"", false,
                {
                    {"nms_threshold", "iou_threshold", "", ""},
                    {"confidence_threshold", "score_threshold", "", ""},
                    {"batch_size", "batch_rois", "", ""},
                }}}},
    {"PRelu", {PReluConverter, {"PReLU", false, {}}}},
    {"Interp", {InterpConverter, {"", false, {}}}},
    {"SplitV", {SplitVConverter, {"", false, {}}}},
    {"FlattenV2", {TransformTypeConverter, {"Flatten", false, {}}}},
    {"ROIAlignV2", {ROIAlignConverter, {"ROIAlign", false, {}}}},
};

/* Reserved attr verify map */
const static std::map<std::string, OP_VERIFY_FUNC> IR_VERIFY_MAP = {
    {"Data", DataVerify},
    {"FullyConnection", FullyConnectionVerify},
    {"Scale", ScaleVerify},
    {"Size", SizeVerify},
    {"LRN", LRNVerify},
    {"BNInference", BNInferenceVerify},
    {"ConvolutionDepthwise", DepthwiseConv2DVerify},
    {"Eltwise", EltwiseVerify},
    {"Shape", ShapeVerify},
    {"Dequantize", DequantizeVerify},
    {"Quantize", QuantizeVerify},
    {"LayerNorm", LayerNormVerify},
    {"DepthToSpace", DepthToSpaceVerify},
    {"Proposal", ProposalVerify},
    {"LSTM", LSTMVerify},
    {"BidirectionLSTM", BidirectionLSTMVerify},
    {"Activation", ActivationVerify},
    {"NonMaxSuppressionV3D", NonMaxSuppressionV3DVerify},
    {"Interp", InterpVerify},
    {"L2Normalize", L2NormalizeVerify},
    {"ConvTranspose", ConvTransposeVerify},
    {"MaxUnpool2D", MaxUnpool2DVerify},
    {"MaxPoolWithArgmaxV2", MaxPoolWithArgmaxV2Verify},
};

const static std::vector<std::string> IR_NEED_CONVERT_VEC = {"HardSwish"};

// 算子版本 3 <-> 5 映射表
const static std::map<std::string, ConvertConfig> OM_DEF_CONVERT_MAP = {
    {"MirrorPad", {MirrorPadOMConverter, {"", false, {}}}},
    {"Convolution", {ConvOMConverter, {"", false, {}}}},
    {"Correlation", {ConvOMConverter, {"", false, {}}}},
    {"ConvolutionDepthwise", {ConvOMConverter, {"", false, {}}}},
    {"ConvTranspose", {ConvOMConverter, {"", false, {}}}},
    {"BiasAdd", {BiasAddOMConverter, {"", false, {}}}},
    {"Interp", {InterpOMConverter, {"", false, {}}}},
    {"ArgMaxExt2", {ArgMaxOMConverter, {"", false, {}}}},
    {"ConcatD", {ConcatDOMConverter, {"", false, {}}}},
    {"SSDDetectionOutput",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"shared_location", "share_location", "", ""},
                    {"nms_threshold", "iou_threshold", "", ""},
                }}}},
    {"Activation", {ReLU6OMConverter, {"", false, {}}}},
    {"Scale", {ScaleOMConverter, {"", false, {}}}},
    {"ResizeNearestNeighborV2", {TransformTypeConverter, {"ResizeNearestNeighbor", false, {}}}},
    {"ResizeNearestNeighbor", {ResizeNearestNeighborOMConverter, {"", false, {}}}},
    {"PriorBox", {PriorBoxConverter, {"SSDPriorBox", false, {}}}},
    {"Normalize", {TransformTypeConverter, {"SSDNormalize", false, {}}}},
    {"PRelu", {PReluConverter, {"PReLU", false, {}}}},
    {"LayerNorm", {LayerNormOMConverter, {"", false, {}}}},
    {"InstanceNorm", {EmptyInputNodeRemoveForOMConverter, {"", false, {}}}},
    {"RandomUniformNoSeed", {EmptyInputNodeRemoveForOMConverter, {"", false, {}}}},
    {"MatMul", {ConstantOMConverter, {"", false, {}}}},
    {"GemmD", {ConstantOMConverter, {"", false, {}}}},
    {"Softmax", {AlgoOMConverter, {"", false, {}}}},
    {"LogSoftmax", {AlgoOMConverter, {"", false, {}}}},
    {"Sum", {SumOMConverter, {"", false, {}}}},
    {"ReduceSum", {SumOMConverter, {"", false, {}}}},
    {"If", {ControlDataFlowOMConverter, {"", false, {}}}},
    {"While", {ControlDataFlowOMConverter, {"", false, {}}}},
    {"ROIAlign", {ROIAlignOMConverter, {"ROIAlignV2", true, {}}}},
    {"RNN", {LSTMOMConverter, {"LSTM", true, {}}}},
    {"LSTM", {LSTMOMConverter, {"RNN", false, {}}}},
    {"L2Normalize", {L2NormalizeOMConverter, {"", false, {}}}},
    {"NonMaxSuppressionV3D", {NonMaxSuppressionOMConverter, {"", false, {}}}},
    {"CropAndResize", {CropAndResizeOMConverter, {"", false, {}}}},
    {"FSRDetectionOutput", {FSRDetectionOutputOMConverter, {"", false, {}}}},
    {"GatherV2D", {GatherV2DOMConverter, {"", false, {}}}},
    {"ReduceProdD", {EmptyInputNodeRemoveForOMConverter, {"", false, {}}}},
    {"ReduceMax", {ReduceMaxOMConverter, {"", false, {}}}},
    {"SplitD", {SplitDOMConverter, {"", false, {}}}},
    {"DepthToSpace", {DepthToSpaceOMConverter, {"", false, {}}}},
    {"CastT", {CastOMConverter, {"", false, {}}}},
    {"DecodeBBox",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"decodeClip", "decode_clip", "", ""},
                }}}},
    {"Proposal", {ProposalOMConverter, {"", false, {}}}},
    {"FullyConnection", {FullyConnectionOMConverter, {"", false, {}}}},
    {"Reshape", {ReshapeOMConverter, {"", false, {}}}},
    {"Pad", {PadOMConverter, {"", false, {}}}},
    {"SplitV", {SplitVOMConverter, {"", false, {}}}},
    {"FlattenV2", {FlattenOMConverter, {"Flatten", false, {}}}},
    {"Permute", {ChangeDimOrderOMConverter, {"", false, {}}}},
    {"Multinomial",
        {UpdateAttrConverter,
            {"", false,
                {
                    {"output_dtype", "dtype", "", ""},
                }}}},
    {"StridedSliceV2", {StridedSliceV2OMConverter, {"", false, {}}}},
    {"Transpose", {ChangeDimOrderOMConverter, {"", false, {}}}},
    {"YoloDetectionOutput", {YoloDetectionOutputOMConverter, {"", false, {}}}},
};

namespace {
static void GetRomVersion(bool& isOldRom, bool& is990C20Rom, string& aiRomVersion)
{
    if (aiRomVersion.empty()) {
        return;
    }

    bool is3rdRomVersion = Is3rdRomVersion(aiRomVersion.c_str());

    if (aiRomVersion.length() > BASE_990C20_VERSION.length()) {
        aiRomVersion = aiRomVersion.substr(0, BASE_990C20_VERSION.length());
        is990C20Rom = aiRomVersion == BASE_990C20_VERSION;
        aiRomVersion = aiRomVersion.substr(0, BASE_ROM_VERSION.length());
    }

    if (aiRomVersion <= BASE_ROM_VERSION && !is3rdRomVersion) {
        isOldRom = true;
    }
}

// convert ir version between 0 <-> 3
bool IRConverter(ge::Node& node, string aiRomVersion, bool& isGraphChanged)
{
    bool isOldRom = false; // newRom
    bool is990C20Rom = false;

    GetRomVersion(isOldRom, is990C20Rom, aiRomVersion);

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    int64_t versionIR = 0;
    (void)ge::AttrUtils::GetInt(opDesc, OP_VERSION, versionIR);

    bool isOldToNew = false; // newIR-> oldIR
    // when RomVersion is empty, isOldRom is false, meaning must convert to new (version 3)
    if (isOldRom && versionIR == VESION_VALUE_DEFAULT) {
        // old rom old IR
        return true;
    } else if (!isOldRom && versionIR == VESION_VALUE_THREE) {
        // new rom new IR
        return true;
    } else if (!isOldRom && versionIR == VESION_VALUE_DEFAULT) {
        // new rom old IR
        isOldToNew = true;
        (void)ge::AttrUtils::SetInt(opDesc, OP_VERSION, VESION_VALUE_THREE);
    } else if (isOldRom && versionIR == VESION_VALUE_THREE) {
        // old rom new IR
        isOldToNew = false;
        (void)ge::AttrUtils::SetInt(opDesc, OP_VERSION, VESION_VALUE_DEFAULT);
    } else {
        // if ir version is 5, do nothing
        return true;
    }

    isGraphChanged = true;
    auto convertItem = IR_DEF_CONVERT_MAP.find(opDesc.GetType());
    if (convertItem != IR_DEF_CONVERT_MAP.end()) {
        // for hardswish
        if (std::find(IR_NEED_CONVERT_VEC.begin(), IR_NEED_CONVERT_VEC.end(), opDesc.GetType()) !=
            IR_NEED_CONVERT_VEC.end()) {
            isOldToNew = !is990C20Rom;
        }
        if (convertItem->second.func(node, convertItem->second.cfg, isOldToNew) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("IR mapping failed");
            return false;
        }
    }
    return true;
}

// convert ir version between 3 <-> 5
bool OMConverter(ge::Node& node, string aiRomVersion, bool& isGraphChanged)
{
    bool isOldRom = false; // true : old rom(< 100.333) ; false : new rom (>= 100.333)
    bool is3rdRomVersion = Is3rdRomVersion(aiRomVersion.c_str());
    if (!is3rdRomVersion && !aiRomVersion.empty()) {
        if (aiRomVersion.length() > BASE_UX11_VERSION.length()) {
            aiRomVersion = aiRomVersion.substr(0, BASE_UX11_VERSION.length());
        }
        if (aiRomVersion < BASE_UX11_VERSION || aiRomVersion == BASE_BALC10_VERSION) {
            isOldRom = true;
        }
    }

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    int versionIR = 0;
    (void)ge::AttrUtils::GetInt(opDesc, OP_VERSION, versionIR);

    bool isOldToNew = false; // newIR-> oldIR
    if (isOldRom && versionIR != VESION_VALUE_FIVE) {
        // old rom old om
        return true;
    } else if (!isOldRom && versionIR == VESION_VALUE_FIVE) {
        // new rom new IR
        return true;
    } else if (!isOldRom && versionIR != VESION_VALUE_FIVE) {
        // new rom old IR
        isOldToNew = true;
        (void)ge::AttrUtils::SetInt(opDesc, OP_VERSION, VESION_VALUE_FIVE);
    } else {
        // old rom new IR
        isOldToNew = false;
        (void)ge::AttrUtils::SetInt(opDesc, OP_VERSION, VESION_VALUE_THREE);
    }

    isGraphChanged = true;
    auto convertItem = OM_DEF_CONVERT_MAP.find(opDesc.GetType());
    if (convertItem != OM_DEF_CONVERT_MAP.end()) {
        ge::ComputeGraph& originGraph = node.ROLE(NodeSpec).OwnerComputeGraph();
        bool modelFlag = false;
        (void)ge::AttrUtils::GetBool(originGraph, "graph_from_liteV100_model", modelFlag);
        string nodeType = opDesc.GetType();
        if (modelFlag && (nodeType == "MatMul" || nodeType == "ConcatD")) {
            return true;
        }
        if (convertItem->second.func(node, convertItem->second.cfg, isOldToNew) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("IR mapping failed");
            return false;
        }
    }
    return true;
}
} // namespace

bool IRTransformer::TransferToStandard(ge::ComputeGraphPtr graph)
{
    bool isChanged = false;
    return TransferToTargetVersion(graph, "", isChanged);
}

bool IRTransformer::IsCompatible(ge::ComputeGraphPtr graph, string hiaiRomVersion)
{
    HIAI_EXPECT_NOT_NULL_R(graph, false);
    HIAI_EXPECT_TRUE_R(!hiaiRomVersion.empty(), false);

    if (hiaiRomVersion.length() > BASE_UX11_VERSION.length()) {
        hiaiRomVersion = hiaiRomVersion.substr(0, BASE_UX11_VERSION.length());
    }

    auto visitor = [&hiaiRomVersion](ge::Node& node) {
        ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
        int versionIR = 0;
        (void)ge::AttrUtils::GetInt(opDesc, OP_VERSION, versionIR);
        if (versionIR == VESION_VALUE_FIVE) {
            if (hiaiRomVersion < BASE_UX11_VERSION || hiaiRomVersion == BASE_BALC10_VERSION) {
                return hiai::COMM_EXCEPTION;
            }
        } else if (versionIR == VESION_VALUE_THREE) {
            if (hiaiRomVersion < BASE_VERSION3_BASE ||
                (hiaiRomVersion >= BASE_UX11_VERSION && hiaiRomVersion != BASE_BALC10_VERSION)) {
                return hiai::COMM_EXCEPTION;
            }
        } else if (versionIR == VESION_VALUE_DEFAULT) {
            if (hiaiRomVersion > BASE_ROM_VERSION) {
                return hiai::COMM_EXCEPTION;
            }
        }
        return hiai::SUCCESS;
    };

    return graph->ROLE(GraphListWalker).WalkAllNodes(visitor) == hiai::COMM_EXCEPTION;
}

namespace {
ge::GraphErrCodeStatus CacheSubgraphNodes(ge::NodeSubGraph& nodeSubGraph, std::vector<ge::Node*>& cacheNodes)
{
    auto visitor = [&cacheNodes](ge::ComputeGraphPtr& graph) {
        return graph->ROLE(GraphListWalker).WalkAllNodes([&cacheNodes](ge::Node& node) {
            if (node.ROLE(NodeSpec).Type() != hiai::op::Const::TYPE) {
                cacheNodes.push_back(&node);
            }
            return hiai::SUCCESS;
        });
    };
    return nodeSubGraph.WalkSubGraphs(visitor);
}
} // namespace

bool IRTransformer::TransferToTargetVersion(ge::ComputeGraphPtr graph, string aiRomVersion, bool& isGraphChanged)
{
    HIAI_EXPECT_NOT_NULL_R(graph, false);

    std::vector<ge::Node*> cacheNodes;

    auto visitor = [&cacheNodes](ge::Node& node) {
        if (node.ROLE(NodeSpec).Type() != hiai::op::Const::TYPE) {
            auto& subGraph = node.ROLE(NodeSubGraph);
            if (subGraph.SubGraphsSize() != 0) {
                CacheSubgraphNodes(subGraph, cacheNodes);
            }
            cacheNodes.push_back(&node);
        }
        return hiai::SUCCESS;
    };

    (void)graph->ROLE(GraphListWalker).WalkAllNodes(visitor);
    for (const auto node : cacheNodes) {
        ge::OpDesc& opDesc = node->ROLE(NodeSpec).OpDesc();
        int versionIR = 0;
        (void)ge::AttrUtils::GetInt(opDesc, OP_VERSION, versionIR);

        if (versionIR == VESION_VALUE_DEFAULT || versionIR == VESION_VALUE_THREE) {
            if (!IRConverter(*node, aiRomVersion, isGraphChanged)) {
                FMK_LOGE("ir converter failed.");
                return false;
            }
            if (!OMConverter(*node, aiRomVersion, isGraphChanged)) {
                FMK_LOGE("om converter failed.");
                return false;
            }
        }
        if (versionIR == VESION_VALUE_FIVE) {
            if (!OMConverter(*node, aiRomVersion, isGraphChanged)) {
                FMK_LOGE("om converter failed.");
                return false;
            }
            if (!IRConverter(*node, aiRomVersion, isGraphChanged)) {
                FMK_LOGE("ir converter failed.");
                return false;
            }
        }
    }
    return true;
}

bool IRTransformer::VerifyIrReservedField(ge::ComputeGraphPtr graph)
{
    HIAI_EXPECT_NOT_NULL_R(graph, false);

    auto visitor = [](ge::Node& node) {
        auto verifyItem = IR_VERIFY_MAP.find(node.ROLE(NodeSpec).Type());
        if (verifyItem != IR_VERIFY_MAP.end()) {
            HIAI_EXPECT_EXEC(verifyItem->second(node));
        }
        return hiai::SUCCESS;
    };

    return graph->ROLE(GraphListWalker).WalkAllNodes(visitor) == hiai::SUCCESS;
}
} // namespace hiai
