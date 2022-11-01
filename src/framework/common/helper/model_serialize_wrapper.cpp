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


#include "common/helper/model_serialize_wrapper.h"

#include "infra/base/assertion.h"

#include "framework/infra/log/log.h"
#include "framework/graph/core/cgraph/graph_serializer.h"

#include "graph/persistance/interface/model_def.h"
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/proxy/proto_factory.h"

#include "common/helper/om_file_helper.h"

namespace hiai {
FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::SaveGraphToGraphDef(
    ge::ComputeGraphPtr graph, void*& outputGraphDef)
{
    HIAI_EXPECT_NOT_NULL_R(graph, false);
    auto graphDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    HIAI_EXPECT_NOT_NULL_R(graphDef, false);

    bool ret = graph->ROLE(GraphSerializer).SerializeTo(graphDef);
    if (!ret) {
        FMK_LOGE("SaveGraphToGraphDef failed");
        hiai::ProtoFactory::Instance()->DestroyGraphDef(graphDef);
        return false;
    }
    outputGraphDef = reinterpret_cast<void*>(graphDef);
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::GetGraphDefBufferSize(
    const void* graphDef, size_t& size)
{
    if (graphDef == nullptr) {
        FMK_LOGE("GetGraphDefBufferSize graphDef is nullptr!");
        return false;
    }
    const IGraphDef* iGraphDef = reinterpret_cast<const IGraphDef*>(graphDef);
    size = iGraphDef->GetGraphDefSize();
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::SerializeGraphDefToBuffer(
    const void* graphDef, void* data, size_t size)
{
    if (graphDef == nullptr) {
        FMK_LOGE("SerializeGraphDefToBuffer graphDef is nullptr");
        return false;
    }
    const IGraphDef* iGraphDef = reinterpret_cast<const IGraphDef*>(graphDef);
    bool isSuccess = iGraphDef->SaveTo(reinterpret_cast<std::uint8_t*>(data), size);
    if (!isSuccess) {
        FMK_LOGE("SerializeGraphDefToBuffer failed.");
        return false;
    }
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY void ModelSerializeWrapper::ReleaseGraphDef(void* graphDef)
{
    if (graphDef != nullptr) {
        IGraphDef* iGraphDef = reinterpret_cast<IGraphDef*>(graphDef);
        hiai::ProtoFactory::Instance()->DestroyGraphDef(iGraphDef);
    }
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::SaveModelToModelDef(
    const ge::Model& model, void*& outputModelDef)
{
    auto modelDef = hiai::ProtoFactory::Instance()->CreateModelDef();
    if (modelDef == nullptr) {
        FMK_LOGE("modelDef is nullptr");
        return false;
    }

    auto ret = model.SerializeTo(modelDef);
    if (ret != ge::GRAPH_SUCCESS) {
        FMK_LOGE("SerializeModelToModelDef failed");
        hiai::ProtoFactory::Instance()->DestroyModelDef(modelDef);
        return false;
    }
    outputModelDef = reinterpret_cast<void*>(modelDef);
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::GetModelDefBufferSize(
    const void* modelDef, size_t& size)
{
    HIAI_EXPECT_NOT_NULL_R(modelDef, false);
    const IModelDef* iModelDef = reinterpret_cast<const IModelDef*>(modelDef);
    size = iModelDef->GetModelDefSize();
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool ModelSerializeWrapper::SerializeModelDefToBuffer(
    const void* modelDef, void* data, size_t size)
{
    if (modelDef == nullptr) {
        FMK_LOGE("SerializeModelDefToBuffer modelDef is nullptr!");
        return false;
    }
    const IModelDef* iModelDef = reinterpret_cast<const IModelDef*>(modelDef);
    bool isSuccess = iModelDef->SaveTo(reinterpret_cast<std::uint8_t*>(data), size);
    if (!isSuccess) {
        FMK_LOGE("SerializeModelDefToBuffer failed.");
        return false;
    }
    return true;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY void ModelSerializeWrapper::ReleaseModelDef(void* modelDef)
{
    if (modelDef != nullptr) {
        IModelDef* iModelDef = reinterpret_cast<IModelDef*>(modelDef);
        hiai::ProtoFactory::Instance()->DestroyModelDef(iModelDef);
    }
}
} // namespace hiai