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

#include "framework/graph/utils/node_utils.h"

#include <algorithm>

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"

// api/framework
#include "graph/op/const_defs.h"
#include "graph/op/control_flow_defs.h"

// inc/framework
#include "framework/common/hcs_types.h"

namespace ge {
GraphErrCodeStatus NodeUtils::SetAllAnchorStatus(Node& node)
{
    node.anchorStatusUpdated_ = true;
    return GRAPH_SUCCESS;
}

bool NodeUtils::IsAnchorStatusSet(const Node& node)
{
    return node.anchorStatusUpdated_;
}

namespace {
const std::vector<std::string> WITH_SUB_GRAPH_TYPES {
    hiai::op::If::TYPE,
    hiai::op::While::TYPE,
    ge::GRAPH_OP_TYPE
};
} // namespace

bool NodeUtils::IsWithSubGraphType(const std::string& type)
{
    return std::find(WITH_SUB_GRAPH_TYPES.cbegin(), WITH_SUB_GRAPH_TYPES.cend(), type) != WITH_SUB_GRAPH_TYPES.cend();
}

bool NodeUtils::IsConstNode(const Node& node)
{
    return (node.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE) ||
        (node.ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE);
}
} // namespace ge