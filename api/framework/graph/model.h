/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: model
 */

#ifndef GE_MODEL_H
#define GE_MODEL_H

#include <map>
#include <string>
#include <vector>

#include "graph/attr_value.h"
#include "graph/attributes_holder.h"
#include "graph/graph.h"

namespace hiai {
class IModelDef;
}

namespace ge {
using std::string;
using std::vector;

class GRAPH_API_EXPORT Model : public AttrHolder {
public:
    Model();

    Model(const string& name, const string& customVersion);

    ~Model() override;

    Model(const Model& other) = delete;

    Model& operator=(const Model& other) = delete;

    const string& GetName() const;

    void SetName(const string& name);

    uint32_t GetVersion() const;

    void SetVersion(uint32_t version);

    const string& GetPlatformVersion() const;

    void SetPlatformVersion(const std::string& version);

    Graph GetGraph() const;

    void SetGraph(const Graph& graph);

    GraphErrCodeStatus SerializeTo(hiai::IModelDef* modelDef) const;

    GraphErrCodeStatus Save(Buffer& buffer) const;

    GraphErrCodeStatus Load(const uint8_t* data, size_t len);

    GraphErrCodeStatus Dump(const std::string& outFile);

    bool IsValid() const;

protected:
    const hiai::IAttrMapDef* GetAttrMapDef() const override;

    hiai::IAttrMapDef* MutableAttrMapDef() override;

private:
    Model(hiai::IModelDef* modelDef);

    hiai::IModelDef* modelDef_;

    Graph graph_;
};

} // namespace ge

#endif // GE_MODEL_H
