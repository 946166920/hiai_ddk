/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: identity
 */

#ifndef HIAI_FRAMEWORK_INFRA_MEM_IDENTITY_H
#define HIAI_FRAMEWORK_INFRA_MEM_IDENTITY_H

namespace hiai {
template <typename T>
struct Identity {
    using type = T;
};

template <typename T>
using IdentityType = typename Identity<T>::type;
} // namespace hiai

#endif // HIAI_FRAMEWORK_INFRA_MEM_IDENTITY_H