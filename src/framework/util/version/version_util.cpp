/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: version util implementation
 */
#include "util/version_util.h"
#include "framework/c/hiai_version.h"

namespace hiai {
const char* VersionUtil::GetVersion()
{
    return HIAI_GetVersion();
}
} // namespace hiai