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
#include "proto_attr_def.h"

#include "graph/persistance/proto_impl/proto_named_attr_def.h"
#include "graph/persistance/proto_impl/proto_tensor_desc_def.h"
#include "graph/persistance/proto_impl/proto_tensor_def.h"
#include "graph/persistance/proto_impl/proto_graph_def.h"
#include "graph/persistance/proto_impl/proto_attr_list_def.h"

namespace hiai {
ProtoAttrDef::ProtoAttrDef() : ProtoAttrDef(new (std::nothrow) hiai::proto::AttrDef(), true)
{
}

ProtoAttrDef::ProtoAttrDef(hiai::proto::AttrDef* attrDef, bool isOwner) : attrDef_(attrDef), isOwner_(isOwner)
{
}

ProtoAttrDef::~ProtoAttrDef()
{
    IMPL_PROTO_CUSTOM_MEMBER_FREE(func);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(td);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(t);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(g);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(list);

    if (isOwner_) {
        delete attrDef_;
    }
    attrDef_ = nullptr;
}

void ProtoAttrDef::SetValueType(ge::AttrValue::ValueType type)
{
    // list type need extral set
    if (type >= ge::AttrValue::VT_LIST_STRING && type <= ge::AttrValue::VT_LIST_NAMED_ATTRS) {
        auto list = mutable_list();
        if (list != nullptr) {
            list->SetValueType(type);
        }
    }
}

ge::AttrValue::ValueType ProtoAttrDef::GetValueType() const
{
    static ge::AttrValue::ValueType typeDef[] = {
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_STRING,
        ge::AttrValue::VT_INT,
        ge::AttrValue::VT_FLOAT,
        ge::AttrValue::VT_BOOL,
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_BYTES,
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_NAMED_ATTRS,
        ge::AttrValue::VT_TENSOR_DESC,
        ge::AttrValue::VT_TENSOR,
        ge::AttrValue::VT_GRAPH
    };
    if (attrDef_ != nullptr) {
        auto valueCase = attrDef_->value_case();
        if (valueCase != hiai::proto::AttrDef::kList) {
            return typeDef[valueCase];
        } else {
            if (list() != nullptr) {
                return list()->GetValueType();
            }
        }
    }
    return ge::AttrValue::VT_NONE;
}

void ProtoAttrDef::CopyFrom(const IAttrDef* other)
{
    if (other != nullptr && attrDef_ != nullptr && other->GetSerializeType() == PROTOBUF) {
        *attrDef_ = *(static_cast<const ProtoAttrDef*>(other)->attrDef_);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(func);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(td);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(t);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(g);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(list);
    }
}

SerializeType ProtoAttrDef::GetSerializeType() const
{
    return PROTOBUF;
}

IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, std::string, s);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, int64_t, i, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, float, f, 0.0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, bool, b, false);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, std::string, bt);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, INamedAttrDef, ProtoNamedAttrDef, func);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, ITensorDescDef, ProtoTensorDescDef, td);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, ITensorDef, ProtoTensorDef, t);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, IGraphDef, ProtoGraphDef, g);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoAttrDef, attrDef_, IAttrListDef, ProtoAttrListDef, list);

extern "C" GRAPH_API_EXPORT IAttrDef* CreateAttrDef()
{
    return new (std::nothrow) ProtoAttrDef();
}

extern "C" GRAPH_API_EXPORT void DestroyAttrDef(IAttrDef* attrDef)
{
    delete attrDef;
}

} // namespace hiai