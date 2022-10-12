/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description:
 */
#ifndef HIAI_API_MODEL_MANAGER_API_EXPORT_H
#define HIAI_API_MODEL_MANAGER_API_EXPORT_H

#ifdef HIAI_MM_API_VISIABLE
#ifdef _MSC_VER
#define HIAI_MM_API_EXPORT __declspec(dllexport)
#else
#define HIAI_MM_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define HIAI_MM_API_EXPORT
#endif

#endif // HIAI_API_MODEL_MANAGER_API_EXPORT_H
