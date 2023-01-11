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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_BYPASSER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_BYPASSER_H

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"

namespace ge {
class Node;
class GraphStore;
class GraphModifier;

EXPORT_ROLE(GraphBypasser)
{
public:
    bool PreCheck(const Node& node);
    hiai::Status ByPassNode(const Node& node);

private:
    USE_ROLE(GraphStore);
    USE_ROLE(GraphModifier);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_BYPASSER_H