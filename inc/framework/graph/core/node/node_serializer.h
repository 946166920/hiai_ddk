/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: node serializer
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
    bool GetSubGraphInNode(const std::string& graphNameAttr, const std::string& graphAttr);

private:
    USE_ROLE(NodeSpec);
    USE_ROLE(NodeStore);
    USE_ROLE(NodeSubGraph);
};
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_CGRAPH_NODE_SERIALIZER_H