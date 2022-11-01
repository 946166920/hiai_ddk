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

#include "transformer_utils.h"

// api/framework
#include "graph/op/const_defs.h"
#include "graph/op/math_defs.h"

// inc/framework
#include "infra/base/assertion.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/utils/replacer/graph_replacer.h"
#include "framework/graph/debug/ge_graph_attr_define.h"
#include "framework/util/tensor/trans_tensor.h"

// src/framework
#include "infra/math/fp16_t.h"

namespace hiai {
namespace {
bool IsHCSSubGraph(const ge::ComputeGraph& graph)
{
    bool isSubGraph = false;
    (void)graph.ROLE(GraphListWalker).WalkAllNodes([&isSubGraph](ge::Node& node) {
        if (node.ROLE(NodeSpec).Type() == "Data" && node.ROLE(NodeSpec).Name().find("SubGraph_") != std::string::npos) {
            isSubGraph = true;
            return hiai::FAILURE;
        }
        return hiai::SUCCESS;
    });
    return isSubGraph;
}

int64_t GetWeightDataSize(ge::Tensor* weight)
{
    if (weight->GetData().GetSize() == 0) {
        return ge::TensorUtils::GetWeightSize(weight->GetTensorDesc());
    } else {
        return weight->GetData().size();
    }
}

ge::GraphErrCodeStatus TransWeightsForFLOAT16(
    ge::Tensor* weight, int32_t xShapeValue, int32_t yShapeValue, bool trans, float scaler)
{
    const char* wData = reinterpret_cast<const char*>(weight->GetData().data());
    HIAI_EXPECT_NOT_NULL(wData);

    int64_t weightSize = xShapeValue * yShapeValue;
    HIAI_EXPECT_TRUE(weightSize > 0);

    int64_t wSize = GetWeightDataSize(weight) / static_cast<int64_t>(sizeof(uint16_t));
    HIAI_EXPECT_TRUE(weightSize == wSize);

    uint16_t* buf = new (std::nothrow) uint16_t[weightSize]();
    HIAI_EXPECT_NOT_NULL(buf);
    for (int32_t i = 0; i < xShapeValue; ++i) {
        for (int32_t j = 0; j < yShapeValue; ++j) {
            const uint16_t* p1Buff = reinterpret_cast<const uint16_t*>(wData) + ((i * yShapeValue) + (j));
            uint16_t* p2Buff = nullptr;
            if (trans) {
                p2Buff = buf + ((j * xShapeValue) + (i));
            } else {
                p2Buff = buf + ((i * yShapeValue) + (j));
            }

            if (abs(scaler - 1.0) > (1e-6) && abs(scaler) > (1e-6)) { // scaler is not 1, 0
                ge::fp16_t tmp = 0;
                tmp.val = (*p1Buff);
                ge::fp16_t rslt;
                rslt = tmp.toFloat() / scaler;
                *p2Buff = rslt.val;
            } else {
                *p2Buff = (*p1Buff);
            }
        }
    }
    weight->SetData(reinterpret_cast<const uint8_t*>(buf), static_cast<size_t>(weightSize) * sizeof(uint16_t));
    weight->MutableTensorDesc().SetFormat(ge::FORMAT_NCHW);
    if ((buf) != nullptr) {
        delete[] buf;
        buf = nullptr;
    }
    return ge::GRAPH_SUCCESS;
}

template <typename Dtype>
ge::GraphErrCodeStatus TransWeightsValue(
    ge::Tensor* weight, int32_t xShapeValue, int32_t yShapeValue, bool trans, float scaler)
{
    const char* wData = reinterpret_cast<const char*>(weight->GetData().data());
    HIAI_EXPECT_NOT_NULL(wData);

    int64_t weightSize = xShapeValue * yShapeValue;
    HIAI_EXPECT_TRUE(weightSize > 0);

    int64_t wSize = GetWeightDataSize(weight) / static_cast<int64_t>(sizeof(Dtype));
    HIAI_EXPECT_TRUE(weightSize == wSize);

    Dtype* buf = new (std::nothrow) Dtype[weightSize]();
    HIAI_EXPECT_NOT_NULL(buf);

    for (int32_t i = 0; i < xShapeValue; ++i) {
        for (int32_t j = 0; j < yShapeValue; ++j) {
            const Dtype* p1Buff = reinterpret_cast<const Dtype*>(wData) + ((i * yShapeValue) + (j));
            Dtype* p2Buff = nullptr;
            if (trans) {
                p2Buff = buf + ((j * xShapeValue) + (i));
            } else {
                p2Buff = buf + ((i * yShapeValue) + (j));
            }
            if (abs(scaler - 1.0) > (1e-6) && abs(scaler) > (1e-6)) { // scaler is not 1, 0
                *p2Buff = (*p1Buff) / static_cast<Dtype>(scaler);
            } else {
                *p2Buff = (*p1Buff);
            }
        }
    }
    weight->SetData(reinterpret_cast<const uint8_t*>(buf), static_cast<size_t>(weightSize) * sizeof(Dtype));
    weight->MutableTensorDesc().SetFormat(ge::FORMAT_NCHW);
    if ((buf) != nullptr) {
        delete[] buf;
        buf = nullptr;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus TransWeightsInfo(
    ge::Tensor* weight, int32_t xShapeValue, int32_t yShapeValue, bool trans = true, float scaler = 1)
{
    switch (weight->GetTensorDesc().GetDataType()) {
        case ge::DT_INT8:
            return TransWeightsValue<int8_t>(weight, xShapeValue, yShapeValue, trans, scaler);
        case ge::DT_FLOAT16:
            return TransWeightsForFLOAT16(weight, xShapeValue, yShapeValue, trans, scaler);
        case ge::DT_FLOAT:
            return TransWeightsValue<float>(weight, xShapeValue, yShapeValue, trans, scaler);
        default:
            return ge::GRAPH_FAILED;
    }
}

ge::GraphErrCodeStatus GemmDAdjustConstDimSize(ge::Node& node, bool transposeFlag, const vector<float>& transFactor)
{
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    HIAI_EXPECT_TRUE(weightsVec.size() <= 2);
    for (size_t i = 0; i < weightsVec.size(); ++i) {
        if (weightsVec[i] == nullptr) {
            continue;
        }
        ge::TensorDesc& weightDesc = weightsVec[i]->MutableTensorDesc();
        ge::Shape weightShape = weightDesc.GetShape();
        // if dim num is 4, dimh and dimw is 1
        if (weightShape.GetDimNum() == 4 && weightShape.GetDim(2) == 1 && weightShape.GetDim(3) == 1) {
            vector<int64_t> shapeValue;
            if (i == 0 && !transposeFlag) {
                shapeValue.push_back(weightShape.GetDim(1));
                shapeValue.push_back(weightShape.GetDim(0));
                weightDesc.SetShape(ge::Shape(shapeValue));
                (void)TransWeightsInfo(weightsVec[i].get(), static_cast<int32_t>(weightShape.GetDim(0)),
                    static_cast<int32_t>(weightShape.GetDim(1)), true, transFactor[i]);
            } else {
                shapeValue.push_back(weightShape.GetDim(0));
                shapeValue.push_back(weightShape.GetDim(1));
                weightDesc.SetShape(ge::Shape(shapeValue));
                (void)TransWeightsInfo(weightsVec[i].get(), static_cast<int32_t>(weightShape.GetDim(0)),
                    static_cast<int32_t>(weightShape.GetDim(1)), false, transFactor[i]);
            }
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus MatMulTransWeights(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (desc.GetInputsSize() != 2) {
        return ge::GRAPH_SUCCESS;
    }
    // 权值打散
    ge::ComputeGraph& originGraph = node.ROLE(NodeSpec).OwnerComputeGraph();

    if (IsHCSSubGraph(originGraph)) {
        return ge::GRAPH_SUCCESS;
    }
    if (originGraph.HasAttr(SRC_MERGED_WEIGHT_ADDR) && originGraph.HasAttr(SRC_MERGED_WEIGHT_SIZE)) {
        HIAI_EXPECT_EXEC(SplitGraphMergedWeight(originGraph));
    }

    bool transposeFlag = false;
    (void)ge::AttrUtils::GetBool(desc, hiai::op::MatMul::transpose_x2, transposeFlag);
    // 转换权值
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    for (size_t i = 0; i < weightsVec.size(); ++i) {
        if (weightsVec[i] == nullptr) {
            continue;
        }
        ge::TensorDesc& weightDesc = weightsVec[i]->MutableTensorDesc();
        ge::Shape weightShape = weightDesc.GetShape();
        if (weightShape.GetDimNum() == 4 && weightShape.GetDim(2) == 1 && weightShape.GetDim(3) == 1) {
            vector<int64_t> shapeValue;
            if (i == 0 && weightsVec.size() != 1) {
                shapeValue.push_back(weightShape.GetDim(0));
                shapeValue.push_back(weightShape.GetDim(1));
                weightDesc.SetShape(ge::Shape(shapeValue));
                continue;
            }
            if (!ge::OpDescUtils::IsNonConstInput(node, 1) && !transposeFlag) {
                shapeValue.push_back(weightShape.GetDim(1));
                shapeValue.push_back(weightShape.GetDim(0));
                weightDesc.SetShape(ge::Shape(shapeValue));
                (void)TransWeightsInfo(weightsVec[i].get(), static_cast<int32_t>(weightShape.GetDim(0)),
                    static_cast<int32_t>(weightShape.GetDim(1)));
            } else {
                shapeValue.push_back(weightShape.GetDim(0));
                shapeValue.push_back(weightShape.GetDim(1));
                weightDesc.SetShape(ge::Shape(shapeValue));
            }
        }
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus GemmDAdjustDimSize(ge::Node& node)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    bool transposeFlag = false;
    float alpha = 1.0;
    float beta = 1.0;
    (void)ge::AttrUtils::GetBool(desc, hiai::op::GemmD::transpose_b, transposeFlag);
    (void)ge::AttrUtils::GetFloat(desc, hiai::op::GemmD::alpha, alpha);
    (void)ge::AttrUtils::GetFloat(desc, hiai::op::GemmD::beta, beta);
    vector<float> transFactor = {alpha, beta};
    // change inputdesc shape dim num to 2
    for (size_t i = 0; i < desc.GetInputsSize(); i++) {
        ge::TensorDescPtr inputDesc = desc.MutableInputDesc(i);
        ge::Shape dataShape = inputDesc->GetShape();
        if (dataShape.GetDimNum() > 2) {
            vector<int64_t> shapeValue;
            shapeValue.push_back(dataShape.GetDim(0));
            shapeValue.push_back(dataShape.GetDim(1));
            inputDesc->SetShape(ge::Shape(shapeValue));
        }
    }

    ge::ComputeGraph& originGraph = node.ROLE(NodeSpec).OwnerComputeGraph();
    if (originGraph.HasAttr(SRC_MERGED_WEIGHT_ADDR) && originGraph.HasAttr(SRC_MERGED_WEIGHT_SIZE)) {
        HIAI_EXPECT_EXEC(SplitGraphMergedWeight(originGraph));
    }
    // change const node shape dim num to 2
    HIAI_EXPECT_EXEC(GemmDAdjustConstDimSize(node, transposeFlag, transFactor));
    return ge::GRAPH_SUCCESS;
}
} // namespace

// 转换函数
ge::GraphErrCodeStatus DataFormatToEnum(ge::OpDesc& desc)
{
    // data_format string ->int
    const std::map<string, int64_t> attrDataFormatMap = {
        {"NCHW", 0},
        {"NHWC", 1},
    };
    std::string DataFormat;
    if (ge::AttrUtils::GetStr(desc, "data_format", DataFormat)) {
        HIAI_EXPECT_TRUE(attrDataFormatMap.count(DataFormat) > 0);
        (void)ge::AttrUtils::SetInt(desc, "format", attrDataFormatMap.at(DataFormat));
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus TransformTypeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (config.isOldToNew != isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (config.dstOpType != "") {
        desc.SetType(config.dstOpType);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus TypeAndAttrConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    HIAI_EXPECT_EXEC(TransformTypeConverter(node, config, isOldToNew));
    HIAI_EXPECT_EXEC(UpdateAttrConverter(node, config, isOldToNew));
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SetIntAttrDefValueConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (!isOldToNew) {
        // new ir -> old ir, set attr default value
        HIAI_EXPECT_TRUE(config.attrInfos.size() > 0);
        if (!config.attrInfos[0].srcDefValue.empty() && !desc.HasAttr(config.attrInfos[0].srcName)) {
            int64_t value = atoi(config.attrInfos[0].srcDefValue.c_str());
            (void)ge::AttrUtils::SetInt(desc, config.attrInfos[0].srcName, value);
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus UpdateAttrConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    string srcName = "";
    string dstName = "";
    for (size_t i = 0; i < config.attrInfos.size(); i++) {
        if (isOldToNew) {
            srcName = config.attrInfos[i].srcName;
            dstName = config.attrInfos[i].dstName;
        } else {
            srcName = config.attrInfos[i].dstName;
            dstName = config.attrInfos[i].srcName;
        }
        AttrValue attrValue;
        if (srcName != "" && dstName != "" && desc.GetAttr(srcName, attrValue) == ge::GRAPH_SUCCESS) {
            HIAI_EXPECT_EXEC(desc.DelAttr(srcName));
            desc.SetAttr(dstName, attrValue);
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus EmptyInputNodeRemoveForOMConverter(
    ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    if (node.ROLE(NodeSpec).IdleInputEndpoints().size() == 0) {
        return ge::GRAPH_SUCCESS;
    }
    return node.ROLE(NodeCompatibler).RemoveIdleEndpoint();
}

ge::GraphErrCodeStatus ConstantOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        int versionIR = 0;
        ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
        (void)ge::AttrUtils::GetInt(desc, OP_VERSION, versionIR);
        if (versionIR != VESION_VALUE_FIVE) {
            return ge::GRAPH_SUCCESS;
        }
        if (desc.GetType() == hiai::op::GemmD::TYPE) {
            return GemmDAdjustDimSize(node);
        }
        if (desc.GetType() == hiai::op::MatMul::TYPE) {
            return MatMulTransWeights(node);
        }
        return ge::GRAPH_SUCCESS;
    }

    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    for (auto& weight : weightsVec) {
        ge::TensorDesc& weightTensorDesc = weight->MutableTensorDesc();
        if (weightTensorDesc.GetDataType() == ge::DT_FLOAT16) {
            ge::ccTensor_t srcWeightTensor;
            ge::ccTensor_t dstWeightTensor;
            srcWeightTensor.dataSize = weight->GetData().GetSize();
            dstWeightTensor.dataSize = weight->GetData().GetSize() * 2;

            uint32_t output_size = weight->GetData().GetSize() * 2;
            if (output_size == 0) {
                return ge::GRAPH_SUCCESS;
            }
            char* output = new (std::nothrow) char[output_size];
            HIAI_EXPECT_NOT_NULL(output);
            hiai::Status ret =
                TransTensorHALFToFloat(srcWeightTensor, weight->GetData().GetData(), dstWeightTensor, output);
            if (ret != hiai::SUCCESS) {
                FMK_LOGE("trans weight from fp16 to fp32 fail !");
                delete[] output;
                return ge::GRAPH_FAILED;
            }
            weight->SetData(reinterpret_cast<uint8_t*>(output), output_size);
            weightTensorDesc.SetDataType(ge::DT_FLOAT);
            delete[] output;
        }
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SplitGraphMergedWeight(ge::ComputeGraph& graph)
{
    int64_t srcSize = 0;
    int64_t srcAddr = 0;
    (void)ge::AttrUtils::GetInt(graph, SRC_MERGED_WEIGHT_SIZE, srcSize);
    (void)ge::AttrUtils::GetInt(graph, SRC_MERGED_WEIGHT_ADDR, srcAddr);

    auto visitor = [&srcSize, &srcAddr](ge::Node& node) {
        ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
        if (opDesc.GetType() == hiai::op::Const::TYPE) {
            ge::TensorPtr weight = nullptr;
            (void)ge::AttrUtils::MutableTensor(opDesc, "value", weight);
            HIAI_EXPECT_NOT_NULL(weight);

            int64_t offset = 0;
            HIAI_EXPECT_TRUE(ge::AttrUtils::GetInt(weight->GetTensorDesc(), "merged_offset", offset));

            uint32_t weightSize = ge::TensorUtils::GetWeightSize(weight->GetTensorDesc());
            HIAI_EXPECT_TRUE(offset >= 0 && offset <= UINT32_MAX && (static_cast<int64_t>(weightSize) <= srcSize) &&
                (srcSize - static_cast<int64_t>(weightSize) >= offset));

            // copy weight
            HIAI_EXPECT_EXEC(
                weight->SetData(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(srcAddr + offset)), weightSize));

            // rm {attr : {merge_offset} & weight_size
            ge::TensorUtils::DeleteAttr(weight->MutableTensorDesc(), "merged_offset");
            ge::TensorUtils::SetWeightSize(weight->MutableTensorDesc(), 0);
        }
        return hiai::SUCCESS;
    };

    HIAI_EXPECT_EXEC(graph.ROLE(GraphListWalker).WalkAllNodes(visitor));
    (void)graph.DelAttr(WEIGHT_MERGED);
    (void)graph.DelAttr(SRC_MERGED_WEIGHT_SIZE);
    (void)graph.DelAttr(SRC_MERGED_WEIGHT_ADDR);
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai