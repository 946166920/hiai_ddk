/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: inline variable
 */

#ifndef HIAI_FRAMEWORK_INFRA_MEM_INLINE_VARIABLE_H
#define HIAI_FRAMEWORK_INFRA_MEM_INLINE_VARIABLE_H

#include "framework/infra/mem/identity.h"

#define HIAI_INLINE_VARIABLE(type, name, val)                                                                          \
    template <typename = void>                                                                                         \
    struct InlineVariable##name {                                                                                      \
        constexpr static IdentityType<type> value = val;                                                               \
    };                                                                                                                 \
                                                                                                                       \
    template <typename T>                                                                                              \
    constexpr IdentityType<type> InlineVariable##name<T>::value;                                                       \
                                                                                                                       \
    constexpr static const IdentityType<type>& name = InlineVariable##name<>::value;

#endif // HIAI_FRAMEWORK_INFRA_MEM_INLINE_VARIABLE_H