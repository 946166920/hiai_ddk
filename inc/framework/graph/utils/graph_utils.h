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

#ifndef GE_GRAPH_UTIL_H
#define GE_GRAPH_UTIL_H

#include <iostream>
#include <fstream>
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/cgraph/compute_graph.h"

#include "framework/graph/utils/anchor_utils.h"
#include "graph/graph.h"

namespace ge {
class GraphUtils {
    using GraphVisitor = std::function<GraphErrCodeStatus(ComputeGraphPtr&)>;
    using NodeVisitor = std::function<GraphErrCodeStatus(ge::Node&)>;

public:
    GRAPH_API_EXPORT static ComputeGraphPtr GetComputeGraph(const Graph& graph);

    GRAPH_API_EXPORT static Graph CreateGraphFromComputeGraph(const ComputeGraphPtr computeGraph);

    GRAPH_API_EXPORT static GraphErrCodeStatus AddEdge(OutDataAnchorPtr src, InDataAnchorPtr dst);

    GRAPH_API_EXPORT static GraphErrCodeStatus AddEdge(
        OutDataAnchorPtr src, Format srcFormat, InDataAnchorPtr dst, Format dstFormat);

    GRAPH_API_EXPORT static GraphErrCodeStatus AddEdge(OutControlAnchorPtr src, InControlAnchorPtr dst);

    // check whether src is link to dst and then remove
    GRAPH_API_EXPORT static GraphErrCodeStatus RemoveEdge(OutDataAnchorPtr src, InDataAnchorPtr dst);

    GRAPH_API_EXPORT static GraphErrCodeStatus RemoveEdge(OutControlAnchorPtr src, InControlAnchorPtr dst);

    GRAPH_API_EXPORT static GraphErrCodeStatus InsertNodeBetweenDataAnchors(
        OutDataAnchorPtr src, InDataAnchorPtr dst, NodePtr newNode);

    GRAPH_API_EXPORT static GraphErrCodeStatus InsertTransNode(
        ComputeGraph& computeGraph, InDataAnchorPtr in_data_anchor, std::vector<OpDescPtr> vecOpDesc);

    GRAPH_API_EXPORT static GraphErrCodeStatus InsertTransNode(
        ComputeGraph& computeGraph, OutDataAnchorPtr outDataAnchor, std::vector<OpDescPtr> vecOpDesc);

    GRAPH_API_EXPORT static void RecordOriginalNames(const std::vector<ge::Node*>& originalNodes, const ge::Node& node);
    GRAPH_API_EXPORT static void RecordOriginalNames(std::vector<ge::NodePtr> originalNodes, ge::NodePtr node);
    static void RecordOriginalNames(const std::vector<ge::NodePtr>& sharedOriginalNodes, const ge::Node& node);

    GRAPH_API_EXPORT static void RecordOriginalNames(std::vector<std::string> namesTmp, ge::NodePtr node);

    static ComputeGraphPtr CreateGraphFromOperator(const string& name, const std::vector<Operator>& inputs);

    static GraphErrCodeStatus WalkAllNodeSubGraphs(const Node& node, GraphVisitor visitor);
    GRAPH_API_EXPORT static GraphErrCodeStatus WalkAllSubGraphs(const ComputeGraph& graph, GraphVisitor visitor);
    static GraphErrCodeStatus WalkAllNodeSubGraphNodes(const Node& node, NodeVisitor visitor);
    GRAPH_API_EXPORT static GraphErrCodeStatus WalkAllSubGraphNodes(const ComputeGraph& graph, NodeVisitor visitor);

    static ComputeGraphPtr CreateGraphFromAllOperator(
        const string& name, const std::vector<ge::Operator>& seqOperators);

    GRAPH_API_EXPORT static ComputeGraphPtr Clone(const ComputeGraphPtr graph);

    GRAPH_API_EXPORT static void Dump(const ComputeGraphPtr graph);
};
} // namespace ge

#endif // GE_GRAPH_UTIL_H
