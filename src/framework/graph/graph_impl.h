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