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
#ifndef _HCS_TYPES_H_
#define _HCS_TYPES_H_

#include <memory>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include "graph/types.h"
#include "infra/base/hiai_types.h"
#include "model_builder/model_builder_types.h"
#include "framework/util/om_options.h"
#include "framework/util/bin_object.h"
#include "model_manager/model_manager_types.h"

namespace ge {
using namespace hiai;
class MemoryAllocator;
#define HCS_CONSTRUCTOR __attribute__((constructor))
#define HCS_DESTRUCTOR __attribute__((destructor))

#ifndef HCS_API_EXPORT
#if (!defined USE_STATIC_LIB) && (!defined NO_HCS_API_EXPORT)
#if defined(_WIN32)
#define HCS_API_EXPORT __declspec(dllexport)
#else
#if __GNUC__ >= 4
#define HCS_API_EXPORT __attribute__((__visibility__("default")))
#else
#define HCS_API_EXPORT
#endif
#endif
#else
#define HCS_API_EXPORT
#endif
#endif

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&&... params)
{
    return std::unique_ptr<T>(new (std::nothrow) T(std::forward<Ts>(params)...));
}

using ExecutionCallback = std::function<void(uint32_t modelId, uint32_t taskId, int result)>;

struct ModelConfig {
    ge::ExecuteFallBackMode fallBackMode = ge::ExecuteFallBackMode::ENABLE;
    std::map<std::string, std::vector<ge::DeviceType>> opDeviceOrder;
};

constexpr int MODEL_VOTE_TIMEOUT_MS = 3000;
using DynamicShape = hiai::DynamicShapeConfig;
struct LoadModelOptions {
    PerfMode perfMode = PerfMode::UNSET;
    int perfTimeOut = MODEL_VOTE_TIMEOUT_MS;
    int32_t priority = 0;
    uint32_t pid = 0;
    std::string key;
    OMOptions omOptions1;
    bool usePipeline = false;
    bool isSupportTask = false;
    DynamicShape dynamicShape;
    std::vector<std::vector<int64_t>> inputShapes;
    bool dedicatedModel = false;
    int useClientMemModelCnt = 0;
    std::shared_ptr<MemoryAllocator> memAllocator = nullptr;
    hiai::PrecisionMode precisionMode = hiai::PrecisionMode::PRECISION_MODE_FP32;
    hiai::TuningStrategy tuningStrategy = hiai::TuningStrategy::OFF;
    // 静态多Shape加载模型指定的shape档位
    uint32_t multiShapeIndex = 0;
};

const char* const OM_YOLO_DETECTION_TYPE = "YoloPostDetectionOutput";
const char* const GRAPH_OP_TYPE = "GraphOp";
const char* const CAST_T_OP_TYPE = "CastT";
const char* const CAST_OP_TYPE = "Cast";
const char* const AIPP_CONFIG_OP_TYPE = "AippConfig";
const char* const OM_DATA_OP_TYPE = "Old_om_data";
const char* const OM_NETOUT_OP_TYPE = "Old_om_Netoutput";
const char* const GRAPHOP_ATTR_WEIGHT_SIZE = "weight_size";
const char* const GRAPHOP_ATTR_FEATUREMAP_SIZE = "memory_size";
const char* const GRAPHOP_ATTR_SUBGRAPH = "subGraph";
const char* const GRAPHOP_ATTR_HIAI_VERSION = "hiai_version";
const char* const GRAPHOP_ATTR_CL_NAME = "cl_name";
const char* const GRAPH_ATTR_NPU_INFERSHAPED = "graph_infershaped_flag";
const char* const GRAPH_ATTR_IR_INFERSHAPED = "ir_infershaped";
const char* const GRAPH_ATTR_USE_ORIGIN_FORMAT = "use_origin_format";
const char* const GRAPH_ATTR_DEVICE_MEMORY_REUSE_PLAN = "device_memory_reuse_plan";

const char* const NPU_CL = "NPUCL";
const char* const DNNA_CL = "DNNACL";
const char* const CPU_CL_APP = "CPUCL_APP";
const char* const CPU_CL_ROM = "CPUCL_ROM";
const char* const GPU_CL = "GPUCL";
const char* const FMK_CL = "FMK_CL";
const char* const NPUCL_ISPNN = "NPUCL_ISPNN";

constexpr int32_t NCHW_DIM_N = 0;
constexpr int32_t NCHW_DIM_C = 1;
constexpr int32_t NCHW_DIM_H = 2;
constexpr int32_t NCHW_DIM_W = 3;

constexpr int32_t NHWC_DIM_N = 0;
constexpr int32_t NHWC_DIM_H = 1;
constexpr int32_t NHWC_DIM_W = 2;
constexpr int32_t NHWC_DIM_C = 3;

constexpr int32_t MONITOR_LOOPDELAYTIME_MS = 1000;
constexpr int32_t MONITOR_WATCHDOG_TIMEOUT_MS = 1000;
} // namespace ge
#endif
