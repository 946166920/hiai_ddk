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
#ifndef __IR_TRANSFORMER_H__
#define __IR_TRANSFORMER_H__

#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/common/hcs_types.h"

namespace hiai {
class HCS_API_EXPORT IRTransformer {
public:
    static bool IsCompatible(ge::ComputeGraphPtr graph, string hiaiRomVersion);
    static bool TransferToStandard(ge::ComputeGraphPtr graph);
    static bool TransferToTargetVersion(ge::ComputeGraphPtr graph, string aiRomVersion, bool& isGraphChanged);
    static bool VerifyIrReservedField(ge::ComputeGraphPtr graph);
};
} // namespace hiai
#endif // __IR_TRANSFORMER_H__
