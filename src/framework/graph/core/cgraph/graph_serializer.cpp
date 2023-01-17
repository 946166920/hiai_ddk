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
#include "framework/graph/core/cgraph/graph_serializer.h"

#include <climits>

#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_serializer.h"
#include "framework/graph/core/node/node_spec.h"

#include "graph/buffer.h"
#include "graph/core/cgraph/graph_store.h"
#include "graph/core/node/node_store.h"
#include "graph/core/op/op_desc_factory.h"
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/interface/op_def.h"
#include "graph/persistance/proxy/proto_factory.h"

// framework/inc
#include "infra/base/assertion.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"

namespace ge {
bool GraphSerializer::Save(Buffer& buffer) const
{
    auto graphDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, false);
    if (!SerializeTo(graphDef)) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef);
        return false;
    }

    buffer.Resize(graphDef->GetGraphDefSize());

    if (!graphDef->SaveTo(buffer.MutableData(), buffer.GetSize())) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef);
        return false;
    }

    hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef);
    return true;
}

bool GraphSerializer::Load(const uint8_t* data, size_t len)
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, false);
    HIAI_EXPECT_TRUE_R(graphDef->LoadFrom(data, len), false);

    return UnSerialize();
}

bool GraphSerializer::SerializeTo(hiai::IGraphDef* dstDef) const
{
    HIAI_EXPECT_NOT_NULL_R(dstDef, false);
    dstDef->set_name(ROLE(GraphStore).Name());

    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, false);
    dstDef->set_attr(graphDef->mutable_attr());

    hiai::Status status = ROLE(GraphListWalker).WalkInNodes([&dstDef](Node& node) {
        dstDef->add_input(node.ROLE(NodeSpec).Name() + ":0");
        return hiai::SUCCESS;
    });
    if (status != hiai::SUCCESS) {
        return false;
    }

    status = ROLE(GraphListWalker).WalkOutNodes([&dstDef](Node& node) {
        dstDef->add_output(node.ROLE(NodeSpec).Name() + ":0");
        return hiai::SUCCESS;
    });
    if (status != hiai::SUCCESS) {
        return false;
    }

    status = ROLE(GraphListWalker).WalkAllNodes([&dstDef](Node& node) {
        return node.ROLE(NodeSerializer).SerializeTo(dstDef->add_op());
    });
    return status == hiai::SUCCESS;
}

namespace {
hiai::Status SplitNameAndIndex(const string& org, string& nodeName, int32_t& srcOutIndex)
{
    auto sep = org.rfind(":");
    if (sep == std::string::npos) {
        return hiai::FAILURE;
    }
    nodeName = org.substr(0, sep);
    auto indexStr = org.substr(sep + 1);
    if (indexStr.length() == 0) {
        return hiai::FAILURE;
    }
    std::string checkStr = indexStr;
    if (indexStr[0] == '-') {
        checkStr = indexStr.substr(1, indexStr.length());
    }
    if (!isdigit(*(checkStr.c_str()))) {
        return hiai::FAILURE;
    }
    long indexL = strtol(indexStr.c_str(), nullptr, 0);
    if (indexL > INT_MAX || indexL < INT_MIN) {
        return hiai::FAILURE;
    }
    srcOutIndex = static_cast<int32_t>(indexL);
    return hiai::SUCCESS;
}

hiai::Status GetNodeNameAndIndex(
    hiai::IOpDef* opDef, std::map<string, Node*>& nodeMap, Node* node, std::vector<NodeNameNodeReq>& nodeInputNodeNames)
{
    const std::string& dstNodeName = opDef->name();
    int32_t dstIndex = 0;
    for (size_t j = 0; j < opDef->input_size(); j++) {
        const string& input = opDef->input(j);
        if (input.empty()) {
            dstIndex++;
            continue;
        }
        std::string srcNodeName;
        int32_t srcOutIndex = 0;
        HIAI_EXPECT_EXEC(SplitNameAndIndex(input, srcNodeName, srcOutIndex));

        nodeInputNodeNames.emplace_back(NodeNameNodeReq{srcNodeName, srcOutIndex, node, dstIndex, dstNodeName});
        if (srcOutIndex >= 0) {
            dstIndex++;
        }
    }
    // node名称不能重复
    auto pair = nodeMap.emplace(dstNodeName, node);
    HIAI_EXPECT_TRUE(pair.second);
    return hiai::SUCCESS;
}
} // namespace

hiai::Status GraphSerializer::CreateAllNodes(
    std::map<string, Node*>& nodeMap, std::vector<NodeNameNodeReq>& nodeInputNodeNames)
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL(graphDef);

    for (size_t i = 0; i < graphDef->op_size(); i++) {
        auto opDef = graphDef->mutable_op(i);
        HIAI_EXPECT_NOT_NULL(opDef);

        OpDescPtr opDesc = OpDescFactory::GetInstance().Create(opDef);
        HIAI_EXPECT_NOT_NULL(opDesc);

        HIAI_EXPECT_EXEC(opDesc->UnSerialize());

        auto node = ROLE(GraphModifier).AddNode(opDesc);
        HIAI_EXPECT_NOT_NULL(node);

        HIAI_EXPECT_EXEC(node->ROLE(NodeSerializer).UnSerializeSubGraphs());

        HIAI_EXPECT_EXEC(GetNodeNameAndIndex(opDef, nodeMap, node, nodeInputNodeNames));
    }
    return hiai::SUCCESS;
}

hiai::Status GraphSerializer::CreateInputNodes(const std::map<std::string, Node*>& nodeMap)
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL(graphDef);

    for (size_t i = 0; i < graphDef->input_size(); i++) {
        std::string inputName;
        int32_t inputIndex = 0;
        HIAI_EXPECT_EXEC(SplitNameAndIndex(graphDef->input(i), inputName, inputIndex));

        auto nodeIt = nodeMap.find(inputName);
        HIAI_EXPECT_TRUE(nodeIt != nodeMap.end());

        HIAI_EXPECT_EXEC(ROLE(GraphModifier).AddInput(*nodeIt->second));
    }
    return hiai::SUCCESS;
}

hiai::Status GraphSerializer::CreateOutputNodes(const std::map<std::string, Node*>& nodeMap)
{
    hiai::IGraphDef* graphDef = ROLE(GraphStore).GraphDef();
    HIAI_EXPECT_NOT_NULL(graphDef);

    for (size_t i = 0; i < graphDef->output_size(); i++) {
        std::string outputName;
        int32_t outputIndex = 0;
        HIAI_EXPECT_EXEC(SplitNameAndIndex(graphDef->output(i), outputName, outputIndex));

        auto nodeIt = nodeMap.find(outputName);
        HIAI_EXPECT_TRUE(nodeIt != nodeMap.end());

        HIAI_EXPECT_EXEC(ROLE(GraphModifier).AddOutput(*nodeIt->second));
    }
    return hiai::SUCCESS;
}

hiai::Status GraphSerializer::HandleNodeNameRef(
    const std::map<std::string, Node*>& nodeMap, const std::vector<NodeNameNodeReq>& nodeInputNodeNames)
{
    // edges
    for (auto& item : nodeInputNodeNames) {
        auto srcNodeIt = nodeMap.find(item.srcNodeName);
        HIAI_EXPECT_TRUE(srcNodeIt != nodeMap.end());

        if (item.srcOutIndex >= 0) {
            auto srcAnchor = srcNodeIt->second->ROLE(NodeStore).OutDataAnchor(item.srcOutIndex);
            HIAI_EXPECT_NOT_NULL(srcAnchor);

            auto dstAnchor = item.dstNode->ROLE(NodeStore).InDataAnchor(item.dstInIndex);
            HIAI_EXPECT_NOT_NULL(dstAnchor);

            (void)srcAnchor->LinkTo(dstAnchor);
        } else { // control edge
            auto srcAnchor = srcNodeIt->second->ROLE(NodeStore).OutCtrlAnchor();
            auto dstAnchor = item.dstNode->ROLE(NodeStore).InCtrlAnchor();
            if (srcAnchor && dstAnchor) {
                (void)srcAnchor->LinkTo(dstAnchor);
            }
        }
    }
    return hiai::SUCCESS;
}

bool GraphSerializer::UnSerialize()
{
    std::map<std::string, Node*> nodeMap;
    std::vector<NodeNameNodeReq> nodeInputNodeNames;

    HIAI_EXPECT_EXEC_R(CreateAllNodes(nodeMap, nodeInputNodeNames), false);

    HIAI_EXPECT_EXEC_R(CreateInputNodes(nodeMap), false);

    HIAI_EXPECT_EXEC_R(CreateOutputNodes(nodeMap), false);

    HIAI_EXPECT_EXEC_R(HandleNodeNameRef(nodeMap, nodeInputNodeNames), false);
    return true;
}
} // namespace ge