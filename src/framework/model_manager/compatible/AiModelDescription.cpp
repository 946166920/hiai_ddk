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
#include <string>
#include <vector>

#include "compatible/hiai_base_types_cpt.h"
#include "compatible/HiAiModelManagerService.h"
#include "framework/infra/log/log.h"
#include "model_builder/model_builder_types.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {

AiModelDescription::AiModelDescription(const std::string& pmodelName, const int32_t frequency, const int32_t framework,
    const int32_t pmodelType, const int32_t pdeviceType)
{
    this->model_name_ = pmodelName;
    this->frequency_ = frequency;
    this->framework_ = framework;
    this->modelType_ = pmodelType;
    this->deviceType_ = pdeviceType;
}

AiModelDescription::~AiModelDescription()
{
}

const std::string& AiModelDescription::GetName() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return this->model_name_;
}

int32_t AiModelDescription::GetFrequency() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return this->frequency_;
}

int32_t AiModelDescription::GetFramework() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return this->framework_;
}

int32_t AiModelDescription::GetModelType() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return this->modelType_;
}

int32_t AiModelDescription::GetDeviceType() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return this->deviceType_;
}

AIStatus AiModelDescription::GetDynamicShapeConfig(DynamicShapeConfig& dynamicShape) const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    dynamicShape = dynamicShape_;
    return AI_SUCCESS;
}

AIStatus AiModelDescription::SetDynamicShapeConfig(const DynamicShapeConfig& dynamicShape)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dynamicShape.enable == false) {
        FMK_LOGI("not set dynamic shape config");
        dynamicShape_ = dynamicShape;
        return AI_SUCCESS;
    }

    if (dynamicShape.cacheMode != static_cast<CacheMode>(CacheMode::CACHE_BUILDED_MODEL) &&
        dynamicShape.cacheMode != static_cast<CacheMode>(CacheMode::CACHE_LOADED_MODEL)) {
        FMK_LOGE("dynamic shape cache mode is error");
        return AI_FAILED;
    }
    if (dynamicShape.maxCachedNum < hiai::MIN_DYNAMIC_SHAPE_CACHE_NUM ||
        dynamicShape.maxCachedNum > hiai::MAX_DYNAMIC_SHAPE_CACHE_NUM) {
        FMK_LOGE("max cached num should be from %d to %d", hiai::MIN_DYNAMIC_SHAPE_CACHE_NUM,
            hiai::MAX_DYNAMIC_SHAPE_CACHE_NUM);
        return AI_FAILED;
    }
    dynamicShape_ = dynamicShape;
    return AI_SUCCESS;
}

AIStatus AiModelDescription::GetInputDims(std::vector<TensorDimension>& inputDims) const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    inputDims = inputDims_;
    return AI_SUCCESS;
}

AIStatus AiModelDescription::SetInputDims(const std::vector<TensorDimension>& inputDims)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    inputDims_ = inputDims;
    return AI_SUCCESS;
}

void* AiModelDescription::GetModelBuffer() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return modelNetBuffer_;
}

AIStatus AiModelDescription::SetModelBuffer(const void* data, uint32_t size)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (data == nullptr) {
        FMK_LOGE("AiModelDescription SetModelBuffer failed, data can not be null");
        return AI_INVALID_PARA;
    }
    modelNetBuffer_ = (void*)data;
    modelNetSize_ = size;
    modelPath_ = "";
    return AI_SUCCESS;
}

AIStatus AiModelDescription::SetModelPath(const std::string& modelPath)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    modelPath_ = modelPath;
    modelNetBuffer_ = nullptr;
    modelNetSize_ = 0;
    return AI_SUCCESS;
}

uint32_t AiModelDescription::GetModelNetSize() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return modelNetSize_;
}

const std::string& AiModelDescription::GetModelPath() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return modelPath_;
}

AIStatus AiModelDescription::GetPrecisionMode(PrecisionMode& precisionMode) const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    precisionMode = precisionMode_;
    return AI_SUCCESS;
}

AIStatus AiModelDescription::SetPrecisionMode(const PrecisionMode& precisionMode)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (precisionMode != PrecisionMode::PRECISION_MODE_FP32 &&
        precisionMode != PrecisionMode::PRECISION_MODE_FP16) {
        FMK_LOGE("precision mode is error");
        return AI_FAILED;
    }
    precisionMode_ = precisionMode;
    return AI_SUCCESS;
}

AIStatus AiModelDescription::SetTuningStrategy(const TuningStrategy& tuningStrategy)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (tuningStrategy < TuningStrategy::OFF ||
        tuningStrategy > TuningStrategy::ON_CLOUD_TUNING) {
        FMK_LOGE("tuning strategy is error");
        return AI_FAILED;
    }
    tuningStrategy_ = tuningStrategy;
    return AI_SUCCESS;
}

const TuningStrategy AiModelDescription::GetTuningStrategy() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return tuningStrategy_;
}
} // namespace hiai
