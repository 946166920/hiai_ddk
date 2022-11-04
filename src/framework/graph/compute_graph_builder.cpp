/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: compute_graph_builder.cpp
 */

#include "graph/compute_graph_builder.h"

#include "framework/graph/core/cgraph/graph_builder.h"

namespace ge {
ComputeGraphPtr ComputeGraphBuilder::BuildGraph(const std::string& name, std::vector<Operator>& inputs)
{
    return GraphBuilder::Build(name, inputs);
}
} // namespace ge
