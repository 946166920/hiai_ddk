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