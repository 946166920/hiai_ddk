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

#ifndef GE_GRAPH_H
#define GE_GRAPH_H

#include <memory>

#include "graph/operator.h"

namespace ge {
class GraphImpl;
using GraphImplPtr = std::shared_ptr<GraphImpl>;

class GRAPH_API_EXPORT Graph {
public:
    explicit Graph(const std::string& name);
    explicit Graph(const GraphImplPtr& graphImpl);
    Graph() = default;

    ~Graph() = default;

    const GraphImplPtr& GetImpl() const;

    Graph& SetInputs(std::vector<Operator>& inputs);

    Graph& SetOutputs(std::vector<Operator>& outputs);

    bool IsValid() const;

    GraphErrCodeStatus AddOp(ge::Operator& op);

    ge::Operator FindOpByName(const string& name) const;

    GraphErrCodeStatus CheckOpByName(const string& name) const;

    GraphErrCodeStatus GetAllOpName(std::vector<string>& opName) const;

private:
    GraphImplPtr impl_ {nullptr};
};
} // namespace ge

#endif // GE_MODEL_H
