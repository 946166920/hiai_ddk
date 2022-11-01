/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the graph operation class
 * @file graph.cpp
 *
 * @brief
 *
 * @version 1.0
 *
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
