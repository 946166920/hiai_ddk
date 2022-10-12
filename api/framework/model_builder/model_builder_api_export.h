/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: model builder api export
 */
#ifndef HIAI_API_MODEL_BUILDER_API_EXPORT_H
#define HIAI_API_MODEL_BUILDER_API_EXPORT_H

#ifdef HIAI_MB_API_VISIABLE
    #ifdef _MSC_VER
        #define HIAI_MB_API_EXPORT __declspec(dllexport)
    #else
        #define HIAI_MB_API_EXPORT __attribute__((__visibility__("default")))
    #endif
#else
    #define HIAI_MB_API_EXPORT
#endif

#endif // HIAI_API_MODEL_BUILDER_API_EXPORT_H