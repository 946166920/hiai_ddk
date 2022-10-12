/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: graph
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
