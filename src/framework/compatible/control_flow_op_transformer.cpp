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
#include "control_flow_op_transformer.h"
#include <set>

#include "infra/base/assertion.h"

#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/edge/edge.h"
#include "graph/op/array_defs.h"
#include "graph/op/control_flow_defs.h"

namespace hiai {
static ge::GraphErrCodeStatus CreatNewData4SubGraphNewToOld(
    std::map<int64_t, ge::Node*>& inputNodeMaps, ge::ComputeGraphPtr subGraph)
{
    // 创建新的多输入、多输出Data节点
    ge::OpDescPtr dataOpDesc =
        std::shared_ptr<ge::OpDesc>(new (std::nothrow) ge::OpDesc(string("data"), hiai::op::Data::TYPE));
    HIAI_EXPECT_NOT_NULL(dataOpDesc);

    for (size_t i = 0; i < inputNodeMaps.size(); i++) {
        auto& inputOpDesc = inputNodeMaps[i]->ROLE(NodeSpec).OpDesc();
        HIAI_EXPECT_EXEC(dataOpDesc->AddInputDesc(inputOpDesc.GetInputDesc(0)));
        HIAI_EXPECT_EXEC(dataOpDesc->AddOutputDesc(inputOpDesc.GetOutputDesc(0)));
    }

    ge::Node* dataNode = subGraph->ROLE(GraphModifier).AddNode(dataOpDesc);
    HIAI_EXPECT_NOT_NULL(dataNode);

    // 为新节点建立边关系，并删除原有单输入单输出节点
    for (size_t i = 0; i < inputNodeMaps.size(); i++) {
        // 添加边连接Arg和Data
        HIAI_EXPECT_EXEC(
            subGraph->ROLE(GraphModifier).AddEdge({*dataNode, static_cast<int>(i)}, {*inputNodeMaps[i], 0}));
        // 删除Arg节点
        HIAI_EXPECT_EXEC(subGraph->ROLE(GraphBypasser).ByPassNode(*inputNodeMaps[i]));
    }

    return subGraph->ROLE(GraphModifier).AddInput(*dataNode);
}
static ge::GraphErrCodeStatus ControlDataFlowNewToOldForSubGraph(ge::ComputeGraphPtr subGraph)
{
    // Data个数小于等于1时不需要优化,及时返回
    if (subGraph->ROLE(GraphSpec).InNodesNum() <= 1) {
        return ge::GRAPH_SUCCESS;
    }

    std::map<int64_t, ge::Node*> inputNodeMaps;
    int64_t maxIndex = 0;

    auto visitor = [&maxIndex, &inputNodeMaps](Node& node) {
        const std::string& type = node.ROLE(NodeSpec).Type();
        HIAI_EXPECT_TRUE(type == hiai::op::Data::TYPE);

        int64_t index = 0;
        HIAI_EXPECT_TRUE(ge::AttrUtils::GetInt(node.ROLE(NodeSpec).OpDesc(), hiai::op::Data::index, index));
        HIAI_EXPECT_TRUE(inputNodeMaps.count(index) == 0);
        inputNodeMaps[index] = &node;
        maxIndex = (index > maxIndex) ? index : maxIndex;
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(subGraph->ROLE(GraphListWalker).WalkInNodes(visitor));

    HIAI_EXPECT_TRUE(inputNodeMaps.size() == static_cast<size_t>(maxIndex + 1));

    return CreatNewData4SubGraphNewToOld(inputNodeMaps, subGraph);
}

static ge::GraphErrCodeStatus CreatNewData4SubGraphOldToNew(
    ge::ComputeGraphPtr subGraph, const string& subGraphName, ge::Node& orgData)
{
    vector<Edge> outEdges;
    std::set<int> srcIdxs;
    auto nodeVistor = [&outEdges, &srcIdxs](ge::Edge& edge) {
        outEdges.push_back(edge);
        srcIdxs.insert(edge.SrcIdx());
        return hiai::SUCCESS;
    };
    (void)orgData.ROLE(NodeWalker).ListOutDataEdges(nodeVistor);

    for (size_t i = 0; i < orgData.ROLE(NodeSpec).OpDesc().GetOutputsSize(); i++) {
        ge::OpDescPtr graphOpDataOpDesc = hiai::make_shared_nothrow<ge::OpDesc>();
        HIAI_EXPECT_NOT_NULL(graphOpDataOpDesc);

        graphOpDataOpDesc->SetName(subGraphName + "_data_" + std::to_string(i));
        graphOpDataOpDesc->SetType(hiai::op::Data::TYPE);
        graphOpDataOpDesc->SetAttr(
            hiai::op::Data::index, ge::AttrValue::CreateFrom(static_cast<ge::AttrValue::INT>(i)));

        auto inputTensorDesc = orgData.ROLE(NodeSpec).OpDesc().GetInputDesc(i);
        auto outputTensorDesc = orgData.ROLE(NodeSpec).OpDesc().GetOutputDesc(i);
        graphOpDataOpDesc->AddInputDesc(inputTensorDesc);
        graphOpDataOpDesc->AddOutputDesc(outputTensorDesc);

        Node* newData = subGraph->ROLE(GraphModifier).AddNode(graphOpDataOpDesc);
        HIAI_EXPECT_NOT_NULL(newData);

        // remove org edges add add new edges
        for (const auto& e : outEdges) {
            if (e.SrcIdx() == static_cast<int>(i)) {
                HIAI_EXPECT_EXEC(subGraph->ROLE(GraphModifier).RemoveEdge(e));
                HIAI_EXPECT_EXEC(subGraph->ROLE(GraphModifier).AddEdge({*newData, 0}, e.Dst()));
            }
        }
    }
    // remove org DataNode
    return subGraph->ROLE(GraphModifier).RemoveNode(orgData);
}

static ge::GraphErrCodeStatus ControlDataFlowOldToNewForSubGraph(
    ge::ComputeGraphPtr subGraph, const string& subGraphName)
{
    auto visitor = [&subGraph, &subGraphName](Node& node) {
        auto& nodeSpec = node.ROLE(NodeSpec);
        if (nodeSpec.Type() == hiai::op::Data::TYPE && nodeSpec.OpDesc().GetOutputsSize() > 1) {
            return CreatNewData4SubGraphOldToNew(subGraph, subGraphName, node);
        }
        return hiai::SUCCESS;
    };
    return subGraph->ROLE(GraphListWalker).WalkInNodes(visitor);
}

ge::GraphErrCodeStatus ControlDataFlowOMConverter(ge::Node& node, const ConvertConfigInfo& config, bool isOldToNew)
{
    (void)config;

    const std::string& opType = node.ROLE(NodeSpec).Type();
    vector<string> graphNameAttrs;
    if (opType == hiai::op::If::TYPE) {
        graphNameAttrs = {"then_branch", "else_branch"};
    } else if (opType == hiai::op::While::TYPE) {
        graphNameAttrs = {"cond", "body"};
    }

    ge::OpDesc& opDesc = node.ROLE(NodeSpec).OpDesc();

    for (const auto& graphNameAttr : graphNameAttrs) {
        string subGraphName = "";
        HIAI_EXPECT_TRUE(ge::AttrUtils::GetStr(opDesc, graphNameAttr, subGraphName));

        ge::ComputeGraphPtr subGraph = node.ROLE(NodeSubGraph).FindSubGraphPtr(subGraphName);
        HIAI_EXPECT_NOT_NULL(subGraph);

        if (subGraph->ROLE(GraphSpec).NodesNum() <= 1) {
            continue;
        }

        if (!isOldToNew) {
            if (ControlDataFlowNewToOldForSubGraph(subGraph) != ge::GRAPH_SUCCESS) {
                FMK_LOGE("control dataflow from new to old for subgraph failed.");
                return ge::GRAPH_FAILED;
            }
        } else {
            if (ControlDataFlowOldToNewForSubGraph(subGraph, subGraphName) != ge::GRAPH_SUCCESS) {
                FMK_LOGE("control dataflow from old to new for subgraph failed.");
                return ge::GRAPH_FAILED;
            }
        }
    }
    return ge::GRAPH_SUCCESS;
}
} // namespace hiai
