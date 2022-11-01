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

#include "framework/graph/core/cgraph/graph_bypasser.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_const_input.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/endpoint.h"

// src/framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/cgraph/graph_store.h"

namespace ge {
namespace {
inline bool CheckNodeOneInDataEdge(NodeSpec& spec)
{
    return (spec.OutDataEdgeSize() + spec.OutCtrlEdgeSize()) > 0;
}

inline bool CheckNodeNonInDataEdge(NodeSpec& spec)
{
    return spec.OutDataEdgeSize() == 0 && spec.InCtrlEdgeSize() > 0 && spec.OutCtrlEdgeSize() > 0;
}
} // namespace

bool GraphBypasser::PreCheck(const Node& node)
{
    HIAI_EXPECT_TRUE_R(ROLE(GraphStore).HasNode(node), false);

    NodeSpec& spec = node.ROLE(NodeSpec);
    if (spec.InDataEdgeSize() == 1) {
        return CheckNodeOneInDataEdge(spec);
    } else if (spec.InDataEdgeSize() == 0) {
        return CheckNodeNonInDataEdge(spec);
    }

    return false;
}

namespace {
const std::vector<Endpoint> SaveSrcPoint(const Node& node)
{
    std::vector<Endpoint> points;

    (void)node.ROLE(NodeWalker).ListInEdges([&points](Edge& edge) {
        points.push_back({edge.SrcNode(), edge.SrcIdx()});
        return hiai::SUCCESS;
    });

    return points;
}

const std::vector<Endpoint> SaveDstPoint(const Node& node)
{
    std::vector<Endpoint> points;

    (void)node.ROLE(NodeWalker).ListOutEdges([&points](Edge& edge) {
        points.push_back({edge.DstNode(), edge.DstIdx()});
        return hiai::SUCCESS;
    });

    return points;
}

hiai::Status LinkPoints(GraphModifier& modifier, GraphStore& store, const std::vector<Endpoint>& srcPoints,
    const std::vector<Endpoint>& dstPoints)
{
    for (const auto& src : srcPoints) {
        for (const auto& dst : dstPoints) {
            if ((src.IsData() && dst.IsData()) || (src.IsCtrl() && dst.IsCtrl())) {
                HIAI_EXPECT_EXEC(modifier.AddEdge(src, dst));
            } else if (src.IsData() && dst.IsCtrl()) {
                if (!store.HasEdge({{src.Node(), -1}, dst})) {
                    HIAI_EXPECT_EXEC(modifier.AddEdge({src.Node(), -1}, dst));
                }
            } else if (src.IsCtrl() && dst.IsData()) {
                return hiai::FAILURE;
            }
        }
    }
    return hiai::SUCCESS;
}
} // namespace

hiai::Status GraphBypasser::ByPassNode(const Node& node)
{
    HIAI_EXPECT_TRUE(PreCheck(node));

    const std::vector<Endpoint> srcPoints = SaveSrcPoint(node);
    const std::vector<Endpoint> dstPoints = SaveDstPoint(node);

    HIAI_EXPECT_EXEC(ROLE(GraphModifier).RemoveNode(node));

    HIAI_EXPECT_EXEC(LinkPoints(ROLE(GraphModifier), ROLE(GraphStore), srcPoints, dstPoints));

    return hiai::SUCCESS;
}
} // namespace ge
