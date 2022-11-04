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
#include "proto_graph_def.h"

#include "graph/persistance/proto_impl/proto_attr_map_def.h"
#include "graph/persistance/proto_impl/proto_op_def.h"

namespace hiai {
ProtoGraphDef::ProtoGraphDef() : ProtoGraphDef(new (std::nothrow) hiai::proto::GraphDef(), true)
{
}

ProtoGraphDef::ProtoGraphDef(hiai::proto::GraphDef* graphDef, bool isOwner) : graphDef_(graphDef), isOwner_(isOwner)
{
}

ProtoGraphDef::~ProtoGraphDef()
{
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(op);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);

    if (isOwner_) {
        delete graphDef_;
    }
    graphDef_ = nullptr;
}

void ProtoGraphDef::CopyFrom(const IGraphDef* other)
{
    if (graphDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *graphDef_ = *(static_cast<const ProtoGraphDef*>(other)->graphDef_);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(op);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
    }
}

SerializeType ProtoGraphDef::GetSerializeType() const
{
    return PROTOBUF;
}

bool ProtoGraphDef::SaveTo(uint8_t* data, size_t len) const
{
    if (graphDef_ != nullptr) {
        return graphDef_->SerializeToArray(data, len);
    }
    return false;
}

bool ProtoGraphDef::LoadFrom(const uint8_t* data, size_t len)
{
    if (data == nullptr || len == 0) {
        return false;
    }
    if (graphDef_ == nullptr) {
        return false;
    }
    google::protobuf::io::CodedInputStream coded_stream(data, len);
    coded_stream.SetTotalBytesLimit(INT32_MAX);
    return graphDef_->ParseFromCodedStream(&coded_stream);
}

size_t ProtoGraphDef::GetGraphDefSize() const
{
    if (graphDef_ == nullptr) {
        return 0;
    }
#if GOOGLE_PROTOBUF_VERSION < 3013000
    return graphDef_->ByteSize();
#else
    return graphDef_->ByteSizeLong();
#endif
}

bool ProtoGraphDef::Swap(IGraphDef* other)
{
    if (graphDef_ == nullptr || other == nullptr ||
        other->GetSerializeType() != PROTOBUF) {
        return false;
    }
    graphDef_->Swap(static_cast<ProtoGraphDef*>(other)->graphDef_);
    return true;
}

IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoGraphDef, graphDef_, std::string, name);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoGraphDef, graphDef_, std::string, input);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoGraphDef, graphDef_, std::string, output);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ProtoGraphDef, graphDef_, IOpDef, ProtoOpDef, op);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoGraphDef, graphDef_, IAttrMapDef, ProtoAttrMapDef, attr);

extern "C" GRAPH_API_EXPORT IGraphDef* CreateGraphDef()
{
    return new (std::nothrow) ProtoGraphDef();
}

extern "C" GRAPH_API_EXPORT void DestroyGraphDef(IGraphDef* graphDef)
{
    delete graphDef;
}
} // namespace hiai