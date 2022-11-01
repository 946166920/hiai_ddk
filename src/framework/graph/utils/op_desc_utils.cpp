/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the opdesc commom operation class
 * @file opdesc_utils.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */

#include "framework/graph/utils/op_desc_utils.h"

#include <algorithm>

// inc/api
#include "graph/attr_value.h"
#include "graph/op/const_defs.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_functor.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/node/node_compatibler.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/core/edge/edge_visitor.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/node_utils.h"
#include "framework/graph/debug/ge_graph_attr_define.h"
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/debug/ge_log.h"

// framework/inc
#include "infra/base/assertion.h"

// src/framework
#include "graph/operator_impl.h"

using namespace std;

/*lint -e512 -e737 -e752*/
namespace ge {
namespace {
const std::string OP_DESC_SPARSE_ALGORITHM_PARAMS = "sparse_algorithm_params";
} // namespace

Operator OpDescUtils::CreateOperatorFromOpDesc(const OpDescPtr& opDesc)
{
    OperatorImplPtr operatorImplPtr = hiai::make_shared_nothrow<OperatorImpl>(opDesc);
    if (operatorImplPtr == nullptr) {
        return Operator(opDesc->GetName(), opDesc->GetType());
    }
    return Operator(std::move(operatorImplPtr));
}

OpDescPtr OpDescUtils::GetOpDescFromOperator(const Operator& op)
{
    return op.GetImpl()->GetOpDesc();
}

bool OpDescUtils::HasSparseAlgorithmParams(OpDescPtr opDesc)
{
    HIAI_EXPECT_NOT_NULL_R(opDesc, false);
    return opDesc->HasAttr(OP_DESC_SPARSE_ALGORITHM_PARAMS);
}

GraphErrCodeStatus OpDescUtils::GetSparseAlgorithmParams(const OpDescPtr& opDesc, SparseAlgorithmParams& coordGrid)
{
    HIAI_EXPECT_NOT_NULL(opDesc);
    AttrValue attrValue;
    HIAI_EXPECT_EXEC(opDesc->GetAttr(OP_DESC_SPARSE_ALGORITHM_PARAMS, attrValue));
    AttrValue::NamedAttrs namedAttrs = attrValue.GetNamedAttrs();
    coordGrid.sparseAlgorithmOffset = namedAttrs.GetItem("sparseAlgorithmOffset").GetInt();
    coordGrid.sparseAlgorithmData = namedAttrs.GetItem("sparseAlgorithmData").GetBuffer();

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDescUtils::SetSparseAlgorithmParams(
    const OpDescPtr& opDesc, const SparseAlgorithmParams& coordGrid)
{
    HIAI_EXPECT_NOT_NULL(opDesc);
    AttrValue::NamedAttrs namedAttrs;
    namedAttrs.SetAttr("sparseAlgorithmOffset", AttrValue::CreateFrom(coordGrid.sparseAlgorithmOffset));
    namedAttrs.SetAttr("sparseAlgorithmData", AttrValue::CreateFrom(coordGrid.sparseAlgorithmData));
    auto attr = AttrValue::CreateFrom(namedAttrs);
    return opDesc->SetAttr(OP_DESC_SPARSE_ALGORITHM_PARAMS, attr); //lint !e732
}

TensorPtr OpDescUtils::MutableWeights(OpDesc& opDesc)
{
    TensorPtr weight = nullptr;
    AttrUtils::MutableTensor(&opDesc, hiai::ATTR_NAME_WEIGHTS, weight);
    return weight;
}

TensorPtr OpDescUtils::MutableWeights(const OpDescPtr& opDesc)
{
    if (!opDesc) {
        return nullptr;
    }
    return MutableWeights(*opDesc);
}

GraphErrCodeStatus OpDescUtils::SetWeights(OpDesc& opDesc, const TensorPtr weight)
{
    if (weight == nullptr) {
        return GRAPH_FAILED;
    }
    return AttrUtils::SetTensor(&opDesc, hiai::ATTR_NAME_WEIGHTS, weight) ? GRAPH_SUCCESS : GRAPH_FAILED; //lint !e737
}

GraphErrCodeStatus OpDescUtils::SetWeights(const OpDescPtr& opDesc, const TensorPtr weight)
{
    HIAI_EXPECT_NOT_NULL(opDesc);
    HIAI_EXPECT_NOT_NULL(weight);
    return SetWeights(*opDesc, weight);
}

vector<ConstTensorPtr> OpDescUtils::GetWeights(const ge::Node& node)
{
    auto weights = MutableWeights(node);
    vector<ConstTensorPtr> ret;
    for (const auto& item : weights) {
        ret.push_back(item);
    }
    return ret;
}

const vector<TensorPtr> OpDescUtils::GetWeightsWithNoConst(const ge::Node& node)
{
    vector<TensorPtr> ret;

    auto weights = MutableWeights(node);
    for (const auto& item : weights) {
        ret.push_back(item);
    }
    return ret;
}

vector<ConstTensorPtr> OpDescUtils::GetWeights(ge::ConstNodePtr node)
{
    if (!node) {
        return vector<ge::ConstTensorPtr>();
    }
    return GetWeights(*node);
}

size_t OpDescUtils::GetNonConstInputsSize(const ge::Node& node)
{
    return node.ROLE(NodeSpec).InNonConstSize();
}

size_t OpDescUtils::GetNonConstInputsSize(const ge::ConstNodePtr& node)
{
    if (!node) {
        return 0;
    }
    return GetNonConstInputsSize(*node);
}

size_t OpDescUtils::GetNonConstOutputsSize(const ge::Node& node)
{
    return node.ROLE(NodeSpec).OutDataEdgeSize();
}

size_t OpDescUtils::GetNonConstOutputsSize(const ge::ConstNodePtr& node)
{
    if (!node) {
        return 0;
    }
    return GetNonConstOutputsSize(*node);
}

namespace {
bool FindNonConstInDataByNode(const ge::Node& node, const size_t indexNonConst, std::size_t& index)
{
    std::size_t i = 0;
    auto visitor = [&i, indexNonConst, &index](Edge& edge) {
        if (i == indexNonConst) {
            index = static_cast<std::size_t>(edge.DstIdx()); //lint !e571
            return hiai::COMM_EXCEPTION;
        }
        ++i;
        return hiai::SUCCESS;
    };
    return node.ROLE(NodeWalker).ListInDataEdgesNonConst(std::move(visitor)) == hiai::COMM_EXCEPTION;
}
} // namespace

TensorDesc OpDescUtils::GetNonConstInputTensorDesc(const ge::Node& node, size_t indexNonConst)
{
    std::size_t index = 0;
    if (FindNonConstInDataByNode(node, indexNonConst, index)) {
        return node.ROLE(NodeSpec).OpDesc().GetInputDesc(index);
    }
    return TensorDesc();
}

TensorDesc OpDescUtils::GetNonConstInputTensorDesc(const ge::ConstNodePtr& node, size_t indexNonConst)
{
    if (node == nullptr) {
        FMK_LOGE("node is null.");
        return TensorDesc();
    }
    return GetNonConstInputTensorDesc(*node, indexNonConst);
}

bool OpDescUtils::GetNonConstInputIndex(const ge::Node& node, size_t indexNonConst, size_t& index)
{
    return FindNonConstInDataByNode(node, indexNonConst, index);
}

bool OpDescUtils::GetNonConstInputIndex(const ge::ConstNodePtr& node, size_t indexNonConst, size_t& index)
{
    CHECK_FALSE_EXEC(node != nullptr, return false);
    return GetNonConstInputIndex(*node, indexNonConst, index);
}

bool OpDescUtils::IsNonConstInput(const ge::Node& node, size_t index)
{
    if (index >= static_cast<size_t>(node.ROLE(NodeSpec).OpDesc().GetInputsDescSize())) { //lint !e571
        return false;
    }

    bool ret = false;
    auto visitor = [index, &ret](Edge& edge) {
        if (static_cast<std::size_t>(edge.DstIdx()) == index) { //lint !e571
            ret = edge.SrcNode().ROLE(NodeSpec).Type() != hiai::op::Const::TYPE &&
                edge.SrcNode().ROLE(NodeSpec).Type() != hiai::op::QuantizedConst::TYPE;
            return hiai::COMM_EXCEPTION;
        }
        return hiai::SUCCESS;
    };
    (void)node.ROLE(NodeWalker).ListInDataEdges(std::move(visitor));
    return ret;
}

bool OpDescUtils::IsNonConstInput(const ge::ConstNodePtr& node, size_t index)
{
    CHECK_FALSE_EXEC(node != nullptr, return false);
    return IsNonConstInput(*node, index);
}

vector<ge::Node*> OpDescUtils::GetConstInputs(const ge::ConstNodePtr& node)
{
    if (node == nullptr) {
        return vector<ge::Node*>();
    }
    return GetConstInputs(*node);
}

vector<ge::TensorDesc> OpDescUtils::GetNonConstTensorDesc(const ge::Node& node)
{
    vector<ge::TensorDesc> ret;
    (void)node.ROLE(NodeWalker).ListInDataEdgesNonConst([&ret](Edge& edge) {
        ret.push_back(edge.DstNode().ROLE(NodeSpec).OpDesc().GetInputDesc(edge.DstIdx()));
        return hiai::SUCCESS;
    });
    return ret;
}

vector<ge::TensorDesc> OpDescUtils::GetNonConstTensorDesc(const ge::ConstNodePtr& node)
{
    if (node == nullptr) {
        return vector<ge::TensorDesc>();
    }
    return GetNonConstTensorDesc(*node);
}

vector<ge::Node*> OpDescUtils::GetConstInputs(const ge::Node& node)
{
    vector<ge::Node*> ret;
    (void)node.ROLE(NodeWalker).ListInDataNodes([&ret](ge::Node& peerNode) {
        if (peerNode.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE ||
            peerNode.ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE) {
            ret.push_back(&peerNode);
        }
        return hiai::SUCCESS;
    });
    return ret;
}

vector<string> OpDescUtils::GetConstInputNames(const ge::Node& node)
{
    vector<string> ret;
    auto visitor = NodeFunctor::Typed({ hiai::op::Const::TYPE, hiai::op::QuantizedConst::TYPE }, [&ret](Node& input) {
        ret.push_back(input.ROLE(NodeSpec).Name());
        return hiai::SUCCESS;
    });
    (void)node.ROLE(NodeWalker).ListInDataNodes(std::move(visitor));

    return ret;
}

vector<TensorPtr> OpDescUtils::MutableWeights(const ge::Node& node)
{
    vector<TensorPtr> ret;
    // const算子，直接取权值
    auto& nodeSpec = node.ROLE(NodeSpec);
    if (nodeSpec.Type() == hiai::op::Const::TYPE || nodeSpec.Type() == hiai::op::QuantizedConst::TYPE) {
        auto weight = MutableWeights(nodeSpec.OpDesc());
        if (weight == nullptr) {
            FMK_LOGD("const op has no weight, op name:%s", nodeSpec.Name().c_str());
            return ret;
        }
        ret.push_back(weight);
        return ret;
    }
    // 其他算子从相连的constop中获取权值
    auto input_nodes = GetConstInputs(node);
    for (const auto& input_node : input_nodes) {
        GE_CHECK_NOTNULL_EXEC(input_node, continue);
        auto temp_weight = MutableWeights(input_node->ROLE(NodeSpec).OpDesc());
        if (temp_weight == nullptr) {
            FMK_LOGE("const op's weight is null, name: %s", input_node->ROLE(NodeSpec).Name().c_str());
            return vector<TensorPtr>();
        }
        ret.push_back(temp_weight);
    }
    return ret;
}

vector<TensorPtr> OpDescUtils::MutableWeights(const ge::NodePtr& node)
{
    if (!node) {
        return vector<ge::TensorPtr>();
    }
    return MutableWeights(*node);
}

GraphErrCodeStatus OpDescUtils::SetWeights(ge::Node& node, const vector<ge::TensorPtr>& weights)
{
    const auto& type = node.ROLE(NodeSpec).Type();
    if (type == hiai::op::Const::TYPE || type == hiai::op::QuantizedConst::TYPE) {
        const uint32_t constOpNormalWeightSize = 1;
        if (weights.size() == constOpNormalWeightSize) {
            return SetWeights(node.ROLE(NodeSpec).OpDesc(), weights[0]);
        }
        FMK_LOGD("const op weight size %zu should be 1", weights.size());
        return GRAPH_PARAM_INVALID;
    }

    auto inputNodes = GetConstInputs(node);
    if (weights.size() < inputNodes.size()) {
        FMK_LOGE("weights count can't be less than const input count");
        return GRAPH_PARAM_INVALID;
    }

    ge::AttrValue::NamedAttrs namedAttrs;
    bool ret = ge::AttrUtils::SetListTensor(namedAttrs, "key", weights);
    if (!ret) {
        FMK_LOGE("set Weights failed");
        return GRAPH_PARAM_INVALID;
    }
    vector<ge::TensorPtr> copyWeights;
    ret = ge::AttrUtils::MutableListTensor(namedAttrs, "key", copyWeights);
    if (!ret) {
        FMK_LOGE("set Weights failed");
        return GRAPH_PARAM_INVALID;
    }
    for (size_t i = 0; i < inputNodes.size(); ++i) {
        GE_CHECK_NOTNULL_EXEC(inputNodes[i], continue);
        SetWeights(inputNodes[i]->ROLE(NodeSpec).OpDesc(), copyWeights[i]);
    }

    // 如果设置的weights个数比constop多，需要添加constop
    vector<ge::TensorPtr> remainWeights;
    remainWeights.assign(copyWeights.begin() + inputNodes.size(), copyWeights.end());

    return node.ROLE(NodeCompatibler).TransTensorToConstInput(remainWeights);
}

GraphErrCodeStatus OpDescUtils::SetOutNodeWeightDef(Node& node, std::vector<TensorPtr>& vWeight)
{
    // 只考虑权值数为1或权值数不为1但与出边条数相等的情况
    if (vWeight.size() == 0 || !(vWeight.size() == 1 || vWeight.size() == node.ROLE(NodeSpec).OutEdgeSize())) {
        FMK_LOGE("Op: %s weights number should be 1 or equal to size of out edges", node.ROLE(NodeSpec).Name().c_str());
        return GRAPH_PARAM_INVALID;
    }

    // 遍历所有输出边
    string nodeName = node.ROLE(NodeSpec).Name();
    ComputeGraph& graph = node.ROLE(NodeSpec).OwnerComputeGraph();
    auto visitor = [&](Edge& edge) {
        // 输出边所对应的index
        int32_t index = edge.DstIdx();
        // 输出节点opdef
        OpDesc& opDesc = edge.DstNode().ROLE(NodeSpec).OpDesc();
        std::vector<bool> isInputConst;
        isInputConst = opDesc.GetIsInputConst();
        if (index >= static_cast<int32_t>(isInputConst.size())) {
            isInputConst.resize(static_cast<uint32_t>(index + 1));
        }

        if (index < static_cast<int64_t>(isInputConst.size())) {
            isInputConst[index] = true;
            opDesc.SetIsInputConst(isInputConst);

            // 如果算子有多个权值, 按照出边的顺序取权值
            if (static_cast<unsigned int>(edge.SrcIdx()) >= vWeight.size()) {
                FMK_LOGE("node %s of weights size is less than out anchors size.", nodeName.c_str());
                return hiai::FAILURE;
            }
            TensorPtr weight = vWeight[edge.SrcIdx()];
            HIAI_EXPECT_NOT_NULL(weight.get());

            HIAI_EXPECT_EXEC(graph.ROLE(GraphModifier).RemoveEdge(edge));
            if (opDesc.GetType() != NETOUTPUT) {
                edge.DstNode().ROLE(NodeCompatibler).TransTensorToConstInput(weight, edge.DstIdx());
            }
        }
        return hiai::SUCCESS;
    };
    if (node.ROLE(NodeWalker).ListOutDataEdges(std::move(visitor)) != hiai::SUCCESS) {
        return GRAPH_FAILED;
    }
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDescUtils::SetWeights(ge::NodePtr node, const vector<ge::TensorPtr>& weights)
{
    HIAI_EXPECT_NOT_NULL(node);
    return SetWeights(*node, weights);
}

GraphErrCodeStatus OpDescUtils::ClearWeights(ge::Node& node)
{
    ComputeGraph& graph = node.ROLE(NodeSpec).OwnerComputeGraph();

    (void)node.ROLE(NodeWalker).ListInDataNodes([&graph](ge::Node& input) {
        if (input.ROLE(NodeSpec).Type() == hiai::op::Const::TYPE ||
            input.ROLE(NodeSpec).Type() == hiai::op::QuantizedConst::TYPE) {
            (void)graph.ROLE(GraphModifier).RemoveNode(input);
        }
        return hiai::SUCCESS;
    });

    return hiai::SUCCESS;
}

GraphErrCodeStatus OpDescUtils::ClearWeights(const ge::NodePtr& node)
{
    HIAI_EXPECT_NOT_NULL(node);
    return ClearWeights(*node);
}
} // namespace ge
