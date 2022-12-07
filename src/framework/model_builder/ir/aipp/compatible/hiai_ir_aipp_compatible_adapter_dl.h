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

#ifndef FRAMEWORK_MODEL_BUILDER_IR_AIPP_HIAI_IR_AIPP_COMPATIBLE_ADAPTER_DL_H
#define FRAMEWORK_MODEL_BUILDER_IR_AIPP_HIAI_IR_AIPP_COMPATIBLE_ADAPTER_DL_H

#include <string>

#include "base/error_types.h"

namespace ge {
class ComputeGraph;
}

namespace hiai {
#ifdef __cplusplus
extern "C" {
#endif
Status GenerateAippCompatibleInfoAdapter(ge::ComputeGraph& graph, std::string& customData);
#ifdef __cplusplus
}
#endif
} // namespace hiai

#endif // FRAMEWORK_MODEL_BUILDER_IR_AIPP_HIAI_IR_AIPP_COMPATIBLE_ADAPTER_DL_H
