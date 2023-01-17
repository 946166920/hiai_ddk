/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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