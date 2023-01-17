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
#ifndef __MATH_OP_TRANSFORMER_H__
#define __MATH_OP_TRANSFORMER_H__
#include "transformer_utils.h"

namespace hiai {
ge::GraphErrCodeStatus ArgMaxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus L2NormalizeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ReduceMaxConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ReduceMinConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ArgMaxOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus SumOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus L2NormalizeOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ReduceMaxOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus CastOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
} // namespace hiai
#endif // __MATH_OP_TRANSFORMER_H__