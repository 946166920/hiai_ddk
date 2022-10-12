/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: anchor fwd
 */

#ifndef FRAMEWORK_GRAPH_CORE_EDGE_ANCHOR_FWD_H
#define FRAMEWORK_GRAPH_CORE_EDGE_ANCHOR_FWD_H

#include <memory>

namespace ge {
class Anchor;
using AnchorPtr = std::shared_ptr<Anchor>;
using ConstAnchor = const Anchor;

class DataAnchor;
using DataAnchorPtr = std::shared_ptr<DataAnchor>;

class InDataAnchor;
using InDataAnchorPtr = std::shared_ptr<InDataAnchor>;

class OutDataAnchor;
using OutDataAnchorPtr = std::shared_ptr<OutDataAnchor>;

class ControlAnchor;
using ControlAnchorPtr = std::shared_ptr<ControlAnchor>;

class InControlAnchor;
using InControlAnchorPtr = std::shared_ptr<InControlAnchor>;

class OutControlAnchor;
using OutControlAnchorPtr = std::shared_ptr<OutControlAnchor>;
} // namespace ge

#endif // FRAMEWORK_GRAPH_CORE_EDGE_ANCHOR_FWD_H