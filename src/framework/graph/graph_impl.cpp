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

#include "graph/graph_impl.h"

// inc/api
#include "graph/operator.h"
#include "graph/attr_value.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_spec.h"
#include "framework/graph/core/cgraph/graph_finder.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/debug/ge_log.h"

// src/framework
#include "graph/operator_impl.h"
#include "graph/compute_graph_builder.h"

using namespace std;

namespace ge {
GraphImpl::GraphImpl(ComputeGraphPtr computeGraph)
    : name_(computeGraph->ROLE(GraphSpec).Name()), computeGraph_(computeGraph)
{
}

GraphImpl::GraphImpl(const std::string& name) : name_(name)
{
}

ComputeGraphPtr GraphImpl::GetComputeGraph()
{
    return computeGraph_;
}

GraphErrCodeStatus GraphImpl::SetInputs(std::vector<Operator>& inputs)
{
    GE_CHK_BOOL_EXEC(!inputs.empty(), return GRAPH_FAILED, "SetInputs failed: input operator size can not be 0.");
    computeGraph_ = ComputeGraphBuilder::BuildGraph(name_, inputs);

    GE_CHK_BOOL_RET_STATUS(computeGraph_ != nullptr, GRAPH_FAILED, "Build Graph failed.");
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus GraphImpl::SetOutputs(std::vector<Operator>& outputs)
{
    GE_CHK_BOOL_RET_STATUS(computeGraph_ != nullptr, GRAPH_FAILED, "Build Graph failed.");

    std::vector<Node*> nodes;
    GraphFinder& finder = computeGraph_->ROLE(GraphFinder);
    for (const auto& output : outputs) {
        if (output.GetImpl() != nullptr) {
            const auto& name = output.GetImpl()->GetName();
            Node* node = finder.FindNode(name);
            if (node != nullptr) {
                nodes.push_back(node);
            }
        }
    }

    return computeGraph_->ROLE(GraphModifier).SetOutputs(nodes);
}

bool GraphImpl::IsValid()
{
    return computeGraph_ != nullptr;
}

GraphErrCodeStatus GraphImpl::AddOp(ge::Operator& op)
{
    std::pair<std::map<string, ge::Operator>::iterator, bool> ret;
    if (op.GetImpl() == nullptr) {
        FMK_LOGE("the op is invaild");
        return GRAPH_FAILED;
    }
    ret = opList_.insert(std::pair<string, ge::Operator>(op.GetImpl()->GetName(), op));
    GE_CHK_BOOL_RET_STATUS(
        ret.second, GRAPH_FAILED, "the op have added before, op name:%s.", op.GetImpl()->GetName().c_str());
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus GraphImpl::GetAllOpName(std::vector<string>& opName) const
{
    for (auto it : opList_) {
        opName.push_back(it.first);
    }
    return GRAPH_SUCCESS;
}

ge::Operator GraphImpl::FindOpByName(const string& name) const
{
    auto it = opList_.find(name);
    Operator opFindDef;
    GE_CHK_BOOL_EXEC(it != opList_.end(), return opFindDef, "Error: there is no op: %s.", name.c_str());
    return it->second;
}

GraphErrCodeStatus GraphImpl::CheckOpByName(const string& name) const
{
    auto it = opList_.find(name);
    GE_CHK_BOOL_EXEC(it != opList_.end(), return GRAPH_FAILED, "Error: there is no op: %s.", name.c_str());
    return GRAPH_SUCCESS;
}
} // namespace ge
