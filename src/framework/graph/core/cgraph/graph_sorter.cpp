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

#include "framework/graph/core/cgraph/graph_sorter.h"

#include <unordered_map>
#include <algorithm>

// inc/framework
#include "framework/graph/op/internal_defs.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_functor.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/utils/checker/node_checker.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

namespace ge {
namespace {
class GraphDfsSorter : public NodeFunctor {
public:
    GraphDfsSorter(GraphStore& store, GraphListWalker& walker) : store_(store), walker_(walker)
    {
    }
    ~GraphDfsSorter() = default;

    hiai::Status Sort()
    {
        HIAI_EXPECT_EXEC(PrepareNodes());

        HIAI_EXPECT_EXEC(SortDfsNodes());

        HIAI_EXPECT_EXEC(WriteBackNodes());

        return hiai::SUCCESS;
    }

private:
    void SaveNoEntryNode(Node& node)
    {
        if (NodeChecker::IsNonInputType(node.ROLE(NodeSpec).Type())) {
            dataIter_ = noEntryNodes_.insert(dataIter_, &node);
        } else {
            noEntryNodes_.push_front(&node);
        }

        return;
    }

    hiai::Status Visit(Node& node) override
    {
        std::size_t inEdgeSize = node.ROLE(NodeSpec).InEdgeSize();
        if ((node.ROLE(NodeSpec).Type() == hiai::op::NextIteration::TYPE) && (inEdgeSize > 0)) {
            inEdgeSize--;
        }

        nodeEdgeMap_[&node] = inEdgeSize;

        if (inEdgeSize == 0) {
            SaveNoEntryNode(node);
        }

        return hiai::SUCCESS;
    }

    hiai::Status PrepareNodeInEdge()
    {
        (void)walker_.WalkAllNodes(ToVisitor());

        return hiai::SUCCESS;
    }

    hiai::Status PrepareNoEntryNodes()
    {
        GraphSorter::StableSort(
            noEntryNodes_, store_.InputOrder(), [](uint32_t idxI, uint32_t idxJ) { return idxI > idxJ; });

        return hiai::SUCCESS;
    }

    hiai::Status PrepareNodes()
    {
        HIAI_EXPECT_EXEC(PrepareNodeInEdge());
        HIAI_EXPECT_EXEC(PrepareNoEntryNodes());

        return hiai::SUCCESS;
    }

    hiai::Status WalkNodesByDfs()
    {
        auto& noEntryNodes = noEntryNodes_;
        auto& nodeEdgeMap = nodeEdgeMap_;

        while (!noEntryNodes.empty()) {
            Node& node = *noEntryNodes.back();
            noEntryNodes.pop_back();
            dfsNodes_.emplace_back(&node);

            auto visitor = [&nodeEdgeMap, &noEntryNodes](Edge& edge) {
                Node& dstNode = edge.DstNode();
                const auto& iter = nodeEdgeMap.find(&dstNode);
                if (iter == nodeEdgeMap.end()) {
                    return hiai::SUCCESS;
                }
                iter->second--;
                if (iter->second == 0) {
                    noEntryNodes.emplace_back(&dstNode);
                }
                return hiai::SUCCESS;
            };
            (void)node.ROLE(NodeWalker).ListOutEdges(visitor);
        }

        return hiai::SUCCESS;
    }

    bool IsGraphNoCycle()
    {
        return dfsNodes_.size() == nodeEdgeMap_.size();
    }

    bool IsBeginWithData() const
    {
        if (!dfsNodes_.empty()) {
            const Node& node = *dfsNodes_.front();
            const std::string& type = node.ROLE(NodeSpec).Type();
            return NodeChecker::IsGraphInputType(type);
        }

        return true;
    }

    hiai::Status CheckGraphValid()
    {
        HIAI_EXPECT_TRUE(IsBeginWithData());
        HIAI_EXPECT_TRUE(IsGraphNoCycle());

        return hiai::SUCCESS;
    }

    hiai::Status SortDfsNodes()
    {
        HIAI_EXPECT_EXEC(WalkNodesByDfs());

        return CheckGraphValid();
    }

    hiai::Status WriteBackNodes()
    {
        return store_.UpdateNodes(dfsNodes_);
    }

private:
    GraphStore& store_;
    GraphListWalker& walker_;

private:
    std::list<Node*> noEntryNodes_ {};
    std::list<Node*>::iterator dataIter_ {noEntryNodes_.begin()};
    std::unordered_map<Node*, std::size_t> nodeEdgeMap_ {};
    std::vector<Node*> dfsNodes_ {};
};
} // namespace

hiai::Status GraphSorter::SortNodesDFS()
{
    GraphDfsSorter sorter(ROLE(GraphStore), ROLE(GraphListWalker));
    return sorter.Sort();
}

template <typename T>
std::function<bool(const T& left, const T& right)> SortByOrder(
    const std::map<std::string, uint32_t>& inputOrder, const Comparator& order)
{
    return [&](const T& left, const T& right) {
        const auto& iterI = inputOrder.find(left->ROLE(NodeSpec).Name());
        const auto& iterJ = inputOrder.find(right->ROLE(NodeSpec).Name());
        if ((iterI != inputOrder.end()) && (iterJ != inputOrder.end())) {
            return order(iterI->second, iterJ->second);
        }

        if (iterJ != inputOrder.end()) {
            return true;
        }

        return false;
    };
}

// make sure the inputs order matches with user-designated
// 1. Get the index of two input nodes in the user-inputs-order(inputs_order_)
// 2. Compare two indexs, if not match, swap the positions of two inputs
// *: Remind: stack is reverse-order
void GraphSorter::StableSort(
    std::vector<NodePtr>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order)
{
    std::stable_sort(nodes.begin(), nodes.end(), SortByOrder<NodePtr>(inputOrder, order));
}

void GraphSorter::StableSort(
    std::list<Node*>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order)
{
    nodes.sort(SortByOrder<Node*>(inputOrder, order));
}

void GraphSorter::StableSort(
    std::vector<Node*>& nodes, const std::map<std::string, uint32_t>& inputOrder, const Comparator& order)
{
    std::stable_sort(nodes.begin(), nodes.end(), SortByOrder<Node*>(inputOrder, order));
}
} // namespace ge
