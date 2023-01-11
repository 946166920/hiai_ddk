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
#include "proto_op_def.h"

#include <functional>

#include "graph/persistance/proto_impl/proto_attr_map_def.h"
#include "graph/persistance/proto_impl/proto_tensor_desc_def.h"

namespace hiai {
namespace {
const static std::vector<std::string> NEED_ADD_INPUTDESC = {
    "LSTM",
    "BidirectionLSTM",
    "RNN",
    "NonMaxSuppressionV6",
};

void OptionalInputHandle(hiai::proto::OpDef& opDef)
{
    auto it = std::find(NEED_ADD_INPUTDESC.begin(), NEED_ADD_INPUTDESC.end(), opDef.type());
    if (it == NEED_ADD_INPUTDESC.end()) {
        return;
    }

    if (opDef.input_size() > opDef.input_desc_size()) {
        // copy original input desc
        auto orgInputDesc = opDef.input_desc();
        // clear original input desc
        opDef.clear_input_desc();
        // add new input desc
        for (int i = 0, j = 0; i < opDef.input_size(); i++) {
            if (opDef.input(i) == "") {
                ::hiai::proto::TensorDescriptor td;
                td.set_has_out_attr(true);
                *opDef.add_input_desc() = td;
            } else {
                *opDef.add_input_desc() = orgInputDesc.Get(j);
                j++;
            }
        }
    }
}

using ATTR_MAP_FUNC = std::function<void(hiai::proto::OpDef&, const hiai::proto::AttrDef&)>;

const static std::map<std::string, ATTR_MAP_FUNC> OPDEF_COMPATIBLE_MAP = {
    {"id", [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) { opDef.set_id(attrDef.i()); }},
    {"stream_id",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) { opDef.set_stream_id(attrDef.i()); }},
    {"input_name",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (const auto& s : attrDef.list().s()) {
                opDef.add_input_name(s);
            }
        }},
    {"src_name",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (const auto& s : attrDef.list().s()) {
                opDef.add_src_name(s);
            }
        }},
    {"src_index",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_src_index(i);
            }
        }},
    {"input",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_input_i(i);
            }
        }},
    {"output",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_output_i(i);
            }
        }},
    {"dst_name",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (const auto& s : attrDef.list().s()) {
                opDef.add_dst_name(s);
            }
        }},
    {"dst_index",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_dst_index(i);
            }
        }},
    {"workspace",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_workspace(i);
            }
        }},
    {"workspace_bytes",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto i : attrDef.list().i()) {
                opDef.add_workspace_bytes(i);
            }
        }},
    {"is_input_const",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            for (auto b : attrDef.list().b()) {
                opDef.add_is_input_const(b);
            }
        }},
    {"input_desc",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            *opDef.mutable_input_desc() = attrDef.list().tf();
        }},
    {"output_desc",
        [](hiai::proto::OpDef& opDef, const hiai::proto::AttrDef& attrDef) {
            *opDef.mutable_output_desc() = attrDef.list().tf();
        }},
};

void CompatibleMemberHandle(hiai::proto::OpDef& opDef)
{
    if (opDef.has_out_attr()) {
        return;
    }

    // 老模型，需要做兼容性处理，从attrMap提取参数设置到成员变量中
    const auto& attrMap = opDef.attr();
    for (auto it = attrMap.begin(); it != attrMap.end(); it++) {
        std::map<std::string, ATTR_MAP_FUNC>::const_iterator iter = OPDEF_COMPATIBLE_MAP.find(it->first);
        if (iter != OPDEF_COMPATIBLE_MAP.end()) {
            iter->second(opDef, it->second);
        }
    }
    opDef.set_has_out_attr(true);
}
} // namespace

ProtoOpDef::ProtoOpDef(hiai::proto::OpDef& opDef) : opDef_(opDef)
{
    OptionalInputHandle(opDef_);
    CompatibleMemberHandle(opDef_);
}

ProtoOpDef::~ProtoOpDef()
{
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(input_desc);
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(output_desc);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
}

void ProtoOpDef::CopyFrom(const IOpDef* other)
{
    if (other != nullptr && other->GetSerializeType() == PROTOBUF) {
        opDef_ = static_cast<const ProtoOpDef*>(other)->opDef_;
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(input_desc);
        IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(output_desc);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
    }
}

SerializeType ProtoOpDef::GetSerializeType() const
{
    return PROTOBUF;
}

bool ProtoOpDef::LoadFrom(const uint8_t* data, size_t len)
{
    if (data == nullptr || len == 0) {
        return false;
    }

    google::protobuf::io::CodedInputStream coded_stream(data, len);
    coded_stream.SetTotalBytesLimit(INT32_MAX);
    if (!opDef_.ParseFromCodedStream(&coded_stream)) {
        return false;
    }

    OptionalInputHandle(opDef_);
    CompatibleMemberHandle(opDef_);
    return true;
}

bool ProtoOpDef::SaveTo(uint8_t* data, size_t len)
{
    return opDef_.SerializeToArray(data, len);
}

size_t ProtoOpDef::GetOpDefSize() const
{
#if GOOGLE_PROTOBUF_VERSION < 3013000
    return opDef_.ByteSize();
#else
    return opDef_.ByteSizeLong();
#endif
}

IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, name);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, type);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, input);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, IAttrMapDef, ProtoAttrMapDef, attr);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, bool, has_out_attr, false);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, id, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, stream_id, 0);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, input_name);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, src_name);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, src_index, -1);
IMPL_PROTO_PERSISTENCE_STANDARD_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, std::string, dst_name);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, dst_index, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, input_i, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, output_i, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, workspace, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, int64_t, workspace_bytes, -1);
IMPL_PROTO_PERSISTENCE_BASIC_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, bool, is_input_const, false);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ProtoOpDef, opDef_, ITensorDescDef, ProtoTensorDescDef, input_desc);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(
    ProtoOpDef, opDef_, ITensorDescDef, ProtoTensorDescDef, output_desc);

extern "C" GRAPH_API_EXPORT IOpDef* CreateOpDef()
{
    auto op = new (std::nothrow) DefaultProtoOpDef();
    if (op != nullptr) {
        op->set_has_out_attr(true);
    }
    return op;
}

extern "C" GRAPH_API_EXPORT void DestroyOpDef(IOpDef* opDef)
{
    delete opDef;
}
} // namespace hiai