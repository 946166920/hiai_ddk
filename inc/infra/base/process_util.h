/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description:The description of Stats util in framework.
 */
#ifndef INFRA_BASE_UTIL_H
#define INFRA_BASE_UTIL_H

#include <string>

namespace hiai {
std::string GetProcessName(int pid = -1);
}
#endif // INFRA_BASE_UTIL_H
