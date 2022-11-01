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
#include "proto_attr_list_def.h"

#include "graph/persistance/proto_impl/proto_tensor_desc_def.h"
#include "graph/persistance/proto_impl/proto_tensor_def.h"
#include "graph/persistance/proto_impl/proto_graph_def.h"
#include "graph/persistance/proto_impl/proto_named_attr_def.h"

namespace hiai {
ProtoAttrListDef::ProtoAttrListDef() : ProtoAttrListDef(new (std::nothrow) hiai::proto::AttrDef_ListValue(), true)
{
}

ProtoAttrListDef::ProtoAttrListDef(hiai::proto::AttrDef_ListValue* attrListDef, bool isOwner)
    : attrListDef_(attrListDef), isOwner_(isOwner)
{
}

ge::AttrValue::ValueType ProtoAttrListDef::GetValueType() const
{
    static ge::AttrValue::ValueType listTypeDef[] = {
        ge::AttrValue::VT_NONE,
        ge::AttrValue::VT_LIST_STRING,
        ge::AttrValue::VT_LIST_INT,
        ge::AttrValue::VT_LIST_FLOAT,
        ge::AttrValue::VT_LIST_BOOL,
        ge::AttrValue::VT_LIST_BYTES,
        ge::AttrValue::VT_LIST_TENSOR_DESC,
        ge::AttrValue::VT_LIST_TENSOR,
        ge::AttrValue::VT_LIST_GRAPH,
        ge::AttrValue::VT_LIST_NAMED_ATTRS
    };
    if (attrListDef_ != nullptr) {
        auto valueType = attrListDef_->val_type();
        if (valueType <= hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_NAMED_ATTRS &&
            valueType >= hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_NONE) {
            return listTypeDef[valueType];
        }
    }
    return ge::AttrValue::VT_NONE;
}

void ProtoAttrListDef::SetValueType(ge::AttrValue::ValueType type)
{
    static std::map<ge::AttrValue::ValueType, hiai::proto::AttrDef_ListValue_ListValueType> attrValListTypeMap = {
        {ge::AttrValue::VT_LIST_INT, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_INT},
        {ge::AttrValue::VT_LIST_FLOAT, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_FLOAT},
        {ge::AttrValue::VT_LIST_BOOL, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_BOOL},
        {ge::AttrValue::VT_LIST_STRING, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_STRING},
        {ge::AttrValue::VT_LIST_TENSOR, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_TENSOR},
        {ge::AttrValue::VT_LIST_TENSOR_DESC, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_TENSOR_DESC},
        {ge::AttrValue::VT_LIST_GRAPH, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_GRAPH},
        {ge::AttrValue::VT_LIST_BYTES, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_BYTES},
        {ge::AttrValue::VT_LIST_NAMED_ATTRS, hiai::proto::AttrDef_ListValue_ListValueType_VT_LIST_NAMED_ATTRS}};

    std::map<ge::AttrValue::ValueType, hiai::proto::AttrDef_ListValue_ListValueType>::const_iterator it =
        attrValListTypeMap.find(type);
    if (attrListDef_ != nullptr && it != attrValListTypeMap.end()) {
        attrListDef_->set_val_type(it->second);
    }
}

SerializeType ProtoAttrListDef::GetSerializeType() const
{
    return PROTOBUF;
}

ProtoAttrListDef::~ProtoAttrListDef()
{
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(tf);
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(t);
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(g);
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(na);

    if (isOwner_) {
        delete attrListDef_;
    }
    attrListDef_ = nullptr;
}

void ProtoAttrListDef::CopyFrom(const IAttrListDef* other)
{
    if (attrListDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *attrListDef_ = *(static_cast<const ProtoAttrListDef*>(other)->attrListDef_);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(tf);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(t);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(g);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(na);
    }
}

IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, std::string, s);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, int64_t, i, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, float, f, 1.0);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, bool, b, false);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, std::string, bt);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(
    ProtoAttrListDef, attrListDef_, INamedAttrDef, ProtoNamedAttrDef, na);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(
    ProtoAttrListDef, attrListDef_, ITensorDescDef, ProtoTensorDescDef, tf);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, ITensorDef, ProtoTensorDef, t);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ProtoAttrListDef, attrListDef_, IGraphDef, ProtoGraphDef, g);

} // namespace hiai