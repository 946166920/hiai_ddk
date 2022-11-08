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
#ifndef FRAMEWORK_C_HIAI_MODEL_BUILDER_TYPES_H
#define FRAMEWORK_C_HIAI_MODEL_BUILDER_TYPES_H
#include "hiai_c_api_export.h"
#include "hiai_base_types.h"
#include "hiai_nd_tensor_desc.h"
#include "hiai_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif
// build option begin
typedef struct HIAI_ModelBuildOptions HIAI_ModelBuildOptions;

AICP_C_API_EXPORT HIAI_ModelBuildOptions* HIAI_ModelBuildOptions_Create(void);
AICP_C_API_EXPORT void HIAI_ModelBuildOptions_Destroy(HIAI_ModelBuildOptions** options);

// inputTensorDescs will free in HIAI_ModelBuilderOptions_Destroy
AICP_C_API_EXPORT void HIAI_ModelBuildOptions_SetInputTensorDescs(
    HIAI_ModelBuildOptions* options, size_t inputNum, HIAI_NDTensorDesc** inputTensorDescs);
AICP_C_API_EXPORT size_t HIAI_ModelBuildOptions_GetInputSize(const HIAI_ModelBuildOptions* options);
AICP_C_API_EXPORT HIAI_NDTensorDesc** HIAI_ModelBuildOptions_GetInputTensorDescs(const HIAI_ModelBuildOptions* options);

typedef enum { HIAI_FORMAT_MODE_USE_NCHW = 0, HIAI_FORMAT_MODE_USE_ORIGIN = 1 } HIAI_FORMAT_MODE_OPTION;
AICP_C_API_EXPORT void HIAI_ModelBuildOptions_SetFormatModeOption(
    HIAI_ModelBuildOptions* options, HIAI_FORMAT_MODE_OPTION formatMode);
AICP_C_API_EXPORT HIAI_FORMAT_MODE_OPTION HIAI_ModelBuildOptions_GetFormatModeOption(
    const HIAI_ModelBuildOptions* options);

typedef enum { HIAI_PRECISION_MODE_FP32 = 0, HIAI_PRECISION_MODE_FP16 } HIAI_PRECISION_MODE_OPTION;

AICP_C_API_EXPORT HIAI_Status HIAI_ModelBuildOptions_SetPrecisionModeOption(
    HIAI_ModelBuildOptions* options, HIAI_PRECISION_MODE_OPTION precisionMode);
AICP_C_API_EXPORT HIAI_PRECISION_MODE_OPTION HIAI_ModelBuildOptions_GetPrecisionModeOption(
    const HIAI_ModelBuildOptions* options);

typedef enum {
    HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL,
    HIAI_DYNAMIC_SHAPE_CACHE_LOADED_MODEL
} HIAI_DYNAMIC_SHAPE_CACHE_MODE;

typedef enum { HIAI_DYNAMIC_SHAPE_DISABLE = 0, HIAI_DYNAMIC_SHAPE_ENABLE = 1 } HIAI_DYNAMIC_SHAPE_ENABLE_MODE;

typedef struct HIAI_DynamicShapeConfig HIAI_DynamicShapeConfig;

AICP_C_API_EXPORT HIAI_DynamicShapeConfig* HIAI_DynamicShapeConfig_Create(void);

AICP_C_API_EXPORT void HIAI_DynamicShapeConfig_SetEnableMode(
    HIAI_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_ENABLE_MODE mode);
AICP_C_API_EXPORT HIAI_DYNAMIC_SHAPE_ENABLE_MODE HIAI_DynamicShapeConfig_GetEnableMode(
    const HIAI_DynamicShapeConfig* config);

// maxCacheNum should be between 1 and 10.
AICP_C_API_EXPORT void HIAI_DynamicShapeConfig_SetMaxCacheNum(HIAI_DynamicShapeConfig* config, size_t maxCacheNum);
AICP_C_API_EXPORT size_t HIAI_DynamicShapeConfig_GetMaxCacheNum(const HIAI_DynamicShapeConfig* config);
AICP_C_API_EXPORT
AICP_C_API_EXPORT void HIAI_DynamicShapeConfig_SetCacheMode(
    HIAI_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_CACHE_MODE mode);
AICP_C_API_EXPORT HIAI_DYNAMIC_SHAPE_CACHE_MODE HIAI_DynamicShapeConfig_GetCacheMode(
    const HIAI_DynamicShapeConfig* config);

AICP_C_API_EXPORT void HIAI_DynamicShapeConfig_Destroy(HIAI_DynamicShapeConfig** config);

AICP_C_API_EXPORT void HIAI_ModelBuildOptions_SetDynamicShapeConfig(
    HIAI_ModelBuildOptions* options, HIAI_DynamicShapeConfig* dynamicShapeConfig);
AICP_C_API_EXPORT HIAI_DynamicShapeConfig* HIAI_ModelBuildOptions_GetDynamicShapeConfig(
    const HIAI_ModelBuildOptions* options);

typedef enum {
    HIAI_DEVICE_CONFIG_MODE_AUTO,
    HIAI_DEVICE_CONFIG_MODE_MODEL_LEVEL,
    HIAI_DEVICE_CONFIG_MODE_OP_LEVEL
} HIAI_DEVICE_CONFIG_MODE;

typedef enum { HIAI_FALLBACK_MODE_ENABLE, HIAI_FALLBACK_MODE_DISABLE } HIAI_FALLBACK_MODE;

typedef enum { HIAI_EXECUTE_DEVICE_NPU = 0, HIAI_EXECUTE_DEVICE_CPU = 1 } HIAI_EXECUTE_DEVICE;

typedef enum {
    HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET = 0,
    HIAI_DEVICE_MEMORY_REUSE_PLAN_LOW = 1,
    HIAI_DEVICE_MEMORY_REUSE_PLAN_HIGH = 2
} HIAI_DEVICE_MEMORY_REUSE_PLAN;

// op device select config begin
typedef struct HIAI_OpDeviceOrder HIAI_OpDeviceOrder;

AICP_C_API_EXPORT HIAI_OpDeviceOrder* HIAI_OpDeviceOrder_Create(void);
AICP_C_API_EXPORT void HIAI_OpDeviceOrder_SetOpName(HIAI_OpDeviceOrder* config, const char* opName);
AICP_C_API_EXPORT const char* HIAI_OpDeviceOrder_GetOpName(const HIAI_OpDeviceOrder* config);
AICP_C_API_EXPORT void HIAI_OpDeviceOrder_SetDeviceOrder(
    HIAI_OpDeviceOrder* config, size_t supportedDeviceNum, HIAI_EXECUTE_DEVICE* supportedDevices);
AICP_C_API_EXPORT size_t HIAI_OpDeviceOrder_GetSupportedDeviceNum(const HIAI_OpDeviceOrder* config);
AICP_C_API_EXPORT HIAI_EXECUTE_DEVICE* HIAI_OpDeviceOrder_GetSupportedDevices(const HIAI_OpDeviceOrder* config);

AICP_C_API_EXPORT void HIAI_OpDeviceOrder_Destroy(HIAI_OpDeviceOrder** config);
// op device select config end

// cl Customizations select config begin
typedef struct HIAI_CLCustomization HIAI_CLCustomization;

AICP_C_API_EXPORT HIAI_CLCustomization* HIAI_CLCustomization_Create(void);
AICP_C_API_EXPORT void HIAI_CLCustomization_SetOpName(HIAI_CLCustomization* config, const char* opName);
AICP_C_API_EXPORT const char* HIAI_CLCustomization_GetOpName(const HIAI_CLCustomization* config);
AICP_C_API_EXPORT void HIAI_CLCustomization_SetCustomization(HIAI_CLCustomization* config, const char* customization);
AICP_C_API_EXPORT const char* HIAI_CLCustomization_GetCustomization(const HIAI_CLCustomization* config);
AICP_C_API_EXPORT void HIAI_CLCustomization_Destroy(HIAI_CLCustomization** config);
// cl Customizations select config end

// device select config begin
typedef struct HIAI_ModelDeviceConfig HIAI_ModelDeviceConfig;

AICP_C_API_EXPORT HIAI_ModelDeviceConfig* HIAI_ModelDeviceConfig_Create(void);

AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetDeviceConfigMode(
    HIAI_ModelDeviceConfig* config, HIAI_DEVICE_CONFIG_MODE deviceConfigMode);
AICP_C_API_EXPORT HIAI_DEVICE_CONFIG_MODE HIAI_ModelDeviceConfig_GetDeviceConfigMode(
    const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetFallBackMode(
    HIAI_ModelDeviceConfig* config, HIAI_FALLBACK_MODE fallBackMode);
AICP_C_API_EXPORT HIAI_FALLBACK_MODE HIAI_ModelDeviceConfig_GetFallBackMode(const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetModelDeviceOrder(
    HIAI_ModelDeviceConfig* config, size_t configModelNum, HIAI_EXECUTE_DEVICE* modelDeviceOrder);
AICP_C_API_EXPORT size_t HIAI_ModelDeviceConfig_GetConfigModelNum(const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT HIAI_EXECUTE_DEVICE* HIAI_ModelDeviceConfig_GetModelDeviceOrder(const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetOpDeviceOrder(
    HIAI_ModelDeviceConfig* config, size_t configOpNum, HIAI_OpDeviceOrder** opDeviceOrder);
AICP_C_API_EXPORT size_t HIAI_ModelDeviceConfig_GetConfigOpNum(const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT HIAI_OpDeviceOrder** HIAI_ModelDeviceConfig_GetOpDeviceOrder(const HIAI_ModelDeviceConfig* config);

AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetDeviceMemoryReusePlan(
    HIAI_ModelDeviceConfig* config, HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan);
AICP_C_API_EXPORT HIAI_DEVICE_MEMORY_REUSE_PLAN HIAI_ModelDeviceConfig_GetDeviceMemoryReusePlan(
    const HIAI_ModelDeviceConfig* config);
AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_SetCLCustomization(
    HIAI_ModelDeviceConfig* config, HIAI_CLCustomization** clCustomization);
AICP_C_API_EXPORT HIAI_CLCustomization** HIAI_ModelDeviceConfig_GetCLCustomization(HIAI_ModelDeviceConfig* config);

AICP_C_API_EXPORT void HIAI_ModelDeviceConfig_Destroy(HIAI_ModelDeviceConfig** config);
// device select config end

AICP_C_API_EXPORT void HIAI_ModelBuildOptions_SetModelDeviceConfig(
    HIAI_ModelBuildOptions* options, HIAI_ModelDeviceConfig* modelDeviceConfig);
AICP_C_API_EXPORT HIAI_ModelDeviceConfig* HIAI_ModelBuildOptions_GetModelDeviceConfig(
    const HIAI_ModelBuildOptions* options);

typedef enum {
    HIAI_TUNING_STRATEGY_OFF = 0,
    HIAI_TUNING_STRATEGY_ON_DEVICE_TUNING,
    HIAI_TUNING_STRATEGY_ON_DEVICE_PREPROCESS_TUNING,
    HIAI_TUNING_STRATEGY_ON_CLOUD_TUNING
} HIAI_TUNING_STRATEGY;

AICP_C_API_EXPORT HIAI_Status HIAI_ModelBuildOptions_SetTuningStrategy(
    HIAI_ModelBuildOptions* options, HIAI_TUNING_STRATEGY tuningStrategy);
AICP_C_API_EXPORT HIAI_TUNING_STRATEGY HIAI_ModelBuildOptions_GetTuningStrategy(const HIAI_ModelBuildOptions* options);

AICP_C_API_EXPORT void HIAI_ModelBuildOptions_SetEstimatedOutputSize(HIAI_ModelBuildOptions* options, size_t size);
AICP_C_API_EXPORT size_t HIAI_ModelBuildOptions_GetEstimatedOutputSize(const HIAI_ModelBuildOptions* options);

typedef struct HIAI_ConfigBuffer HIAI_ConfigBuffer;
AICP_C_API_EXPORT HIAI_Status HIAI_ModelBuildOptions_SetQuantizeConfig(
    HIAI_ModelBuildOptions* options, uint8_t* data, size_t size);
AICP_C_API_EXPORT uint8_t* HIAI_ModelBuildOptions_GetQuantizeConfigData(const HIAI_ModelBuildOptions* options);
AICP_C_API_EXPORT size_t HIAI_ModelBuildOptions_GetQuantizeConfigSize(const HIAI_ModelBuildOptions* options);

// build option end

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_BUILDER_H
