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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_GRAPH_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_GRAPH_DEF_H
#include "graph/persistance/interface/graph_def.h"
#include "proto_func_macro_def.h"

namespace hiai {
class ProtoGraphDef : public IGraphDef {
public:
    ProtoGraphDef();
    ProtoGraphDef(hiai::proto::GraphDef* graphDef, bool isOwner = false);
    ~ProtoGraphDef() override;

    SerializeType GetSerializeType() const override;
    void CopyFrom(const IGraphDef* other) override;

    bool LoadFrom(const uint8_t* data, size_t len) override;
    bool SaveTo(uint8_t* data, size_t len) const override;
    size_t GetGraphDefSize() const override;

    bool Swap(IGraphDef* other) override;

    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, input);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, output);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(IOpDef, op);
    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);

private:
    hiai::proto::GraphDef* graphDef_;
    bool isOwner_;
};
} // namespace hiai

#endif