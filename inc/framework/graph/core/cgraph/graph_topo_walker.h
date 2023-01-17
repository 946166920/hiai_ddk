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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_TOPO_WALKER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_TOPO_WALKER_H

// inc/api
#include "graph/graph_api_export.h"

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"
#include "framework/graph/core/cgraph/graph_listener.h"
#include "framework/graph/core/node/node_visitor.h"

namespace ge {
class GraphSorter;
class GraphListWalker;

class GRAPH_API_EXPORT GraphTopoWalker : public GraphListener {
public:
    hiai::Status DFSWalk(NodeVisitor v);

private:
    void OnAddNode(Node&) override;
    void OnDelNode(const Node&) override;
    void OnAddEdge(const Edge&) override;
    void OnDelEdge(const Edge&) override;
    void OnTopoChanged() override;

private:
    void SetTopoFlag(bool flag);
    bool GetTopoFlag() const;

private:
    bool change_{false};

private:
    USE_ROLE(GraphSorter);
    USE_ROLE(GraphListWalker);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_TOPO_WALKER_H