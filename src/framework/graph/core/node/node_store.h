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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_STORE_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_STORE_H

#include <vector>

// inc/framework
#include "base/error_types.h"
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/edge/anchor_fwd.h"

namespace ge {
class NodeStore {
public:
    hiai::Status Init();

    ComputeGraphPtr OwnerComputeGraphPtr() const;
    void SetOwnerComputeGraph(const ComputeGraphPtr& graph);

    std::vector<InDataAnchorPtr>& InDataAnchors();
    std::vector<OutDataAnchorPtr>& OutDataAnchors();
    InControlAnchorPtr& InCtrlAnchor();
    OutControlAnchorPtr& OutCtrlAnchor();

    InDataAnchorPtr InDataAnchor(std::size_t idx) const;
    OutDataAnchorPtr OutDataAnchor(std::size_t idx) const;

    OpDescPtr& OpDesc();

    static hiai::Status AddEdge(NodeStore& src, int srcIdx, NodeStore& dst, int dstIdx);
    static hiai::Status RemoveEdge(NodeStore& src, int srcIdx, NodeStore& dst, int dstIdx);
    static hiai::Status RemoveEdge(const Node& src, const Node& dst);
    void RemoveEdges();

protected:
    NodeStore(const ComputeGraphPtr& graph, const OpDescPtr& op);
    virtual ~NodeStore();

private:
    void RemoveInEdges();
    void RemoveOutEdges();

private:
    // Note: USE_ROLE(Node) return Node&, but anchor init need NodePtr.
    // Delete the interface after shared_ptr cleared.
    virtual NodePtr GetNode() = 0;

private:
    std::weak_ptr<ComputeGraph> graph_;
    std::vector<InDataAnchorPtr> inDataAnchors_;
    std::vector<OutDataAnchorPtr> outDataAnchors_;
    InControlAnchorPtr inControlAnchor_;
    OutControlAnchorPtr outControlAnchor_;
    OpDescPtr op_;
    bool hasInit_ {false};
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_NODE_STORE_H