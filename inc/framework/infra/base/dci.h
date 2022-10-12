/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: data context interaction
 */

#ifndef HIAI_FRAMEWORK_INFRA_BASE_DCI_H
#define HIAI_FRAMEWORK_INFRA_BASE_DCI_H

#include "graph/graph_api_export.h"
/*lint -e773*/
#define ROLE(type) Get##type()
#define ROLE_PROTO_TYPE(type) type& ROLE(type) const
#define USE_ROLE(type) virtual ROLE_PROTO_TYPE(type) = 0
#define HAS_ROLE(type) USE_ROLE(type)
#define SET_ROLE(type) Set##type
#define DECL_ROLE(type) ROLE_PROTO_TYPE(type)
#define ROLE_VAR(type) type##_
#define CAST_TO_ROLE(type, obj)
/*lint +e773*/
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