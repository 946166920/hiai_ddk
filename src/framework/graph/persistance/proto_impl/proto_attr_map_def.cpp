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
#include "proto_attr_map_def.h"

namespace hiai {
ProtoAttrMapDef::ProtoAttrMapDef() : ProtoAttrMapDef(new (std::nothrow) ProtoMap(), true)
{
}

ProtoAttrMapDef::ProtoAttrMapDef(ProtoMap* attrMapDef, bool isOwner) : attrMapDef_(attrMapDef), isOwner_(isOwner)
{
}

ProtoAttrMapDef::~ProtoAttrMapDef()
{
    IMPL_PROTO_CUSTOM_MAP_MEMBER_FREE(attr);

    if (isOwner_) {
        delete attrMapDef_;
    }
    attrMapDef_ = nullptr;
}

void ProtoAttrMapDef::CopyFrom(const IAttrMapDef* other)
{
    if (attrMapDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *attrMapDef_ = *(static_cast<const ProtoAttrMapDef*>(other)->attrMapDef_);
        IMPL_PROTO_CUSTOM_MAP_MEMBER_FREE(attr);
    }
}

SerializeType ProtoAttrMapDef::GetSerializeType() const
{
    return PROTOBUF;
}

IMPL_PROTO_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC(
    ProtoAttrMapDef, attrMapDef_, std::string, IAttrDef, ProtoAttrDef, attr);


extern "C" GRAPH_API_EXPORT IAttrMapDef* CreateAttrMapDef() {
    return new (std::nothrow) ProtoAttrMapDef();
}

extern "C" GRAPH_API_EXPORT void DestroyAttrMapDef(IAttrMapDef* attrDef) {
    delete attrDef;
}

} // namespace hiai