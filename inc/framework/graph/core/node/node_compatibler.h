
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Notes: node compatibler
 */

#ifndef FRAMEWORK_GRAPH_CORE_NODE_NODE_COMPATIBLER_H
#define FRAMEWORK_GRAPH_CORE_NODE_NODE_COMPATIBLER_H

#include <functional>
#include <memory>
#include <vector>

// inc/framework
#include "base/error_types.h"
#include "framework/infra/base/dci.h"

namespace ge {
class Node;
class NodeStore;
class NodeSpec;
class NodeWalker;
class Tensor;
class OpDesc;

using TensorPtr = std::shared_ptr<Tensor>;

EXPORT_ROLE(NodeCompatibler)
{
public:
    using WeightToAttrFunc = std::function<hiai::Status(const ge::Tensor&, OpDesc&)>;

    hiai::Status TransConstInputToAttr(std::size_t index, WeightToAttrFunc weightToFunc);

    hiai::Status TransTensorToConstInput(const std::vector<ge::TensorPtr> weights);
    hiai::Status TransTensorToConstInput(const ge::TensorPtr& weight, int dstIndex);

    hiai::Status RemoveSpecificInput(std::size_t index);
    hiai::Status RemoveIdleEndpoint();

private:
    USE_ROLE(NodeStore);
    USE_ROLE(Node);
    USE_ROLE(NodeSpec);
    USE_ROLE(NodeWalker);
};
} // namespace ge
#endif