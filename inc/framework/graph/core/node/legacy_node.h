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

#ifndef FRAMEWORK_GRAPH_CORE_NODE_LEGACY_NODE_H
#define FRAMEWORK_GRAPH_CORE_NODE_LEGACY_NODE_H

#include <memory>
#include <string>
#include <vector>
#include <map>

// api/framework
#include "graph/attr_value.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "framework/graph/core/cgraph/graph_fwd.h"
#include "framework/graph/core/node/node_fwd.h"
#include "framework/graph/core/op/op_desc.h"
#include "framework/graph/core/edge/anchor_fwd.h"
#include "framework/graph/utils/range_vistor.h"
/*lint -e553*/

namespace ge {
using KFusionDataFlowVec = std::vector<std::multimap<std::string, ge::AnchorPtr>>;
/*
 * LegacyNode is a component of ComputeGraph
 *
 */
/*lint -e148*/
/*lint -e1509*/

class NodeSpec;
class NodeStore;
class NodeSubGraph;

class LegacyNode : public std::enable_shared_from_this<LegacyNode> {
public:
    friend class ComputeGraph;
    friend class LegacyGraph;

    friend class ModelSerializeImp;

    friend class NodeUtils;

    template<class T>
    using Vistor = RangeVistor<T, std::shared_ptr<ConstNode>>;

    virtual ~LegacyNode() = default;

protected:
    explicit LegacyNode(NodeStore& store);

public:
    GRAPH_API_EXPORT Vistor<InDataAnchorPtr> GetAllInDataAnchors() const;

    GRAPH_API_EXPORT Vistor<OutDataAnchorPtr> GetAllOutDataAnchors() const;

    GRAPH_API_EXPORT InDataAnchorPtr GetInDataAnchor(int idx) const;

    GRAPH_API_EXPORT OutDataAnchorPtr GetOutDataAnchor(int idx) const;

    GRAPH_API_EXPORT InControlAnchorPtr GetInControlAnchor() const;

    GRAPH_API_EXPORT OutControlAnchorPtr GetOutControlAnchor() const;

    // All in Data nodes
    GRAPH_API_EXPORT Vistor<NodePtr> GetInDataNodes() const;

    // All in Control nodes
    GRAPH_API_EXPORT Vistor<NodePtr> GetInControlNodes() const;

    GRAPH_API_EXPORT Vistor<NodePtr> GetInAllNodes() const;

    // All out Data nodes
    GRAPH_API_EXPORT Vistor<NodePtr> GetOutDataNodes() const;

    // All out Control nodes
    GRAPH_API_EXPORT Vistor<NodePtr> GetOutControlNodes() const;

    GRAPH_API_EXPORT Vistor<NodePtr> GetOutAllNodes() const;

    GRAPH_API_EXPORT OpDescPtr GetOpDesc() const;

    GraphErrCodeStatus AddLinkFrom(Node& input_node);
    GRAPH_API_EXPORT GraphErrCodeStatus AddLinkFrom(NodePtr input_node);

    GraphErrCodeStatus AddLinkFrom(Node& input_node, uint32_t index);
    GRAPH_API_EXPORT GraphErrCodeStatus AddLinkFrom(NodePtr input_node, uint32_t index);

    GRAPH_API_EXPORT GraphErrCodeStatus AddLinkFrom(OutDataAnchorPtr peerOutAnchor);

    GRAPH_API_EXPORT GraphErrCodeStatus RemoveInDataAnchor(int idx);

    GRAPH_API_EXPORT const KFusionDataFlowVec& GetFusionInputFlowList() const
    {
        return fusionInputDataflowList_;
    }

    GRAPH_API_EXPORT const KFusionDataFlowVec& GetFusionOutputFlowList() const
    {
        return fusionOutputDataflowList_;
    }

    GRAPH_API_EXPORT void SetFusionInputFlowList(const KFusionDataFlowVec& fusion_input_list)
    {
        fusionInputDataflowList_ = fusion_input_list;
    }

    GRAPH_API_EXPORT void SetFusionOutputFlowList(const KFusionDataFlowVec& fusion_output_list)
    {
        fusionOutputDataflowList_ = fusion_output_list;
    }

    /**
     * @brief get subgraphs except subgraphs belonging to subgraphs
     * @return node vistor
     */
    void GetDirectSubGraphs(vector<ComputeGraphPtr>& graphs) const;
    GRAPH_API_EXPORT vector<ComputeGraphPtr> GetDirectSubGraphs() const;
private:
    OpDescPtr& op_;
    vector<InDataAnchorPtr>& inDataAnchors_;
    vector<OutDataAnchorPtr>& outDataAnchors_;
    InControlAnchorPtr& inControlAnchor_;
    OutControlAnchorPtr& outControlAnchor_;
    bool anchorStatusUpdated_{false};
    KFusionDataFlowVec fusionInputDataflowList_;
    KFusionDataFlowVec fusionOutputDataflowList_;

private:
    USE_ROLE(NodeStore);
    USE_ROLE(NodeSpec);
    USE_ROLE(NodeSubGraph);
};
/*lint +e148*/
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_NODE_LEGACY_NODE_H
