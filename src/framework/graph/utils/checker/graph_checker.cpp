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

#include "framework/graph/utils/checker/graph_checker.h"

// api/framework
#include "graph/op/nn_defs.h"
#include "graph/op/detection_defs.h"
#include "graph/op/math_defs.h"

// inc/framework
#include "framework/common/hcs_types.h"
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
bool GraphChecker::IsInputsFullyLinked(const ComputeGraph& graph)
{
    auto v = [](Node& node) {
        const auto& type = node.ROLE(NodeSpec).Type();
        if (type == GRAPH_OP_TYPE) {
            return node.ROLE(NodeSubGraph).WalkSubGraphs([](ge::ComputeGraphPtr& subGraph) {
                return GraphChecker::IsInputsFullyLinked(*subGraph);
            });
        }

        if (type == hiai::op::ConvTranspose::TYPE) {
            return hiai::SUCCESS;
        }

        if (type == hiai::op::SSDDetectionOutput::TYPE) {
            return hiai::SUCCESS;
        }

        if (type == ge::SSDPOSTPROCESSOR) {
            return hiai::SUCCESS;
        }

        if (type == "SSDAnchorGenerator") {
            return hiai::SUCCESS;
        }
        if (type == "_Retval") {
            return hiai::SUCCESS;
        }

        return node.ROLE(NodeSpec).IsInputFullyLinked() ? hiai::SUCCESS : hiai::FAILURE;
    };

    return graph.ROLE(GraphListWalker).WalkAllNodes(std::move(v)) == hiai::SUCCESS;
}
} // namespace ge