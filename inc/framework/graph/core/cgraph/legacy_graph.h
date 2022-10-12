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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_LEGACY_GRAPH_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_LEGACY_GRAPH_H

#include <memory>
#include <string>
#include <vector>

// api/framework
#include "graph/operator.h"
#include "graph/debug/ge_error_codes.h"
#include "graph/detail/attributes_holder.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/op/op_desc.h"
#include "framework/graph/core/edge/anchor_fwd.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/utils/range_vistor.h"

namespace ge {
class AttrValue;
using AttrValuePtr = std::shared_ptr<AttrValue>;

class LegacyGraph;
using LegacyGraphPtr = std::shared_ptr<LegacyGraph>;

class ComputeGraph;
class GraphStore;
class GraphSpec;
class GraphModifier;
class GraphBypasser;
class GraphListWalker;
class GraphTopoWalker;
class GraphSorter;
class GraphFinder;

class LegacyGraph : public std::enable_shared_from_this<LegacyGraph>, public AttrHolder {
public:
    template <class T>
    using Vistor = RangeVistor<T, std::shared_ptr<const LegacyGraph>>;

public:
    GRAPH_API_EXPORT virtual ~LegacyGraph() override;

    /**
     * @brief get nodes except nodes belonging to subgraph
     * @return node vistor
     */
    GRAPH_API_EXPORT Vistor<NodePtr> GetDirectNodes() const;

    /**
     * @brief get nodes include nodes belonging to subgraph
     * @return node vistor
     */
    GRAPH_API_EXPORT Vistor<NodePtr> GetAllNodes() const;

    GRAPH_API_EXPORT Vistor<NodePtr> GetOutputNodes() const;

    GRAPH_API_EXPORT NodePtr FindNode(const std::string& name) const;

    GRAPH_API_EXPORT NodePtr FindNode(const int64_t id) const;

    /*lint -e504*/
    GRAPH_API_EXPORT NodePtr AddNode(NodePtr node);

    GRAPH_API_EXPORT NodePtr AddNode(OpDescPtr op);

    GRAPH_API_EXPORT GraphErrCodeStatus RemoveNode(Node& node);

    GRAPH_API_EXPORT GraphErrCodeStatus RemoveNode(NodePtr node);
    /**
     * @brief get subgraphs include subgraphs belonging to subgraph
     * @return node vistor
     */
    GRAPH_API_EXPORT vector<ComputeGraphPtr> GetAllSubGraphs() const;
    /**
     * @brief get subgraphs except subgraphs belonging to subgraphs
     * @return node vistor
     */
    GRAPH_API_EXPORT vector<ComputeGraphPtr> GetDirectSubGraphs() const;

    /*lint +e504*/
    const std::map<std::vector<std::string>, std::vector<std::string>>& GetShareParamLayer() const
    {
        return paramsShareMap_;
    }

    void SetShareParamLayer(const std::map<std::vector<std::string>, std::vector<std::string>>& params_share_map)
    {
        paramsShareMap_ = params_share_map;
    }

    void SetGraphOutNodes(const std::map<std::string, std::vector<int32_t>>& out_nodes_map)
    {
        outNodesMap_ = out_nodes_map;
    }

    const std::map<std::string, std::vector<int32_t>>& GetGraphOutNodes() const
    {
        return outNodesMap_;
    }

    void SetOrigGraph(ComputeGraphPtr origGraph)
    {
        origGraph_ = origGraph;
    }

    ComputeGraphPtr GetOrigGraph(void)
    {
        return origGraph_;
    }

protected:
    explicit LegacyGraph(GraphStore& store);

    const hiai::IAttrMapDef* GetAttrMapDef() const override;

    hiai::IAttrMapDef* MutableAttrMapDef() override;

private:
    GraphErrCodeStatus SortNodes(std::vector<NodePtr>& stack, std::map<NodePtr, uint32_t>& mapInEdgeNum);

    GraphErrCodeStatus SortNodes(std::vector<NodePtr>& nodes) const;

    GraphErrCodeStatus RemoveExtraOutEdge(NodePtr node);

private:
    std::vector<NodePtr>& nodes_;
    std::vector<NodePtr>& inputNodes_;
    std::vector<NodePtr>& outputNodes_;
    std::map<std::vector<std::string>, std::vector<std::string>> paramsShareMap_;
    std::map<std::string, std::vector<int32_t>> outNodesMap_;
    std::map<std::string, uint32_t>& inputs_order_;
    // Graph Before buffer fusion
    ComputeGraphPtr origGraph_;

private:
    USE_ROLE(ComputeGraph);
    USE_ROLE(GraphStore);
    USE_ROLE(GraphSpec);
    USE_ROLE(GraphModifier);
    USE_ROLE(GraphBypasser);
    USE_ROLE(GraphListWalker);
    USE_ROLE(GraphTopoWalker);
    USE_ROLE(GraphSorter);
    USE_ROLE(GraphFinder);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_LEGACY_GRAPH_H
