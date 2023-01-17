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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_NAMED_ATTR_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_NAMED_ATTR_DEF_H
#include "proto_func_macro_def.h"
#include "proto_wrapper.h"
#include "graph/persistance/interface/named_attr_def.h"

namespace hiai {
class ProtoNamedAttrDef : public INamedAttrDef {
public:
    ProtoNamedAttrDef(hiai::proto::NamedAttrs& namedAttrs);
    ~ProtoNamedAttrDef() override;

private:
    void CopyFrom(const INamedAttrDef* other) override;
    SerializeType GetSerializeType() const override;

    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);

private:
    hiai::proto::NamedAttrs& namedAttrs_;
};

class DefaultProtoNamedAttrDef : private ProtoWrapper<hiai::proto::NamedAttrs>, public ProtoNamedAttrDef {
public:
    DefaultProtoNamedAttrDef() : ProtoNamedAttrDef(GetProto())
    {
    }
    ~DefaultProtoNamedAttrDef() override = default;
};

} // namespace hiai
#endif