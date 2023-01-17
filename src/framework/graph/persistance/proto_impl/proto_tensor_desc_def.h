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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_TENSOR_DESC_DEF_PROTO_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_TENSOR_DESC_DEF_PROTO_H
#include "proto_func_macro_def.h"
#include "proto_wrapper.h"
#include "graph/persistance/interface/tensor_desc_def.h"

namespace hiai {
class ProtoTensorDescDef : public ITensorDescDef {
public:
    ProtoTensorDescDef(hiai::proto::TensorDescriptor& tdDef);
    ~ProtoTensorDescDef() override;

private:
    void CopyFrom(const ITensorDescDef* other) override;
    SerializeType GetSerializeType() const override;

    bool LoadFrom(const uint8_t* data, size_t len) override;
    bool SaveTo(uint8_t* data, size_t len) override;
    size_t GetTensorDescDefSize() const override;

    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ge::DataType, dtype);
    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IShapeDef, shape);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ge::Format, layout);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, has_out_attr);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, size);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, weight_size);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, reuse_input);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, output_tensor);
    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, device_type);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, input_tensor);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, real_dim_cnt);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, reuse_input_index);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, data_offset);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, cmps_size);
    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, cmps_tab);
    DEF_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, cmps_tab_offset);
    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);

private:
    hiai::proto::TensorDescriptor& tdDef_;
};

class DefaultProtoTensorDescDef : private ProtoWrapper<hiai::proto::TensorDescriptor>, public ProtoTensorDescDef {
public:
    DefaultProtoTensorDescDef() : ProtoTensorDescDef(GetProto())
    {
    }
    ~DefaultProtoTensorDescDef() override = default;
};

} // namespace hiai

#endif