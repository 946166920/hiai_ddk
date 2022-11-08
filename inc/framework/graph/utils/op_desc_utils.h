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

#ifndef FRAMEWORK_GRAPH_UTILS_OP_DESC_UTILS_H
#define FRAMEWORK_GRAPH_UTILS_OP_DESC_UTILS_H

#include <string>
#include <memory>

// inc/api
#include "graph/operator.h"
#include "graph/attr_value.h"

// inc/framework
#include "framework/graph/core/def_types.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/edge/anchor_fwd.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/utils/range_vistor.h"

namespace ge {
class OpDescUtils {
public:
    template <class T>
    using Vistor = RangeVistor<T, std::shared_ptr<OpDesc>>;

public:
    OpDescUtils() = default;

    ~OpDescUtils() = default;

    GRAPH_API_EXPORT static bool HasSparseAlgorithmParams(OpDescPtr opDesc);

    GRAPH_API_EXPORT static GraphErrCodeStatus GetSparseAlgorithmParams(
        const OpDescPtr& opDesc, SparseAlgorithmParams& coordGrid);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetSparseAlgorithmParams(
        const OpDescPtr& opDesc, const SparseAlgorithmParams& coordGrid);

    GRAPH_API_EXPORT static vector<ConstTensorPtr> GetWeights(const ge::Node& node);

    GRAPH_API_EXPORT static vector<ConstTensorPtr> GetWeights(ge::ConstNodePtr node);

    GRAPH_API_EXPORT static const vector<TensorPtr> GetWeightsWithNoConst(const ge::Node& node);

    GRAPH_API_EXPORT static vector<TensorPtr> MutableWeights(const ge::Node& node);

    GRAPH_API_EXPORT static vector<TensorPtr> MutableWeights(const ge::NodePtr& node);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetWeights(ge::Node& node, const vector<ge::TensorPtr>& weights);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetWeights(ge::NodePtr node, const vector<ge::TensorPtr>& weights);

    GRAPH_API_EXPORT static GraphErrCodeStatus ClearWeights(ge::Node& node);

    GRAPH_API_EXPORT static GraphErrCodeStatus ClearWeights(const ge::NodePtr& node);

    GRAPH_API_EXPORT static vector<ge::Node*> GetConstInputs(const ge::Node& node);

    GRAPH_API_EXPORT static vector<ge::Node*> GetConstInputs(const ge::ConstNodePtr& node);

    GRAPH_API_EXPORT static size_t GetNonConstInputsSize(const ge::Node& node);

    GRAPH_API_EXPORT static size_t GetNonConstInputsSize(const ge::ConstNodePtr& node);

    GRAPH_API_EXPORT static size_t GetNonConstOutputsSize(const ge::Node& node);

    GRAPH_API_EXPORT static size_t GetNonConstOutputsSize(const ge::ConstNodePtr& node);

    // index: Indicates the index of all non const inputs
    GRAPH_API_EXPORT static TensorDesc GetNonConstInputTensorDesc(const ge::Node& node, size_t indexNonConst = 0);

    GRAPH_API_EXPORT static TensorDesc GetNonConstInputTensorDesc(
        const ge::ConstNodePtr& node, size_t indexNonConst = 0);

    GRAPH_API_EXPORT static vector<std::string> GetConstInputNames(const ge::Node& node);

    GRAPH_API_EXPORT static bool GetNonConstInputIndex(const ge::Node& node, size_t indexNonConst, size_t& index);

    GRAPH_API_EXPORT static bool GetNonConstInputIndex(
        const ge::ConstNodePtr& node, size_t indexNonConst, size_t& index);

    // index: Indicates the index of all inputs
    GRAPH_API_EXPORT static bool IsNonConstInput(const ge::Node& node, size_t index = 0);

    GRAPH_API_EXPORT static bool IsNonConstInput(const ge::ConstNodePtr& node, size_t index = 0);

    GRAPH_API_EXPORT static vector<ge::TensorDesc> GetNonConstTensorDesc(const ge::Node& node);

    GRAPH_API_EXPORT static vector<ge::TensorDesc> GetNonConstTensorDesc(const ge::ConstNodePtr& node);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetOutNodeWeightDef(Node& node, std::vector<TensorPtr>& vWeight);

    GRAPH_API_EXPORT static Operator CreateOperatorFromOpDesc(const OpDescPtr& opDesc);

    GRAPH_API_EXPORT static OpDescPtr GetOpDescFromOperator(const Operator& op);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetWeights(OpDesc& opDesc, const TensorPtr weight);

    GRAPH_API_EXPORT static GraphErrCodeStatus SetWeights(const OpDescPtr& opDesc, const TensorPtr weight);

    GRAPH_API_EXPORT static TensorPtr MutableWeights(const ge::OpDescPtr& opDesc);

private:
    static TensorPtr MutableWeights(ge::OpDesc& opDesc);
};
} // namespace ge
#endif // FRAMEWORK_GRAPH_UTILS_OP_DESC_UTILS_H