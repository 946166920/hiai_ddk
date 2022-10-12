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

#ifndef FRAMEWORK_GRAPH_PERSISTENCE_NAMED_ATTR_DEF_H
#define FRAMEWORK_GRAPH_PERSISTENCE_NAMED_ATTR_DEF_H
#include "func_macro_def.h"

namespace hiai {

class IAttrMapDef;

class INamedAttrDef {
public:
    INamedAttrDef() = default;
    virtual ~INamedAttrDef() = default;

    INamedAttrDef(const INamedAttrDef&) = delete;
    INamedAttrDef& operator=(const INamedAttrDef&) = delete;

    virtual void CopyFrom(const INamedAttrDef* other) = 0;
    virtual SerializeType GetSerializeType() const = 0;

    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);
};

} // namespace hiai

#endif