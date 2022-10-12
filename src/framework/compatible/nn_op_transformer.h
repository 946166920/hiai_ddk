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

#ifndef __NN_OP_TRANSFORMER_H__
#define __NN_OP_TRANSFORMER_H__
#include "transformer_utils.h"

namespace hiai {
ge::GraphErrCodeStatus HardSwishConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus SpaceToDepthConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ToDynamicInputConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus Conv2DTransposeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus FullyConnectionConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ScaleConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus InstanceNormConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus LayerNormConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ProposalConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus LSTMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus BidirectionLSTMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ConvOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus BiasAddOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ReLU6OMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ScaleOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus PriorBoxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus PReluConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus LayerNormOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus AlgoOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus LSTMOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ProposalOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus FullyConnectionOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
} // namespace hiai
#endif // __NN_OP_TRANSFORMER_H__