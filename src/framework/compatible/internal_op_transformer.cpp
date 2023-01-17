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
#include "internal_op_transformer.h"

#include "infra/base/assertion.h"

#include "framework/graph/op/internal_defs.h"
#include "framework/graph/op/internal_nn_defs.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "graph/op/detection_defs.h"
#include "graph/op/array_defs.h"

namespace hiai {
const int PADV3_WEIGHT_WITH_CONSTANT_NUM = 2;
ge::GraphErrCodeStatus PadV3Converter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();

    vector<ge::TensorPtr> weightVec = ge::OpDescUtils::MutableWeights(node);
    // if has constant input, need to convert to padv2
    ConvertConfigInfo newConfig = config;
    if (weightVec.size() == PADV3_WEIGHT_WITH_CONSTANT_NUM) {
        newConfig.dstOpType = hiai::op::PadV2::TYPE;
    }

    // type mapping
    GE_CHK_BOOL_RET_STATUS(TransformTypeConverter(node, newConfig, isOldToNew) == ge::GRAPH_SUCCESS, ge::GRAPH_FAILED,
        "transfer type failed!");
    // mode
    int mode = 0;
    if (!ge::AttrUtils::GetInt(desc, hiai::op::PadV3::mode, mode)) {
        (void)ge::AttrUtils::SetInt(desc, hiai::op::PadV3::mode, mode);
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ChangeDimOrderOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (!isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }

    if (desc.GetInputsDescSize() != 2) {
        return ge::GRAPH_SUCCESS;
    }

    if (node.ROLE(NodeSpec).Type() == hiai::op::Transpose::TYPE) {
        desc.SetType(hiai::op::Permute::TYPE);
    }

    return node.ROLE(NodeCompatibler).RemoveSpecificInput(1);
}

ge::GraphErrCodeStatus YoloDetectionOutputOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;
    ge::OpDesc& desc = node.ROLE(NodeSpec).OpDesc();
    if (isOldToNew) {
        return ge::GRAPH_SUCCESS;
    }
    (void)ge::AttrUtils::SetFloat(desc, "iou_threshold_decay", 1);
    (void)ge::AttrUtils::SetFloat(desc, "coor_scale_factor", 1);
    (void)ge::AttrUtils::SetFloat(desc, "alpha", 1);
    (void)ge::AttrUtils::SetFloat(desc, "beta", 0);
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai