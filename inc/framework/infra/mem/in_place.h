/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: in place
 */

#ifndef HIAI_FRAMEWORK_INFRA_MEM_IN_PLACE_H
#define HIAI_FRAMEWORK_INFRA_MEM_IN_PLACE_H

#include "framework/infra/mem/inline_variable.h"

namespace hiai {
struct InPlace {
    struct InPlaceTag {};

    constexpr explicit InPlace(InPlaceTag) {}
};

HIAI_INLINE_VARIABLE(InPlace, IN_PLACE, InPlace{InPlace::InPlaceTag{}});
} // namespace hiai

#endif // HIAI_FRAMEWORK_INFRA_MEM_IN_PLACE_H