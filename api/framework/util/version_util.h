/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: version util
 */
#ifndef FRAMEWORK_UTIL_VERSION_UTIL_H
#define FRAMEWORK_UTIL_VERSION_UTIL_H
#include "util_api_export.h"

namespace hiai {
class VersionUtil {
public:
    HIAI_UTIL_API_EXPORT static const char* GetVersion();
};
} // namespace hiai
#endif // FRAMEWORK_UTIL_VERSION_H
