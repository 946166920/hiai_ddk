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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SERIALIZER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SERIALIZER_H

#include <map>
#include <string>
#include <vector>

// inc/framework
#include "base/error_types.h"
#include "framework/infra/base/dci.h"
#include "framework/graph/core/node/node_fwd.h"

namespace hiai {
class IGraphDef;
}

namespace ge {
class Buffer;
class GraphStore;
class GraphListWalker;
class GraphModifier;

struct NodeNameNodeReq {
    std::string srcNodeName;
    int32_t srcOutIndex;
    Node* dstNode;
    int32_t dstInIndex;
    std::string dstNodeName;
};

EXPORT_ROLE(GraphSerializer)
{
public:
    bool SerializeTo(hiai::IGraphDef* dstDef) const;
    bool UnSerialize();

    bool Save(Buffer& buffer) const;
    bool Load(const uint8_t* data, size_t len);

private:
    hiai::Status CreateAllNodes(
        std::map<std::string, Node*> & nodeMap, std::vector<NodeNameNodeReq> & nodeInputNodeNames);
    hiai::Status CreateInputNodes(const std::map<std::string, Node*>& nodeMap);
    hiai::Status CreateOutputNodes(const std::map<std::string, Node*>& nodeMap);
    hiai::Status HandleNodeNameRef(
        const std::map<std::string, Node*>& nodeMap, const std::vector<NodeNameNodeReq>& nodeInputNodeNames);

private:
    USE_ROLE(GraphStore);
    USE_ROLE(GraphListWalker);
    USE_ROLE(GraphModifier);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_GRAPH_SERIALIZER_H