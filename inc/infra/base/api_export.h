/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description:util_api_export
 */
#ifndef INFRA_BASE_API_EXPORT_H
#define INFRA_BASE_API_EXPORT_H

#ifdef INFRA_API_VISIABLE
#ifdef _MSC_VER
#define INFRA_API_EXPORT __declspec(dllexport)
#else
#define INFRA_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define INFRA_API_EXPORT
#endif

#endif // INFRA_BASE_API_EXPORT_H
