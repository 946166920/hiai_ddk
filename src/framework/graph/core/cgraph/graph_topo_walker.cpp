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

#include "framework/graph/core/cgraph/graph_topo_walker.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_sorter.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
hiai::Status GraphTopoWalker::DFSWalk(NodeVisitor v)
{
    if (GetTopoFlag()) {
        HIAI_EXPECT_EXEC(ROLE(GraphSorter).SortNodesDFS());
        SetTopoFlag(false);
    }

    return ROLE(GraphListWalker).WalkAllNodes(std::move(v));
}

void GraphTopoWalker::OnAddNode(Node&)
{
    SetTopoFlag(true);
}

void GraphTopoWalker::OnDelNode(const Node&)
{
    SetTopoFlag(true);
}

void GraphTopoWalker::OnAddEdge(const Edge&)
{
    SetTopoFlag(true);
}

void GraphTopoWalker::OnDelEdge(const Edge&)
{
    SetTopoFlag(true);
}

void GraphTopoWalker::OnTopoChanged()
{
    SetTopoFlag(true);
}

void GraphTopoWalker::SetTopoFlag(bool flag)
{
    change_ = flag;
}

bool GraphTopoWalker::GetTopoFlag() const
{
    return change_;
}
} // namespace ge
