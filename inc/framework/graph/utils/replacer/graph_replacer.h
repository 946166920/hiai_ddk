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

#ifndef FMK_GRAPH_PATTERN_REPLACE_H
#define FMK_GRAPH_PATTERN_REPLACE_H

#include <vector>
#include <set>

#include "base/error_types.h"
#include "framework/graph/core/edge/endpoint.h"

namespace ge {
class Node;
class Edge;
class GraphModifier;

class GraphDstBoundary;
class GRAPH_API_EXPORT GraphSrcBoundary
{
public:
    GraphSrcBoundary(std::vector<Endpoint> inputs, std::vector<Endpoint> outputs, GraphModifier& g)
        : inputs_(std::move(inputs)), outputs_(std::move(outputs)), modifier_(g)
    {
    }
    ~GraphSrcBoundary()
    {
    }

    GRAPH_API_EXPORT hiai::Status ReplaceTo(const GraphDstBoundary& dst);

private:
    std::vector<Edge> InDataEdges();

    hiai::Status RelinkInputs(const GraphDstBoundary& dst);
    hiai::Status RelinkOutputs(const GraphDstBoundary& dst);

    hiai::Status RemoveAllNodes();

    void MarkConstInputs(Node* n, std::vector<Node*>& delNodes);
    void MarkRecursive(Node* n, std::vector<Node*>& delNodes);
    void MarkForward(Node* n, std::vector<Node*>& delNodes);
    void MarkBackward(Node* n, std::vector<Node*>& delNodes);

private:
    std::vector<Endpoint> inputs_;
    std::vector<Endpoint> outputs_;

    GraphModifier& modifier_;
};

class GRAPH_API_EXPORT GraphDstBoundary {
public:
    GraphDstBoundary(std::vector<Endpoint> inputs, std::vector<Endpoint> outputs)
        : inputs_(std::move(inputs)), outputs_(std::move(outputs))
    {
    }
    ~GraphDstBoundary()
    {
    }

    std::vector<Endpoint> IdleInEndpoints() const;
    std::vector<Endpoint> IdleOutEndpoints() const;

private:
    std::vector<Endpoint> inputs_;
    std::vector<Endpoint> outputs_;
};
} // namespace ge
#endif // FMK_GRAPH_PATTERN_REPLACE_H
