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

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H
#define FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H

// api/framework
#include "graph/graph_api_export.h"

namespace ge {
class Node;

class GRAPH_API_EXPORT Endpoint {
public:
    Endpoint(ge::Node& node, int idx);
    ~Endpoint() = default;

    bool IsCtrl() const;
    bool IsData() const;

    ge::Node& Node() const;
    int Idx() const;

    bool operator==(const Endpoint& other) const;

private:
    ge::Node* node_;
    int idx_;
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_ENDPOINT_H