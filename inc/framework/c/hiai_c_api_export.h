/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: legacy_api_export
 */
#ifndef FRAMEWORK_C_HIAI_C_API_EXPORT_H
#define FRAMEWORK_C_HIAI_C_API_EXPORT_H

#ifdef HIAI_C_API_VISIABLE
#ifdef _MSC_VER
#define AICP_C_API_EXPORT __declspec(dllexport)
#else
#define AICP_C_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define AICP_C_API_EXPORT
#endif

#endif // FRAMEWORK_C_HIAI_C_API_EXPORT_H
