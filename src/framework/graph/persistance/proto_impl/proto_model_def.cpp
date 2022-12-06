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
#include "proto_model_def.h"

#if defined(GRAPH_HOST) || defined(GRAPH_DEVICE)
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#endif

#include "graph/persistance/interface/attr_def.h"
#include "graph/persistance/proto_impl/proto_attr_map_def.h"
#include "graph/persistance/proto_impl/proto_graph_def.h"

#include "framework/graph/debug/ge_graph_attr_define.h"

namespace hiai {
ProtoModelDef::ProtoModelDef() : modelDef_(new (std::nothrow) hiai::proto::ModelDef())
{
    if (modelDef_ != nullptr) {
        auto attrMap = modelDef_->mutable_attr();
        if (attrMap != nullptr) {
            (*attrMap)[hiai::ATTR_MODEL_MEMORY_SIZE].set_i(0);
            (*attrMap)[hiai::ATTR_MODEL_STREAM_NUM].set_i(0);
            (*attrMap)[hiai::ATTR_MODEL_EVENT_NUM].set_i(0);
            (*attrMap)[hiai::ATTR_MODEL_WEIGHT_SIZE].set_i(0);
        }
    }
}

ProtoModelDef::~ProtoModelDef()
{
    IMPL_PROTO_CUSTOM_LIST_MEMBER_FREE(graph);
    IMPL_PROTO_CUSTOM_MEMBER_FREE(attr);

    delete modelDef_;
    modelDef_ = nullptr;
}

SerializeType ProtoModelDef::GetSerializeType() const
{
    return PROTOBUF;
}

bool ProtoModelDef::LoadFrom(const uint8_t* data, size_t len)
{
    if (data == nullptr || len == 0 || modelDef_ == nullptr || len > INT_MAX) {
        return false;
    }
    google::protobuf::io::CodedInputStream coded_stream(data, len);
    coded_stream.SetTotalBytesLimit(INT32_MAX);

    return modelDef_->ParseFromCodedStream(&coded_stream);
}

bool ProtoModelDef::SaveTo(uint8_t* data, size_t len) const
{
    if (len > INT_MAX) {
        return false;
    }
    if (modelDef_ != nullptr) {
        return modelDef_->SerializeToArray(data, len);
    }
    return false;
}

size_t ProtoModelDef::GetModelDefSize() const
{
    if (modelDef_ == nullptr) {
        return 0;
    }
#if GOOGLE_PROTOBUF_VERSION < 3013000
    return modelDef_->ByteSize();
#else
    return modelDef_->ByteSizeLong();
#endif
}

bool ProtoModelDef::Dump(const std::string& file) const
{
    (void)file;
#if defined(GRAPH_HOST) || defined(GRAPH_DEVICE)
    char path[PATH_MAX + 1] = {0x00};
    if (file.size() > PATH_MAX || realpath(file.c_str(), path) == nullptr) {
        return false;
    }
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        return false;
    }
    google::protobuf::io::FileOutputStream* output = new (std::nothrow) google::protobuf::io::FileOutputStream(fd);
    if (output == nullptr) {
        close(fd);
        return false;
    }
    google::protobuf::TextFormat::Print(*modelDef_, output);
    delete output;
    close(fd);
    return true;
#endif
    return false;
}

IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoModelDef, modelDef_, std::string, name);
IMPL_PROTO_PERSISTENCE_BASIC_MEMBER_PURE_FUNC(ProtoModelDef, modelDef_, int64_t, version, 0);
IMPL_PROTO_PERSISTENCE_STANDARD_MEMBER_PURE_FUNC(ProtoModelDef, modelDef_, std::string, custom_version);
IMPL_PROTO_PERSISTENCE_CUSTOM_MEMBER_PURE_FUNC(ProtoModelDef, modelDef_, IAttrMapDef, ProtoAttrMapDef, attr);
IMPL_PROTO_PERSISTENCE_CUSTOM_LIST_MEMBER_PURE_FUNC(ProtoModelDef, modelDef_, IGraphDef, ProtoGraphDef, graph);

extern "C" GRAPH_API_EXPORT IModelDef* CreateModelDef()
{
    return new (std::nothrow) ProtoModelDef();
}

extern "C" GRAPH_API_EXPORT void DestroyModelDef(IModelDef* modelDef)
{
    delete modelDef;
}

} // namespace hiai