/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: IR convert for compatible.
 */
#ifndef __IR_BUILD_TRANSFORMER_H__
#define __IR_BUILD_TRANSFORMER_H__

#include "framework/graph/core/cgraph/compute_graph.h"

namespace hiai {
class IRBuildTransformer {
public:
    static bool TransferIRToTargetVersion(ge::ComputeGraphPtr graph, std::string aiRomVersion);
};
} // namespace hiai
#endif // __IR_BUILD_TRANSFORMER_H__
