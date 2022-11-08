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