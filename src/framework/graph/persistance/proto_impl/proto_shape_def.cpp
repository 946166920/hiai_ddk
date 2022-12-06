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
#include "proto_shape_def.h"
#include "graph/graph_api_export.h"
namespace hiai {
ProtoShapeDef::ProtoShapeDef() : ProtoShapeDef(new (std::nothrow) hiai::proto::ShapeDef(), true)
{
}

ProtoShapeDef::ProtoShapeDef(hiai::proto::ShapeDef* shapeDef, bool isOwner) : shapeDef_(shapeDef), isOwner_(isOwner)
{
}

ProtoShapeDef::~ProtoShapeDef()
{
    if (isOwner_) {
        delete shapeDef_;
    }
    shapeDef_ = nullptr;
}

void ProtoShapeDef::CopyFrom(const IShapeDef* other)
{
    if (shapeDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *shapeDef_ = *(static_cast<const ProtoShapeDef*>(other)->shapeDef_);
    }
}

SerializeType ProtoShapeDef::GetSerializeType() const
{
    return PROTOBUF;
}

IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoShapeDef, shapeDef_, int64_t, dim, 0);

extern "C" GRAPH_API_EXPORT IShapeDef* CreateShapeDef()
{
    return new (std::nothrow) ProtoShapeDef();
}

extern "C" GRAPH_API_EXPORT void DestroyShapeDef(IShapeDef* shapeDef)
{
    delete shapeDef;
}

} // namespace hiai