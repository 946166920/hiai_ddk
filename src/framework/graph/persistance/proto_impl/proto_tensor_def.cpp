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
#include "proto_tensor_def.h"

#include "graph/persistance/proto_impl/proto_tensor_desc_def.h"

namespace hiai {
ProtoTensorDef::ProtoTensorDef() : ProtoTensorDef(new (std::nothrow) hiai::proto::TensorDef(), true)
{
}

ProtoTensorDef::ProtoTensorDef(hiai::proto::TensorDef* tensorDef, bool isOwner)
    : tensorDef_(tensorDef), isOwner_(isOwner)
{
}

ProtoTensorDef::~ProtoTensorDef()
{
    IMPL_PROTO_CUSTOM_MEMBER_FREE(desc);

    if (isOwner_) {
        delete tensorDef_;
    }
    tensorDef_ = nullptr;
}

void ProtoTensorDef::CopyFrom(const ITensorDef* other)
{
    if (tensorDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *tensorDef_ = *(static_cast<const ProtoTensorDef*>(other)->tensorDef_);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(desc);
    }
}

SerializeType ProtoTensorDef::GetSerializeType() const
{
    return PROTOBUF;
}

IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoTensorDef, tensorDef_, ITensorDescDef, ProtoTensorDescDef, desc);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoTensorDef, tensorDef_, std::string, data);

extern "C" GRAPH_API_EXPORT ITensorDef* CreateTensorDef() {
    return new (std::nothrow) ProtoTensorDef();
}

extern "C" GRAPH_API_EXPORT void DestroyTensorDef(ITensorDef* tensorDef) {
    delete tensorDef;
}

} // namespace hiai