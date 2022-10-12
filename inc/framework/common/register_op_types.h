/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: common op type
 */

#ifndef DOMI_COMMON_REGISTER_OP_TYPE_H__
#define DOMI_COMMON_REGISTER_OP_TYPE_H__

#if defined(HOST_VISIBILITY) || defined(DEV_VISIBILITY)
#define GE_API_VISIBILITY __attribute__((visibility("default")))
#else
#ifdef _MSC_VER
#ifdef LIBAI_FMK_OP_DEF_EXPORTS
#define GE_API_VISIBILITY __declspec(dllexport)
#else
#define GE_API_VISIBILITY __declspec(dllimport)
#endif
#else
#define GE_API_VISIBILITY
#endif
#endif
#endif // DOMI_COMMON_REGISTER_OP_TYPE_H__
