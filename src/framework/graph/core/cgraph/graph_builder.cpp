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

#include "framework/graph/core/cgraph/graph_builder.h"

#include <map>
#include <unordered_map>

// api/framework
#include "graph/graph.h"
#include "graph/graph_impl.h"
#include "graph/operator_impl.h"
#include "graph/op/control_flow_defs.h"

// inc/framework
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/edge/endpoint.h"
#include "framework/graph/op/control_flow_attr_defs.h"
#include "framework/graph/utils/checker/node_checker.h"
#include "framework/graph/op/internal_defs.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
class SubGraphBuilder {
public:
    static hiai::Status Build(Node& node, const OperatorImpl& opImpl)
    {
        std::vector<std::string> subGraphNames;
        HIAI_EXPECT_EXEC(GetNodeSubGraphAttrs(node, subGraphNames));

        for (auto it = subGraphNames.cbegin(); it != subGraphNames.cend(); it++) {
            GraphBuilderFn func = opImpl.GetGraphBuilder(*it);
            if (func != nullptr) {
                HIAI_EXPECT_EXEC(BuildNodeSubGraph(node, func, *it));
            }
        }

        return hiai::SUCCESS;
    }

private:
    using FuncAttrs = std::vector<std::string>;
    using OpFuncAttrs = std::pair<std::string, FuncAttrs>;

    static hiai::Status GetNodeSubGraphAttrs(const Node& node, std::vector<std::string>& subGraphNames)
    {
        static const std::vector<OpFuncAttrs> attrMap = {
            {{hiai::op::If::TYPE}, {hiai::ATTR_NAME_THEN_BRANCH, hiai::ATTR_NAME_ELSE_BRANCH}},
            {{hiai::op::While::TYPE}, {hiai::ATTR_NAME_BODY, hiai::ATTR_NAME_COND}}};

        const auto& type = node.ROLE(NodeSpec).Type();
        for (const auto& it : attrMap) {
            if (it.first == type) {
                for (const std::string& attr : it.second) {
                    std::string name = GetNodeSubGraphName(node, attr);
                    HIAI_EXPECT_TRUE(!name.empty());
                    subGraphNames.push_back(name);
                }
            }
        }

        if (type == hiai::op::Case::TYPE) {
            AttrValue attrValue;
            HIAI_EXPECT_EXEC(node.ROLE(NodeSpec).OpDesc().GetAttr(hiai::op::Case::GRAPH_NAME_BRANCHES, attrValue));
            for (const std::string& subGraphName : attrValue.GetStringList()) {
                subGraphNames.push_back(subGraphName);
            }
        }

        return hiai::SUCCESS;
    }

    static std::string GetNodeSubGraphName(const Node& node, const std::string& attr)
    {
        AttrValue attrValue;
        HIAI_EXPECT_EXEC_R(node.ROLE(NodeSpec).OpDesc().GetAttr(attr, attrValue), "");

        return attrValue.GetString();
    }

    static hiai::Status BuildNodeSubGraph(Node& node, GraphBuilderFn& func, const std::string& name)
    {
        Graph graph(name);
        func(graph);

        ComputeGraphPtr subComputeGraph = graph.GetImpl()->GetComputeGraph();
        return node.ROLE(NodeSubGraph).AddSubGraph(subComputeGraph);
    }
};
} // namespace

namespace {
class ComputeGraphBuilder {
public:
    ComputeGraphBuilder(std::vector<Operator>& inputs, ComputeGraphPtr& graph)
        : inputs_(inputs), graph_(graph), modifier_(graph->ROLE(GraphModifier))
    {
    }
    ~ComputeGraphBuilder() = default;

    static hiai::Status PreCheckPara(std::vector<Operator>& inputs)
    {
        HIAI_EXPECT_TRUE(!inputs.empty());

        for (const auto& input : inputs) {
            HIAI_EXPECT_TRUE(IsOperatorNotNull(input));
            HIAI_EXPECT_TRUE(IsOperatorDataOp(input));
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildGraph()
    {
        HIAI_EXPECT_EXEC(BuildGraphNodes());

        HIAI_EXPECT_EXEC(BuildGraphEdges());

        HIAI_EXPECT_EXEC(BuildInputOrder());

        // 结束之后应该做图检查：1.无环 2.InAnchor都有对端
        // 是否应该做Topo排序呢？Topo内部会检查上面2个条件。
        // 个人感觉不需要做Topo排序，只检查图Valid即可，Topo排序由需要的业务去做。
        HIAI_EXPECT_TRUE(HasExecutableNodes());

        return hiai::SUCCESS;
    }

private:
    static bool IsOperatorNotNull(const Operator& op)
    {
        const auto& impl = op.GetImpl();
        HIAI_EXPECT_NOT_NULL_R(impl, false);

        const auto& desc = impl->GetOpDesc();
        HIAI_EXPECT_NOT_NULL_R(desc, false);

        return true;
    }

    static bool IsOperatorDataOp(const Operator& op)
    {
        const std::string& type = op.GetImpl()->GetOpDesc()->GetType();
        return NodeChecker::IsGraphInputType(type);
    }

    bool IsOpAdded(const OperatorImpl& op) const
    {
        return nodesMap_.find(&op) != nodesMap_.end();
    }

    bool IsOpNodeOutput(const OperatorImpl& op) const
    {
        return op.GetOpDesc()->GetName() == "Node_Output";
    }

    hiai::Status AddNodeByOp(const OperatorImpl& op)
    {
        Node* node = modifier_.AddNode(op.GetOpDesc());
        HIAI_EXPECT_NOT_NULL(node);

        HIAI_EXPECT_EXEC(SubGraphBuilder::Build(*node, op));

        nodesMap_.insert(std::make_pair(&op, node));

        return hiai::SUCCESS;
    }

    hiai::Status BuildInNode(const OperatorImpl& op)
    {
        for (const auto& inLink : op.GetAllInputLinks()) {
            const OpAnchor& inOp = inLink.second;
            const auto& inOpImpl = inOp.first.lock();
            HIAI_EXPECT_NOT_NULL(inOpImpl);
            HIAI_EXPECT_EXEC(BuildNode(*inOpImpl));
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildOutNode(const OperatorImpl& op)
    {
        for (const auto& outLink : op.GetAllOutputLinks()) {
            for (const auto& out : outLink.second) {
                const auto& outOpImpl = out.first.lock();
                HIAI_EXPECT_NOT_NULL(outOpImpl);
                HIAI_EXPECT_EXEC(BuildNode(*outOpImpl));
            }
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildNode(const OperatorImpl& op)
    {
        if (!IsOpAdded(op)) {
            HIAI_EXPECT_TRUE(!IsOpNodeOutput(op));

            HIAI_EXPECT_EXEC(AddNodeByOp(op));

            HIAI_EXPECT_EXEC(BuildOutNode(op));
            HIAI_EXPECT_EXEC(BuildInNode(op));
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildGraphNodes()
    {
        for (const auto& op : inputs_) {
            HIAI_EXPECT_EXEC(BuildNode(*op.GetImpl()));
        }

        return hiai::SUCCESS;
    }

    hiai::Status AddEdge(Node& srcNode, int srcIdx, Node& dstNode, int dstIdx)
    {
        if (dstIdx == -1) {
            HIAI_EXPECT_EXEC(modifier_.AddEdge({srcNode, -1}, {dstNode, -1}));
        } else {
            HIAI_EXPECT_EXEC(modifier_.AddEdge({srcNode, srcIdx}, {dstNode, dstIdx}));
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildNodeEdges(Node& srcNode, const OperatorImpl& srcImpl)
    {
        for (const auto& outInfo : srcImpl.GetAllOutputLinks()) {
            int srcIdx = srcImpl.GetOpDesc()->GetOutputIndexByName(outInfo.first);
            HIAI_EXPECT_TRUE(srcIdx >= 0);

            for (const OpAnchor& dstOpAnchor : outInfo.second) {
                const OperatorImpl* dstImpl = dstOpAnchor.first.lock().get();
                const auto& dstInfo = nodesMap_.find(dstImpl);
                HIAI_EXPECT_TRUE(dstInfo != nodesMap_.end());

                Node& dstNode = *dstInfo->second;
                int32_t dstIdx = dstOpAnchor.second;

                HIAI_EXPECT_EXEC(AddEdge(srcNode, srcIdx, dstNode, dstIdx));
            }
        }

        return hiai::SUCCESS;
    }

    hiai::Status BuildGraphEdges()
    {
        for (const auto& srcInfo : nodesMap_) {
            const OperatorImpl& srcImpl = *srcInfo.first;
            Node& srcNode = *srcInfo.second;

            HIAI_EXPECT_EXEC(BuildNodeEdges(srcNode, srcImpl));
        }
        return hiai::SUCCESS;
    }

    hiai::Status BuildInputOrder()
    {
        std::map<std::string, uint32_t> order;

        for (std::size_t i = 0; i < inputs_.size(); ++i) {
            const auto& impl = inputs_[i].GetImpl();
            order[impl->GetOpDesc()->GetName()] = i;
        }

        return graph_->ROLE(GraphModifier).SetInputOrder(std::move(order));
    }

    bool HasExecutableNodes()
    {
        return inputs_.size() < nodesMap_.size();
    }

private:
    std::vector<Operator>& inputs_;
    ComputeGraphPtr graph_;
    GraphModifier& modifier_;
    std::unordered_map<const OperatorImpl*, Node*> nodesMap_ {};
};
} // namespace

ComputeGraphPtr GraphBuilder::Build(const std::string& name, std::vector<Operator>& inputs)
{
    HIAI_EXPECT_EXEC_R(ComputeGraphBuilder::PreCheckPara(inputs), nullptr);

    ComputeGraphPtr graph = ComputeGraph::Make(name);
    HIAI_EXPECT_NOT_NULL_R(graph, nullptr);

    ComputeGraphBuilder builder(inputs, graph);
    HIAI_EXPECT_EXEC_R(builder.BuildGraph(), nullptr);

    HIAI_EXPECT_EXEC_R(graph->ROLE(GraphSpec).IsValid(), nullptr);

    return graph;
}
} // namespace ge
