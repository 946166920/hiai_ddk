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
#include "BuildOptionUtil.h"

#include <string>
#include <map>
#include <vector>

#include "tensor/nd_tensor_desc.h"

namespace hiai {
static void Convert2NDTensorDesc(const std::vector<int64_t>& dims, NDTensorDesc& desc)
{
    desc.dims.resize(dims.size());
    for (int64_t dim : dims) {
        desc.dims.emplace_back(static_cast<int32_t>(dim));
    }
    desc.dataType = DataType::FLOAT32;
    desc.format = Format::NCHW;
}

static void Convert2OpDeviceConfig(const std::map<std::string, std::vector<OpExecuteDevice>>& deviceConfig,
    std::map<std::string, std::vector<ExecuteDevice>>& opDeviceConfig)
{
    for (const auto& itOp : deviceConfig) {
        auto& devList = opDeviceConfig[itOp.first];
        for (auto itDevice : itOp.second) {
            devList.push_back(static_cast<ExecuteDevice>(itDevice));
        }
    }
}

static void Convert2ModelDeviceConfig(const BuildOptions& options, ModelDeviceConfig& modelDeviceConfig)
{
    modelDeviceConfig.fallBackMode = (options.mode == CUSTOM ? FallBackMode::DISABLE : FallBackMode::ENABLE);
    modelDeviceConfig.deviceConfigMode = DeviceConfigMode::AUTO;
    Convert2OpDeviceConfig(options.opExecuteDeviceConfig, modelDeviceConfig.opDeviceOrder);
    if (!modelDeviceConfig.opDeviceOrder.empty()) {
        modelDeviceConfig.deviceConfigMode = DeviceConfigMode::OP_LEVEL;
    }
}

ModelBuildOptions BuildOptionUtil::Convert2ModelBuildOptions(const BuildOptions& options)
{
    ModelBuildOptions tmpOptions;
    tmpOptions.formatMode = (options.useOriginFormat ? FormatMode::USE_ORIGIN : FormatMode::USE_NCHW);
    tmpOptions.precisionMode =
        (options.weightDataType == WeightDataType::FP32 ? PRECISION_MODE_FP32 : PRECISION_MODE_FP16);
    tmpOptions.inputTensorDescs.resize(options.inputShapes.size());
    for (size_t i = 0; i < options.inputShapes.size(); i++) {
        Convert2NDTensorDesc(options.inputShapes[i], tmpOptions.inputTensorDescs[i]);
    }
    Convert2ModelDeviceConfig(options, tmpOptions.modelDeviceConfig);
    return tmpOptions;
}
} // namespace hiai