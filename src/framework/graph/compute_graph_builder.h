/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description:
 * compute_graph_builder.h
 */

#ifndef HIAI_GRAPH_COMPUTE_GRAPH_BUILDER_H
#define HIAI_GRAPH_COMPUTE_GRAPH_BUILDER_H

#include <vector>
#include <string>

#include "framework/graph/core/cgraph/graph_fwd.h"

namespace ge {
class Operator;

class ComputeGraphBuilder {
public:
    static ComputeGraphPtr BuildGraph(const std::string& name, std::vector<Operator>& inputs);
};
}; // namespace ge

#endif
