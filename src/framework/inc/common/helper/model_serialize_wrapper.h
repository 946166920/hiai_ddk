/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: model serialize wrapper
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