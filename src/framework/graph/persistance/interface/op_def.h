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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_OP_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_OP_DEF_H
#include <string>
#include "func_macro_def.h"

namespace hiai {
class IAttrMapDef;
class ITensorDescDef;

class IOpDef {
public:
    IOpDef() = default;
    virtual ~IOpDef() = default;

    IOpDef(const IOpDef&) = delete;
    IOpDef& operator=(const IOpDef&) = delete;

    virtual void CopyFrom(const IOpDef* other) = 0;
    virtual SerializeType GetSerializeType() const = 0;

    virtual bool LoadFrom(const uint8_t* data, size_t len) = 0;
    virtual bool SaveTo(uint8_t* data, size_t len) = 0;
    virtual size_t GetOpDefSize() const = 0;

    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, type);

    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, input);
    DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);

    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, has_out_attr);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, id);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, stream_id);
    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, input_name);
    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, src_name);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, src_index);
    DEF_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, dst_name);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, dst_index);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, input_i);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, output_i);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, workspace);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, workspace_bytes);
    DEF_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(bool, is_input_const);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, input_desc);
    DEF_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, output_desc);
};

} // namespace hiai

#endif