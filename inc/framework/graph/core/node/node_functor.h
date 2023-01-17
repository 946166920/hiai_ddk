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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H

#include <vector>
#include <string>

// api/framework
#include "graph/graph_api_export.h"

// inc/framework
#include "base/error_types.h"
#include "framework/graph/core/node/node_visitor.h"

namespace ge {
class GRAPH_API_EXPORT NodeFunctor {
public:
    NodeVisitor ToVisitor()
    {
        return [this](Node& node) { return Visit(node); };
    }

    static NodeVisitor Typed(std::vector<std::string> types, NodeVisitor visitor);

    virtual hiai::Status Visit(Node& node) = 0;
    virtual ~NodeFunctor() = default;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_FUNCTOR_H
