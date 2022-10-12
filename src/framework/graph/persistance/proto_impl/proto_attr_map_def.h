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

#ifndef FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_ATTR_MAP_DEF_H
#define FRAMEWORK_GRAH_PERSISTENCE_PROTO_PROTO_ATTR_MAP_DEF_H
#include "proto_func_macro_def.h"
#include "proto_attr_def.h"
#include "graph/persistance/interface/attr_map_def.h"

namespace google {
namespace protobuf {
class MessageLite;

template <typename Key, typename T> class Map;
} // namespace protobuf
} // namespace google

namespace hiai {
namespace proto {
class AttrDef;
}
} // namespace ge

namespace hiai {
using ProtoMap = ::google::protobuf::Map<::std::string, ::hiai::proto::AttrDef>;

class ProtoAttrMapDef : public IAttrMapDef {
public:
    ProtoAttrMapDef();
    ProtoAttrMapDef(ProtoMap* attrMapDef, bool isOwner = false);
    ~ProtoAttrMapDef() override;

    void CopyFrom(const IAttrMapDef* other) override;
    SerializeType GetSerializeType() const override;

    DEF_PROTO_PERSISTENCE_CUSTOM_MAP_MEMBER_PURE_FUNC(std::string, IAttrDef, attr);

private:
    ProtoMap* attrMapDef_;
    bool isOwner_;
};

} // namespace hiai

#endif