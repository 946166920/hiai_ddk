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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_TENSOR_DEF_PROTO_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_TENSOR_DEF_PROTO_H
#include "proto_func_macro_def.h"
#include "graph/persistance/interface/tensor_def.h"

namespace hiai {
class ProtoTensorDef : public ITensorDef {
public:
    ProtoTensorDef();
    ProtoTensorDef(hiai::proto::TensorDef* tensorDef, bool isOwner = false);
    ~ProtoTensorDef() override;

    void CopyFrom(const ITensorDef* other) override;
    SerializeType GetSerializeType() const override;

    DEF_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ITensorDescDef, desc);
    DEF_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(std::string, data);

private:
    hiai::proto::TensorDef* tensorDef_;
    bool isOwner_;
};
} // namespace hiai

#endif