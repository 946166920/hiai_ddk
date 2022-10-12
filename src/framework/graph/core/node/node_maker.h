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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_MAKE_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_MAKE_H

// inc/framework
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node_fwd.h"

namespace ge {
int64_t NewNodeId();

class NodeMaker {
public:
    static NodePtr Make(const OpDescPtr& op, const ComputeGraphPtr& ownerGraph);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_MAKE_H
