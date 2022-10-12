/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Notes: node modifier
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_MODIFIER_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_MODIFIER_H

// inc/api
#include "graph/graph_api_export.h"

// inc/framework
#include "base/error_types.h"
#include "framework/infra/base/dci.h"

namespace ge {
class NodeStore;

class GRAPH_API_EXPORT NodeModifier {
public:
    virtual ~NodeModifier() = default;

    hiai::Status DelLastEmptyInTensor();

private:
    USE_ROLE(NodeStore);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_MODIFIER_H