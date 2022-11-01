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
#ifndef __IR_VERIFY_H__
#define __IR_VERIFY_H__

#include "framework/graph/core/node/node.h"
#include "graph/debug/ge_error_codes.h"
namespace hiai {
/* 预留字段校验函数 */
ge::GraphErrCodeStatus DataVerify(ge::Node& node);
ge::GraphErrCodeStatus FullyConnectionVerify(ge::Node& node);
ge::GraphErrCodeStatus ScaleVerify(ge::Node& node);
ge::GraphErrCodeStatus SizeVerify(ge::Node& node);
ge::GraphErrCodeStatus LRNVerify(ge::Node& node);
ge::GraphErrCodeStatus BNInferenceVerify(ge::Node& node);
ge::GraphErrCodeStatus DepthwiseConv2DVerify(ge::Node& node);
ge::GraphErrCodeStatus ConvFormatVerify(ge::Node& node);
ge::GraphErrCodeStatus EltwiseVerify(ge::Node& node);
ge::GraphErrCodeStatus ShapeVerify(ge::Node& node);
ge::GraphErrCodeStatus DequantizeVerify(ge::Node& node);
ge::GraphErrCodeStatus QuantizeVerify(ge::Node& node);
ge::GraphErrCodeStatus LayerNormVerify(ge::Node& node);
ge::GraphErrCodeStatus DepthToSpaceVerify(ge::Node& node);
ge::GraphErrCodeStatus ProposalVerify(ge::Node& node);
ge::GraphErrCodeStatus LSTMVerify(ge::Node& node);
ge::GraphErrCodeStatus BidirectionLSTMVerify(ge::Node& node);
ge::GraphErrCodeStatus ActivationVerify(ge::Node& node);
ge::GraphErrCodeStatus NonMaxSuppressionV3DVerify(ge::Node& node);
ge::GraphErrCodeStatus InterpVerify(ge::Node& node);
ge::GraphErrCodeStatus L2NormalizeVerify(ge::Node& node);
ge::GraphErrCodeStatus ConvTransposeVerify(ge::Node& node);
ge::GraphErrCodeStatus MaxUnpool2DVerify(ge::Node& node);
ge::GraphErrCodeStatus MaxPoolWithArgmaxV2Verify(ge::Node& node);
} // namespace hiai
#endif // __IR_VERIFY_H__