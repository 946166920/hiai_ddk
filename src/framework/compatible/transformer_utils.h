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

#ifndef __TRANSFORMER_UTILS_H__
#define __TRANSFORMER_UTILS_H__

#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/utils/op_desc_utils.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/tensor_utils.h"
#include "framework/graph/debug/ge_log.h"

namespace hiai {
const std::string OP_VERSION = "version";
const int VESION_VALUE_DEFAULT = 0;
const int VESION_VALUE_THREE = 3;
const int VESION_VALUE_FIVE = 5;

const std::string ATTR_ALGO = "algo";
const std::string ATTR_MODE = "mode";
const std::string ATTR_AXIS = "axis";
const std::string INPUT_NUM = "N";

const std::string INTERP_HEIGHT = "height";
const std::string INTERP_WIDTH = "width";

struct AttrInfo {
    std::string srcName; // old ir attr name
    std::string dstName; // new ir attr name
    std::string srcDefValue;
    std::string dstDefValue;
};

struct ConvertConfigInfo {
    // 转换算子type类型
    std::string dstOpType;
    // 是否老转新
    bool isOldToNew;
    // 属性信息
    std::vector<AttrInfo> attrInfos;
};

ge::GraphErrCodeStatus DataFormatToEnum(ge::OpDesc& desc);

ge::GraphErrCodeStatus TransformTypeConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus TypeAndAttrConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus SetIntAttrDefValueConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus UpdateAttrConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus EmptyInputNodeRemoveForOMConverter(ge::Node& node, const ConvertConfigInfo& config,
    bool isOldToNew);

ge::GraphErrCodeStatus ConstantOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew);

ge::GraphErrCodeStatus SplitGraphMergedWeight(ge::ComputeGraph& graph);
} // namespace hiai
#endif // __TRANSFORMER_UTILS_H__