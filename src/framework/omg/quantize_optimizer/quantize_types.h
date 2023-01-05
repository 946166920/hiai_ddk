/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: 常量及结构体定义
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