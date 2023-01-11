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

#ifndef __ARRAY_OP_TRANSFORMER_H__
#define __ARRAY_OP_TRANSFORMER_H__
#include "transformer_utils.h"
namespace hiai {
ge::GraphErrCodeStatus SelectConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus GatherV2DConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus ConcatDConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus OneHotConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus PadConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus SplitVConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus MirrorPadOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus ConcatDOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus GatherV2DOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus SplitDOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus DepthToSpaceOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus ReshapeOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus PadOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus SplitVOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus FlattenOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
ge::GraphErrCodeStatus StridedSliceV2OMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);
} // namespace hiai
#endif // __ARRAY_OP_TRANSFORMER_H__