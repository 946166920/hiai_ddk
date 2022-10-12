/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: node const input
 */

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_CONST_INPUT_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_CONST_INPUT_H

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"

namespace ge {
class NodeWalker;
class NodeSpec;

DEFINE_ROLE(NodeConstInput)
{
public:
    hiai::Status Update();

private:
    USE_ROLE(NodeWalker);
    USE_ROLE(NodeSpec);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_CONST_INPUT_H