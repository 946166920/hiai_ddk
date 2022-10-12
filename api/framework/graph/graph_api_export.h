/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description:graph_api_export
 */

#ifndef GRAPH_API_EXPORT_H
#define GRAPH_API_EXPORT_H

namespace ge {
#ifdef GRAPH_API_VISIABLE
#ifdef _MSC_VER
#define GRAPH_API_EXPORT __declspec(dllexport)
#else
#define GRAPH_API_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define GRAPH_API_EXPORT
#endif
} // namespace ge

#endif // GRAPH_API_EXPORT_H
