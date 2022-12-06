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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_ATTR_LIST_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_ATTR_LIST_DEF_H
#include <string>
#include "func_macro_def.h"
#include "graph/attr_value.h"

namespace hiai {
class ITensorDescDef;
class ITensorDef;
class IGraphDef;
class INamedAttrDef;

class IAttrListDef {
public:
    IAttrListDef() = default;
    virtual ~IAttrListDef() = default;

    IAttrListDef(const IAttrListDef&) = delete;
    IAttrListDef& operator=(const IAttrListDef&) = delete;

    virtual ge::AttrValue::ValueType GetValueType() const = 0;
    virtual void SetValueType(ge::AttrValue::ValueType type) = 0;

    virtual void CopyFrom(const IAttrListDef* other) = 0;
    virtual SerializeType GetSerializeType() const = 0;

    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, s);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, i);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(float, f);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(bool, b);
    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, bt);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, tf);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDef, t);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(IGraphDef, g);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(INamedAttrDef, na);
};
} // namespace hiai

#endif