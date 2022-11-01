/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the node commom operation class
 * @file node_utils.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */

#include "framework/graph/utils/node_utils.h"

#include <algorithm>

// inc/framework
#include "framework/graph/core/node/node.h"

// api/framework
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
} // namespace ge