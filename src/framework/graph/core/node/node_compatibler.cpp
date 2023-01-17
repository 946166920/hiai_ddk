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
#include "framework/graph/core/node/node_compatibler.h"

// inc/infra
#include "infra/base/securestl.h"

// api/framework
#include "graph/op/const_defs.h"
#include "graph/tensor.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/op/op_desc.h"

#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/debug/ge_graph_attr_define.h"

// src/framework
#include "graph/core/node/node_store.h"

// framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
ge::TensorPtr GetConstTensor(ge::Node& node, std::size_t index)
{
    ge::Node* constNode = node.ROLE(NodeWalker).InDataNode(index);
    HIAI_EXPECT_NOT_NULL_R(constNode, nullptr);

    auto& constNodeSpec = constNode->ROLE(NodeSpec);
    HIAI_EXPECT_TRUE_R((constNodeSpec.Type() == hiai::op::Const::TYPE ||
        constNodeSpec.Type() == hiai::op::QuantizedConst::TYPE), nullptr);

    ge::TensorPtr weight = nullptr;
    ge::AttrUtils::MutableTensor(constNodeSpec.OpDesc(), hiai::ATTR_NAME_WEIGHTS, weight);

    return weight;
}

inline bool IsIsolateNode(ge::Node& node)
{
    return node.ROLE(NodeSpec).InDataEdgeSize() == 0 && node.ROLE(NodeSpec).OutEdgeSize() == 0;
}

hiai::Status UnlinkEdgeAndRemoveIsolateInput(ge::Node& node, std::size_t index)
{
    auto edge = node.ROLE(NodeWalker).InDataEdge(index);
    if (edge.Exist()) {
        auto& graphModifier = node.ROLE(NodeSpec).OwnerComputeGraph().ROLE(GraphModifier);
        HIAI_EXPECT_EXEC(graphModifier.RemoveEdge(edge.Value()));
        auto& peerNode = edge.Value().SrcNode();
        if (IsIsolateNode(peerNode)) {
            HIAI_EXPECT_EXEC(graphModifier.RemoveNode(peerNode));
        }
    }
    return hiai::SUCCESS;
}

void RemoveIdleAnchor(std::vector<InDataAnchorPtr>& inDataAnchors)
{
    for (int i = static_cast<int>(inDataAnchors.size()) - 1; i >= 0; --i) {
        if (inDataAnchors[i]->GetPeerOutAnchor() != nullptr) {
            break;
        }
        inDataAnchors.erase(inDataAnchors.cbegin() + i);
    }
}

hiai::Status UpdateIsInputConst(ge::Node& node)
{
    std::vector<bool> isInputConst;
    auto visitor = [&isInputConst](ge::Node& inNode) {
        if (inNode.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE ||
            inNode.ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE) {
            isInputConst.push_back(true);
        } else {
            isInputConst.push_back(false);
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(node.ROLE(NodeWalker).ListInDataNodes(visitor));

    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    opDesc.SetIsInputConst(isInputConst);
    return hiai::SUCCESS;
}

ge::Node* CreateConstNode(ge::Node& node, ge::OpDesc& opDesc, size_t index, const TensorPtr& tensorPtr)
{
    HIAI_EXPECT_NOT_NULL_R(tensorPtr, nullptr);

    shared_ptr<OpDesc> constOpDesc = hiai::make_shared_nothrow<ge::OpDesc>();
    HIAI_EXPECT_NOT_NULL_R(constOpDesc, nullptr);

    HIAI_EXPECT_TRUE_R(AttrUtils::SetTensor(constOpDesc, hiai::ATTR_NAME_WEIGHTS, tensorPtr), nullptr);

    constOpDesc->SetType(hiai::op::Const::TYPE);
    constOpDesc->SetName(opDesc.GetName() + "_inner_const_" + std::to_string(index));
    constOpDesc->AddOutputDesc(tensorPtr->GetTensorDesc());

    return node.ROLE(NodeSpec).OwnerComputeGraph().ROLE(GraphModifier).AddNodeFront(constOpDesc);
}

hiai::Status AddEndpoint(const ge::Node& node, int& index)
{
    auto& inDataAnchors = node.ROLE(NodeStore).InDataAnchors();

    auto nodePtr = node.ROLE(NodeSpec).OwnerComputeGraph().ROLE(GraphFinder).FindNodePtr(node);
    shared_ptr<InDataAnchor> anchor = hiai::make_shared_nothrow<InDataAnchor>(nodePtr, inDataAnchors.size());
    HIAI_EXPECT_NOT_NULL(anchor);
    inDataAnchors.push_back(anchor);
    index = static_cast<int>(inDataAnchors.size()) - 1;
    return hiai::SUCCESS;
}

hiai::Status UpdateInputDesc(ge::OpDesc& opDesc, const ge::TensorDesc& weight, int dstIndex)
{
    if (dstIndex < opDesc.GetInputsDescSize()) {
        HIAI_EXPECT_EXEC(opDesc.UpdateInputDesc(dstIndex, weight));
    } else {
        opDesc.AddInputDesc(weight);
    }
    return hiai::SUCCESS;
}

hiai::Status GetDstIndex(vector<uint32_t> unsetInputIndexs, size_t weightIndex, const ge::Node& node, int& dstIndex)
{
    if (weightIndex < unsetInputIndexs.size()) {
        dstIndex = static_cast<int>(unsetInputIndexs[weightIndex]);
    } else {
        HIAI_EXPECT_EXEC(AddEndpoint(node, dstIndex));
    }
    return hiai::SUCCESS;
}
} // namespace

hiai::Status NodeCompatibler::TransConstInputToAttr(std::size_t index, WeightToAttrFunc weightToFunc)
{
    ge::TensorPtr weight = GetConstTensor(ROLE(Node), index);
    HIAI_EXPECT_NOT_NULL(weight);

    weightToFunc(*weight, ROLE(NodeSpec).OpDesc());

    return RemoveSpecificInput(index);
}

hiai::Status NodeCompatibler::TransTensorToConstInput(const std::vector<ge::TensorPtr> weights)
{
    auto& opDesc = ROLE(NodeSpec).OpDesc();
    vector<uint32_t> unsetInputIndexs;
    opDesc.GetUnSetInputIndexs(unsetInputIndexs);

    int dstIndex = 0;
    for (size_t i = 0; i < weights.size(); i++) {
        HIAI_EXPECT_EXEC(GetDstIndex(unsetInputIndexs, i, ROLE(Node), dstIndex));
        HIAI_EXPECT_EXEC(TransTensorToConstInput(weights[i], dstIndex));
    }
    return hiai::SUCCESS;
}

hiai::Status NodeCompatibler::TransTensorToConstInput(const ge::TensorPtr& weight, int dstIndex)
{
    HIAI_EXPECT_NOT_NULL(weight);

    auto& opDesc = ROLE(NodeSpec).OpDesc();
    auto constNode = CreateConstNode(ROLE(Node), opDesc, dstIndex, weight);
    HIAI_EXPECT_NOT_NULL(constNode);

    HIAI_EXPECT_EXEC(
        ROLE(NodeSpec).OwnerComputeGraph().ROLE(GraphModifier).AddEdge({*constNode, 0}, {ROLE(Node), dstIndex}));

    HIAI_EXPECT_EXEC(UpdateInputDesc(opDesc, weight->GetTensorDesc(), dstIndex));

    HIAI_EXPECT_TRUE(ge::AttrUtils::SetListStr(constNode->ROLE(NodeSpec).OpDesc(), "original_op_names", {""}));
    return hiai::SUCCESS;
}

hiai::Status NodeCompatibler::RemoveSpecificInput(std::size_t index)
{
    HIAI_EXPECT_EXEC(UnlinkEdgeAndRemoveIsolateInput(ROLE(Node), index));
    HIAI_EXPECT_EXEC(RemoveIdleEndpoint());
    return hiai::SUCCESS;
}

hiai::Status NodeCompatibler::RemoveIdleEndpoint()
{
    auto& opDesc = ROLE(NodeSpec).OpDesc();
    auto oldOpDescs = opDesc.GetAllInputsDesc();
    opDesc.ClearAllInputsDesc();

    std::vector<InDataAnchorPtr>& inDataAnchors = ROLE(NodeStore).InDataAnchors();
    for (size_t i = 0; i < inDataAnchors.size(); i++) {
        if (inDataAnchors[i]->GetPeerOutAnchor() != nullptr) {
            HIAI_EXPECT_EXEC(opDesc.AddInputDesc(oldOpDescs.at(i)));
            continue;
        }
        for (size_t j = i + 1; j < inDataAnchors.size(); j++) {
            auto peerAnchor = inDataAnchors[j]->GetPeerOutAnchor();
            if (peerAnchor == nullptr) {
                continue;
            }
            HIAI_EXPECT_EXEC(inDataAnchors[j]->Unlink(peerAnchor));
            HIAI_EXPECT_EXEC(peerAnchor->LinkTo(inDataAnchors[i]));
            HIAI_EXPECT_EXEC(opDesc.AddInputDesc(oldOpDescs.at(j)));
            break;
        }
    }
    RemoveIdleAnchor(inDataAnchors);
    HIAI_EXPECT_EXEC(UpdateIsInputConst(ROLE(Node)));
    return hiai::SUCCESS;
}
} // namespace ge