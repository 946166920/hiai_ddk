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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_SPEC_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_SPEC_H

#include <cstddef>
#include <string>
#include <vector>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node_fwd.h"

namespace ge {
class NodeStore;
class Endpoint;

DEFINE_ROLE(NodeSpec)
{
public:
    GRAPH_API_EXPORT ComputeGraph& OwnerComputeGraph() const;
    GRAPH_API_EXPORT ComputeGraphPtr OwnerComputeGraphPtr() const;
    GRAPH_API_EXPORT void SetOwnerComputeGraph(const ComputeGraphPtr& graph);

    GRAPH_API_EXPORT const class OpDesc& OpDesc() const;
    GRAPH_API_EXPORT class OpDesc& OpDesc();

    GRAPH_API_EXPORT const std::string& Name() const;
    GRAPH_API_EXPORT const std::string& Type() const;
    GRAPH_API_EXPORT int64_t Id() const;

    GRAPH_API_EXPORT bool HasEdge() const;

    GRAPH_API_EXPORT std::size_t InDataEdgeSize() const;
    GRAPH_API_EXPORT std::size_t InNonConstSize() const;
    GRAPH_API_EXPORT std::size_t InCtrlEdgeSize() const;
    GRAPH_API_EXPORT std::size_t InEdgeSize() const;

    GRAPH_API_EXPORT std::size_t OutDataEdgeSize() const;
    GRAPH_API_EXPORT std::size_t OutCtrlEdgeSize() const;
    GRAPH_API_EXPORT std::size_t OutEdgeSize() const;

    GRAPH_API_EXPORT bool IsInputFullyLinked() const;

    GRAPH_API_EXPORT hiai::Status IsValid() const;

    GRAPH_API_EXPORT std::vector<Endpoint> IdleInputEndpoints() const;
    GRAPH_API_EXPORT std::vector<Endpoint> IdleOutputEndpoints() const;
    GRAPH_API_EXPORT std::vector<Node*> ConstInputs() const;

    GRAPH_API_EXPORT bool IsDataOp() const;
    GRAPH_API_EXPORT bool IsConstOp() const;
    GRAPH_API_EXPORT bool IsAippConfig() const;
    GRAPH_API_EXPORT bool IsNetOutputOp() const;
    GRAPH_API_EXPORT bool IsGraphOp() const;

private:
    bool IsInOutAnchorValid() const;

private:
    USE_ROLE(NodeStore);
    USE_ROLE(Node);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_SPEC_H
