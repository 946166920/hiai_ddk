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

// inc/framework
#include "framework/graph/core/node/node_modifier.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/op/op_desc.h"

// framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/core/node/node_store.h"

namespace ge {
hiai::Status NodeModifier::DelLastEmptyInTensor()
{
    auto& anchors = ROLE(NodeStore).InDataAnchors();
    HIAI_EXPECT_TRUE(!anchors.empty());
    HIAI_EXPECT_TRUE(anchors.back()->GetPeerOutAnchor() == nullptr);

    OpDescPtr desc = ROLE(NodeStore).OpDesc();
    auto& tensors = const_cast<std::vector<TensorDescPtr>&>(desc->GetInputsDesc());
    HIAI_EXPECT_TRUE(anchors.size() == tensors.size());

    anchors.pop_back();
    tensors.pop_back();

    std::vector<bool> isInputConst = desc->GetIsInputConst();
    isInputConst.pop_back();
    desc->SetIsInputConst(isInputConst);

    return hiai::SUCCESS;
}
} // namespace ge