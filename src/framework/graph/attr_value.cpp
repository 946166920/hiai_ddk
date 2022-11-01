/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the AttrValue operation class
 * @file attr_value.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */
#include "graph/attr_value.h"

#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_serializer.h"

#include "graph/persistance/proxy/proto_factory.h"
#include "graph/persistance/interface/attr_def.h"
#include "graph/persistance/interface/attr_list_def.h"
#include "graph/persistance/interface/named_attr_def.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/interface/graph_def.h"

using namespace std;

namespace ge {
static std::string STR_EMPTY = "";

AttrValue::NamedAttrs::NamedAttrs()
    : AttrValue::NamedAttrs(hiai::ProtoFactory::Instance()->CreateNamedAttrDef(), true)
{
}

AttrValue::NamedAttrs::NamedAttrs(hiai::INamedAttrDef* namedDef, bool isOwner) : namedDef_(namedDef), isOwner_(isOwner)
{
}

AttrValue::NamedAttrs::~NamedAttrs()
{
    if (isOwner_ && namedDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyNamedAttrDef(namedDef_);
    }
    namedDef_ = nullptr;
}

const hiai::IAttrMapDef* AttrValue::NamedAttrs::GetAttrMapDef() const
{
    return namedDef_ != nullptr ? namedDef_->attr() : nullptr;
}

hiai::IAttrMapDef* AttrValue::NamedAttrs::MutableAttrMapDef()
{
    return namedDef_ != nullptr ? namedDef_->mutable_attr() : nullptr;
}

AttrValue::NamedAttrs::NamedAttrs(const NamedAttrs& other) : AttrValue::NamedAttrs()
{
    if (namedDef_ != nullptr) {
        namedDef_->CopyFrom(other.namedDef_);
    }
}

AttrValue::NamedAttrs& AttrValue::NamedAttrs::operator=(const AttrValue::NamedAttrs& other)
{
    if (&other != this) {
        namedDef_->CopyFrom(other.namedDef_);
    }
    return *this;
}

void AttrValue::NamedAttrs::SetName(const std::string& name)
{
    if (namedDef_ != nullptr) {
        namedDef_->set_name(name);
    }
}

const string& AttrValue::NamedAttrs::GetName() const
{
    return namedDef_ != nullptr ? namedDef_->name() : STR_EMPTY;
}

const AttrValue& AttrValue::NamedAttrs::GetItem(const string& key) const
{
    static AttrValue nil(nullptr, false);
    HIAI_EXPECT_NOT_NULL_R(namedDef_, nil);
    HIAI_EXPECT_NOT_NULL_R(GetAttrMapDef(), nil);

    std::map<string, AttrValue>::const_iterator it = attrMap_.find(key);
    if (it != attrMap_.end()) {
        return it->second;
    }

    const hiai::IAttrDef* valueDef = GetAttrMapDef()->attr(key);
    HIAI_EXPECT_NOT_NULL_R(valueDef, nil);

    auto ret = attrMap_.emplace(std::piecewise_construct, std::forward_as_tuple(key),
        std::forward_as_tuple(const_cast<hiai::IAttrDef*>(valueDef), false));

    return ret.first->second;
}

bool AttrValue::NamedAttrs::SerializeTo(hiai::INamedAttrDef* to) const
{
    if (to != nullptr) {
        to->CopyFrom(namedDef_);
        return true;
    }
    return false;
}

AttrValue::AttrValue() : AttrValue(hiai::ProtoFactory::Instance()->CreateAttrDef(), true)
{
}

AttrValue::AttrValue(hiai::IAttrDef* attrDef, bool isOwner) : attrDef_(attrDef), isOwner_(isOwner)
{
    if (attrDef_ != nullptr) {
        type_ = attrDef_->GetValueType();
    }
}

AttrValue::~AttrValue()
{
    if (isOwner_ && attrDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyAttrDef(attrDef_);
    }
    attrDef_ = nullptr;
}

AttrValue::AttrValue(const AttrValue& other) : AttrValue()
{
    if (attrDef_ != nullptr) {
        attrDef_->CopyFrom(other.attrDef_);
        type_ = other.type_;
    }
}

AttrValue& AttrValue::operator=(const AttrValue& other)
{
    if (&other == this) {
        return *this;
    }
    if (attrDef_ != nullptr) {
        attrDef_->CopyFrom(other.attrDef_);
        type_ = other.type_;
    }
    return *this;
}

bool AttrValue::SerializeTo(hiai::IAttrDef* to) const
{
    if (to != nullptr) {
        to->CopyFrom(attrDef_);
        return true;
    }
    return false;
}

bool AttrValue::SetInt(int64_t val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_INT || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    attrDef_->set_i(val);
    type_ = VT_INT;
    return true;
}

int64_t AttrValue::GetInt() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, 0);
    return attrDef_->i();
}

AttrValue AttrValue::CreateFrom(int64_t val)
{
    AttrValue v;
    v.SetInt(val);
    return v;
}

bool AttrValue::SetFloat(float val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_FLOAT || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    attrDef_->set_f(val);
    type_ = VT_FLOAT;
    return true;
}

float AttrValue::GetFloat() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, 0.0);
    return attrDef_->f();
}

AttrValue AttrValue::CreateFrom(float val)
{
    AttrValue v;
    v.SetFloat(val);
    return v;
}

bool AttrValue::SetBool(bool val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_BOOL || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    attrDef_->set_b(val);
    type_ = VT_BOOL;
    return true;
}

bool AttrValue::GetBool() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);
    return attrDef_->b();
}

AttrValue AttrValue::CreateFrom(bool val)
{
    AttrValue v;
    v.SetBool(val);
    return v;
}

bool AttrValue::SetString(const std::string& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_STRING || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    attrDef_->set_s(val);
    type_ = VT_STRING;
    return true;
}

const std::string& AttrValue::GetString() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, STR_EMPTY);
    return attrDef_->s();
}

AttrValue AttrValue::CreateFrom(const std::string& val)
{
    AttrValue v;
    v.SetString(val);
    return v;
}

bool AttrValue::SetTensor(const TensorPtr& val)
{
    HIAI_EXPECT_NOT_NULL_R(val, false);
    HIAI_EXPECT_TRUE_R(type_ == VT_TENSOR || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    HIAI_EXPECT_TRUE_R(val->SerializeTo(attrDef_->mutable_t()), false);
    type_ = VT_TENSOR;
    return true;
}

const TensorPtr AttrValue::GetTensor() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, nullptr);
    return hiai::make_shared_nothrow<Tensor>(attrDef_->mutable_t(), false);
}

AttrValue AttrValue::CreateFrom(const TensorPtr& val)
{
    AttrValue v;
    v.SetTensor(val);
    return v;
}

bool AttrValue::SetNamedAttrs(const AttrValue::NamedAttrs& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_NAMED_ATTRS || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    HIAI_EXPECT_TRUE_R(val.SerializeTo(attrDef_->mutable_func()), false);
    type_ = VT_NAMED_ATTRS;
    return true;
}

AttrValue::NamedAttrs AttrValue::GetNamedAttrs() const
{
    hiai::INamedAttrDef* na = nullptr;
    if (attrDef_ != nullptr) {
        na = attrDef_->mutable_func();
    }
    return NamedAttrs(na, false);
}

AttrValue AttrValue::CreateFrom(const AttrValue::NamedAttrs& val)
{
    AttrValue v;
    v.SetNamedAttrs(val);
    return v;
}

bool AttrValue::SetGraph(const ComputeGraphPtr& val)
{
    HIAI_EXPECT_NOT_NULL_R(val, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);
    HIAI_EXPECT_TRUE_R(type_ == VT_GRAPH || type_ == VT_NONE, false);

    auto attrGraphDef = attrDef_->mutable_g();
    HIAI_EXPECT_NOT_NULL_R(attrGraphDef, false);

    auto swapGraphDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    HIAI_EXPECT_NOT_NULL_R(val, false);

    if (!val->ROLE(GraphSerializer).SerializeTo(swapGraphDef)) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(swapGraphDef);
        return false;
    }

    bool ret = attrGraphDef->Swap(swapGraphDef);
    hiai::ProtoFactory::Instance()->DestroyGraphDef(swapGraphDef);
    type_ = VT_GRAPH;
    return ret;
}

const ComputeGraphPtr AttrValue::GetGraph() const
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, nullptr);

    auto attrGraphDef = attrDef_->mutable_g();
    HIAI_EXPECT_NOT_NULL_R(attrGraphDef, nullptr);

    hiai::IGraphDef* swapGraphDef = hiai::ProtoFactory::Instance()->CreateGraphDef();
    HIAI_EXPECT_NOT_NULL_R(swapGraphDef, nullptr);

    if (!swapGraphDef->Swap(attrGraphDef)) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(swapGraphDef);
        return nullptr;
    }

    auto graph = ComputeGraph::Make(swapGraphDef, true);
    if (graph == nullptr) {
        hiai::ProtoFactory::Instance()->DestroyGraphDef(swapGraphDef);
        return nullptr;
    }

    if (!graph->ROLE(GraphSerializer).UnSerialize()) {
        return nullptr;
    }

    return graph;
}

bool AttrValue::SetBuffer(const Buffer& val)
{
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);
    HIAI_EXPECT_TRUE_R(type_ == VT_BYTES || type_ == VT_NONE, false);

    attrDef_->set_bt(std::string(reinterpret_cast<const char*>(val.GetData()), val.GetSize()));

    type_ = VT_BYTES;
    return true;
}

Buffer AttrValue::GetBuffer() const
{
    string* str = nullptr;
    if (attrDef_ != nullptr) {
        str = attrDef_->mutable_bt();
    }
    return Buffer(str, false);
}

AttrValue AttrValue::CreateFrom(const Buffer& val)
{
    AttrValue v;
    v.SetBuffer(val);
    return v;
}

bool AttrValue::SetTensorDesc(const TensorDesc& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_TENSOR_DESC || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    HIAI_EXPECT_TRUE_R(val.SerializeTo(attrDef_->mutable_td()), false);
    type_ = VT_TENSOR_DESC;
    return true;
}

TensorDesc AttrValue::GetTensorDesc() const
{
    hiai::ITensorDescDef* td = nullptr;
    if (attrDef_ != nullptr) {
        td = attrDef_->mutable_td();
    }
    return TensorDesc(td, false);
}

AttrValue AttrValue::CreateFrom(const TensorDesc& val)
{
    AttrValue v;
    v.SetTensorDesc(val);
    return v;
}

bool AttrValue::SetIntList(const std::vector<int64_t>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_INT || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_i();
    for (auto i : val) {
        listDef->add_i(i);
    }
    listDef->SetValueType(VT_LIST_INT);
    type_ = VT_LIST_INT;
    return true;
}

const std::vector<int64_t> AttrValue::GetIntList() const
{
    std::vector<int64_t> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->i_size(); i++) {
            result.push_back(listDef->i(i));
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<int64_t>& val)
{
    AttrValue v;
    v.SetIntList(val);
    return v;
}

bool AttrValue::SetFloatList(const std::vector<float>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_FLOAT || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_f();
    for (auto f : val) {
        listDef->add_f(f);
    }
    listDef->SetValueType(VT_LIST_FLOAT);
    type_ = VT_LIST_FLOAT;
    return true;
}

const std::vector<float> AttrValue::GetFloatList() const
{
    std::vector<float> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->f_size(); i++) {
            result.push_back(listDef->f(i));
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<float>& val)
{
    AttrValue v;
    v.SetFloatList(val);
    return v;
}

bool AttrValue::SetBoolList(const std::vector<bool>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_BOOL || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_b();
    for (auto b : val) {
        listDef->add_b(b);
    }
    listDef->SetValueType(VT_LIST_BOOL);
    type_ = VT_LIST_BOOL;
    return true;
}

const std::vector<bool> AttrValue::GetBoolList() const
{
    std::vector<bool> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->b_size(); i++) {
            result.push_back(listDef->b(i));
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<bool>& val)
{
    AttrValue v;
    v.SetBoolList(val);
    return v;
}

bool AttrValue::SetStringList(const std::vector<std::string>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_STRING || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_s();
    for (const auto& s : val) {
        listDef->add_s(s);
    }
    listDef->SetValueType(VT_LIST_STRING);
    type_ = VT_LIST_STRING;
    return true;
}

const std::vector<std::string> AttrValue::GetStringList() const
{
    std::vector<std::string> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->s_size(); i++) {
            result.push_back(listDef->s(i));
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<std::string>& val)
{
    AttrValue v;
    v.SetStringList(val);
    return v;
}

bool AttrValue::SetTensorList(const std::vector<TensorPtr>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_TENSOR || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_t();
    for (auto t : val) {
        if (t == nullptr || !t->SerializeTo(listDef->add_t())) {
            return false;
        }
    }
    listDef->SetValueType(VT_LIST_TENSOR);
    type_ = VT_LIST_TENSOR;
    return true;
}

const std::vector<TensorPtr> AttrValue::GetTensorList() const
{
    std::vector<TensorPtr> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->t_size(); i++) {
            auto tensorPtr = hiai::make_shared_nothrow<Tensor>(listDef->mutable_t(i), false);
            result.push_back(tensorPtr);
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<TensorPtr>& val)
{
    AttrValue v;
    v.SetTensorList(val);
    return v;
}

bool AttrValue::SetTensorDescList(const std::vector<TensorDesc>& val)
{
    HIAI_EXPECT_TRUE_R(type_ == VT_LIST_TENSOR_DESC || type_ == VT_NONE, false);
    HIAI_EXPECT_NOT_NULL_R(attrDef_, false);

    auto listDef = attrDef_->mutable_list();
    HIAI_EXPECT_NOT_NULL_R(listDef, false);

    listDef->clear_tf();
    for (auto& v : val) {
        if (!v.SerializeTo(listDef->add_tf())) {
            return false;
        }
    }
    attrDef_->SetValueType(VT_LIST_TENSOR_DESC);
    type_ = VT_LIST_TENSOR_DESC;
    return true;
}

const std::vector<TensorDesc> AttrValue::GetTensorDescList() const
{
    std::vector<TensorDesc> result;
    HIAI_EXPECT_NOT_NULL_R(attrDef_, result);

    auto listDef = attrDef_->mutable_list();
    if (listDef != nullptr) {
        for (size_t i = 0; i < listDef->tf_size(); i++) {
            result.emplace_back(TensorDesc(listDef->mutable_tf(i), false));
        }
    }
    return result;
}

AttrValue AttrValue::CreateFrom(const std::vector<TensorDesc>& val)
{
    AttrValue v;
    v.SetTensorDescList(val);
    return v;
}

AttrValue::ValueType AttrValue::GetValueType() const
{
    return type_;
}

bool AttrValue::IsEmpty() const
{
    return type_ == VT_NONE;
}
} // namespace ge
