/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the GraphImpl operation class
 * @file graph_impl.h
 *
 * @brief
 *
 * @version 1.0
 *
 */
#ifndef FRAMEWORK_GRAPH_GRAPH_IMPL_H
#define FRAMEWORK_GRAPH_GRAPH_IMPL_H

#include <string>
#include <vector>

#include "graph/operator.h"

#include "graph/debug/ge_error_codes.h"
#include "framework/graph/core/cgraph/graph_fwd.h"

namespace ge {
class GraphImpl {
public:
    explicit GraphImpl(ComputeGraphPtr computeGraph);
    explicit GraphImpl(const std::string& name);

    ~GraphImpl() = default;

    ComputeGraphPtr GetComputeGraph();

    GraphErrCodeStatus SetInputs(std::vector<Operator>& inputs);

    GraphErrCodeStatus SetOutputs(std::vector<Operator>& outputs __attribute__((__unused__)));

    bool IsValid();

    GraphErrCodeStatus AddOp(ge::Operator& op);

    ge::Operator FindOpByName(const std::string& name) const;

    GraphErrCodeStatus CheckOpByName(const std::string& name) const;

    GraphErrCodeStatus GetAllOpName(std::vector<std::string>& opName) const;

private:
    std::string name_ {""};
    std::map<string, ge::Operator> opList_ {};
    ComputeGraphPtr computeGraph_ {nullptr};
};
} // namespace ge
#endif // FRAMEWORK_GRAPH_GRAPH_IMPL_H