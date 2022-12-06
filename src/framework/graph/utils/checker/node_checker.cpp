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

#include "framework/graph/utils/checker/node_checker.h"

#include <algorithm>

// api/framework
#include "graph/op/array_defs.h"
#include "graph/op/detection_defs.h"
#include "graph/op/image_defs.h"
#include "graph/op/math_defs.h"
#include "graph/op/nn_defs.h"

// inc/framework
#include "framework/common/op_types.h"
#include "framework/graph/op/control_flow_attr_defs.h"
#include "framework/graph/op/internal_defs.h"
#include "framework/graph/op/internal_nn_defs.h"
#include "framework/graph/debug/ge_op_types.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
const std::vector<std::string> GRAPH_INPUT_TYPES {
    hiai::op::Data::TYPE,
    hiai::op::ImageData::TYPE,
    hiai::op::DynamicImageData::TYPE,
    hiai::op::ConfigData::TYPE
};
}

bool NodeChecker::IsGraphInputType(const std::string& type)
{
    return std::find(GRAPH_INPUT_TYPES.cbegin(), GRAPH_INPUT_TYPES.cend(), type) != GRAPH_INPUT_TYPES.cend();
}

namespace { // list op types which not need input.
const std::vector<std::string> NON_INPUT_TYPES {
    /* ir graph input types */
    hiai::op::Data::TYPE,
    hiai::op::ImageData::TYPE,
    hiai::op::DynamicImageData::TYPE,
    hiai::op::Aipp::TYPE, // Note: Aipp should has input Data/AippConfig, but RDV exist may net Aipp no input.
    ge::INPUT_TYPE, // Note: Input is Caffe Input, usage is analysing.
    hiai::ARG, // Note: ARG is TF control node, change to hiai::op::Data in GraphOptimizer.ControlFlowPass.
    ge::AIPP_CONFIG // Note: AippConfig added in AippPass, in anchor not link peer.
};
} // namespace

bool NodeChecker::IsNonInputType(const std::string& type)
{
    return std::find(NON_INPUT_TYPES .cbegin(), NON_INPUT_TYPES .cend(), type) != NON_INPUT_TYPES .cend();
}

namespace { // init op optional inputs
struct OpDefineInputs {
    std::string type;
    std::vector<uint32_t> inputs;
};

const std::vector<OpDefineInputs> OP_DEFINE_OPTIONAL_INPUTS{
    {{hiai::op::StridedSliceV2::TYPE}, {3, 4}},
    {{hiai::op::FSRDetectionOutput::TYPE}, {4}},
    {{hiai::op::ROIAlignV2::TYPE}, {2, 3}},
    {{hiai::op::NonMaxSuppressionV6::TYPE}, {2, 3, 4}},
    {{hiai::op::MatMul::TYPE}, {2}},
    {{hiai::op::QuantizedMatMul::TYPE}, {2}},
    {{hiai::op::GemmD::TYPE}, {2}},
    {{hiai::op::BNInference::TYPE}, {3, 4}},
    {{hiai::op::Convolution::TYPE}, {2, 3}},
    {{hiai::op::QuantizedConvolution::TYPE}, {2}},
    {{hiai::op::ConvTranspose::TYPE}, {0, 3, 4}},
    {{hiai::op::ConvolutionDepthwise::TYPE}, {2, 3}},
    {{hiai::op::QuantizedConvolutionDepthwise::TYPE}, {2}},
    {{hiai::op::FullyConnection::TYPE}, {2, 3}},
    {{hiai::op::QuantizedFullyConnection::TYPE}, {2}},
    {{hiai::op::Scale::TYPE}, {2}},
    {{hiai::op::PriorBox::TYPE}, {1}},
    {{hiai::op::Proposal::TYPE}, {3}},
    {{hiai::op::LSTM::TYPE}, {5, 6, 7, 8}},
    {{hiai::op::Correlation::TYPE}, {2, 3}},
    {{hiai::op::BidirectionLSTM::TYPE}, {1, 4, 5, 6, 7}},
    {{hiai::op::AippConfigFusion::TYPE}, {0, 1, 2, 3, 4, 5, 6}},
    {{hiai::op::PadV3::TYPE}, {2}},
    {{hiai::op::LSTMV2::TYPE}, {20, 21, 22, 23}},
    {{hiai::op::BidirectionalSequenceLstm::TYPE},
        {35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55}},
    {{hiai::op::BidirectionalSequenceRnn::TYPE}, {9, 10, 11}},
    {{hiai::op::ROIPooling::TYPE}, {2}},
    {{hiai::op::Convolution3D::TYPE}, {2, 3}},
    {{hiai::op::WinoConvolution::TYPE}, {2, 3}},
    {{hiai::op::ReduceMin::TYPE}, {1}},
    {{hiai::op::ReduceSum::TYPE}, {1}},
    {{hiai::op::ReduceMax::TYPE}, {1}},
    {{hiai::op::ConfigData::TYPE}, {0}},
    {{hiai::op::DequantizeV2::TYPE}, {1}},
};
} // namespace

bool NodeChecker::IsOptionalInput(const std::string& type, uint32_t idx)
{
    for (const auto& op : OP_DEFINE_OPTIONAL_INPUTS) {
        if (op.type == type) {
            const auto& idxs = op.inputs;
            return std::binary_search(idxs.cbegin(), idxs.cend(), idx);
        }
    }

    return false;
}
} // namespace ge
