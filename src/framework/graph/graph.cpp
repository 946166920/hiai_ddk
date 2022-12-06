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
#include "graph/graph.h"
#include "graph/operator.h"
#include "graph/attr_value.h"
#include "graph_impl.h"
#include "infra/base/securestl.h"
#include "framework/graph/debug/ge_log.h"

using namespace std;

namespace ge {
Graph::Graph(const std::string& name)
{
    impl_ = hiai::make_shared_nothrow<GraphImpl>(name);
}
Graph::Graph(const GraphImplPtr& graphImpl)
{
    impl_ = graphImpl;
}

const GraphImplPtr& Graph::GetImpl() const
{
    return impl_;
}

Graph& Graph::SetInputs(vector<ge::Operator>& inputs)
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return *this, "graph can not be used, impl_ is nullptr.")
    GE_CHK_BOOL_EXEC(impl_->SetInputs(inputs) == GRAPH_SUCCESS, return *this, "SetInputs failed!")
    return *this;
}

Graph& Graph::SetOutputs(vector<ge::Operator>& outputs)
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return *this, "graph can not be used, impl_ is nullptr.")
    GE_CHK_BOOL_EXEC(impl_->SetOutputs(outputs) == GRAPH_SUCCESS, return *this, "SetOutputs failed!")
    return *this;
}
GraphErrCodeStatus Graph::AddOp(ge::Operator& op)
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return GRAPH_FAILED, "graph can not be used, impl_ is nullptr.");
    return impl_->AddOp(op);
}

GraphErrCodeStatus Graph::GetAllOpName(std::vector<string>& opName) const
{
    GE_CHK_BOOL_EXEC(
        impl_ != nullptr, return GRAPH_FAILED, "graph can not be used, impl_ is nullptr.");
    return impl_->GetAllOpName(opName);
}

Operator Graph::FindOpByName(const string& name) const
{
    Operator opFindOpDef;
    GE_CHK_BOOL_EXEC(
        impl_ != nullptr, return opFindOpDef, "graph can not be used, impl_ is nullptr.");
    return impl_->FindOpByName(name);
}

GraphErrCodeStatus Graph::CheckOpByName(const string& name) const
{
    GE_CHK_BOOL_EXEC(
        impl_ != nullptr, return GRAPH_FAILED, "graph can not be used, impl_ is nullptr.");
    return impl_->CheckOpByName(name);
}
bool Graph::IsValid() const
{
    if (!impl_) {
        return false;
    }
    return impl_->IsValid();
}
} // namespace ge
