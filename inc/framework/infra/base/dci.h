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

#ifndef HIAI_FRAMEWORK_INFRA_BASE_DCI_H
#define HIAI_FRAMEWORK_INFRA_BASE_DCI_H

#include "graph/graph_api_export.h"
#define ROLE(type) Get##type()
#define ROLE_PROTO_TYPE(type) type& ROLE(type) const
#define USE_ROLE(type) virtual ROLE_PROTO_TYPE(type) = 0
#define HAS_ROLE(type) USE_ROLE(type)
#define SET_ROLE(type) Set##type
#define DECL_ROLE(type) ROLE_PROTO_TYPE(type)
#define ROLE_VAR(type) type##_
#define CAST_TO_ROLE(type, obj)
#define IMPL_ROLE(type)                                                                                                \
private:                                                                                                               \
    ROLE_PROTO_TYPE(type) override                                                                                     \
    {                                                                                                                  \
        return const_cast<type&>(static_cast<const type&>(*this));                                                     \
    }

#define IMPL_ROLE_WITH_VAR(type)                                                                                       \
private:                                                                                                               \
    ROLE_PROTO_TYPE(type) override                                                                                     \
    {                                                                                                                  \
        return const_cast<type&>(ROLE_VAR(type));                                                                      \
    }                                                                                                                  \
    type ROLE_VAR(type);

#define DECL_DEPENDENCY(type)                                                                                          \
public:                                                                                                                \
    void SET_ROLE(type)(type & role)                                                                                   \
    {                                                                                                                  \
        ROLE_VAR(type) = &role;                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    ROLE_PROTO_TYPE(type)                                                                                              \
    {                                                                                                                  \
        return const_cast<type&>(*ROLE_VAR(type));                                                                     \
    }                                                                                                                  \
                                                                                                                       \
private:                                                                                                               \
    type* ROLE_VAR(type);

namespace hiai {
template<typename T>
class Role {
public:
    virtual ~Role() = default;
};
} // namespace hiai

#define DEFINE_ROLE(type) class type : private hiai::Role<type>

#define EXPORT_ROLE(type) class GRAPH_API_EXPORT type : private hiai::Role<type>

#endif // HIAI_FRAMEWORK_INFRA_BASE_DCI_H