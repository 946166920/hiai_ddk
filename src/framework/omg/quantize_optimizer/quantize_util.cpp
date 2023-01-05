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

#include "omg/quantize_optimizer/quantize_util.h"

#include <string>
#include <utility>
#include <set>

#include "base/error_types.h"
#include "graph/op/all_ops.h"
#include "framework/infra/log/log.h"
#include "framework/graph/op/internal_nn_defs.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/debug/ge_graph_attr_define.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/edge_visitor.h"
#include "omg/quantize_optimizer/quantize_math_util.h"
#include "common/math/math_util.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

using namespace std;
using namespace ge;
using namespace hiai;

namespace hiai {
bool QuantizeUtil::IsSupportQuantOpType(const std::string& opType)
{
    const std::set<std::string> quantOpTypes = { hiai::op::Convolution::TYPE, hiai::op::ConvolutionDepthwise::TYPE,
        hiai::op::FullyConnection::TYPE, hiai::op::MatMul::TYPE, hiai::op::ConvTranspose::TYPE };
    if (quantOpTypes.find(opType) == quantOpTypes.end()) {
        return false;
    }
    return true;
}

namespace {
const char* const OP_DESC_QUANT_TYPE = "quantType";
const char* const OP_DESC_QUANT_INFO = "QuantizeInfo";
const char* const OP_DESC_QUANT_INFO_EXT = "QuantizeInfoExt";
const char* const OP_DESC_QUANT_INFO_EXT_V2 = "quantizeInfoExt";
const char* const ATTR_NAME_IS_ONE_SIDE_QUANTIZED = "is_one_side_quantized";
const char* const OP_DESC_TRANS_SCALE = "trans_scale";
const char* const OP_DESC_POWER_TRANS_SCALE = "power_trans_scale";
}

Status QuantizeUtil::CheckOneSideQuantize(ge::ComputeGraph& graph, bool& isOneSideQuant)
{
    if (ge::AttrUtils::GetBool(graph, ATTR_NAME_IS_ONE_SIDE_QUANTIZED, isOneSideQuant)) {
        return hiai::SUCCESS;
    }

    auto checkNodeOneSideQuantize = [&isOneSideQuant](Node& node) {
        if (isOneSideQuant) {
            return hiai::SUCCESS;
        }
        OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
        if (!IsSupportQuantOpType(opDesc.GetType())) {
            return hiai::SUCCESS;
        }

        int64_t quantType = RESERVE_QUANTIZED;
        if (AttrUtils::GetInt(opDesc, OP_DESC_QUANT_TYPE, quantType)) {
            HIAI_EXPECT_IN_RANGE_R(quantType, UINT8_INT8_QUANTIZED, INT16_INT8_QUANTIZED, hiai::FAILED);
            if (quantType == static_cast<int64_t>(INT8_FILTER_QUANTIZED)) {
                isOneSideQuant = true;
                return hiai::SUCCESS;
            }
        }

        QuantizeConfig config;
        if (GetOpQuantizeInfo(node, config) == hiai::SUCCESS) {
            isOneSideQuant = ((config.inputDataType == DT_FLOAT) && (config.weightDataType == DT_INT8));
            return hiai::SUCCESS;
        }
        return hiai::SUCCESS;
    };
    return graph.ROLE(GraphListWalker).WalkAllNodes(std::move(checkNodeOneSideQuantize));
}

hiai::Status QuantizeUtil::SetOneSideQuantize(ge::ComputeGraph& graph, bool value)
{
    if (!ge::AttrUtils::SetBool(graph, ATTR_NAME_IS_ONE_SIDE_QUANTIZED, value)) {
        FMK_LOGE("Graph Set one_side_quantize attr fail.");
        return hiai::FAILURE;
    }
    return hiai::SUCCESS;
}

bool QuantizeUtil::CheckGraphQuantized(ge::ComputeGraph& graph)
{
    bool quantizeFlag = false;
    bool oneSideQuantize = false;
    if (CheckOneSideQuantize(graph, oneSideQuantize) == hiai::SUCCESS && oneSideQuantize) {
        quantizeFlag = true;
        return quantizeFlag;
    }

    auto checkNodeQuantized = [&](Node& node) {
        if (quantizeFlag) {
            return hiai::SUCCESS;
        }
        std::set<std::string> quantIROpTypes = { hiai::op::QuantizeV2::TYPE, hiai::op::DequantizeV2::TYPE,
            hiai::op::QuantizedConst::TYPE };
        OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
        if (quantIROpTypes.find(opDesc.GetType()) != quantIROpTypes.end()) {
            quantizeFlag = true;
            return hiai::SUCCESS;
        }
        if (!IsSupportQuantOpType(opDesc.GetType())) {
            return hiai::SUCCESS;
        }
        ge::QuantizeInfo quantInfo;
        if (GetQuantizeInfo(opDesc, quantInfo) == ge::GRAPH_SUCCESS) {
            quantizeFlag = true;
            return hiai::SUCCESS;
        }
        return hiai::SUCCESS;
    };
    (void)graph.ROLE(GraphListWalker).WalkAllNodes(std::move(checkNodeQuantized));
    return quantizeFlag;
}

namespace {
// 将权重由INT4转换为INT8
Status TransFilterINT4ToINT8(Tensor& filter)
{
    const int8_t compressIn4Max = 7;
    const int8_t compress4BitOffset = 4;
    const int8_t compressInt4Nums = 16;

    const ge::Shape& filterShape = filter.GetTensorDesc().GetShape();
    uint32_t filterSize = 1;
    for (size_t i = 0; i < filterShape.GetDimNum(); ++i) {
        filterSize *= static_cast<uint32_t>(filterShape.GetDim(i));
    }

    const int int4Nums = 2; // 1个INT8存储2个INT4
    uint32_t realfilterSize = filter.GetData().GetSize();
    if (filterSize != realfilterSize * int4Nums) {
        FMK_LOGE("Filter data size[%u] is not half of filter dims product[%u]", realfilterSize, filterSize);
        return hiai::FAILURE;
    }
    unique_ptr<int8_t[]> weightDataInt8(new (std::nothrow) int8_t[filterSize]());
    HIAI_EXPECT_NOT_NULL(weightDataInt8);
    const int8_t* weightDataInt4 = reinterpret_cast<const int8_t*>(filter.GetData().GetData());

    for (uint32_t i = 0; i < realfilterSize; ++i) {
        weightDataInt8[i * int4Nums] = static_cast<int8_t>(static_cast<uint8_t>(weightDataInt4[i]) & 0x0f);
        if (weightDataInt8[i * int4Nums] > compressIn4Max) {
            weightDataInt8[i * int4Nums] -= compressInt4Nums; // 生成的int8可能会大于7，需要转为int4的-8~7之间
        }
        uint8_t tmpInt4Data = static_cast<uint8_t>(weightDataInt4[i]) >> compress4BitOffset;
        if (tmpInt4Data > compressIn4Max) { // 生成的uint8数据可能会大于7，需要转为int4的-8~7之间
            weightDataInt8[i * int4Nums + 1] = static_cast<int8_t>(tmpInt4Data - compressInt4Nums);
        } else {
            weightDataInt8[i * int4Nums + 1] = static_cast<int8_t>(tmpInt4Data);
        }
    }

    filter.SetData(reinterpret_cast<uint8_t*>(weightDataInt8.get()), filterSize * sizeof(int8_t));
    filter.MutableTensorDesc().SetDataType(DT_INT8);

    return hiai::SUCCESS;
}

Status TransFilterINT8ToFP32(const Node& peerNode, Tensor& filter, const vector<float>& weightScale)
{
    uint32_t kernelNum = 0;
    uint32_t kernelSize = 1;
    uint32_t weightDataSize = 1;
    filter.MutableTensorDesc().SetDataType(DT_INT8); // 考虑单边量化模型的后向兼容
    if (QuantizeMathUtil::CalculateKernelInfo(peerNode, filter, kernelSize, kernelNum, weightDataSize) !=
        hiai::SUCCESS) {
        FMK_LOGE("Calc kernel info fail, op:%s", peerNode.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }
    if (QuantizeMathUtil::CheckWeightParams(filter, weightScale, kernelNum, weightDataSize) != hiai::SUCCESS) {
        FMK_LOGE("CheckWeightParam failed, op:%s", peerNode.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }

    vector<float> weightDataVec(weightDataSize);
    float* weightDataNew = weightDataVec.data();
    if (QuantizeUtil::NnSet(static_cast<int32_t>(weightDataSize), 0.0f, weightDataNew) != hiai::SUCCESS) {
        FMK_LOGE("NnSet fail, weightDataSize:%u", weightDataSize);
        return hiai::FAILURE;
    }
    if (QuantizeMathUtil::CalculateFP32Data(peerNode, filter, weightScale, weightDataNew) != hiai::SUCCESS) {
        FMK_LOGE("Op: %s DequantizeFilterToFP32 failed.", peerNode.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }
    filter.MutableTensorDesc().SetDataType(DT_FLOAT);
    filter.SetData(reinterpret_cast<uint8_t*>(weightDataNew), weightDataSize * sizeof(float));

    return hiai::SUCCESS;
}

Status DeCompressFilters(const Node& node, const vector<float>& weightScale)
{
    TensorPtr filter = QuantizeUtil::GetFilterTensor(&node);
    HIAI_EXPECT_NOT_NULL_R(filter, hiai::PARAM_INVALID);
    ge::DataType dataType = filter->GetTensorDesc().GetDataType();
    if (dataType == DT_INT4) {
        if (TransFilterINT4ToINT8(*filter) != hiai::SUCCESS) {
            FMK_LOGE("Op %s excute TransFilterINT4ToINT8 failed.", node.ROLE(NodeSpec).Name().c_str());
            return hiai::FAILURE;
        }
    }

    Node* peerNode = node.ROLE(NodeWalker).OutDataNode(0, 0);
    HIAI_EXPECT_NOT_NULL_R(peerNode, hiai::PARAM_INVALID);
    OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (TransFilterINT8ToFP32(*peerNode, *filter, weightScale) != hiai::SUCCESS) {
        FMK_LOGE("Op %s excute TransFilterINT8ToFP32 failed.", node.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }

    TensorDesc outDesc = opDesc.GetOutputDesc(0);
    outDesc.SetDataType(DT_FLOAT);
    if (opDesc.UpdateOutputDesc(0, outDesc) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("Op %s update output desc failed.", node.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }
    auto updateInDataTensor = [](Edge& outEdge) {
        OpDesc& dstOpDesc = outEdge.DstNode().ROLE(NodeSpec).OpDesc();
        TensorDesc inTensor = dstOpDesc.GetInputDesc(outEdge.DstIdx());
        inTensor.SetDataType(DT_FLOAT);
        if (dstOpDesc.UpdateInputDesc(outEdge.DstIdx(), inTensor) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("Op %s update input desc failed.", dstOpDesc.GetName().c_str());
            return hiai::FAILURE;
        }
        return hiai::SUCCESS;
    };
    if (node.ROLE(NodeWalker).ListOutDataEdges(0, updateInDataTensor) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("Op %s update tensor datatype failed.", node.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }

    return hiai::SUCCESS;
}

uint32_t GetBiasIndex(const ge::Node* node)
{
    uint32_t biasIndex = 1;
    vector<TensorPtr> weightsPtr = OpDescUtils::MutableWeights(*node);
    // Deconv存在可选输入，const个数不固定
    if (weightsPtr.size() == 2) {
        return biasIndex;
    }
    if (node->ROLE(NodeSpec).Type() == hiai::op::ConvTranspose::TYPE) {
        /* const常量个数大于1个时，有以下种场景:
            (1)out_shape输入为空，则bias index为1;
            (2)out_shape输入此时为非const类型，则bias index为1;
            (3)out_shape输入为const类型，bias index为2
        */
        InDataAnchorPtr inAnchor = node->GetInDataAnchor(0);
        if (!inAnchor) {
            return biasIndex;
        }
        OutDataAnchorPtr pAnchor = inAnchor->GetPeerOutAnchor();
        if (!pAnchor) {
            return biasIndex;
        }
        NodePtr outShapeNode = pAnchor->GetOwnerNode();
        if (!outShapeNode) {
            return biasIndex;
        }
        if (outShapeNode->ROLE(NodeSpec).Type() != hiai::op::Const::TYPE) {
            biasIndex = 1;
        } else {
            biasIndex = 2;
        }
    }
    return biasIndex;
}

Status TransBiasINT32ToFp32(ge::Tensor& bias, const ge::QuantizeInfo& quantInfo)
{
    uint32_t biasDataSize = bias.GetData().GetSize() / sizeof(int32_t);
    if (biasDataSize == 0) {
        FMK_LOGE("biasDataSize is zero.");
        return hiai::FAILURE;
    }

    unique_ptr<float[]> biasDataFp32(new (std::nothrow) float[biasDataSize]());
    if (biasDataFp32 == nullptr) {
        FMK_LOGE("biasDataFp32 malloc fail.");
        return hiai::FAILURE;
    }
    float* biasDataNew = biasDataFp32.get();
    const int32_t* biasData = reinterpret_cast<const int32_t*>(bias.GetData().GetData());
    if (biasData == nullptr) {
        FMK_LOGE("biasData is nullptr");
        return hiai::FAILURE;
    }

    const float* scaleWeightValue = reinterpret_cast<const float*>(quantInfo.scale_weight_value.GetData());
    if (scaleWeightValue == nullptr) {
        FMK_LOGE("scale_weight_value is nullptr");
        return hiai::FAILURE;
    }

    uint32_t scaleWeightSize = quantInfo.scale_weight_value.GetSize() / sizeof(float);
    int scaleFlag = 0;
    if (quantInfo.scale_weight_mode == VECTOR_SCALE) {
        if (biasDataSize != scaleWeightSize) {
            FMK_LOGE("scale weight size [%u] is not equal to bias data size [%u].", scaleWeightSize, biasDataSize);
            return hiai::FAILURE;
        }
        scaleFlag = 1;
    }
    float scaleDataValue = quantInfo.scale_data_value;

    for (uint32_t i = 0; i < biasDataSize; ++i) {
        biasDataNew[i] =
            static_cast<float>(biasData[i]) * scaleWeightValue[i * static_cast<uint32_t>(scaleFlag)] * scaleDataValue;
    }

    bias.SetData(reinterpret_cast<uint8_t*>(biasDataFp32.get()), biasDataSize * sizeof(float));
    bias.MutableTensorDesc().SetDataType(DT_FLOAT);
    return hiai::SUCCESS;
}

Status DeCompressBias(const Node& node, const ge::QuantizeInfo& quantInfo)
{
    vector<TensorPtr> weights = ge::OpDescUtils::MutableWeights(node);
    size_t weightsSize = weights.size();
    uint32_t biasIndex = GetBiasIndex(&node);
    if (biasIndex >= weightsSize) {
        return hiai::SUCCESS;
    }
    TensorPtr bias = weights[biasIndex];
    if (bias->GetTensorDesc().GetDataType() != ge::DT_INT32) {
        return hiai::SUCCESS;
    }
    HIAI_EXPECT_EXEC(TransBiasINT32ToFp32(*bias, quantInfo));
    return hiai::SUCCESS;
}
Status DequantizeOldIR(const ge::Node& node)
{
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    ge::QuantizeInfo quantInfo;
    if (QuantizeUtil::GetQuantizeInfo(opDesc, quantInfo) != ge::GRAPH_SUCCESS) {
        return hiai::SUCCESS;
    }
    HIAI_EXPECT_TRUE(quantInfo.scale_weight_value.GetSize() != 0);

    vector<float> weightScale(quantInfo.scale_weight_value.GetSize() / sizeof(float), 0.0f);
    HIAI_EXPECT_TRUE(memcpy_s(&weightScale[0], quantInfo.scale_weight_value.GetSize(),
        quantInfo.scale_weight_value.GetData(), quantInfo.scale_weight_value.GetSize()) == 0);

    vector<Node*> constNodes = OpDescUtils::GetConstInputs(node);
    uint32_t filterIndex = QuantizeUtil::GetFilterIndex(&node);
    HIAI_EXPECT_TRUE(filterIndex < constNodes.size());
    Node* filterNode = constNodes[filterIndex];
    HIAI_EXPECT_NOT_NULL_R(filterNode, hiai::PARAM_INVALID);

    HIAI_EXPECT_EXEC(DeCompressFilters(*filterNode, weightScale));
    HIAI_EXPECT_EXEC(DeCompressBias(node, quantInfo));
    return hiai::SUCCESS;
}
} // namespace

Status QuantizeUtil::DequantizeComputeGraph(ge::ComputeGraph& graph)
{
    FMK_LOGI("graph node size:%zu.", graph.ROLE(GraphSpec).NodesNum());
    vector<Node*> bypassNodes;
    auto dequantizeNode = [&](Node& node) {
        OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
        if (opDesc.GetType() == hiai::op::QuantizeV2::TYPE || opDesc.GetType() == hiai::op::DequantizeV2::TYPE) {
            bypassNodes.push_back(&node);
        } else if (opDesc.GetType() == hiai::op::QuantizedConst::TYPE) {
            // 权值数据反量化
            vector<float> weightScale;
            if (!AttrUtils::GetListFloat(opDesc, hiai::op::QuantizedConst::scale, weightScale)) {
                FMK_LOGE("Get weight scale fail, op:%s", opDesc.GetName().c_str());
                return hiai::FAILURE;
            }

            if (DeCompressFilters(node, weightScale) != hiai::SUCCESS) {
                FMK_LOGE("Decompress filter fail.");
                return hiai::FAILURE;
            }
            opDesc.SetType(hiai::op::Const::TYPE);
            (void)opDesc.DelAttr(hiai::op::QuantizedConst::scale);
            (void)opDesc.DelAttr(hiai::op::QuantizedConst::offset);
            return hiai::SUCCESS;
        } else {
            if (!IsSupportQuantOpType(opDesc.GetType())) {
                return hiai::SUCCESS;
            }
            HIAI_EXPECT_EXEC(DequantizeOldIR(node));
            (void)opDesc.DelAttr(OP_DESC_QUANT_INFO);
            (void)opDesc.DelAttr(OP_DESC_QUANT_INFO_EXT);
            (void)opDesc.DelAttr(OP_DESC_QUANT_INFO_EXT_V2);
            (void)opDesc.DelAttr(OP_DESC_QUANT_TYPE);
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(graph.ROLE(GraphListWalker).WalkAllNodes(std::move(dequantizeNode)));
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveNodes(bypassNodes));
    if (graph.HasAttr(ATTR_NAME_IS_ONE_SIDE_QUANTIZED)) {
        HIAI_EXPECT_EXEC(SetOneSideQuantize(graph, false));
    }
    return hiai::SUCCESS;
}

uint32_t QuantizeUtil::GetFilterIndex(const ge::Node* node)
{
    uint32_t filterIndex = 0;
    vector<TensorPtr> weightsPtr = OpDescUtils::MutableWeights(*node);
    // Deconv存在可选输入，const个数不固定
    if (weightsPtr.size() == 1) {
        return filterIndex;
    }
    if (node->ROLE(NodeSpec).Type() == hiai::op::ConvTranspose::TYPE) {
        /* const常量个数大于1个时，有以下种场景:
            (1)out_shape输入为空，则filter index为0;
            (2)out_shape输入此时为非const类型，则filter index为0;
            (3)out_shape输入为const类型，filterIndex为1
        */
        InDataAnchorPtr inAnchor = node->GetInDataAnchor(0);
        if (!inAnchor) {
            return filterIndex;
        }
        OutDataAnchorPtr pAnchor = inAnchor->GetPeerOutAnchor();
        if (!pAnchor) {
            return filterIndex;
        }
        NodePtr outShapeNode = pAnchor->GetOwnerNode();
        if (!outShapeNode) {
            return filterIndex;
        }
        if (outShapeNode->ROLE(NodeSpec).Type() != hiai::op::Const::TYPE) {
            filterIndex = 0;
        } else {
            filterIndex = 1;
        }
    }
    return filterIndex;
}

TensorPtr QuantizeUtil::GetFilterTensor(const ge::Node* node)
{
    vector<TensorPtr> weightsPtr = OpDescUtils::MutableWeights(*node);
    if (weightsPtr.size() == 0) {
        return nullptr;
    }
    uint32_t index = GetFilterIndex(node);
    return weightsPtr[index];
}

namespace {
Status GetOpOutChannelNum(const Node& node, int32_t& outChannelNum)
{
    if (node.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE ||
        node.ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE) {
        Node* nextNode = node.ROLE(NodeWalker).OutDataNode(0, 0);
        HIAI_EXPECT_NOT_NULL_R(nextNode, hiai::PARAM_INVALID);
        return GetOpOutChannelNum(*nextNode, outChannelNum);
    }

    TensorPtr filter = QuantizeUtil::GetFilterTensor(&node);
    HIAI_EXPECT_NOT_NULL_R(filter, hiai::PARAM_INVALID);
    const uint32_t fcWeightScaleCount = 1;
    if (node.ROLE(NodeSpec).Type() == hiai::op::Convolution::TYPE ||
        node.ROLE(NodeSpec).Type() == hiai::op::ConvolutionDepthwise::TYPE) {
        // ConvTranspose op weight format:[Cout,Cin, H, W]
        outChannelNum = filter->GetTensorDesc().GetShape().GetDim(0);
    } else if (node.ROLE(NodeSpec).Type() == hiai::op::FullyConnection::TYPE ||
        node.ROLE(NodeSpec).Type() == hiai::op::MatMul::TYPE) {
        outChannelNum = fcWeightScaleCount;
    } else if (node.ROLE(NodeSpec).Type() == hiai::op::ConvTranspose::TYPE) {
        // ConvTranspose op weight format:[Cin,Cout, H, W]
        outChannelNum = filter->GetTensorDesc().GetShape().GetDim(1);
    } else {
        FMK_LOGE("The op %s does not support quantize.", node.ROLE(NodeSpec).Name().c_str());
        return hiai::FAILURE;
    }
    return hiai::SUCCESS;
}

bool GetFeaturemapQuantParams(const OpDesc& opDesc, QuantizeConfig& config)
{
    vector<float> scaleValues;
    vector<float> offsetValues;
    if ((!AttrUtils::GetListFloat(opDesc, hiai::op::QuantizeV2::scale, scaleValues)) ||
        (!AttrUtils::GetListFloat(opDesc, hiai::op::QuantizeV2::offset, offsetValues))) {
        FMK_LOGE("Get scale or offset attr from node:%s fail.", opDesc.GetName().c_str());
        return false;
    }
    if (scaleValues.size() == 0 || offsetValues.size() == 0) {
        FMK_LOGE("Get scale or offset data from node:%s fail.", opDesc.GetName().c_str());
        return false;
    }

    ge::AttrValue outputTypeValue;
    if (opDesc.GetAttr(hiai::op::QuantizeV2::dtype, outputTypeValue) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("Get out_dtype attr from node:%s fail.", opDesc.GetName().c_str());
        return false;
    }
    config.inputScale.swap(scaleValues);
    config.inputOffset.swap(offsetValues);
    config.inputDataType = static_cast<ge::DataType>(outputTypeValue.GetInt());

    return true;
}

bool GetWeightQuantParams(const OpDesc& opDesc, QuantizeConfig& config)
{
    vector<float> scaleValues;
    vector<float> offsetValues;
    if ((!AttrUtils::GetListFloat(opDesc, hiai::op::QuantizedConst::scale, scaleValues)) ||
        (!AttrUtils::GetListFloat(opDesc, hiai::op::QuantizedConst::offset, offsetValues))) {
        FMK_LOGE("Get scale or offset attr from node:%s fail.", opDesc.GetName().c_str());
        return false;
    }
    if (scaleValues.size() == 0 || offsetValues.size() == 0) {
        FMK_LOGE("Get scale or offset data from node:%s fail.", opDesc.GetName().c_str());
        return false;
    }

    const ge::TensorDesc& outTensor = opDesc.GetOutputDesc(0);

    config.weightScale.swap(scaleValues);
    config.weightOffset.swap(offsetValues);
    config.weightDataType = outTensor.GetDataType();

    return true;
}

Status GetQuantDataType(int64_t type, ge::DataType& inputDType, ge::DataType& weightDType)
{
    if (type >= static_cast<int64_t>(RESERVE_QUANTIZED) || type <= 0) {
        FMK_LOGE("Get quantize type:%lld invalid.", type);
        return hiai::FAILURE;
    }
    std::map<QuantizeType, std::pair<ge::DataType, ge::DataType>> compressTypeMaps = {
        { UINT8_INT8_QUANTIZED, { DT_UINT8, DT_INT8 } },
        { UINT8_INT2_QUANTIZED, { DT_UINT8, DT_2BIT } },
        { INT4_INT4_QUANTIZED, { DT_INT4, DT_INT4 } },
        { INT8_FILTER_QUANTIZED, { DT_FLOAT, DT_INT8 } },
        { INT16_INT8_QUANTIZED, { DT_INT16, DT_INT8 } },
    };
    std::map<QuantizeType, std::pair<ge::DataType, ge::DataType>>::const_iterator it =
        compressTypeMaps.find(static_cast<QuantizeType>(type));
    if (it == compressTypeMaps.cend()) {
        FMK_LOGE("quant type:%d not supported.", type);
        return hiai::FAILURE;
    }
    inputDType = it->second.first;
    weightDType = it->second.second;
    return hiai::SUCCESS;
}

Status GetOpQuantizeInfoFromOldIR(const Node& node, QuantizeConfig& config)
{
    ge::QuantizeInfo quantInfo;
    int64_t quantType;
    OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if ((QuantizeUtil::GetQuantizeInfo(opDesc, quantInfo) != hiai::SUCCESS) ||
        !ge::AttrUtils::GetInt(node.GetOpDesc(), OP_DESC_QUANT_TYPE, quantType)) {
        return hiai::FAILURE;
    }
    if (opDesc.HasAttr(AIPP_CONV_FLAG)) {
        return hiai::SUCCESS;
    }
    HIAI_EXPECT_EXEC(GetQuantDataType(quantType, config.inputDataType, config.weightDataType));
    config.inputScale.push_back(quantInfo.scale_data_value);
    config.inputOffset.push_back(quantInfo.offset_data_value);
    size_t kernelNum = quantInfo.scale_weight_value.GetSize() / sizeof(float);
    const float* weightScale = reinterpret_cast<const float*>(quantInfo.scale_weight_value.GetData());
    for (size_t i = 0; i < kernelNum; i++) {
        config.weightScale.push_back(weightScale[i]);
        config.weightOffset.push_back(0.0f);
    }
    return hiai::SUCCESS;
}

Status TransFilterFP32ToINT8(
    const ge::Node& node, TensorPtr filter, const vector<float>& weightScale, ge::DataType weightDataType)
{
    Node* peerNode = node.ROLE(NodeWalker).OutDataNode(0, 0);
    HIAI_EXPECT_NOT_NULL_R(peerNode, hiai::PARAM_INVALID);
    OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    uint32_t kernelNum = 0;
    uint32_t kernelSize = 1;
    uint32_t weightDataSize = 1;
    if (QuantizeMathUtil::CalculateKernelInfo(*peerNode, *filter, kernelSize, kernelNum, weightDataSize) !=
        hiai::SUCCESS) {
        FMK_LOGE("Calc kernel info fail, op:%s", peerNode->ROLE(NodeSpec).Name().c_str());
        return hiai::FAILED;
    }

    if (QuantizeMathUtil::CheckWeightParams(*filter, weightScale, kernelNum, weightDataSize) != hiai::SUCCESS) {
        FMK_LOGE("CheckWeightParam failed, op:%s", peerNode->ROLE(NodeSpec).Name().c_str());
        return hiai::FAILED;
    }

    unique_ptr<int8_t[]> weightDataInt8(new (std::nothrow) int8_t[weightDataSize]());
    HIAI_EXPECT_NOT_NULL(weightDataInt8);
    int8_t zero = 0;
    HIAI_EXPECT_EXEC(QuantizeUtil::NnSet(weightDataSize, zero, reinterpret_cast<int8_t*>(weightDataInt8.get())));
    int8_t* weightDataNew = weightDataInt8.get();

    filter->MutableTensorDesc().SetDataType(weightDataType);
    if (QuantizeMathUtil::CalculateINT8Data(*peerNode, *filter, weightScale, weightDataNew) != hiai::SUCCESS) {
        FMK_LOGE("Op: %s CalculateINT8Data failed.", opDesc.GetName().c_str());
        return hiai::FAILED;
    }
    filter->SetData(reinterpret_cast<uint8_t*>(weightDataInt8.get()), weightDataSize * sizeof(int8_t));
    return hiai::SUCCESS;
}
} // namespace

Status QuantizeUtil::CheckOpQuantizeInfo(const ge::Node& node, const QuantizeConfig& quantizeConfig)
{
    const float eps = 0.000001;
    const int32_t floatSize = 4;
    int32_t outChannelNum = 0;
    if (GetOpOutChannelNum(node, outChannelNum) != SUCCESS) {
        FMK_LOGE("Get op OutChannelNum fail.");
        return hiai::FAILURE;
    }
    if (CheckInt32MulOverflow(outChannelNum, floatSize) != hiai::SUCCESS) {
        FMK_LOGE("WeightSize multiply 4 is larger than INT_MAX.");
        return hiai::FAILURE;
    }

    bool checkLen = (quantizeConfig.weightScale.size() == static_cast<uint32_t>(outChannelNum)) &&
        (quantizeConfig.weightOffset.size() == static_cast<uint32_t>(outChannelNum));
    if (!checkLen) {
        FMK_LOGE("weight scale size[%u] or offset size[%u] is not equel to weight size[%d].",
            quantizeConfig.weightScale.size(), quantizeConfig.weightOffset.size(), outChannelNum);
        return hiai::FAILURE;
    }
    const float* weightOffset = reinterpret_cast<const float*>(&quantizeConfig.weightOffset[0]);
    HIAI_EXPECT_NOT_NULL_R(weightOffset, hiai::PARAM_INVALID);
    for (auto i = 0; i < outChannelNum; i++) {
        if (fabs(weightOffset[i]) > eps) {
            FMK_LOGE("Weight Quantize type only support HALF_OFFSET, but op %s weight offset is not 0.",
                node.ROLE(NodeSpec).Name().c_str());
            return hiai::FAILURE;
        }
    }
    return hiai::SUCCESS;
}

Status QuantizeUtil::GetOpQuantizeInfo(const Node& node, QuantizeConfig& config)
{
    bool hasQuantizedV2 = false;
    bool hasQuantizedConst = false;
    auto getQuantizeInfo = [&](Node& preNode) {
        OpDesc& preOpDesc = preNode.ROLE(NodeSpec).OpDesc();
        if (preOpDesc.GetType() == hiai::op::QuantizeV2::TYPE) {
            hasQuantizedV2 = true;
            // 1.从输入的QuantizeV2算子中获取featuremap量化参数信息
            if (!GetFeaturemapQuantParams(preOpDesc, config)) {
                FMK_LOGE("Get featuremap quant params fail, node:%s.", node.ROLE(NodeSpec).Name().c_str());
                return hiai::FAILURE;
            }
        } else if (preOpDesc.GetType() == hiai::op::QuantizedConst::TYPE) {
            // 2.从权值节点中获取权值量化参数信息
            hasQuantizedConst = true;
            if (!GetWeightQuantParams(preOpDesc, config)) {
                FMK_LOGE("Get weight quant params fail, node:%s.", node.ROLE(NodeSpec).Name().c_str());
                return hiai::FAILURE;
            }
            ge::TensorPtr filter = QuantizeUtil::GetFilterTensor(&preNode);
            HIAI_EXPECT_NOT_NULL_R(filter, hiai::PARAM_INVALID);
            config.weightDataType = filter->GetTensorDesc().GetDataType();
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(node.ROLE(NodeWalker).ListInDataNodes(getQuantizeInfo));
    OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (opDesc.HasAttr(OP_DESC_QUANT_INFO_EXT_V2)) {
        config.hasQuantInfoExt = true;
        std::string quantInfoExt = "";
        if (!AttrUtils::GetStr(opDesc, OP_DESC_QUANT_INFO_EXT_V2, quantInfoExt)) {
            FMK_LOGE("Get quantize ext info fail, node:%s.", opDesc.GetName().c_str());
            return hiai::FAILURE;
        }
        config.quantInfoExt = quantInfoExt;
    }
    if (!hasQuantizedConst) {
        return GetOpQuantizeInfoFromOldIR(node, config);
    }
    if (!hasQuantizedV2) { // 单边量化场景
        config.inputDataType = DT_FLOAT;
    }
    return hiai::SUCCESS;
}

Status QuantizeUtil::GetQuantizeInfo(const OpDesc& opDesc, QuantizeInfo& quantInfo)
{
    AttrValue attrValue;
    if (opDesc.GetAttr(OP_DESC_QUANT_INFO, attrValue) != GRAPH_SUCCESS) {
        return hiai::FAILURE;
    }
    AttrValue::NamedAttrs namedAttrs = attrValue.GetNamedAttrs();
    quantInfo.quantize_algo = static_cast<uint32_t>(namedAttrs.GetItem("quantize_algo").GetInt());
    quantInfo.scale_data_value = namedAttrs.GetItem("scale_data_value").GetFloat();
    quantInfo.offset_data_value = namedAttrs.GetItem("offset_data_value").GetFloat();
    quantInfo.scale_weight_mode = static_cast<uint32_t>(namedAttrs.GetItem("scale_weight_mode").GetInt());
    quantInfo.scale_weight_value = namedAttrs.GetItem("scale_weight_value").GetBuffer();
    quantInfo.offset_weight_value = namedAttrs.GetItem("offset_weight_value").GetBuffer();

    return hiai::SUCCESS;
}

Status QuantizeUtil::SetQuantizeInfo(OpDesc& opDesc, const QuantizeInfo& quantInfo)
{
    AttrValue::NamedAttrs namedAttrs;
    namedAttrs.SetAttr("quantize_algo", AttrValue::CreateFrom(static_cast<int64_t>(quantInfo.quantize_algo)));
    namedAttrs.SetAttr("scale_data_value", AttrValue::CreateFrom(quantInfo.scale_data_value));
    namedAttrs.SetAttr("offset_data_value", AttrValue::CreateFrom(quantInfo.offset_data_value));
    namedAttrs.SetAttr("scale_weight_mode", AttrValue::CreateFrom(static_cast<int64_t>(quantInfo.scale_weight_mode)));
    namedAttrs.SetAttr("scale_weight_value", AttrValue::CreateFrom(quantInfo.scale_weight_value));
    namedAttrs.SetAttr("offset_weight_value", AttrValue::CreateFrom(quantInfo.offset_weight_value));
    auto attr = AttrValue::CreateFrom(namedAttrs);
    if (opDesc.SetAttr(OP_DESC_QUANT_INFO, attr) != GRAPH_SUCCESS) {
        return hiai::FAILURE;
    }
    return hiai::SUCCESS;
}

hiai::Status SetQuantAttr(ge::OpDesc& opDesc, const string& attrKey, const AttrValue& attrValue)
{
    if (opDesc.SetAttr(attrKey, attrValue) != ge::GRAPH_SUCCESS) {
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}
hiai::Status QuantizeUtil::SetQuantType(ge::OpDesc& opDesc, int64_t quantType)
{
    return SetQuantAttr(opDesc, OP_DESC_QUANT_TYPE, AttrValue::CreateFrom(quantType));
}

hiai::Status QuantizeUtil::SetQuantInfoExt(ge::OpDesc& opDesc, const string& quantInfoExt)
{
    return SetQuantAttr(opDesc, OP_DESC_QUANT_INFO_EXT_V2, AttrValue::CreateFrom(quantInfoExt));
}

hiai::Status QuantizeUtil::SetPowerTransScale(ge::OpDesc& opDesc, float powerTransScale)
{
    return SetQuantAttr(opDesc, OP_DESC_POWER_TRANS_SCALE, AttrValue::CreateFrom(powerTransScale));
}

bool QuantizeUtil::GetPowerTransScale(const ge::OpDesc& opDesc, float& powerTransScale)
{
    return AttrUtils::GetFloat(opDesc, OP_DESC_POWER_TRANS_SCALE, powerTransScale);
}

bool QuantizeUtil::HasPowerTransScale(const ge::OpDesc& opDesc)
{
    return opDesc.HasAttr(OP_DESC_POWER_TRANS_SCALE);
}

void QuantizeUtil::DelPowerTransScale(ge::OpDesc& opDesc)
{
    opDesc.DelAttr(OP_DESC_POWER_TRANS_SCALE);
}

hiai::Status QuantizeUtil::SetTransScale(ge::OpDesc& opDesc, const vector<float>& transScale)
{
    return SetQuantAttr(opDesc, OP_DESC_TRANS_SCALE, AttrValue::CreateFrom(transScale));
}

bool QuantizeUtil::GetTransScale(const ge::OpDesc& opDesc, vector<float>& transScale)
{
    return AttrUtils::GetListFloat(opDesc, OP_DESC_TRANS_SCALE, transScale);
}

bool QuantizeUtil::HasTransScale(const ge::OpDesc& opDesc)
{
    return opDesc.HasAttr(OP_DESC_TRANS_SCALE);
}

void QuantizeUtil::DelTransScale(ge::OpDesc& opDesc)
{
    opDesc.DelAttr(OP_DESC_TRANS_SCALE);
}

Node* QuantizeUtil::FindQuantizeWeightNode(Node* node)
{
    vector<Node*> constNodes = OpDescUtils::GetConstInputs(*node);
    uint32_t filterIndex = GetFilterIndex(node);
    if (filterIndex >= constNodes.size()) {
        FMK_LOGE("Get node:%s const inputs fail.", node->ROLE(NodeSpec).Name().c_str());
        return nullptr;
    }
    return constNodes[filterIndex];
}

Status QuantizeUtil::QuantizeWeight(ge::Node& weightNode, const QuantizeConfig& quantizeConfig)
{
    OpDesc& filterOpDesc = weightNode.ROLE(NodeSpec).OpDesc();
    ge::TensorPtr filter = GetFilterTensor(&weightNode);
    HIAI_EXPECT_NOT_NULL_R(filter, hiai::PARAM_INVALID);

    /* 完成权值数据量化 */
    if (TransFilterFP32ToINT8(
        weightNode, filter, quantizeConfig.weightScale, quantizeConfig.weightDataType) != hiai::SUCCESS) {
        FMK_LOGE("Node:%s trans filter to int8 fail.", filterOpDesc.GetName().c_str());
        return hiai::FAILED;
    }
    if (quantizeConfig.weightDataType == DT_2BIT || quantizeConfig.weightDataType == DT_INT4) {
        if (QuantizeMathUtil::CompressWeightToINT8(*filter, quantizeConfig.weightDataType) != hiai::SUCCESS) {
            FMK_LOGE("Node:%s weight compress fail.", filterOpDesc.GetName().c_str());
            return hiai::FAILED;
        }
    }
    // 对单边量化模型，权值数据类型设置为float
    bool isOneSideQuantize =
        (quantizeConfig.inputDataType == DT_FLOAT16) && (quantizeConfig.weightDataType == DT_INT8);
    if (isOneSideQuantize) {
        filter->MutableTensorDesc().SetDataType(DT_FLOAT);
    }

    return hiai::SUCCESS;
}
} // namespace hiai
