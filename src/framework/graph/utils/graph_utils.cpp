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

#include "framework/graph/utils/graph_utils.h"

#include <algorithm>

#include "graph/debug/ge_error_codes.h"

#include "infra/base/securestl.h"

#include "infra/base/assertion.h"

#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_serializer.h"
#include "framework/graph/core/node/node_visitor.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/debug/ge_log.h"
#include "framework/graph/debug/ge_util.h"
#include "graph/persistance/proxy/proto_factory.h"
#include "graph/persistance/interface/graph_def.h"

#include "graph/graph_impl.h"

namespace ge {
GraphErrCodeStatus GraphUtils::AddEdge(OutDataAnchorPtr src, InDataAnchorPtr dst)
{
    if (src != nullptr && src->LinkTo(dst) == GRAPH_SUCCESS) {
        return GRAPH_SUCCESS;
    }
    FMK_LOGE("Add edge Failed.");
    return GRAPH_FAILED;
}

GraphErrCodeStatus GraphUtils::AddEdge(OutDataAnchorPtr src, Format srcFormat, InDataAnchorPtr dst, Format dstFormat)
{
    if (src != nullptr && src->LinkTo(dst) == GRAPH_SUCCESS) {
        AnchorUtils::SetFormat(src, srcFormat);
        AnchorUtils::SetFormat(dst, dstFormat);
        return GRAPH_SUCCESS;
    }
    FMK_LOGE("Add edge Failed.");
    return GRAPH_FAILED;
}

GraphErrCodeStatus GraphUtils::AddEdge(OutControlAnchorPtr src, InControlAnchorPtr dst)
{
    if (src != nullptr && src->LinkTo(dst) == GRAPH_SUCCESS) {
        return GRAPH_SUCCESS;
    }
    FMK_LOGE("Add edge Failed.");
    return GRAPH_FAILED;
}

GraphErrCodeStatus GraphUtils::RemoveEdge(OutDataAnchorPtr src, InDataAnchorPtr dst)
{
    if (src != nullptr && src->Unlink(dst) == GRAPH_SUCCESS) {
        return GRAPH_SUCCESS;
    }
    FMK_LOGE("Remove edge Failed.");
    return GRAPH_FAILED;
}

GraphErrCodeStatus GraphUtils::RemoveEdge(OutControlAnchorPtr src, InControlAnchorPtr dst)
{
    if (src != nullptr && src->Unlink(dst) == GRAPH_SUCCESS) {
        return GRAPH_SUCCESS;
    }
    FMK_LOGE("Remove edge Failed.");
    return GRAPH_FAILED;
}

GraphErrCodeStatus GraphUtils::InsertNodeBetweenDataAnchors(OutDataAnchorPtr src, InDataAnchorPtr dst, NodePtr newNode)
{
    GE_CHECK_NOTNULL(src);
    GE_CHECK_NOTNULL(dst);
    GE_CHECK_NOTNULL(newNode);

    InDataAnchorPtr nodeInAnchor = newNode->GetInDataAnchor(0);
    GE_CHK_BOOL_RET_STATUS(nodeInAnchor != nullptr, GRAPH_FAILED, "this node has not inDataAnchor");
    OutDataAnchorPtr nodeOutAnchor = newNode->GetOutDataAnchor(0);
    GE_CHK_BOOL_RET_STATUS(nodeOutAnchor != nullptr, GRAPH_FAILED, "this node has not outDataAnchor");
    GE_CHK_STATUS_RET(src->ReplacePeer(dst, nodeInAnchor, nodeOutAnchor), "ReplacePeer Failed");
    return GRAPH_SUCCESS;
}

// 给新的节点添加两条边，分别连接原来的边关联的src和dst
// A ---> B 变成 A ---> N ---> B
GraphErrCodeStatus GraphUtils::InsertTransNode(
    ComputeGraph& computeGraph, InDataAnchorPtr in_data_anchor, std::vector<OpDescPtr> vecOpDesc)
{
    if (in_data_anchor == nullptr) {
        FMK_LOGE("The anchor ptr should be not null.");
        return GRAPH_FAILED;
    }
    for (const auto& op_desc : vecOpDesc) {
        GE_CHECK_NOTNULL(op_desc);

        if (op_desc->GetInputsDescSize() == 0 && op_desc->GetOutputsSize() == 0) {
            ge::Shape tensorShape;
            op_desc->AddInputDesc(TensorDesc(tensorShape, FORMAT_ND));
            op_desc->AddOutputDesc(TensorDesc(tensorShape, FORMAT_ND));
        }

        Node* node_to_insert = computeGraph.ROLE(GraphModifier).AddNode(op_desc);
        GE_CHECK_NOTNULL(node_to_insert);
        GE_CHECK_NOTNULL(in_data_anchor->GetPeerOutAnchor());

        auto src = in_data_anchor->GetPeerOutAnchor()->GetOwnerNode();
        GE_CHECK_NOTNULL(src);
        auto src_out_index = in_data_anchor->GetPeerOutAnchor()->GetIdx();

        auto dst = in_data_anchor->GetOwnerNode();
        GE_CHECK_NOTNULL(dst);
        auto dst_in_index = in_data_anchor->GetIdx();

        auto in_data_anchor_src_format = AnchorUtils::GetFormat(in_data_anchor->GetPeerOutAnchor());
        auto in_data_anchor_dst_format = AnchorUtils::GetFormat(in_data_anchor);

        (void)RemoveEdge(src->GetOutDataAnchor(src_out_index), dst->GetInDataAnchor(dst_in_index));

        GE_CHECK_NOTNULL(node_to_insert->GetInDataAnchor(0));
        GE_CHECK_NOTNULL(node_to_insert->GetOutDataAnchor(0));

        GE_CHK_STATUS_RET(
            AddEdge(src->GetOutDataAnchor(src_out_index), node_to_insert->GetInDataAnchor(0)), "Add edge fail.");
        GE_CHK_STATUS_RET(
            AddEdge(node_to_insert->GetOutDataAnchor(0), dst->GetInDataAnchor(dst_in_index)), "Add edge fail.");

        if (op_desc->HasAttr("input_format")) {
            int64_t input_format = 0;
            int64_t output_format = 0;
            if (!AttrUtils::GetInt(op_desc, "input_format", input_format)) {
                FMK_LOGW("get attr input_format failed");
            }
            if (!AttrUtils::GetInt(op_desc, "output_format", output_format)) {
                FMK_LOGW("get attr output_format failed");
            }

            AnchorUtils::SetFormat(node_to_insert->GetInDataAnchor(0)->GetPeerOutAnchor(), in_data_anchor_src_format);
            AnchorUtils::SetFormat(node_to_insert->GetInDataAnchor(0), static_cast<Format>(input_format));
            AnchorUtils::SetFormat(node_to_insert->GetOutDataAnchor(0), static_cast<Format>(output_format));
            AnchorUtils::SetFormat(
                node_to_insert->GetOutDataAnchor(0)->GetPeerInDataAnchors().at(0), in_data_anchor_dst_format);
        }
        std::vector<ge::NodePtr> originalNodes;
        RecordOriginalNames(originalNodes, *node_to_insert);
    }

    return GRAPH_SUCCESS;
}

// A ---> B 变成 A ---> N ---> B
GraphErrCodeStatus GraphUtils::InsertTransNode(
    ComputeGraph& computeGraph, OutDataAnchorPtr outDataAnchor, std::vector<OpDescPtr> vecOpDesc)
{
    GE_CHECK_NOTNULL(outDataAnchor);

    for (int i = static_cast<int>(vecOpDesc.size()) - 1; i >= 0; i--) {
        auto opDesc = vecOpDesc[i];
        GE_CHECK_NOTNULL(opDesc);

        if (opDesc->GetInputsDescSize() == 0 && opDesc->GetOutputsSize() == 0) {
            opDesc->AddInputDesc(TensorDesc());
            opDesc->AddOutputDesc(TensorDesc());
        }

        auto nodeToInsert = computeGraph.ROLE(GraphModifier).AddNode(opDesc);

        GE_CHECK_NOTNULL(nodeToInsert);

        auto src = outDataAnchor->GetOwnerNode();
        GE_CHECK_NOTNULL(src);
        auto srcOutIndex = outDataAnchor->GetIdx();
        auto inDataAnchorSrcFormat = AnchorUtils::GetFormat(outDataAnchor);

        for (const auto& peerInDataAnchor : outDataAnchor->GetPeerInDataAnchors()) {
            GE_CHECK_NOTNULL(peerInDataAnchor);
            auto dst = peerInDataAnchor->GetOwnerNode();
            GE_CHECK_NOTNULL(dst);
            auto dstInIndex = peerInDataAnchor->GetIdx();

            auto inDataAnchorDstFormat = AnchorUtils::GetFormat(peerInDataAnchor);

            (void)RemoveEdge(src->GetOutDataAnchor(srcOutIndex), dst->GetInDataAnchor(dstInIndex));

            GE_CHECK_NOTNULL(nodeToInsert->GetOutDataAnchor(0));
            GE_CHK_STATUS_RET(
                AddEdge(nodeToInsert->GetOutDataAnchor(0), dst->GetInDataAnchor(dstInIndex)), "Add edge fail.");

            if (opDesc->HasAttr("input_format")) {
                int64_t outputFormat = 0;
                (void)AttrUtils::GetInt(opDesc, "output_format", outputFormat);
                AnchorUtils::SetFormat(nodeToInsert->GetOutDataAnchor(0), static_cast<Format>(outputFormat));
                AnchorUtils::SetFormat(
                    nodeToInsert->GetOutDataAnchor(0)->GetPeerInDataAnchors().at(0), inDataAnchorDstFormat);
            }
        }

        GE_CHECK_NOTNULL(nodeToInsert->GetInDataAnchor(0));
        GE_CHK_STATUS_RET(
            AddEdge(src->GetOutDataAnchor(srcOutIndex), nodeToInsert->GetInDataAnchor(0)), "Add edge fail.");
        if (opDesc->HasAttr("input_format")) {
            int64_t inputFormat = 0;
            if (!AttrUtils::GetInt(opDesc, "input_format", inputFormat)) {
                FMK_LOGW("get attr input_format failed");
            }
            AnchorUtils::SetFormat(nodeToInsert->GetInDataAnchor(0)->GetPeerOutAnchor(), inDataAnchorSrcFormat);
            AnchorUtils::SetFormat(nodeToInsert->GetInDataAnchor(0), static_cast<Format>(inputFormat));
        }
    }

    return GRAPH_SUCCESS;
}

void GraphUtils::RecordOriginalNames(const std::vector<ge::NodePtr>& sharedOriginalNodes, const ge::Node& node)
{
    std::vector<ge::Node*> originalNodes;
    for (auto& nodePtr : sharedOriginalNodes) {
        originalNodes.push_back(nodePtr.get());
    }

    RecordOriginalNames(originalNodes, node);
}

void GraphUtils::RecordOriginalNames(const std::vector<ge::Node*>& originalNodes, const ge::Node& node)
{
    std::vector<std::string> originalNames;
    for (ge::Node* nodeTmp : originalNodes) {
        std::vector<std::string> namesTmp;
        GE_CHK_BOOL_EXEC(nodeTmp != nullptr, return, "nodeTmp is null.");
        ge::OpDesc& opdescTmp = nodeTmp->ROLE(NodeSpec).OpDesc();
        (void)ge::AttrUtils::GetListStr(opdescTmp, "original_op_names", namesTmp);
        if (namesTmp.size() != 0) {
            originalNames.insert(originalNames.cend(), namesTmp.cbegin(), namesTmp.cend());
        } else {
            originalNames.push_back(opdescTmp.GetName());
        }
    }
    if (originalNames.size() == 0) {
        std::string tmp;
        originalNames.push_back(tmp);
    }
    GE_CHK_BOOL_EXEC(ge::AttrUtils::SetListStr(node.ROLE(NodeSpec).OpDesc(), "original_op_names", originalNames),
        return, "Set original_op_names fail.");
}

void GraphUtils::RecordOriginalNames(std::vector<ge::NodePtr> originalNodes, ge::NodePtr node)
{
    GE_CHK_BOOL_EXEC(node != nullptr, return, "node is null.");
    RecordOriginalNames(originalNodes, *node);
}

void GraphUtils::RecordOriginalNames(std::vector<std::string> namesTmp, ge::NodePtr node)
{
    GE_CHK_BOOL_EXEC(node != nullptr, return, "node is null.");
    std::vector<std::string> originalNames;
    if (namesTmp.size() != 0) {
        originalNames.insert(originalNames.cend(), namesTmp.cbegin(), namesTmp.cend());
    } else {
        std::string tmp;
        originalNames.push_back(tmp);
    }
    GE_CHK_BOOL_EXEC(ge::AttrUtils::SetListStr(node->ROLE(NodeSpec).OpDesc(), "original_op_names", originalNames),
        return, "Set original_op_names fail.");
}

ComputeGraphPtr GraphUtils::GetComputeGraph(const ge::Graph& graph)
{
    GE_CHK_BOOL_EXEC_NOLOG(graph.IsValid(), return nullptr);
    return graph.GetImpl()->GetComputeGraph();
}

GraphErrCodeStatus GraphUtils::WalkAllNodeSubGraphs(const Node& node, GraphVisitor visitor)
{
    auto graphVisitor = [&visitor](ComputeGraphPtr& subGraph) {
        HIAI_EXPECT_EXEC(visitor(subGraph));
        return WalkAllSubGraphs(*subGraph, visitor);
    };
    return node.ROLE(NodeSubGraph).WalkSubGraphs(graphVisitor);
}

GraphErrCodeStatus GraphUtils::WalkAllSubGraphs(const ComputeGraph& graph, GraphVisitor visitor)
{
    auto nodeVisitor = [&visitor](ge::Node& node) {
        return WalkAllNodeSubGraphs(node, visitor);
    };

    return graph.ROLE(GraphListWalker).WalkAllNodes(nodeVisitor);
}

GraphErrCodeStatus GraphUtils::WalkAllNodeSubGraphNodes(const Node& node, NodeVisitor visitor)
{
    auto graphVisitor = [&visitor](ComputeGraphPtr& subGraph) {
        return WalkAllSubGraphNodes(*subGraph, visitor);
    };

    return node.ROLE(NodeSubGraph).WalkSubGraphs(graphVisitor);
}

GraphErrCodeStatus GraphUtils::WalkAllSubGraphNodes(const ComputeGraph& graph, NodeVisitor visitor)
{
    auto nodeVisitor = [&visitor](ge::Node& node) {
        HIAI_EXPECT_EXEC(visitor(node));
        return WalkAllNodeSubGraphNodes(node, visitor);
    };
    return graph.ROLE(GraphListWalker).WalkAllNodes(nodeVisitor);
}

Graph GraphUtils::CreateGraphFromComputeGraph(const ge::ComputeGraphPtr computeGraph)
{
    GE_CHK_BOOL_EXEC_NOLOG(computeGraph != nullptr, return Graph(""));
    GraphImplPtr graphImplPtr = hiai::make_shared_nothrow<GraphImpl>(computeGraph);
    GE_CHK_BOOL_EXEC_NOLOG(graphImplPtr != nullptr, return Graph(""));
    Graph graph(graphImplPtr);
    return graph;
}

ComputeGraphPtr GraphUtils::Clone(const ComputeGraphPtr graph)
{
    auto clonedDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    if (!graph->ROLE(GraphSerializer).SerializeTo(clonedDef)) {
        FMK_LOGE("serialize graph failed.");
        return nullptr;
    }
    auto clonedGraph = ComputeGraph::Make(clonedDef, true);
    if (clonedGraph == nullptr) {
        FMK_LOGE("make cloned graph failed.");
        return nullptr;
    }
    if (!clonedGraph->ROLE(GraphSerializer).UnSerialize()) {
        FMK_LOGE("unserialize graph failed.");
        return nullptr;
    }
    return clonedGraph;
}

void GraphUtils::Dump(const ComputeGraphPtr graph)
{
    for (const NodePtr& node : graph->GetDirectNodes()) {
        FMK_LOGD("node name %s, type %s.", node->ROLE(NodeSpec).Name().c_str(), node->ROLE(NodeSpec).Type().c_str());

        for (const InDataAnchorPtr& anchor : node->GetAllInDataAnchors()) {
            OutDataAnchorPtr peer_inAnchor = anchor->GetPeerOutAnchor();
            if (peer_inAnchor != nullptr) {
                FMK_LOGD("node name = %s, in data node name = %s %s.", node->ROLE(NodeSpec).Name().c_str(),
                    peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Name().c_str(),
                    peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Type().c_str());
            }
        }
        for (const OutControlAnchorPtr& peer_inAnchor : node->GetInControlAnchor()->GetPeerOutControlAnchors()) {
            (void)peer_inAnchor;
            FMK_LOGD("node name = %s, out control node name = %s %s.", node->ROLE(NodeSpec).Name().c_str(),
                peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Name().c_str(),
                peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Type().c_str());
        }

        for (const OutDataAnchorPtr& anchor : node->GetAllOutDataAnchors()) {
            for (const InDataAnchorPtr& peer_inAnchor : anchor->GetPeerInDataAnchors()) {
                (void)peer_inAnchor;
                FMK_LOGD("node name = %s, out data node name = %s %s.", node->ROLE(NodeSpec).Name().c_str(),
                    peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Name().c_str(),
                    peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Type().c_str());
            }
        }
        for (const InControlAnchorPtr& peer_inAnchor : node->GetOutControlAnchor()->GetPeerInControlAnchors()) {
            (void)peer_inAnchor;
            FMK_LOGD("node name = %s, out control node name = %s %s.", node->ROLE(NodeSpec).Name().c_str(),
                peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Name().c_str(),
                peer_inAnchor->GetOwnerNode()->ROLE(NodeSpec).Type().c_str());
        }
    }
}

} // namespace ge
