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

#include "framework/graph/core/cgraph/graph_spec.h"

// inc/framework
#include "framework/graph/op/internal_defs.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/utils/checker/node_checker.h"
#include "framework/graph/utils/checker/graph_checker.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

namespace ge {
bool GraphSpec::IsEmpty() const
{
    return NodesNum() == 0;
}

std::size_t GraphSpec::NodesNum() const
{
    return ROLE(GraphStore).AllNodes().size();
}

std::size_t GraphSpec::InNodesNum() const
{
    std::size_t num = 0;
    for (const auto& node : ROLE(GraphStore).AllNodes()) {
        const std::string& type = node->ROLE(NodeSpec).Type();
        if (NodeChecker::IsGraphInputType(type)) {
            num++;
        }
    }
    return num;
}

std::size_t GraphSpec::OutNodesNum() const
{
    return ROLE(GraphStore).OutputNodes().size();
}

const std::string& GraphSpec::Name() const
{
    return ROLE(GraphStore).Name();
}

hiai::Status GraphSpec::SetName(std::string name)
{
    HIAI_EXPECT_TRUE(!name.empty());

    ROLE(GraphStore).SetName(std::move(name));
    return hiai::SUCCESS;
}

namespace {
class GraphInDegreeChecker {
public:
    explicit GraphInDegreeChecker(const std::vector<NodePtr>& nodes) : nodes_(nodes)
    {
    }
    ~GraphInDegreeChecker() = default;

    hiai::Status IsValid()
    {
        // check graph no cycle
        HIAI_EXPECT_TRUE(CheckGraphByInDegree(&GraphInDegreeChecker::ActualNodeInDegree));

        return hiai::SUCCESS;
    }

private:
    using NodeInDegreeFunc = std::size_t (GraphInDegreeChecker::*)(const Node&) const;

    std::size_t ExpectNodeInDegree(const Node& node) const
    {
        const NodeSpec& spec = node.ROLE(NodeSpec);
        return spec.InCtrlEdgeSize() + static_cast<unsigned int>(spec.OpDesc().GetInputsDescSize());
    }

    std::size_t ActualNodeInDegree(const Node& node) const
    {
        NodeSpec& spec = node.ROLE(NodeSpec);
        std::size_t size = spec.InEdgeSize();
        if ((size > 0) && (spec.Type() == hiai::op::NextIteration::TYPE)) {
            size--;
        }

        return size;
    }

    void CacheNodesInDegree(const NodeInDegreeFunc& func)
    {
        for (const auto& node : nodes_) {
            std::size_t size = (this->*func)(*node);
            if (size == 0) {
                inDegree_.noInDegreeNodes.push_back(node.get());
            } else {
                inDegree_.inDegreeMap.insert({node.get(), size});
            }
        }
    }

    hiai::Status RecurseOutNodes(const Node& node)
    {
        auto visitor = [this](const Node& outNode) {
            auto& inDegreeMap = inDegree_.inDegreeMap;
            const auto& it = inDegreeMap.find(&outNode);
            HIAI_EXPECT_TRUE(it != inDegreeMap.end());

            if (--it->second == 0) {
                inDegreeMap.erase(it);
                RecurseOutNodes(outNode);
            }
            return hiai::SUCCESS;
        };
        return node.ROLE(NodeWalker).ListOutNodes(std::move(visitor));
    }

    bool CheckNodesInDegree()
    {
        auto& noInDegreeNodes = inDegree_.noInDegreeNodes;
        while (!noInDegreeNodes.empty()) {
            const Node& node = *noInDegreeNodes.back();
            noInDegreeNodes.pop_back();
            HIAI_EXPECT_EXEC(RecurseOutNodes(node));
        }

        return inDegree_.inDegreeMap.empty();
    }

    bool CheckGraphByInDegree(const NodeInDegreeFunc& func)
    {
        CacheNodesInDegree(func);

        return CheckNodesInDegree();
    }

private:
    struct NodesInDegreeInfo {
        std::vector<const Node*> noInDegreeNodes;
        std::map<const Node*, std::size_t> inDegreeMap;
    };

private:
    const std::vector<NodePtr>& nodes_;
    NodesInDegreeInfo inDegree_ {};
};

hiai::Status CheckNodesValid(GraphListWalker& walker)
{
    return walker.WalkAllNodes([](Node& node) { return node.ROLE(NodeSpec).IsValid(); });
}

bool IsBeginWithData(const std::vector<NodePtr>& nodes)
{
    if (!nodes.empty()) {
        const Node& node = *nodes.front();
        const std::string& type = node.ROLE(NodeSpec).Type();
        return NodeChecker::IsGraphInputType(type);
    }

    return true;
}
} // namespace

hiai::Status GraphSpec::IsValid() const
{
    GraphListWalker& walker = ROLE(GraphListWalker);
    HIAI_EXPECT_EXEC(CheckNodesValid(walker));

    const auto& nodes = ROLE(GraphStore).AllNodes();
    GraphInDegreeChecker inDegreechecker(nodes);
    HIAI_EXPECT_EXEC(inDegreechecker.IsValid());

    HIAI_EXPECT_TRUE(IsBeginWithData(nodes));
    HIAI_EXPECT_TRUE(GraphChecker::IsInputsFullyLinked(ROLE(ComputeGraph)));

    return hiai::SUCCESS;
}
} // namespace ge
