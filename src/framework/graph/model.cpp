/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the model operation class
 * @file model.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */
#include "graph/model.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_serializer.h"
#include "framework/graph/utils/graph_utils.h"

// src/framework/inc
#include "infra/base/assertion.h"

#include "graph/persistance/interface/model_def.h"
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/proxy/proto_factory.h"

namespace ge {
const static string STR_EMPTY = "";

Model::Model(const string& name, const string& customVersion) : Model()
{
    if (modelDef_ != nullptr) {
        modelDef_->set_name(name);
        modelDef_->set_custom_version(customVersion);
    }
}

Model::Model() : Model(hiai::ProtoFactory::Instance()->CreateModelDef())
{
}

Model::Model(hiai::IModelDef* modelDef) : modelDef_(modelDef)
{
}

Model::~Model()
{
    if (modelDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef_);
    }
    modelDef_ = nullptr;
}

const hiai::IAttrMapDef* Model::GetAttrMapDef() const
{
    return modelDef_ != nullptr ? modelDef_->attr() : nullptr;
}

hiai::IAttrMapDef* Model::MutableAttrMapDef()
{
    return modelDef_ != nullptr ? modelDef_->mutable_attr() : nullptr;
}

const string& Model::GetName() const
{
    return modelDef_ != nullptr ? modelDef_->name() : STR_EMPTY;
}

void Model::SetName(const string& name)
{
    if (modelDef_ != nullptr) {
        modelDef_->set_name(name);
    }
}

uint32_t Model::GetVersion() const
{
    return modelDef_ != nullptr ? static_cast<uint32_t>(modelDef_->version()) : 0;
}

void Model::SetVersion(uint32_t version)
{
    if (modelDef_ != nullptr) {
        modelDef_->set_version(version);
    }
}

const string& Model::GetPlatformVersion() const
{
    return modelDef_ != nullptr ? modelDef_->custom_version() : STR_EMPTY;
}

void Model::SetPlatformVersion(const std::string& version)
{
    if (modelDef_ != nullptr) {
        modelDef_->set_custom_version(version);
    }
}

void Model::SetGraph(const ge::Graph& graph)
{
    graph_ = graph;
}

Graph Model::GetGraph() const
{
    return graph_;
}

GraphErrCodeStatus Model::SerializeTo(hiai::IModelDef* modelDef) const
{
    modelDef->set_name(modelDef_->name());
    modelDef->set_version(modelDef_->version());
    modelDef->set_custom_version(modelDef_->custom_version());
    modelDef->set_attr(modelDef_->mutable_attr());

    auto computeGraph = GraphUtils::GetComputeGraph(graph_);
    HIAI_EXPECT_NOT_NULL(computeGraph);

    HIAI_EXPECT_TRUE(computeGraph->ROLE(GraphSerializer).SerializeTo(modelDef->add_graph()));

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus Model::Save(Buffer& buffer) const
{
    auto modelDef = hiai::ProtoFactory::Instance()->CreateModelDef();
    HIAI_EXPECT_NOT_NULL(modelDef);

    if (SerializeTo(modelDef) != GRAPH_SUCCESS) {
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
        return GRAPH_FAILED;
    }

    buffer.Resize(modelDef->GetModelDefSize());

    if (!modelDef->SaveTo(buffer.MutableData(), buffer.GetSize())) {
        buffer.Clear();
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
        return GRAPH_FAILED;
    }

    hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus Model::Load(const uint8_t* data, size_t len)
{
    HIAI_EXPECT_NOT_NULL(modelDef_);
    HIAI_EXPECT_TRUE(modelDef_->LoadFrom(data, len));
    HIAI_EXPECT_TRUE(modelDef_->graph_size() > 0);

    hiai::IGraphDef* graphDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    HIAI_EXPECT_NOT_NULL(graphDef);

    if (!graphDef->Swap(modelDef_->mutable_graph(0))) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef);
        return GRAPH_FAILED;
    }

    auto computeGraph = ComputeGraph::Make(graphDef, true);
    HIAI_EXPECT_NOT_NULL(computeGraph);

    HIAI_EXPECT_TRUE(computeGraph->ROLE(GraphSerializer).UnSerialize());

    graph_ = GraphUtils::CreateGraphFromComputeGraph(computeGraph);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus Model::Dump(const string& outFile)
{
    auto modelDef = hiai::ProtoFactory::Instance()->CreateModelDef();
    HIAI_EXPECT_NOT_NULL(modelDef);

    if (SerializeTo(modelDef) != GRAPH_SUCCESS) {
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
        return GRAPH_FAILED;
    }

    if (!modelDef->Dump(outFile)) {
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
        return GRAPH_FAILED;
    }

    hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
    return GRAPH_SUCCESS;
}

bool Model::IsValid() const
{
    return graph_.IsValid();
}
} // namespace ge
