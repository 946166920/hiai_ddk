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
#ifndef MODEL_RUNTIME_CORE_HIAI_MODEL_BUILDER_OPTIONS_V1_H
#define MODEL_RUNTIME_CORE_HIAI_MODEL_BUILDER_OPTIONS_V1_H

#include "framework/c/hiai_model_builder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

HIAI_MR_DynamicShapeConfig* HIAI_DynamicShapeConfigV1_Create(void);
void HIAI_DynamicShapeConfigV1_Destroy(HIAI_MR_DynamicShapeConfig** config);

void HIAI_DynamicShapeConfigV1_SetEnableMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_ENABLE_MODE mode);
HIAI_DYNAMIC_SHAPE_ENABLE_MODE HIAI_DynamicShapeConfigV1_GetEnableMode(const HIAI_MR_DynamicShapeConfig* config);

void HIAI_DynamicShapeConfigV1_SetMaxCacheNum(HIAI_MR_DynamicShapeConfig* config, size_t maxCacheNum);
size_t HIAI_DynamicShapeConfigV1_GetMaxCacheNum(const HIAI_MR_DynamicShapeConfig* config);

void HIAI_DynamicShapeConfigV1_SetCacheMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_CACHE_MODE mode);
HIAI_DYNAMIC_SHAPE_CACHE_MODE HIAI_DynamicShapeConfigV1_GetCacheMode(const HIAI_MR_DynamicShapeConfig* config);

HIAI_MR_OpDeviceOrder* HIAI_OpDeviceOrderV1_Create(void);

void HIAI_OpDeviceOrderV1_SetOpName(HIAI_MR_OpDeviceOrder* config, const char* opName);
const char* HIAI_OpDeviceOrderV1_GetOpName(const HIAI_MR_OpDeviceOrder* config);

void HIAI_OpDeviceOrderV1_SetDeviceOrder(
    HIAI_MR_OpDeviceOrder* config, size_t supportedDeviceNum, HIAI_EXECUTE_DEVICE* supportedDevices);
size_t HIAI_OpDeviceOrderV1_GetSupportedDeviceNum(const HIAI_MR_OpDeviceOrder* config);
HIAI_EXECUTE_DEVICE* HIAI_OpDeviceOrderV1_GetSupportedDevices(const HIAI_MR_OpDeviceOrder* config);

void HIAI_OpDeviceOrderV1_Destroy(HIAI_MR_OpDeviceOrder** config);

HIAI_MR_CLCustomization* HIAI_CLCustomizationV1_Create(void);
void HIAI_CLCustomizationV1_Destroy(HIAI_MR_CLCustomization** config);

void HIAI_CLCustomizationV1_SetOpName(HIAI_MR_CLCustomization* config, const char* opName);
const char* HIAI_CLCustomizationV1_GetOpName(const HIAI_MR_CLCustomization* config);

void HIAI_CLCustomizationV1_SetCustomization(HIAI_MR_CLCustomization* config, const char* customization);
const char* HIAI_CLCustomizationV1_GetCustomization(const HIAI_MR_CLCustomization* config);

// cl
HIAI_MR_ModelDeviceConfig* HIAI_ModelDeviceConfigV1_Create(void);

void HIAI_ModelDeviceConfigV1_SetDeviceConfigMode(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_CONFIG_MODE deviceConfigMode);
HIAI_DEVICE_CONFIG_MODE HIAI_ModelDeviceConfigV1_GetDeviceConfigMode(const HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_SetFallBackMode(HIAI_MR_ModelDeviceConfig* config, HIAI_FALLBACK_MODE fallBackMode);
HIAI_FALLBACK_MODE HIAI_ModelDeviceConfigV1_GetFallBackMode(const HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_SetModelDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configModelNum, HIAI_EXECUTE_DEVICE* modelDeviceOrder);
size_t HIAI_ModelDeviceConfigV1_GetConfigModelNum(const HIAI_MR_ModelDeviceConfig* config);
HIAI_EXECUTE_DEVICE* HIAI_ModelDeviceConfigV1_GetModelDeviceOrder(const HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_SetOpDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configOpNum, HIAI_MR_OpDeviceOrder** opDeviceOrder);
size_t HIAI_ModelDeviceConfigV1_GetConfigOpNum(const HIAI_MR_ModelDeviceConfig* config);
HIAI_MR_OpDeviceOrder** HIAI_ModelDeviceConfigV1_GetOpDeviceOrder(const HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_SetDeviceMemoryReusePlan(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan);
HIAI_DEVICE_MEMORY_REUSE_PLAN HIAI_ModelDeviceConfigV1_GetDeviceMemoryReusePlan(
    const HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_SetCLCustomization(
    HIAI_MR_ModelDeviceConfig* config, HIAI_MR_CLCustomization** clCustomization);
HIAI_MR_CLCustomization** HIAI_ModelDeviceConfigV1_GetCLCustomization(HIAI_MR_ModelDeviceConfig* config);

void HIAI_ModelDeviceConfigV1_Destroy(HIAI_MR_ModelDeviceConfig** config);

HIAI_MR_ModelBuildOptions* HIAI_ModelBuildOptionsV1_Create(void);

void HIAI_ModelBuildOptionsV1_SetInputTensorDescs(
    HIAI_MR_ModelBuildOptions* options, size_t inputNum, HIAI_NDTensorDesc** inputTensorDescs);

size_t HIAI_ModelBuildOptionsV1_GetInputSize(const HIAI_MR_ModelBuildOptions* options);

HIAI_NDTensorDesc** HIAI_ModelBuildOptionsV1_GetInputTensorDescs(const HIAI_MR_ModelBuildOptions* options);

void HIAI_ModelBuildOptionsV1_SetFormatModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_FORMAT_MODE_OPTION formatMode);

HIAI_FORMAT_MODE_OPTION HIAI_ModelBuildOptionsV1_GetFormatModeOption(const HIAI_MR_ModelBuildOptions* options);

HIAI_Status HIAI_ModelBuildOptionsV1_SetPrecisionModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_PRECISION_MODE_OPTION precisionMode);
HIAI_PRECISION_MODE_OPTION HIAI_ModelBuildOptionsV1_GetPrecisionModeOption(const HIAI_MR_ModelBuildOptions* options);

void HIAI_ModelBuildOptionsV1_SetDynamicShapeConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_DynamicShapeConfig* dynamicShapeConfig);
HIAI_MR_DynamicShapeConfig* HIAI_ModelBuildOptionsV1_GetDynamicShapeConfig(const HIAI_MR_ModelBuildOptions* options);

void HIAI_ModelBuildOptionsV1_SetModelDeviceConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_ModelDeviceConfig* modelDeviceConfig);
HIAI_MR_ModelDeviceConfig* HIAI_ModelBuildOptionsV1_GetModelDeviceConfig(const HIAI_MR_ModelBuildOptions* options);

HIAI_Status HIAI_ModelBuildOptionsV1_SetTuningStrategy(
    HIAI_MR_ModelBuildOptions* options, HIAI_TUNING_STRATEGY tuningStrategy);
HIAI_TUNING_STRATEGY HIAI_ModelBuildOptionsV1_GetTuningStrategy(const HIAI_MR_ModelBuildOptions* options);

void HIAI_ModelBuildOptionsV1_SetEstimatedOutputSize(HIAI_MR_ModelBuildOptions* options, size_t size);
size_t HIAI_ModelBuildOptionsV1_GetEstimatedOutputSize(const HIAI_MR_ModelBuildOptions* options);

HIAI_Status HIAI_ModelBuildOptionsV1_SetQuantizeConfig(HIAI_MR_ModelBuildOptions* options, uint8_t* data, size_t size);
uint8_t* HIAI_ModelBuildOptionsV1_GetQuantizeConfigData(const HIAI_MR_ModelBuildOptions* options);
size_t HIAI_ModelBuildOptionsV1_GetQuantizeConfigSize(const HIAI_MR_ModelBuildOptions* options);

void HIAI_ModelBuildOptionsV1_Destroy(HIAI_MR_ModelBuildOptions** options);

#ifdef __cplusplus
}
#endif

#endif