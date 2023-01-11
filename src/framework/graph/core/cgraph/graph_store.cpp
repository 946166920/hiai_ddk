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

#include "graph/core/cgraph/graph_store.h"

#include <algorithm>

// inc/framework
#include "framework/graph/core/cgraph/graph_notifier.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/op/op_desc.h"

// src/framework/inc
#include "infra/base/assertion.h"

#include "graph/persistance/proxy/proto_factory.h"
#include "graph/persistance/interface/graph_def.h"

#include "graph/persistance/interface/attr_map_def.h"

namespace ge {
const static string STR_EMPTY = "";

GraphStore::GraphStore(std::string&& name) : GraphStore(hiai::ProtoFactory::Instance()->CreateGraphDef(), true)
{
    if (graphDef_ != nullptr) {
        graphDef_->set_name(name);
    }
    ownerNode_ = nullptr;
}

GraphStore::GraphStore(hiai::IGraphDef* graphDef, bool isOwner)
    : graphDef_(graphDef), isOwner_(isOwner), ownerNode_(nullptr)
{
}

GraphStore::~GraphStore()
{
    if (isOwner_ && graphDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef_);
    }
    graphDef_ = nullptr;
}

const std::string& GraphStore::Name() const
{
    return graphDef_ != nullptr ? graphDef_->name() : STR_EMPTY;
}

void GraphStore::SetName(std::string name)
{
    if (graphDef_ != nullptr) {
        graphDef_->set_name(name);
    }
}

std::vector<NodePtr>& GraphStore::AllNodes() const
{
    return const_cast<std::vector<NodePtr>&>(nodes_.Nodes());
}

std::vector<NodePtr>& GraphStore::InputNodes()
{
    return inputNodes_;
}

std::vector<NodePtr>& GraphStore::OutputNodes()
{
    return outputNodes_;
}

std::map<std::string, uint32_t>& GraphStore::InputOrder()
{
    return inputOrder_;
}

bool GraphStore::HasNode(const Node& node)
{
    return nodes_.HasNode(node);
}

namespace {
bool HasDataEdge(const Node& dst, int idx)
{
    return dst.ROLE(NodeWalker).InDataEdge(idx).Exist();
}

bool HasCtrlEdge(const Node& src, const Node& dst)
{
    hiai::Status ret = src.ROLE(NodeWalker).ListOutCtrlNodes([&dst](Node& node) {
        return &node == &dst ? hiai::COMM_EXCEPTION : hiai::SUCCESS;
    });

    return ret == hiai::COMM_EXCEPTION;
}

bool HasEdgeImpl(const Edge& edge)
{
    if (edge.IsData()) {
        return HasDataEdge(edge.DstNode(), edge.SrcIdx());
    } else if (edge.IsCtrl()) {
        return HasCtrlEdge(edge.SrcNode(), edge.DstNode());
    } else {
        return false;
    }
}
} // namespace

bool GraphStore::HasEdge(const Edge& edge)
{
    const Node& srcNode = edge.SrcNode();
    HIAI_EXPECT_TRUE_R(HasNode(srcNode), false);
    const Node& dstNode = edge.DstNode();
    HIAI_EXPECT_TRUE_R(HasNode(dstNode), false);

    return HasEdgeImpl(edge);
}

void GraphStore::AddNode(const NodePtr& node)
{
    nodes_.AddNode(node);
    ROLE(GraphNotifier).AddNode(*node);
}

void GraphStore::AddNodeFront(const NodePtr& node)
{
    nodes_.AddNodeFront(node);
    ROLE(GraphNotifier).AddNode(*node);
}

namespace {
hiai::Status EraseNode(std::vector<NodePtr>& nodes, const Node& node)
{
    std::sort(nodes.begin(), nodes.end());
    nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    const auto& it = std::find_if(nodes.cbegin(), nodes.cend(), [&node](const NodePtr& p) {
        return p.get() == &node;
    });

    if (it != nodes.end()) {
        nodes.erase(it);
        return hiai::SUCCESS;
    } else {
        return hiai::FILE_NOT_EXIST;
    }
}
} // namespace

hiai::Status GraphStore::RemoveNode(const Node& node)
{
    (void)EraseNode(inputNodes_, node);
    (void)EraseNode(outputNodes_, node);
    HIAI_EXPECT_EXEC_R(nodes_.DelNode(node), hiai::FILE_NOT_EXIST);

    ROLE(GraphNotifier).DelNode(node);
    return hiai::SUCCESS;
}

namespace {
void EraseNodes(std::vector<NodePtr>& nodes, const NodePred& pred)
{
    for (auto it = nodes.begin(); it != nodes.end();) {
        Node& node = **it;
        it = pred(node) ? nodes.erase(it) : (it + 1);
    }
}
} // namespace

void GraphStore::RemoveNodes(const NodePred& pred, const NodeAction& preAction)
{
    EraseNodes(InputNodes(), pred);
    EraseNodes(OutputNodes(), pred);

    nodes_.DelNodes(pred, preAction);
}

hiai::Status GraphStore::RemoveOutputNode(const Node& node)
{
    HIAI_EXPECT_EXEC_R(EraseNode(outputNodes_, node), hiai::FILE_NOT_EXIST);
    return hiai::SUCCESS;
}

hiai::Status GraphStore::UpdateNodes(const std::vector<Node*>& nodes)
{
    HIAI_EXPECT_EXEC(nodes_.UpdateNodes(nodes));
    return hiai::SUCCESS;
}

namespace {
hiai::Status SetNodes(std::vector<NodePtr>& nodes, const std::vector<Node*>& order, QuickQueryNodes& querier)
{
    nodes.clear();

    for (const auto& node : order) {
        const NodePtr& p = querier.FindNodePtr(*node);
        if (p == nullptr) {
            return hiai::FILE_NOT_EXIST;
        }

        nodes.push_back(p);
    }

    return hiai::SUCCESS;
}
} // namespace

hiai::Status GraphStore::SetInputs(const std::vector<Node*>& nodes)
{
    HIAI_EXPECT_EXEC(SetNodes(inputNodes_, nodes, nodes_));
    return hiai::SUCCESS;
}

hiai::Status GraphStore::SetOutputs(const std::vector<Node*>& nodes)
{
    HIAI_EXPECT_EXEC(SetNodes(outputNodes_, nodes, nodes_));
    return hiai::SUCCESS;
}

namespace {
NodePtr FindNode(const std::vector<NodePtr>& nodes, const Node& node)
{
    auto pred = [&node](const NodePtr& ptr) { return ptr.get() == &node; };

    auto iter = std::find_if(nodes.begin(), nodes.end(), pred);
    if (iter != nodes.end()) {
        return *iter;
    }

    return nullptr;
}
} // namespace

NodePtr GraphStore::MoveNode(const Node& node)
{
    HIAI_EXPECT_TRUE_R(HasNode(node), nullptr);

    auto result = FindNode(nodes_.Nodes(), node);
    if (result != nullptr) {
        RemoveNode(node);
    }

    return result;
}

hiai::IGraphDef* GraphStore::GraphDef()
{
    return graphDef_;
}

const Node* GraphStore::OwnerNode() const
{
    return ownerNode_;
}

void GraphStore::SetOwnerNode(Node* node)
{
    ownerNode_ = node;
}
} // namespace ge