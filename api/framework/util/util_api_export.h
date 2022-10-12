/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description:util_api_export
 */
#ifndef FRAMEWORK_UTIL_API_EXPORT_H
#define FRAMEWORK_UTIL_API_EXPORT_H

#ifdef HIAI_UTIL_API_VISIABLE
#ifdef _MSC_VER
#define HIAI_UTIL_API_EXPORT __declspec(dllexport)
#else
#define HIAI_UTIL_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define HIAI_UTIL_API_EXPORT
#endif

#endif // FRAMEWORK_UTIL_API_EXPORT_H
