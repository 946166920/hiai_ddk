/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2022. All rights reserved.
 * Description: hiai_ir_aipp_compatible_adapter_dl
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
