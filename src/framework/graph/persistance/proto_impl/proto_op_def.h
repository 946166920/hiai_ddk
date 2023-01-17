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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_OP_DEF_PROTO_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_OP_DEF_PROTO_H
#include "graph/persistance/interface/op_def.h"
#include "proto_wrapper.h"
#include "proto_func_macro_def.h"

namespace hiai {

class ProtoOpDef : public IOpDef {
public:
    ProtoOpDef(hiai::proto::OpDef& opDef);
    ~ProtoOpDef() override;

private:
    void CopyFrom(const IOpDef* other) override;
    SerializeType GetSerializeType() const override;

    bool LoadFrom(const uint8_t* data, size_t len) override;
    bool SaveTo(uint8_t* data, size_t len) override;
    size_t GetOpDefSize() const override;

    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, type);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, input);
    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, has_out_attr);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, id);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, stream_id);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, input_name);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, src_name);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, src_index);
    DEF_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(std::string, dst_name);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, dst_index);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, input_i);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, output_i);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, workspace);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(int64_t, workspace_bytes);
    DEF_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(bool, is_input_const);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, input_desc);
    DEF_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ITensorDescDef, output_desc);

private:
    hiai::proto::OpDef& opDef_;
};

class DefaultProtoOpDef : private ProtoWrapper<hiai::proto::OpDef>, public ProtoOpDef {
public:
    DefaultProtoOpDef() : ProtoOpDef(GetProto())
    {
    }
    ~DefaultProtoOpDef() override = default;
};
} // namespace hiai

#endif