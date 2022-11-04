/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: HiAiModelBuilderType
 */
#ifndef FRAMEWORK_BUILD_OPTION_TYPES_H
#define FRAMEWORK_BUILD_OPTION_TYPES_H

#include <string>
#include <vector>
#include <map>

namespace hiai {
enum WeightDataType { FP32, FP16 };

enum OpExecuteDevice {
    NPU_DEV = 0,
    CPU_DEV = 1,
};

enum ExecuteDeviceSelectMode {
    AUTO,
    CUSTOM,
};

struct BuildOptions {
    bool useOriginFormat = false;
    ExecuteDeviceSelectMode mode = AUTO;
    WeightDataType weightDataType = FP32;
    std::map<std::string, std::vector<OpExecuteDevice>> opExecuteDeviceConfig;
    std::vector<std::vector<int64_t>> inputShapes;
};
} // namespace hiai

#endif