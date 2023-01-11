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

#ifndef FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_SERIALIZER_H
#define FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_SERIALIZER_H

#include <string>

// inc/framework
#include "framework/infra/base/dci.h"
#include "base/error_types.h"

namespace hiai {
class IOpDef;
}

namespace ge {
class NodeSpec;
class NodeStore;
class NodeSubGraph;

EXPORT_ROLE(NodeSerializer)
{
public:
    hiai::Status SerializeTo(hiai::IOpDef *dstDef);
    hiai::Status UnSerializeSubGraphs();

private:
    bool SaveSubGraphs();
    bool SaveSubGraphInIfNode();
    bool SaveSubGraphInWhileNode();
    bool SaveSubGraphInNode(const std::string& graphNameAttr, const std::string& graphAttr);
    bool SaveEdge(hiai::IOpDef *dstDef);

    bool GetSubGraphInIfNode();
    bool GetSubGraphInWhileNode();
    bool GetSubGraphInCaseNode();
    bool GetSubGraphInNode(const std::string& graphNameAttr, const std::string& graphAttr);
    bool SaveSubGraphInCaseNode();
private:
    USE_ROLE(NodeSpec);
    USE_ROLE(NodeStore);
    USE_ROLE(NodeSubGraph);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_SERIALIZER_H