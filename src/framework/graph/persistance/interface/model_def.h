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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_MODEL_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_MODEL_DEF_H
#include "func_macro_def.h"

namespace hiai {
class IGraphDef;
class IAttrMapDef;

class IModelDef {
public:
    IModelDef() = default;
    virtual ~IModelDef() = default;

    IModelDef(const IModelDef&) = delete;
    IModelDef& operator=(const IModelDef&) = delete;

    virtual SerializeType GetSerializeType() const = 0;

    virtual bool LoadFrom(const uint8_t* data, size_t len) = 0;
    virtual bool SaveTo(uint8_t* data, size_t len) const = 0;
    virtual size_t GetModelDefSize() const = 0;

    virtual bool Dump(const std::string& file) const = 0;

    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, version);
    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, custom_version);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(IGraphDef, graph);
    DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);
};
} // namespace hiai

#endif
