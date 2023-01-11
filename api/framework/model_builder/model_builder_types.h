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
#ifndef HIAI_API_MODEL_BUILDER_TYPES_H
#define HIAI_API_MODEL_BUILDER_TYPES_H

#include <string>
#include <vector>
#include <map>
#include "tensor/nd_tensor_desc.h"

namespace hiai {
enum class FormatMode { USE_NCHW = 0, USE_ORIGIN = 1 };
enum class DeviceMemoryReusePlan { UNSET = 0, LOW = 1, HIGH = 2 };
enum PrecisionMode { PRECISION_MODE_FP32 = 0, PRECISION_MODE_FP16 };
enum CacheMode { CACHE_BUILDED_MODEL, CACHE_LOADED_MODEL };
enum class DeviceConfigMode { AUTO, MODEL_LEVEL, OP_LEVEL };
enum class FallBackMode { ENABLE, DISABLE };
enum class ExecuteDevice { NPU = 0, CPU = 1 };
enum class TuningStrategy { OFF = 0, ON_DEVICE_TUNING, ON_DEVICE_PREPROCESS_TUNING, ON_CLOUD_TUNING };
static const uint32_t MIN_DYNAMIC_SHAPE_CACHE_NUM = 1;
static const uint32_t MAX_DYNAMIC_SHAPE_CACHE_NUM = 10;
struct DynamicShapeConfig {
    bool enable = false; // mark whether dynamic shape enable.
    uint32_t maxCachedNum = 0; // max cache model
    CacheMode cacheMode = CacheMode::CACHE_BUILDED_MODEL;
};

struct ModelDeviceConfig {
    DeviceConfigMode deviceConfigMode = DeviceConfigMode::AUTO;
    FallBackMode fallBackMode = FallBackMode::ENABLE;
    std::vector<ExecuteDevice> modelDeviceOrder;
    std::map<std::string, std::vector<ExecuteDevice>> opDeviceOrder;
    DeviceMemoryReusePlan deviceMemoryReusePlan = DeviceMemoryReusePlan::UNSET;
};
struct ModelBuildOptions {
    std::vector<NDTensorDesc> inputTensorDescs;
    FormatMode formatMode = FormatMode::USE_NCHW;
    PrecisionMode precisionMode = PrecisionMode::PRECISION_MODE_FP32;
    DynamicShapeConfig dynamicShapeConfig;
    ModelDeviceConfig modelDeviceConfig;
    TuningStrategy tuningStrategy = TuningStrategy::OFF;
    size_t estimatedOutputSize = 0;
    std::string quantizeConfig = "";
};
} // namespace hiai
#endif // HIAI_API_MODEL_BUILDER_TYPES_H
