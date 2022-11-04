/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the AttrHolder operation class
 * @file attributes_holder.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */

#include "graph/attributes_holder.h"
#include "graph/attr_value.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/interface/attr_def.h"

#include "infra/base/assertion.h"

namespace ge {
GraphErrCodeStatus AttrHolder::SetAttr(const std::string& name, const AttrValue& value)
{
    HIAI_EXPECT_TRUE(!value.IsEmpty());

    hiai::IAttrDef* attrDef = MutableAttr(name);
    HIAI_EXPECT_NOT_NULL(attrDef);

    auto oldType = attrDef->GetValueType();
    auto newType = value.GetValueType();
    if (oldType != newType && oldType != AttrValue::VT_NONE) {
        return GRAPH_FAILED;
    }

    HIAI_EXPECT_TRUE(value.SerializeTo(attrDef));
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus AttrHolder::GetAttr(const std::string& name, AttrValue& value) const
{
    const hiai::IAttrDef* attrDef = GetAttr(name);
    if (attrDef == nullptr) {
        return GRAPH_FAILED;
    }

    value = AttrValue(const_cast<hiai::IAttrDef*>(attrDef), false);
    return GRAPH_SUCCESS;
}

hiai::IAttrDef* AttrHolder::MutableAttr(const string& name)
{
    auto mapDef = MutableAttrMapDef();
    HIAI_EXPECT_NOT_NULL_R(mapDef, nullptr);

    return mapDef->mutable_attr(name);
}

const hiai::IAttrDef* AttrHolder::GetAttr(const string& name) const
{
    const auto mapDef = GetAttrMapDef();
    HIAI_EXPECT_NOT_NULL_R(mapDef, nullptr);

    return mapDef->attr(name);
}

bool AttrHolder::HasAttr(const std::string& name) const
{
    const auto mapDef = GetAttrMapDef();
    HIAI_EXPECT_NOT_NULL_R(mapDef, false);

    return GetAttrMapDef()->has_attr(name);
}

GraphErrCodeStatus AttrHolder::DelAttr(const std::string& name)
{
    auto mapDef = MutableAttrMapDef();
    HIAI_EXPECT_NOT_NULL(mapDef);

    mapDef->del_attr(name);
    return GRAPH_SUCCESS;
}

const std::map<string, AttrValue> AttrHolder::GetAllAttrs() const
{
    std::map<string, AttrValue> valueMap;
    auto& defMap = GetAttrMapDef()->attr();
    for (auto it = defMap.begin(); it != defMap.end(); it++) {
        valueMap.emplace(std::piecewise_construct, std::forward_as_tuple(it->first),
            std::forward_as_tuple(it->second, false));
    }

    return valueMap;
}
} // namespace ge
