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

#include "framework/graph/core/node/node_const_input.h"

// api/framework
#include "graph/op/all_ops.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/edge.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
hiai::Status MakeInConstMark(NodeWalker& walker, std::vector<bool>& inConstMark)
{
    auto visitor = [&inConstMark](Edge& edge) {
        HIAI_EXPECT_TRUE(static_cast<std::size_t>(edge.DstIdx()) < inConstMark.size());

        inConstMark[edge.DstIdx()] = (edge.SrcNode().ROLE(NodeSpec).Type() == hiai::op::Const::TYPE ||
            edge.SrcNode().ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE);
        return hiai::SUCCESS;
    };

    return walker.ListInDataEdges(visitor);
}
} // namespace

hiai::Status NodeConstInput::Update()
{
    OpDesc& opDesc = ROLE(NodeSpec).OpDesc();

    std::vector<bool> inConstMark(opDesc.GetInputsDescSize(), false);
    HIAI_EXPECT_EXEC(MakeInConstMark(ROLE(NodeWalker), inConstMark));

    opDesc.SetIsInputConst(inConstMark);

    return hiai::SUCCESS;
}
} // namespace ge
