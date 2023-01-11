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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_STORE_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_STORE_H

#include <vector>
#include <map>
#include <string>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/node/node_pred.h"
#include "framework/graph/core/edge/edge_fwd.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/node/quick_query_nodes.h"

namespace hiai {
class IGraphDef;
class IAttrMapDef;
}

namespace ge {
class GraphNotifier;
class Edge;

class GraphStore {
public:
    const std::string& Name() const;
    void SetName(std::string name);

    std::vector<NodePtr>& AllNodes() const;
    std::vector<NodePtr>& InputNodes();
    std::vector<NodePtr>& OutputNodes();
    std::map<std::string, uint32_t>& InputOrder();

    bool HasNode(const Node& node);
    bool HasEdge(const Edge& edge);

    void AddNode(const NodePtr& node);
    void AddNodeFront(const NodePtr& node);

    hiai::Status RemoveNode(const Node& node);
    void RemoveNodes(const NodePred& pred, const NodeAction& preAction);
    hiai::Status RemoveOutputNode(const Node& node);
    NodePtr MoveNode(const Node& node);

    hiai::Status UpdateNodes(const std::vector<Node*>& nodes);
    hiai::Status SetInputs(const std::vector<Node*>& nodes);
    hiai::Status SetOutputs(const std::vector<Node*>& nodes);

    hiai::IGraphDef* GraphDef();
    const Node* OwnerNode() const;
    void SetOwnerNode(Node* node);

protected:
    explicit GraphStore(std::string&& name);
    GraphStore(hiai::IGraphDef* graphDef, bool isOwner);
    virtual ~GraphStore();

private:
    USE_ROLE(GraphNotifier);

private:
    hiai::IGraphDef* graphDef_;
    bool isOwner_;
    QuickQueryNodes nodes_;
    std::vector<NodePtr> inputNodes_;
    std::vector<NodePtr> outputNodes_;
    std::map<std::string, uint32_t> inputOrder_;
    Node* ownerNode_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_STORE_H