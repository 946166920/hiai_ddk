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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_ATTR_LIST_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_ATTR_LIST_DEF_H
#include "graph/persistance/interface/attr_list_def.h"
#include "proto_func_macro_def.h"

namespace hiai {
class ProtoAttrListDef : public IAttrListDef {
public:
    ProtoAttrListDef();
    ProtoAttrListDef(hiai::proto::AttrDef_ListValue* attrListDef, bool isOwner = false);
    ~ProtoAttrListDef() override;

    ge::AttrValue::ValueType GetValueType() const override;
    void SetValueType(ge::AttrValue::ValueType type) override;

    void CopyFrom(const IAttrListDef* other) override;
    SerializeType GetSerializeType() const override;

    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, s);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, i);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(float, f);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(bool, b);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, bt);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDef, t);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(IGraphDef, g);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(INamedAttrDef, na);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, tf);

private:
    hiai::proto::AttrDef_ListValue* attrListDef_;
    bool isOwner_;
};

} // namespace hiai

#endif