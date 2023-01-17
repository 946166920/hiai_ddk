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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_SHAPE_DEF_PROTO_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_SHAPE_DEF_PROTO_H
#include "proto_func_macro_def.h"
#include "proto_wrapper.h"
#include "graph/persistance/interface/shape_def.h"

namespace hiai {
class ProtoShapeDef : public IShapeDef {
public:
    ProtoShapeDef(hiai::proto::ShapeDef& shapeDef);
    ~ProtoShapeDef() override;

private:
    void CopyFrom(const IShapeDef* other) override;
    SerializeType GetSerializeType() const override;

    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, dim);

private:
    hiai::proto::ShapeDef& shapeDef_;
};

class DefaultProtoShapeDef : private ProtoWrapper<hiai::proto::ShapeDef>, public ProtoShapeDef {
public:
    DefaultProtoShapeDef() : ProtoShapeDef(GetProto())
    {
    }
    ~DefaultProtoShapeDef() override = default;
};

} // namespace hiai

#endif