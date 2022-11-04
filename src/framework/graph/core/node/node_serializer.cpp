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

#include "framework/graph/core/node/node_serializer.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/debug/ge_log.h"

#include "framework/graph/op/control_flow_attr_defs.h"
#include "framework/graph/utils/attr_utils.h"
#include "graph/op/control_flow_defs.h"

// inc/framework
#include "graph/core/node/node_store.h"
#include "graph/persistance/interface/op_def.h"

// framework/inc
#include "infra/base/assertion.h"

namespace ge {
hiai::Status NodeSerializer::SerializeTo(::hiai::IOpDef* dstDef)
{
    HIAI_EXPECT_NOT_NULL(dstDef);

    HIAI_EXPECT_TRUE(SaveSubGraphs());

    HIAI_EXPECT_EXEC(ROLE(NodeStore).OpDesc()->SerializeTo(dstDef));

    HIAI_EXPECT_TRUE(SaveEdge(dstDef));

    return hiai::SUCCESS;
}

hiai::Status NodeSerializer::UnSerializeSubGraphs()
{
    const std::string& type = ROLE(NodeSpec).Type();
    if (type == hiai::op::If::TYPE) {
        HIAI_EXPECT_TRUE(GetSubGraphInIfNode());
    } else if (type == hiai::op::While::TYPE) {
        HIAI_EXPECT_TRUE(GetSubGraphInWhileNode());
    }
    return hiai::SUCCESS;
}

bool NodeSerializer::SaveSubGraphs()
{
    const std::string& type = ROLE(NodeSpec).Type();
    if (type == hiai::op::If::TYPE) {
        return SaveSubGraphInIfNode();
    } else if (type == hiai::op::While::TYPE) {
        return SaveSubGraphInWhileNode();
    }
    return true;
}

bool NodeSerializer::SaveSubGraphInIfNode()
{
    if (!SaveSubGraphInNode(hiai::ATTR_NAME_THEN_BRANCH, hiai::ATTR_NAME_THEN_BRANCH_GRAPH)) {
        return false;
    }
    if (!SaveSubGraphInNode(hiai::ATTR_NAME_ELSE_BRANCH, hiai::ATTR_NAME_ELSE_BRANCH_GRAPH)) {
        return false;
    }
    return true;
}

bool NodeSerializer::SaveSubGraphInWhileNode()
{
    if (!SaveSubGraphInNode(hiai::ATTR_NAME_COND, hiai::ATTR_NAME_COND_GRAPH)) {
        return false;
    }
    if (!SaveSubGraphInNode(hiai::ATTR_NAME_BODY, hiai::ATTR_NAME_BODY_GRAPH)) {
        return false;
    }
    return true;
}

bool NodeSerializer::SaveSubGraphInNode(const std::string& graphNameAttr, const std::string& graphAttr)
{
    string graphName = "";
    auto& op = ROLE(NodeStore).OpDesc();

    if (!ge::AttrUtils::GetStr(op, graphNameAttr, graphName)) {
        FMK_LOGE("get %s node subGraph name failed!", op->GetName().c_str());
        return false;
    }

    ge::ComputeGraphPtr subGraph = ROLE(NodeSubGraph).FindSubGraphPtr(graphName);
    HIAI_EXPECT_NOT_NULL_R(subGraph, false);

    if (!ge::AttrUtils::SetGraph(op, graphAttr, subGraph)) {
        FMK_LOGE("failed to set %s node %s subGraph!", op->GetName().c_str(), graphName.c_str());
        return false;
    }

    return true;
}

bool NodeSerializer::SaveEdge(hiai::IOpDef* dstDef)
{
    // inputs
    dstDef->clear_input();
    for (const auto& inDataAnchor : ROLE(NodeStore).InDataAnchors()) {
        if (inDataAnchor) {
            auto peerOutAnchor = inDataAnchor->GetPeerOutAnchor();
            if (peerOutAnchor && peerOutAnchor->GetOwnerNode()) {
                dstDef->add_input(peerOutAnchor->GetOwnerNode()->ROLE(NodeSpec).Name() + ":" +
                    std::to_string(peerOutAnchor->GetIdx()));
            } else {
                FMK_LOGW("Input Anchor PeerOutAnchor Is Empty nodeName %s nodeType %s", ROLE(NodeSpec).Name().c_str(),
                    ROLE(NodeSpec).Type().c_str());
                dstDef->add_input("");
            }
        }
    }
    // control edge
    auto controlAnchor = ROLE(NodeStore).InCtrlAnchor();
    if (controlAnchor) {
        auto peerOutAnchors = controlAnchor->GetPeerOutControlAnchors();
        for (const auto& peerOutAnchor : peerOutAnchors) {
            if (peerOutAnchor->GetOwnerNode()) {
                dstDef->add_input(peerOutAnchor->GetOwnerNode()->ROLE(NodeSpec).Name() + ":-1");
            }
        }
    }
    return true;
}

bool NodeSerializer::GetSubGraphInIfNode()
{
    if (!GetSubGraphInNode(hiai::ATTR_NAME_ELSE_BRANCH, hiai::ATTR_NAME_ELSE_BRANCH_GRAPH)) {
        return false;
    }
    if (!GetSubGraphInNode(hiai::ATTR_NAME_THEN_BRANCH, hiai::ATTR_NAME_THEN_BRANCH_GRAPH)) {
        return false;
    }
    return true;
}

bool NodeSerializer::GetSubGraphInWhileNode()
{
    if (!GetSubGraphInNode(hiai::ATTR_NAME_COND, hiai::ATTR_NAME_COND_GRAPH)) {
        return false;
    }
    if (!GetSubGraphInNode(hiai::ATTR_NAME_BODY, hiai::ATTR_NAME_BODY_GRAPH)) {
        return false;
    }
    return true;
}

bool NodeSerializer::GetSubGraphInNode(const std::string& graphNameAttr, const std::string& graphAttr)
{
    ge::ComputeGraphPtr subGraph;
    auto& op = ROLE(NodeStore).OpDesc();

    if (!ge::AttrUtils::GetGraph(op, graphAttr, subGraph) || subGraph == nullptr) {
        FMK_LOGE("failed to unserialize node %s attr %s subGraph", op->GetName().c_str(), graphAttr.c_str());
        return false;
    }

    std::string subGraphName;
    if (!ge::AttrUtils::GetStr(op, graphNameAttr, subGraphName)) {
        FMK_LOGE("failed to unserialize node %s attr %s subGraphName", op->GetName().c_str(), graphNameAttr.c_str());
        return false;
    }

    const std::string& savedName = subGraph->ROLE(GraphSpec).Name();
    HIAI_EXPECT_TRUE_R(subGraphName == savedName, false);

    HIAI_EXPECT_EXEC_R(ROLE(NodeSubGraph).AddSubGraph(subGraph), false);

    return true;
}
} // namespace ge