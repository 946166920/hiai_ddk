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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_TENSOR_DESC_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_TENSOR_DESC_DEF_H
#include "func_macro_def.h"
#include "graph/types.h"

namespace hiai {
class IShapeDef;
class IAttrMapDef;

class ITensorDescDef {
public:
    ITensorDescDef() = default;
    virtual ~ITensorDescDef() = default;

    ITensorDescDef(const ITensorDescDef&) = delete;
    ITensorDescDef& operator=(const ITensorDescDef&) = delete;

    virtual void CopyFrom(const ITensorDescDef* other) = 0;
    virtual SerializeType GetSerializeType() const = 0;

    virtual bool LoadFrom(const uint8_t* data, size_t len) = 0;
    virtual bool SaveTo(uint8_t* data, size_t len) = 0;
    virtual size_t GetTensorDescDefSize() const = 0;

    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, name);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ge::DataType, dtype);
    DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IShapeDef, shape);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ge::Format, layout);

    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, has_out_attr);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, size);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, weight_size);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, reuse_input);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, output_tensor);
    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, device_type);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(bool, input_tensor);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, real_dim_cnt);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, reuse_input_index);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, data_offset);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, cmps_size);
    DEF_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, cmps_tab);
    DEF_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(int64_t, cmps_tab_offset);
    DEF_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(IAttrMapDef, attr);
};

} // namespace hiai

#endif