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
#include "math_op_transformer.h"

#include "infra/base/assertion.h"

#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/debug/ge_graph_attr_define.h"
#include "graph/op/math_defs.h"

namespace hiai {
const int64_t SRC_TYPE_INT8 = 2;
const int64_t SRC_TYPE_UINT8 = 4;
ge::GraphErrCodeStatus ArgMaxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    GE_CHK_BOOL_RET_STATUS(TransformTypeConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "transfer type failed!");
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    // old ir x2 is float, convert float axis to int
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    GE_IF_BOOL_EXEC(weightsVec.size() <= 0, FMK_LOGW("ArgMax no weight");
        return ge::GRAPH_SUCCESS);
    ge::ConstTensorPtr weight = weightsVec.at(0);
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(weight == nullptr, return ge::GRAPH_FAILED, "weight is null.");
    if (weight->GetTensorDesc().GetDataType() != ge::DT_FLOAT) {
        return ge::GRAPH_SUCCESS;
    }
    const float* axis = reinterpret_cast<const float*>(weight->GetData().GetData());
    size_t dataCount = weight->GetData().GetSize() / sizeof(float);
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(
        dataCount != 1, return ge::GRAPH_FAILED, "axis count must be 1, now is %zu", dataCount);
    if (axis == nullptr) {
        return ge::GRAPH_FAILED;
    }
    int32_t intAxis = static_cast<int32_t>(*axis);
    // construct weight
    ge::TensorDesc axisDesc(ge::Shape({1}), ge::FORMAT_NCHW, ge::DT_INT32);
    ge::TensorPtr tensorAxis = ge::TensorPtr(hiai::make_shared_nothrow<ge::Tensor>(axisDesc));
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(tensorAxis == nullptr, return ge::GRAPH_FAILED, "tensorAxis construct failed.");
    vector<int32_t> dataValueAxis = {intAxis};
    tensorAxis->SetData(reinterpret_cast<uint8_t*>(dataValueAxis.data()), sizeof(int32_t));
    // update weight
    weightsVec[0] = tensorAxis;
    ge::OpDescUtils::SetWeights(node, weightsVec);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus L2NormalizeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    vector<int64_t> axisList;
    (void)ge::AttrUtils::GetListInt(desc, ATTR_AXIS, axisList);
    if (axisList.size() > 0) {
        GE_CHK_BOOL_RET_STATUS(
            desc.DelAttr(ATTR_AXIS) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED, "Delete ir attr axis to opdesc failed!");
        (void)ge::AttrUtils::SetInt(desc, ATTR_AXIS, axisList[0]);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ReduceMaxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // axis
    if (desc.HasAttr(ATTR_AXIS)) {
        return ge::GRAPH_SUCCESS;
    }
    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.size() != 1) {
        FMK_LOGE("Set attr axis failed");
        return ge::GRAPH_FAILED;
    }
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(
        weightVec[0]->GetTensorDesc().GetDataType() != ge::DT_INT32, return ge::GRAPH_FAILED, "axes must be int32.");
    vector<int32_t> tidxValue;
    size_t axesSize = weightVec[0]->GetData().GetSize() / sizeof(int32_t);
    const int32_t* axes = reinterpret_cast<const int32_t*>(weightVec[0]->GetData().GetData());
    for (size_t i = 0; i < axesSize; i++) {
        tidxValue.push_back(*(axes + i));
    }
    (void)ge::AttrUtils::SetListInt(desc, ATTR_AXIS, tidxValue);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ReduceMinConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    // axis
    if (desc.HasAttr(ATTR_AXIS)) {
        return ge::GRAPH_SUCCESS;
    }
    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    if (weightVec.size() != 1) {
        FMK_LOGE("Set attr axis failed");
        return ge::GRAPH_FAILED;
    }
    GE_CHK_BOOL_TRUE_EXEC_WITH_LOG(
        weightVec[0]->GetTensorDesc().GetDataType() != ge::DT_INT32, return ge::GRAPH_FAILED, "axes must be int32.");
    vector<int> tidxValue;
    size_t axesSize = weightVec[0]->GetData().GetSize() / sizeof(int32_t);
    const int32_t* axes = reinterpret_cast<const int32_t*>(weightVec[0]->GetData().GetData());
    for (size_t i = 0; i < axesSize; i++) {
        tidxValue.push_back(*(axes + i));
    }
    (void)ge::AttrUtils::SetListInt(desc, ATTR_AXIS, tidxValue);
    return ge::GRAPH_SUCCESS;
}

static ge::GraphErrCodeStatus ArgMaxOMConverterOldToNew(ge::Node& node)
{
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (opdesc.GetInputsSize() == 2) {
        return ge::GRAPH_SUCCESS;
    }
    if (opdesc.HasAttr("axis")) {
        ge::ComputeGraph& originGraph = node.ROLE(NodeSpec).OwnerComputeGraph();
        if (originGraph.HasAttr(SRC_MERGED_WEIGHT_ADDR) && originGraph.HasAttr(SRC_MERGED_WEIGHT_SIZE)) {
            GE_CHK_STATUS_RET(SplitGraphMergedWeight(originGraph), "split graph merged weight failed");
        }
        int64_t axis = 0;
        GE_CHK_BOOL_RET_STATUS(ge::AttrUtils::GetInt(opdesc, "axis", axis), ge::GRAPH_FAILED, "Get axis attr failed");
        ge::TensorPtr shapeWeight = hiai::make_shared_nothrow<ge::Tensor>();
        if (shapeWeight == nullptr) {
            FMK_LOGE("shapeWeight is null.");
            return ge::GRAPH_FAILED;
        }
        std::vector<int64_t> shapeVec;
        shapeVec.push_back(axis);
        vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
        shapeWeight->MutableTensorDesc().SetDataType(ge::DT_INT32);
        shapeWeight->MutableTensorDesc().SetShape(ge::Shape({static_cast<int32_t>(shapeVec.size())}));
        shapeWeight->SetData(reinterpret_cast<const uint8_t*>(shapeVec.data()), shapeVec.size() * sizeof(int32_t));
        weightsVec.insert(weightsVec.cend(), shapeWeight);
        GE_CHK_STATUS_RET(ge::OpDescUtils::SetWeights(node, static_cast<const std::vector<ge::TensorPtr>>(weightsVec)),
            "set weights failed");
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ArgMaxOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        if (ArgMaxOMConverterOldToNew(node) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("convert argmax om from old to new failed");
            return ge::GRAPH_FAILED;
        } else {
            return ge::GRAPH_SUCCESS;
        }
    }

    if (opdesc.HasAttr("axis_type")) {
        return ge::GRAPH_SUCCESS;
    }
    size_t inputNum = opdesc.GetInputsSize();
    int outputType = -1;
    (void)ge::AttrUtils::GetInt(opdesc, hiai::op::ArgMaxExt2::output_type, outputType);
    if (inputNum == 1) {
        (void)ge::AttrUtils::SetInt(opdesc, "axis_type", 2);
    } else if (outputType == 0) {
        (void)ge::AttrUtils::SetInt(opdesc, "axis_type", 1);
        vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
        GE_IF_BOOL_EXEC(weightsVec.size() == 0, return ge::GRAPH_FAILED);
        ge::TensorPtr axisTensor = weightsVec.size() == 1 ? weightsVec[0] : weightsVec[1];
        GE_IF_BOOL_EXEC(axisTensor == nullptr, return ge::GRAPH_FAILED);
        const size_t axisSize = axisTensor->GetData().GetSize() / sizeof(int32_t);
        GE_IF_BOOL_EXEC(axisSize == 0, return ge::GRAPH_FAILED);
        const int32_t* axisVec = reinterpret_cast<const int32_t*>(axisTensor->GetData().GetData());
        if (axisVec == nullptr) {
            return ge::GRAPH_FAILED;
        }
        (void)ge::AttrUtils::SetInt(opdesc, "axis", axisVec[0]);
    } else {
        (void)ge::AttrUtils::SetInt(opdesc, "axis_type", 3);
        (void)ge::AttrUtils::SetInt(opdesc, "axis", 0);
    }

    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus SumOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    (void)ge::AttrUtils::SetInt(opDesc, "Tidx", 3);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus L2NormalizeOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew && opDesc.GetInputsDescSize() == 2) {
        return node.ROLE(NodeCompatibler).RemoveSpecificInput(0);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ReduceMaxOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (EmptyInputNodeRemoveForOMConverter(node, config, isOldToNew) != ge::GRAPH_SUCCESS) {
        return ge::GRAPH_FAILED;
    }

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();
    vector<int32_t> axis;
    if (!ge::AttrUtils::GetListInt(opDesc, "axis", axis)) {
        return ge::GRAPH_SUCCESS;
    }

    if (opDesc.GetInputsDescSize() != 1) {
        return ge::GRAPH_SUCCESS;
    }

    // attr transpose input
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    const int64_t axisSize = static_cast<int64_t>(axis.size());
    ge::TensorDesc axisDesc(ge::Shape({axisSize}), ge::FORMAT_ND, ge::DT_INT32);
    ge::TensorPtr tensorAxes = ge::TensorPtr(hiai::make_shared_nothrow<ge::Tensor>(axisDesc));
    if (tensorAxes == nullptr || axis.empty()) {
        FMK_LOGE("tensorAxes is nullptr or axis is empty");
        return ge::GRAPH_FAILED;
    }
    tensorAxes->SetData(reinterpret_cast<uint8_t*>(axis.data()), sizeof(int32_t) * axisSize);
    weightsVec.push_back(tensorAxes);
    (void)ge::OpDescUtils::SetWeights(node, weightsVec);

    // update is_input_const
    vector<bool> inInputConst = opDesc.GetIsInputConst();
    inInputConst.push_back(true);
    opDesc.SetIsInputConst(inInputConst);
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus CastOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    if (node.ROLE(NodeSpec).Name().find("data_cast_") != 0 &&
        node.ROLE(NodeSpec).Name().find("netoutput_cast_") != 0) {
        ge::OpDesc& opdesc = node.ROLE(NodeSpec).OpDesc();
        int64_t type = -1;
        (void)ge::AttrUtils::GetInt(opdesc, hiai::op::CastT::dst_dtype, type);
        // fix Cast xDataType to FP16 in V320_020 DDK IR FP32 scence.
        if (isOldToNew && type == 1) {
            (void)ge::AttrUtils::SetInt(opdesc, hiai::op::CastT::dst_dtype, 0);
        }
        if (!isOldToNew && type == 0) {
            int64_t srcType = 0;
            (void)ge::AttrUtils::GetInt(opdesc, hiai::op::CastT::src_dtype, srcType);
            if (srcType == SRC_TYPE_INT8 || srcType == SRC_TYPE_UINT8) {
                (void)ge::AttrUtils::SetInt(opdesc, hiai::op::CastT::dst_dtype, 1);
            }
        }
    }
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai