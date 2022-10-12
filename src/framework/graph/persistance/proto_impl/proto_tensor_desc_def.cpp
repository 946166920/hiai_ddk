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
#include "proto_tensor_desc_def.h"

#include <functional>

#include "graph/persistance/proto_impl/proto_attr_map_def.h"
#include "graph/persistance/proto_impl/proto_shape_def.h"
#include "graph/types.h"

#include "framework/graph/debug/ge_log.h"

using namespace ge;

namespace hiai {
namespace {
const static char* const KEY_DATA_TYPE_SELF_DEFINED = "__tensor_desc_data_type__";

const static std::map<DataType, ::hiai::proto::DataType> DATA_TYPE_MAP = {
    {DT_UNDEFINED, ::hiai::proto::DT_UNDEFINED},
    {DT_FLOAT, ::hiai::proto::DT_FLOAT},
    {DT_FLOAT16, ::hiai::proto::DT_FLOAT16},
    {DT_INT8, ::hiai::proto::DT_INT8},
    {DT_UINT8, ::hiai::proto::DT_UINT8},
    {DT_INT16, ::hiai::proto::DT_INT16},
    {DT_UINT16, ::hiai::proto::DT_UINT16},
    {DT_INT32, ::hiai::proto::DT_INT32},
    {DT_INT64, ::hiai::proto::DT_INT64},
    {DT_UINT32, ::hiai::proto::DT_UINT32},
    {DT_UINT64, ::hiai::proto::DT_UINT64},
    {DT_BOOL, ::hiai::proto::DT_BOOL},
    {DT_DOUBLE, ::hiai::proto::DT_DOUBLE},
};

const static std::map<DataType, int64_t> SELF_DEFINE_DATA_TYPE_MAP = {
    {DT_DUAL, 13}, {DT_DUAL_SUB_INT8, 14}, {DT_DUAL_SUB_UINT8, 15}, {DT_2BIT, 21}, {DT_INT4, 22}, {DT_RESOURCE, 24}};

const static std::map<Format, std::string> formatToStringMap = {
    {FORMAT_NCHW, "NCHW"},
    {FORMAT_NHWC, "NHWC"},
    {FORMAT_ND, "ND"},
    {FORMAT_NC1HWC0, "NC1HWC0"},
    {FORMAT_NC4HW4, "NC4HW4"},
    {FORMAT_FRACTAL_Z, "FRACTAL_Z"},
    {FORMAT_NC1C0HWPAD, "NC1C0HWPAD"},
    {FORMAT_NHWC1C0, "NHWC1C0"},
    {FORMAT_FSR_NCHW, "FSR_NCHW"},
    {FORMAT_FRACTAL_DECONV, "FRACTAL_DECONV"},
    {FORMAT_C1HWNC0, "C1HWNC0"},
    {FORMAT_FRACTAL_DECONV_TRANSPOSE, "FRACTAL_DECONV_TRANSPOSE"},
    {FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS, "FRACTAL_DECONV_SP_STRIDE_TRANS"},
    {FORMAT_NC1HWC0_C04, "NC1HWC0_C04"},
    {FORMAT_FRACTAL_Z_C04, "FRACTAL_Z_C04"},
    {FORMAT_CHWN, "CHWN"},
    {FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS, "DECONV_SP_STRIDE8_TRANS"},
    {FORMAT_NC1KHKWHWC0, "NC1KHKWHWC0"},
    {FORMAT_BN_WEIGHT, "BN_WEIGHT"},
    {FORMAT_FILTER_HWCK, "FILTER_HWCK"},
    {FORMAT_HWCN, "HWCN"},
    {FORMAT_HASHTABLE_LOOKUP_LOOKUPS, "LOOKUP_LOOKUPS"},
    {FORMAT_HASHTABLE_LOOKUP_KEYS, "LOOKUP_KEYS"},
    {FORMAT_HASHTABLE_LOOKUP_VALUE, "LOOKUP_VALUE"},
    {FORMAT_HASHTABLE_LOOKUP_OUTPUT, "LOOKUP_OUTPUT"},
    {FORMAT_HASHTABLE_LOOKUP_HITS, "LOOKUP_HITS"},
    {FORMAT_FRACTAL_Z_SPARSE, "SPARSE_FRACTAL_Z"},
    {FORMAT_WINO, "WINO"},
    {FORMAT_NC4HW4, "NC4HW4"},
    {FORMAT_FRACTAL_Z_N_TRANS, "FRACTAL_Z_N_TRANS"},
    {FORMAT_FRACTAL_Z_N_NORMAL, "FRACTAL_Z_N_NORMAL"},
    {FORMAT_NC8HW8, "NC8HW8"},
    {FORMAT_NDC1HWC0, "NDC1HWC0"},
    {FORMAT_NCDHW, "NCDHW"},
    {FORMAT_NDHWC, "NDHWC"},
    {FORMAT_DHWCN, "DHWCN"},
    {FORMAT_FRACTAL_Z_5D, "FRACTAL_Z_5D"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0, "FRACTAL_Z_DECONV_SP_K2S2P0"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0, "FRACTAL_Z_DECONV_SP_K4S2P0"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1, "FRACTAL_Z_DECONV_SP_K4S2P1"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2, "FRACTAL_Z_DECONV_SP_K4S2P2"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0, "FRACTAL_Z_DECONV_SP_K3S3P0"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0_S16S8, "FRACTAL_Z_DECONV_SP_K2S2P0_S16S8"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0_S16S8, "FRACTAL_Z_DECONV_SP_K4S2P0_S16S8"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1_S16S8, "FRACTAL_Z_DECONV_SP_K4S2P1_S16S8"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2_S16S8, "FRACTAL_Z_DECONV_SP_K4S2P2_S16S8"},
    {FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0_S16S8, "FRACTAL_Z_DECONV_SP_K3S3P0_S16S8"},
    {FORMAT_WINO_V2, "WINO_V2"},
    {FORMAT_FRACTAL_Z_S16, "FRACTAL_Z_S16"},
    {FORMAT_WINO_V2_S16, "WINO_V2_S16"},
    {FORMAT_FRACTAL_Z_DW_DIAG, "FRACTAL_Z_DW_DIAG"},
    {FORMAT_FRACTAL_Z_DW_V2, "FRACTAL_Z_DW_V2"},
    {FRACTAL_NZ, "FRACTAL_NZ"},
    {FORMAT_RESERVED, "RESERVED"},
};

const static std::map<std::string, Format> stringToFormatMap = {
    {"NCHW", FORMAT_NCHW},
    {"NHWC", FORMAT_NHWC},
    {"ND", FORMAT_ND},
    {"NC1HWC0", FORMAT_NC1HWC0},
    {"NC4HW4", FORMAT_NC4HW4},
    {"FRACTAL_Z", FORMAT_FRACTAL_Z},
    {"NC1C0HWPAD", FORMAT_NC1C0HWPAD},
    {"NHWC1C0", FORMAT_NHWC1C0},
    {"FSR_NCHW", FORMAT_FSR_NCHW},
    {"FRACTAL_DECONV", FORMAT_FRACTAL_DECONV},
    {"C1HWNC0", FORMAT_C1HWNC0},
    {"FRACTAL_DECONV_TRANSPOSE", FORMAT_FRACTAL_DECONV_TRANSPOSE},
    {"FRACTAL_DECONV_SP_STRIDE_TRANS", FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS},
    {"NC1HWC0_C04", FORMAT_NC1HWC0_C04},
    {"FRACTAL_Z_C04", FORMAT_FRACTAL_Z_C04},
    {"CHWN", FORMAT_CHWN},
    {"DECONV_SP_STRIDE8_TRANS", FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS},
    {"NC1KHKWHWC0", FORMAT_NC1KHKWHWC0},
    {"BN_WEIGHT", FORMAT_BN_WEIGHT},
    {"FILTER_HWCK", FORMAT_FILTER_HWCK},
    {"HWCN", FORMAT_HWCN},
    {"LOOKUP_LOOKUPS", FORMAT_HASHTABLE_LOOKUP_LOOKUPS},
    {"LOOKUP_KEYS", FORMAT_HASHTABLE_LOOKUP_KEYS},
    {"LOOKUP_VALUE", FORMAT_HASHTABLE_LOOKUP_VALUE},
    {"LOOKUP_OUTPUT", FORMAT_HASHTABLE_LOOKUP_OUTPUT},
    {"LOOKUP_HITS", FORMAT_HASHTABLE_LOOKUP_HITS},
    {"SPARSE_FRACTAL_Z", FORMAT_FRACTAL_Z_SPARSE},
    {"WINO", FORMAT_WINO},
    {"NC4HW4", FORMAT_NC4HW4},
    {"FRACTAL_Z_N_TRANS", FORMAT_FRACTAL_Z_N_TRANS},
    {"FRACTAL_Z_N_NORMAL", FORMAT_FRACTAL_Z_N_NORMAL},
    {"NC8HW8", FORMAT_NC8HW8},
    {"NDC1HWC0", FORMAT_NDC1HWC0},
    {"NDHWC", FORMAT_NDHWC},
    {"NCDHW", FORMAT_NCDHW},
    {"DHWCN", FORMAT_DHWCN},
    {"FRACTAL_Z_5D", FORMAT_FRACTAL_Z_5D},
    {"FRACTAL_Z_DECONV_SP_K2S2P0", FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0},
    {"FRACTAL_Z_DECONV_SP_K4S2P0", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0},
    {"FRACTAL_Z_DECONV_SP_K4S2P1", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1},
    {"FRACTAL_Z_DECONV_SP_K4S2P2", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2},
    {"FRACTAL_Z_DECONV_SP_K3S3P0", FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0},
    {"FRACTAL_Z_DECONV_SP_K2S2P0_S16S8", FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0_S16S8},
    {"FRACTAL_Z_DECONV_SP_K4S2P0_S16S8", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0_S16S8},
    {"FRACTAL_Z_DECONV_SP_K4S2P1_S16S8", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1_S16S8},
    {"FRACTAL_Z_DECONV_SP_K4S2P2_S16S8", FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2_S16S8},
    {"FRACTAL_Z_DECONV_SP_K3S3P0_S16S8", FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0_S16S8},
    {"WINO_V2", FORMAT_WINO_V2},
    {"FRACTAL_Z_S16", FORMAT_FRACTAL_Z_S16},
    {"WINO_V2_S16", FORMAT_WINO_V2_S16},
    {"FRACTAL_Z_DW_DIAG", FORMAT_FRACTAL_Z_DW_DIAG},
    {"FRACTAL_Z_DW_V2", FORMAT_FRACTAL_Z_DW_V2},
    {"FRACTAL_NZ", FRACTAL_NZ},
    {"RESERVED", FORMAT_RESERVED},
};

static std::string FormatToSerialString(Format format)
{
    auto it = formatToStringMap.find(format);
    if (it != formatToStringMap.end()) {
        return it->second;
    } else {
        FMK_LOGE("Format not support %u", format);
        return "RESERVED";
    }
}

static Format SerialStringToFormat(const std::string& str)
{
    auto it = stringToFormatMap.find(str);
    if (it != stringToFormatMap.end()) {
        return it->second;
    } else {
        FMK_LOGE("Format not support %s", str.c_str());
        return FORMAT_RESERVED;
    }
}

using ATTR_MAP_FUNC = std::function<void(hiai::proto::TensorDescriptor*, const hiai::proto::AttrDef&)>;

const static std::map<std::string, ATTR_MAP_FUNC> TENSORDESCDEF_COMPATIBLE_MAP = {
    {"size",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_size(attrDef.i());
        }},
    {"weight_size",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_weight_size(attrDef.i());
        }},
    {"reuse_input",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_reuse_input(attrDef.b());
        }},
    {"output_tensor",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_output_tensor(attrDef.b());
        }},
    {"device_type",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_device_type(attrDef.s());
        }},
    {"input_tensor",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_input_tensor(attrDef.b());
        }},
    {"real_dim_cnt",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_real_dim_cnt(attrDef.i());
        }},
    {"reuse_input_index",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_reuse_input_index(attrDef.i());
        }},
    {"data_offset",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_data_offset(attrDef.i());
        }},
    {"cmps_size",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_cmps_size(attrDef.i());
        }},
    {"cmps_tab",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_cmps_tab(attrDef.s());
        }},
    {"cmps_tab_offset",
        [](hiai::proto::TensorDescriptor* tdDef, const hiai::proto::AttrDef& attrDef) {
            tdDef->set_cmps_tab_offset(attrDef.i());
        }},
};

void CompatibleMemberHandle(hiai::proto::TensorDescriptor* tdDef)
{
    if (tdDef->has_out_attr()) {
        return;
    }

    tdDef->set_has_out_attr(true);

    if (tdDef->layout() == "") {
        tdDef->set_layout("NCHW");
    }

    if (tdDef->dtype() == ::hiai::proto::DT_UNDEFINED) {
        tdDef->set_dtype(::hiai::proto::DT_FLOAT);
    }

    if (tdDef->device_type() == "") {
        tdDef->set_device_type("NPU");
    }

    // 老模型需要做兼容性处理，从attrMap提取参数设置到成员变量中
    const auto& attrMap = tdDef->attr();
    for (auto it = attrMap.begin(); it != attrMap.end(); it++) {
        std::map<std::string, ATTR_MAP_FUNC>::const_iterator iter = TENSORDESCDEF_COMPATIBLE_MAP.find(it->first);
        if (iter != TENSORDESCDEF_COMPATIBLE_MAP.end()) {
            iter->second(tdDef, it->second);
        }
    }
}
} // namespace

ProtoTensorDescDef::ProtoTensorDescDef() : ProtoTensorDescDef(new (std::nothrow) hiai::proto::TensorDescriptor(), true)
{
}

ProtoTensorDescDef::ProtoTensorDescDef(hiai::proto::TensorDescriptor* tdDef, bool isOwner)
    : tdDef_(tdDef), isOwner_(isOwner)
{
    if (tdDef_ != nullptr) {
        CompatibleMemberHandle(tdDef_);
    }
}

ProtoTensorDescDef::~ProtoTensorDescDef()
{
    IMPL_PROTO_CUSTOM_MEMBER_FREE(shape);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);

    if (isOwner_) {
        delete tdDef_;
    }
    tdDef_ = nullptr;
}

void ProtoTensorDescDef::CopyFrom(const ITensorDescDef* other)
{
    if (tdDef_ != nullptr && other != nullptr && other->GetSerializeType() == PROTOBUF) {
        *tdDef_ = *(static_cast<const ProtoTensorDescDef*>(other)->tdDef_);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(shape);
        IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);
    }
}

SerializeType ProtoTensorDescDef::GetSerializeType() const
{
    return PROTOBUF;
}

bool ProtoTensorDescDef::LoadFrom(const uint8_t* data, size_t len)
{
    if (data == nullptr || len == 0 || tdDef_ == nullptr) {
        return false;
    }

    google::protobuf::io::CodedInputStream coded_stream(data, len);
    coded_stream.SetTotalBytesLimit(INT32_MAX);
    if (!tdDef_->ParseFromCodedStream(&coded_stream)) {
        return false;
    }

    CompatibleMemberHandle(tdDef_);
    return true;
}

bool ProtoTensorDescDef::SaveTo(uint8_t* data, size_t len)
{
    if (tdDef_ != nullptr) {
        return tdDef_->SerializeToArray(data, len);
    }
    return false;
}

size_t ProtoTensorDescDef::GetTensorDescDefSize() const
{
    if (tdDef_ == nullptr) {
        return 0;
    }
#if GOOGLE_PROTOBUF_VERSION < 3013000
    return tdDef_->ByteSize();
#else
    return tdDef_->ByteSizeLong();
#endif
}

ge::DataType ProtoTensorDescDef::dtype() const
{
    if (tdDef_ == nullptr) {
        return ge::DT_UNDEFINED;
    }

    auto& attrMap = tdDef_->attr();

    auto iter = attrMap.find(KEY_DATA_TYPE_SELF_DEFINED);
    if (iter == attrMap.end()) {
        auto dataTypeProto = tdDef_->dtype();
        for (auto it : DATA_TYPE_MAP) {
            if (it.second == dataTypeProto) {
                return it.first;
            }
        }
    } else {
        int64_t dataTypeProto = iter->second.i();
        for (auto it : SELF_DEFINE_DATA_TYPE_MAP) {
            if (it.second == dataTypeProto) {
                return it.first;
            }
        }
    }

    return ge::DT_UNDEFINED;
}

void ProtoTensorDescDef::set_dtype(const ge::DataType value)
{
    if (tdDef_ == nullptr) {
        return;
    }

    auto attrMap = tdDef_->mutable_attr();
    if (attrMap == nullptr) {
        return;
    }

    attrMap->erase(KEY_DATA_TYPE_SELF_DEFINED);

    auto it1 = DATA_TYPE_MAP.find(value);
    if (it1 != DATA_TYPE_MAP.end()) {
        tdDef_->set_dtype(it1->second);
        return;
    }

    auto it2 = SELF_DEFINE_DATA_TYPE_MAP.find(value);
    if (it2 != SELF_DEFINE_DATA_TYPE_MAP.end()) {
        (*attrMap)[KEY_DATA_TYPE_SELF_DEFINED].set_i(it2->second);
        return;
    }
}

ge::Format ProtoTensorDescDef::layout() const
{
    return tdDef_ != nullptr ? SerialStringToFormat(tdDef_->layout()) : FORMAT_RESERVED;
}

void ProtoTensorDescDef::set_layout(const ge::Format value)
{
    if (tdDef_ != nullptr) {
        tdDef_->set_layout(FormatToSerialString(value));
    }
}

IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, IShapeDef, ProtoShapeDef, shape);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, std::string, name);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, bool, has_out_attr, false);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, size, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, weight_size, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, bool, reuse_input, false);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, bool, output_tensor, false);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, std::string, device_type);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, bool, input_tensor, false);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, real_dim_cnt, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, reuse_input_index, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, data_offset, 0);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, cmps_size, 0);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, std::string, cmps_tab);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, int64_t, cmps_tab_offset, 0);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoTensorDescDef, tdDef_, IAttrMapDef, ProtoAttrMapDef, attr);

extern "C" GRAPH_API_EXPORT ITensorDescDef* CreateTensorDescDef()
{
    return new (std::nothrow) ProtoTensorDescDef();
}

extern "C" GRAPH_API_EXPORT void DestroyTensorDescDef(ITensorDescDef* tensorDescDef)
{
    delete tensorDescDef;
}

} // namespace hiai