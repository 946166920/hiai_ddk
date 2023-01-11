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
#include "proto_named_attr_def.h"

#include "graph/persistance/proto_impl/proto_attr_map_def.h"

namespace hiai {
ProtoNamedAttrDef::ProtoNamedAttrDef(hiai::proto::NamedAttrs& namedAttrs) : namedAttrs_(namedAttrs)
{
}

ProtoNamedAttrDef::~ProtoNamedAttrDef()
{
    IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
}

void ProtoNamedAttrDef::CopyFrom(const INamedAttrDef* other)
{
    if (other != nullptr && other->GetSerializeType() == PROTOBUF) {
        namedAttrs_ = static_cast<const ProtoNamedAttrDef*>(other)->namedAttrs_;
        IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
    }
}

SerializeType ProtoNamedAttrDef::GetSerializeType() const
{
    return PROTOBUF;
}

IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoNamedAttrDef, namedAttrs_, std::string, name);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoNamedAttrDef, namedAttrs_, IAttrMapDef, ProtoAttrMapDef, attr);

extern "C" GRAPH_API_EXPORT INamedAttrDef* CreateNamedAttrDef()
{
    return new (std::nothrow) DefaultProtoNamedAttrDef();
}

extern "C" GRAPH_API_EXPORT void DestroyNamedAttrDef(INamedAttrDef* attrDef)
{
    delete attrDef;
}

} // namespace hiai