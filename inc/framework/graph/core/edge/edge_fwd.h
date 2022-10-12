/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: edge fwd
 */

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_EDGE_FWD_H
#define FRAMEWORK_GRAPH_CORE_EDGE_EDGE_FWD_H

#include <memory>

namespace ge {
struct LegacyEdge;
using LegacyEdgePtr = std::shared_ptr<LegacyEdge>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_EDGE_FWD_H