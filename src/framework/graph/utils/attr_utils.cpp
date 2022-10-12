/**
* Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* Apache License for more details at
* http://www.apache.org/licenses/LICENSE-2.0
* Description: The description of the attr operation class
* @file attr_value.cpp
*
* @brief
*
* @version 1.0
*
*/
#include "framework/graph/utils/attr_utils.h"

#include "infra/base/securestl.h"

#include "graph/tensor.h"
#include "graph/attr_value.h"
#include "framework/graph/debug/ge_log.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "graph/persistance/interface/attr_def.h"
#include "graph/persistance/interface/attr_list_def.h"
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/proxy/proto_factory.h"

using namespace std;

namespace ge {
#define ATTR_UTILS_SET_IMP(FuncName, SetImpl, Type) \
    GRAPH_API_EXPORT bool AttrUtils::Set##FuncName(AttrHolderAdapter&& obj, const string& name, const Type& value) \
    { \
        if (obj.get() == nullptr) { \
            return false; \
        } \
        hiai::IAttrDef* attrDef = obj->MutableAttr(name); \
        if (attrDef == nullptr) { \
            return false; \
        } \
        return AttrValue(attrDef, false).SetImpl(value); \
    }

#define ATTR_UTILS_GET_IMP(FuncName, GetImpl, ValueType, Type) \
    GRAPH_API_EXPORT bool AttrUtils::Get##FuncName(ConstAttrHolderAdapter&& obj, const string& name, Type& value) \
    { \
        if (obj.get() == nullptr) { \
            return false; \
        } \
        const hiai::IAttrDef* attrDef = obj->GetAttr(name); \
        if (attrDef == nullptr) { \
            return false; \
        } \
        if (attrDef->GetValueType() != ValueType) { \
            return false; \
        } \
        value = AttrValue(const_cast<hiai::IAttrDef*>(attrDef), false).GetImpl(); \
        return true; \
    }

#define ATTR_UTILS_SET_GET_IMP(FuncName, SetImpl, GetImpl, ValueType, Type) \
    ATTR_UTILS_SET_IMP(FuncName, SetImpl, Type) \
    ATTR_UTILS_GET_IMP(FuncName, GetImpl, ValueType, Type)

ATTR_UTILS_SET_GET_IMP(Int, SetInt, GetInt, AttrValue::VT_INT, int64_t)
ATTR_UTILS_SET_GET_IMP(Float, SetFloat, GetFloat, AttrValue::VT_FLOAT, float)
ATTR_UTILS_SET_GET_IMP(Bool, SetBool, GetBool, AttrValue::VT_BOOL, bool)
ATTR_UTILS_SET_GET_IMP(Str, SetString, GetString, AttrValue::VT_STRING, string)
ATTR_UTILS_SET_GET_IMP(TensorDesc, SetTensorDesc, GetTensorDesc, AttrValue::VT_TENSOR_DESC, TensorDesc)
ATTR_UTILS_SET_GET_IMP(NamedAttrs, SetNamedAttrs, GetNamedAttrs, AttrValue::VT_NAMED_ATTRS, AttrValue::NamedAttrs)
ATTR_UTILS_SET_GET_IMP(Graph, SetGraph, GetGraph, AttrValue::VT_GRAPH, ComputeGraphPtr)
ATTR_UTILS_SET_GET_IMP(Tensor, SetTensor, GetTensor, AttrValue::VT_TENSOR, TensorPtr)
ATTR_UTILS_SET_GET_IMP(Bytes, SetBuffer, GetBuffer, AttrValue::VT_BYTES, Buffer)
ATTR_UTILS_SET_GET_IMP(ListInt, SetIntList, GetIntList, AttrValue::VT_LIST_INT, vector<int64_t>)
ATTR_UTILS_SET_GET_IMP(ListFloat, SetFloatList, GetFloatList, AttrValue::VT_LIST_FLOAT, vector<float>)
ATTR_UTILS_SET_GET_IMP(ListBool, SetBoolList, GetBoolList, AttrValue::VT_LIST_BOOL, vector<bool>)
ATTR_UTILS_SET_GET_IMP(ListStr, SetStringList, GetStringList, AttrValue::VT_LIST_STRING, vector<string>)
ATTR_UTILS_SET_GET_IMP(ListTensor, SetTensorList, GetTensorList, AttrValue::VT_LIST_TENSOR, vector<TensorPtr>)

bool AttrUtils::GetInt(ConstAttrHolderAdapter&& obj, const string& name, int32_t& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    int64_t int64Val = 0;
    if (!AttrUtils::GetInt(std::move(obj), name, int64Val)) {
        return false;
    }
    if (int64Val < INT32_MIN || int64Val > INT32_MAX) {
        FMK_LOGE("%jd int64_t value cannot cast to int32_t", int64Val);
        return false;
    }
    value = static_cast<int32_t>(int64Val);
    return true;
}

bool AttrUtils::GetInt(ConstAttrHolderAdapter&& obj, const string& name, uint32_t& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    int64_t int64Val = 0;
    if (!AttrUtils::GetInt(std::move(obj), name, int64Val)) {
        return false;
    }
    if (int64Val < 0 || int64Val > UINT32_MAX) {
        FMK_LOGE("%jd int64_t value cannot cast to uint32_t", int64Val);
        return false;
    }
    value = static_cast<uint32_t>(int64Val);
    return true;
}

bool AttrUtils::GetListInt(ConstAttrHolderAdapter&& obj, const string& name, vector<int32_t>& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    vector<int64_t> int64List;
    if (!AttrUtils::GetListInt(std::move(obj), name, int64List)) {
        return false;
    }
    for (size_t i = 0; i < int64List.size(); ++i) {
        if (int64List[i] < INT32_MIN || int64List[i] > INT32_MAX) {
            FMK_LOGE("index %zu %jd int64_t value cannot cast to int32_t", i, int64List[i]);
            return false;
        }
    }
    value.clear();
    value.insert(value.cbegin(), int64List.cbegin(), int64List.cend());
    return true;
}

bool AttrUtils::GetListInt(ConstAttrHolderAdapter&& obj, const string& name, vector<uint32_t>& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    vector<int64_t> int64List;
    if (!GetListInt(std::move(obj), name, int64List)) {
        return false;
    }
    for (size_t i = 0; i < int64List.size(); ++i) {
        if (int64List[i] < 0 || int64List[i] > UINT32_MAX) {
            FMK_LOGE("index %zu %jd int64_t value cannot cast to uint32_t", i, int64List[i]);
            return false;
        }
    }
    value.clear();
    value.insert(value.cbegin(), int64List.cbegin(), int64List.cend());
    return true;
}

bool AttrUtils::SetListInt(AttrHolderAdapter&& obj, const string& name, const vector<uint32_t>& value)
{
    std::vector<int64_t> tmp;
    for (auto i : value) {
        tmp.push_back(i);
    }
    return AttrUtils::SetListInt(std::move(obj), name, tmp);
}

bool AttrUtils::SetListInt(AttrHolderAdapter&& obj, const string& name, const vector<int32_t>& value)
{
    std::vector<int64_t> tmp;
    for (auto i : value) {
        tmp.push_back(i);
    }
    return AttrUtils::SetListInt(std::move(obj), name, tmp);
}

bool AttrUtils::MutableTensor(AttrHolderAdapter&& obj, const string& name, TensorPtr& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    const hiai::IAttrDef* v = obj->GetAttr(name);
    if (v == nullptr) {
        return false;
    }
    if (v->GetValueType() != AttrValue::VT_TENSOR) {
        return false;
    }
    value = hiai::make_shared_nothrow<Tensor>(const_cast<hiai::IAttrDef*>(v)->mutable_t(), false);
    return true;
}

bool AttrUtils::MutableListTensor(AttrHolderAdapter&& obj, const string& name, vector<TensorPtr>& value)
{
    if (obj.get() == nullptr) {
        return false;
    }
    const hiai::IAttrDef* v = obj->GetAttr(name);
    if (v == nullptr) {
        return false;
    }
    if (v->GetValueType() != AttrValue::VT_LIST_TENSOR) {
        return false;
    }
    auto listDef = const_cast<hiai::IAttrDef*>(v)->mutable_list();
    if (listDef == nullptr) {
        return false;
    }
    value.clear();
    for (size_t i = 0; i < listDef->t_size(); i++) {
        auto tensorPtr = hiai::make_shared_nothrow<Tensor>(listDef->mutable_t(i), false);
        if (tensorPtr == nullptr) {
            return false;
        }
        value.push_back(tensorPtr);
    }
    return true;
}

OpDescPtr AttrUtils::CloneOpDesc(const OpDescPtr& orgOpDesc)
{
    if (!orgOpDesc) {
        return nullptr;
    }
    return orgOpDesc->Clone();
}
} // namespace ge