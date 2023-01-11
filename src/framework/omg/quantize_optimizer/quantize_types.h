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
#ifndef DOMI_OMG_COMPRESS_COMPRESS_TYPES_H
#define DOMI_OMG_COMPRESS_COMPRESS_TYPES_H

#include "framework/graph/core/def_types.h"
#include "graph/buffer.h"
#include "graph/types.h"

namespace hiai {
struct QuantizeConfig {
    ge::DataType inputDataType = ge::DT_UNDEFINED;
    ge::DataType weightDataType = ge::DT_UNDEFINED;
    std::vector<float> inputScale;
    std::vector<float> inputOffset;
    std::vector<float> weightScale;
    std::vector<float> weightOffset;
    bool hasQuantInfoExt = false;
    std::string quantInfoExt; // add for wino IR attr
};
struct ModelLightWeightParams {
    std::map<std::string, QuantizeConfig> quantizeConfigs;
    bool useWeightName = false;
};
enum QuantizeType {
    UINT8_INT8_QUANTIZED = 1,
    UINT8_INT2_QUANTIZED = 2,
    INT4_INT4_QUANTIZED = 3,
    INT8_FILTER_QUANTIZED = 4,
    INT8_INT4_MIX_QUANTIZED = 5,
    INT16_INT8_QUANTIZED = 6,
    RESERVE_QUANTIZED = 99
};
} // namespace hiai

#endif