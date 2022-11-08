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
#ifndef FRAMEWORK_MODEL_MODEL_SERIALIZE_WRAPPER_H
#define FRAMEWORK_MODEL_MODEL_SERIALIZE_WRAPPER_H
#include "framework/graph/core/cgraph/compute_graph.h"
#include "graph/model.h"

namespace hiai {
class ModelSerializeWrapper {
public:
    /**
     * SaveGraphToGraphDef 将graph保存到proto中,中间proto会申请内存,释放对应内存使用ReleaseGraphDef
     */
    bool SaveGraphToGraphDef(ge::ComputeGraphPtr graph, void*& outputGraphDef);
    bool GetGraphDefBufferSize(const void* graphDef, size_t& size);
    bool SerializeGraphDefToBuffer(const void* graphDef, void* data, size_t size);
    void ReleaseGraphDef(void* graphDef);
    /**
     * SaveModelToModelDef 将graph保存到proto中,中间proto会申请内存,释放对应内存使用ReleaseModelDef
     */
    bool SaveModelToModelDef(const ge::Model& model, void*& modelDef);
    bool GetModelDefBufferSize(const void* modelDef, size_t& size);
    bool SerializeModelDefToBuffer(const void* modelDef, void* data, size_t size);
    void ReleaseModelDef(void* modelDef);
};
} // namespace hiai
#endif