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

#ifndef __INTERNAL_OP_TRANSFORMER_H__
#define __INTERNAL_OP_TRANSFORMER_H__
#include "transformer_utils.h"
namespace hiai {
ge::GraphErrCodeStatus PadV3Converter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus ChangeDimOrderOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus YoloDetectionOutputOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
} // namespace hiai
#endif // __INTERNAL_OP_TRANSFORMER_H__