/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2021. All rights reserved.
 * Description: types
 */

#ifndef _HIAI_TYPE_H_
#define _HIAI_TYPE_H_

#include "util/base_types.h"

#ifndef AICP_API_EXPORT
#if (defined HOST_VISIBILITY) || (defined DEV_VISIBILITY)
#ifdef _MSC_VER
#define AICP_API_EXPORT __declspec(dllexport)
#else
#define AICP_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define AICP_API_EXPORT
#endif
#endif

namespace hiai {
};

#endif // _TYPE_H_
