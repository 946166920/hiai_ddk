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
#include "detection_op_transformer.h"

#include "infra/base/assertion.h"

#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/edge.h"

namespace hiai {
static ge::GraphErrCodeStatus FSRDetectionOutputConverterConstToAttr(ge::Node& node)
{
    vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
    GE_CHK_BOOL_RET_STATUS(weightsVec.size() > 0, ge::GRAPH_FAILED, "get weight failed of fsr!");

    // 如果输入im_info为权值，转化为attr后删除
    auto visitor = [](const ge::Tensor& weight, ge::OpDesc& opDesc) {
        const ge::Shape& imgInfoShape = weight.GetTensorDesc().GetShape();
        HIAI_EXPECT_TRUE(imgInfoShape.GetDimNum() == 1);
        HIAI_EXPECT_TRUE(imgInfoShape.GetDim(0) == 2);
        HIAI_EXPECT_TRUE(weight.GetTensorDesc().GetDataType() == ge::DT_INT32);
        HIAI_EXPECT_TRUE(weight.GetData().GetSize() / sizeof(int32_t) == 2);

        const int32_t* sizeHW = reinterpret_cast<const int32_t*>(weight.GetData().GetData());
        HIAI_EXPECT_NOT_NULL(sizeHW);

        (void)ge::AttrUtils::SetInt(opDesc, "img_h", *sizeHW);
        (void)ge::AttrUtils::SetInt(opDesc, "img_w", *(sizeHW + 1));

        (void)ge::AttrUtils::SetFloat(opDesc, "alpha", 1.0);
        (void)ge::AttrUtils::SetFloat(opDesc, "beta", 0.0);
        return hiai::SUCCESS;
    };

    int32_t inputSize = node.ROLE(NodeSpec).OpDesc().GetInputsDescSize();
    if (inputSize <= 3) {
        return ge::GRAPH_SUCCESS;
    }

    // 如果最后一个actual_rois_num可选输入有配置，删除该输入，否则删除对应的anchor
    if (inputSize > 4) {
        HIAI_EXPECT_EXEC(node.ROLE(NodeCompatibler).RemoveSpecificInput(4));
    }
    return node.ROLE(NodeCompatibler).TransConstInputToAttr(3, visitor);
}

static ge::GraphErrCodeStatus ChangeFSRInputSequence(ge::Node& node)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();

    ge::TensorDesc roisDesc = desc.GetInputDesc(0);
    ge::TensorDesc scoreDesc = desc.GetInputDesc(2);
    HIAI_EXPECT_EXEC(desc.UpdateInputDesc(0, scoreDesc));
    HIAI_EXPECT_EXEC(desc.UpdateInputDesc(2, roisDesc));

    const auto& firstEdge = node.ROLE(NodeWalker).InDataEdge(0);
    HIAI_EXPECT_TRUE(firstEdge.Exist());
    const auto& thirdEdge = node.ROLE(NodeWalker).InDataEdge(2);
    HIAI_EXPECT_TRUE(thirdEdge.Exist());

    auto& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveEdge(thirdEdge.Value()));
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveEdge(firstEdge.Value()));

    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).AddEdge(thirdEdge.Value().Src(), firstEdge.Value().Dst()));
    HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).AddEdge(firstEdge.Value().Src(), thirdEdge.Value().Dst()));
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FSRDetectionOutputConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    auto& desc = node.ROLE(NodeSpec).OpDesc();

    // get node version
    int versionIR = 0;
    (void)ge::AttrUtils::GetInt(desc, OP_VERSION, versionIR);
    // old to new or real new to old
    if (isOldToNew || (versionIR == VESION_VALUE_DEFAULT)) {
        GE_CHK_BOOL_RET_STATUS(UpdateAttrConverter(node, config, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
            "type and attr update failed!");
    }
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    // new to old or new to new
    if (desc.HasAttr("img_h") || desc.HasAttr("img_w")) {
        return ge::GRAPH_SUCCESS;
    }

    // new to new for chang location of  rois(0) and score(2)
    if (ChangeFSRInputSequence(node) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("change FSR input sequence failed");
        return ge::GRAPH_FAILED;
    }

    // change im_info(const op ) to attr img_h and img_w
    if (FSRDetectionOutputConverterConstToAttr(node) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("const to attr failed");
        return ge::GRAPH_FAILED;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus FSRDetectionOutputOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        if (opDesc.HasAttr("img_h") && opDesc.HasAttr("img_w")) {
            int32_t imgH = 0;
            int32_t imgW = 0;
            (void)ge::AttrUtils::GetInt(opDesc, "img_h", imgH);
            (void)ge::AttrUtils::GetInt(opDesc, "img_w", imgW);
            vector<int32_t> imInfoLst;
            imInfoLst.push_back(static_cast<int32_t>(imgH));
            imInfoLst.push_back(static_cast<int32_t>(imgW));
            vector<ge::TensorPtr> weightsVec = ge::OpDescUtils::MutableWeights(node);
            ge::TensorPtr imInfoTensor = hiai::make_shared_nothrow<ge::Tensor>(
                ge::TensorDesc(), reinterpret_cast<const uint8_t*>(imInfoLst.data()), 2 * sizeof(int32_t));
            if (imInfoTensor == nullptr) {
                FMK_LOGE("sizeSplitTensor is null.");
                return ge::GRAPH_FAILED;
            }
            imInfoTensor->MutableTensorDesc().SetShape(ge::Shape({2}));
            imInfoTensor->MutableTensorDesc().SetDataType(ge::DT_INT32);
            imInfoTensor->MutableTensorDesc().SetFormat(ge::FORMAT_NCHW);
            weightsVec.push_back(imInfoTensor);
            GE_CHK_STATUS_RET(ge::OpDescUtils::SetWeights(node, weightsVec), "SetWeights failed");
            vector<bool> isInputConst = opDesc.GetIsInputConst();
            isInputConst.push_back(true);
            opDesc.SetIsInputConst(isInputConst);
        }
        return ge::GRAPH_SUCCESS;
    }

    (void)ge::AttrUtils::SetFloat(opDesc, "alpha", 1.0);
    (void)ge::AttrUtils::SetFloat(opDesc, "beta", 0.0);
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai