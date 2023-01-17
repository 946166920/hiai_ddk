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
#include "hiai_model_build_options_v1.h"

#include "securec.h"
#include "framework/infra/log/log.h"
#include "limits.h"

typedef struct HIAI_DynamicShapeConfigV1 {
    HIAI_DYNAMIC_SHAPE_ENABLE_MODE enableMode;
    size_t maxCachedNum;
    HIAI_DYNAMIC_SHAPE_CACHE_MODE cacheMode;
} HIAI_DynamicShapeConfigV1;

HIAI_MR_DynamicShapeConfig* HIAI_DynamicShapeConfigV1_Create(void)
{
    HIAI_DynamicShapeConfigV1* config = (HIAI_DynamicShapeConfigV1*)malloc(sizeof(HIAI_DynamicShapeConfigV1));
    MALLOC_NULL_CHECK_RET_VALUE(config, NULL);

    config->enableMode = HIAI_DYNAMIC_SHAPE_DISABLE;
    config->maxCachedNum = 0;
    config->cacheMode = HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL;
    return (HIAI_MR_DynamicShapeConfig*)config;
}

void HIAI_DynamicShapeConfigV1_SetEnableMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_ENABLE_MODE mode)
{
    if (config == NULL) {
        return;
    }
    if (mode < HIAI_DYNAMIC_SHAPE_DISABLE || mode > HIAI_DYNAMIC_SHAPE_ENABLE) {
        FMK_LOGE("set enable mode error.");
        return;
    }
    ((HIAI_DynamicShapeConfigV1*)config)->enableMode = mode;
}

HIAI_DYNAMIC_SHAPE_ENABLE_MODE HIAI_DynamicShapeConfigV1_GetEnableMode(const HIAI_MR_DynamicShapeConfig* config)
{
    return config == NULL ? HIAI_DYNAMIC_SHAPE_DISABLE : ((HIAI_DynamicShapeConfigV1*)config)->enableMode;
}

void HIAI_DynamicShapeConfigV1_SetMaxCacheNum(HIAI_MR_DynamicShapeConfig* config, size_t maxCacheNum)
{
    if (config == NULL) {
        return;
    }

    ((HIAI_DynamicShapeConfigV1*)config)->maxCachedNum = maxCacheNum;
}

size_t HIAI_DynamicShapeConfigV1_GetMaxCacheNum(const HIAI_MR_DynamicShapeConfig* config)
{
    return config == NULL ? 0 : ((HIAI_DynamicShapeConfigV1*)config)->maxCachedNum;
}

void HIAI_DynamicShapeConfigV1_SetCacheMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_CACHE_MODE mode)
{
    if (config == NULL) {
        return;
    }

    if (mode < HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL || mode > HIAI_DYNAMIC_SHAPE_CACHE_LOADED_MODEL) {
        FMK_LOGE("set cache mode error.");
        return;
    }
    ((HIAI_DynamicShapeConfigV1*)config)->cacheMode = mode;
}

HIAI_DYNAMIC_SHAPE_CACHE_MODE HIAI_DynamicShapeConfigV1_GetCacheMode(const HIAI_MR_DynamicShapeConfig* config)
{
    return config == NULL ? HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL : ((HIAI_DynamicShapeConfigV1*)config)->cacheMode;
}

void HIAI_DynamicShapeConfigV1_Destroy(HIAI_MR_DynamicShapeConfig** config)
{
    if (config == NULL || *config == NULL) {
        return;
    }
    free((HIAI_DynamicShapeConfigV1*)(*config));
    *config = NULL;
}

typedef struct HIAI_OpDeviceOrderV1 {
    char* opName;
    size_t supportedDeviceNum;
    HIAI_EXECUTE_DEVICE* supportedDevices;
} HIAI_OpDeviceOrderV1;

HIAI_MR_OpDeviceOrder* HIAI_OpDeviceOrderV1_Create(void)
{
    HIAI_OpDeviceOrderV1* config = (HIAI_OpDeviceOrderV1*)malloc(sizeof(HIAI_OpDeviceOrderV1));
    MALLOC_NULL_CHECK_RET_VALUE(config, NULL);
    (void)memset_s(config, sizeof(HIAI_OpDeviceOrderV1), 0, sizeof(HIAI_OpDeviceOrderV1));

    return (HIAI_MR_OpDeviceOrder*)config;
}

static char* HIAI_CopyName(const char* name)
{
    size_t nameLen = strlen(name);
    if (nameLen >= (UINT_MAX - 1)) {
        FMK_LOGW("nameLen set too large.");
        return NULL;
    }
    char* dstName = (char*)malloc(nameLen + 1);
    if (dstName == NULL) {
        FMK_LOGW("malloc faied.");
        return NULL;
    }
    if (memcpy_s(dstName, nameLen + 1, name, nameLen) != 0) {
        FMK_LOGW("memcpy faied.");
        free(dstName);
        dstName = NULL;
        return NULL;
    }
    dstName[nameLen] = '\0';
    return dstName;
}

void HIAI_OpDeviceOrderV1_SetOpName(HIAI_MR_OpDeviceOrder* config, const char* opName)
{
    if (config == NULL || opName == NULL) {
        return;
    }

    HIAI_OpDeviceOrderV1* configV1 = (HIAI_OpDeviceOrderV1*)config;
    if (configV1->opName != NULL) {
        FMK_LOGW("opName set repeatedly.");
        return;
    }

    configV1->opName = HIAI_CopyName(opName);
}

const char* HIAI_OpDeviceOrderV1_GetOpName(const HIAI_MR_OpDeviceOrder* config)
{
    return config == NULL ? NULL : ((HIAI_OpDeviceOrderV1*)config)->opName;
}

void HIAI_OpDeviceOrderV1_SetDeviceOrder(
    HIAI_MR_OpDeviceOrder* config, size_t supportedDeviceNum, HIAI_EXECUTE_DEVICE* supportedDevices)
{
    if (config == NULL || supportedDevices == NULL) {
        return;
    }

    HIAI_OpDeviceOrderV1* configV1 = (HIAI_OpDeviceOrderV1*)config;
    if (configV1->supportedDevices != NULL) {
        FMK_LOGW("supportedDevices set repeatedly.");
        return;
    }
    configV1->supportedDeviceNum = supportedDeviceNum;
    configV1->supportedDevices = supportedDevices;
}

size_t HIAI_OpDeviceOrderV1_GetSupportedDeviceNum(const HIAI_MR_OpDeviceOrder* config)
{
    return config == NULL ? 0 : ((HIAI_OpDeviceOrderV1*)config)->supportedDeviceNum;
}

HIAI_EXECUTE_DEVICE* HIAI_OpDeviceOrderV1_GetSupportedDevices(const HIAI_MR_OpDeviceOrder* config)
{
    return config == NULL ? NULL : ((HIAI_OpDeviceOrderV1*)config)->supportedDevices;
}

void HIAI_OpDeviceOrderV1_Destroy(HIAI_MR_OpDeviceOrder** config)
{
    if (config == NULL || *config == NULL) {
        return;
    }

    HIAI_OpDeviceOrderV1* tmp = (HIAI_OpDeviceOrderV1*)(*config);
    if (tmp->opName != NULL) {
        free(tmp->opName);
    }

    if (tmp->supportedDevices != NULL) {
        free(tmp->supportedDevices);
    }
    free(tmp);
    *config = NULL;
}

typedef struct HIAI_CLCustomizationV1 {
    char* opName;
    char* customization;
} HIAI_CLCustomizationV1;

HIAI_MR_CLCustomization* HIAI_CLCustomizationV1_Create(void)
{
    HIAI_CLCustomizationV1* config = (HIAI_CLCustomizationV1*)malloc(sizeof(HIAI_CLCustomizationV1));
    MALLOC_NULL_CHECK_RET_VALUE(config, NULL);
    (void)memset_s(config, sizeof(HIAI_CLCustomizationV1), 0, sizeof(HIAI_CLCustomizationV1));
    return (HIAI_MR_CLCustomization*)config;
}

void HIAI_CLCustomizationV1_SetOpName(HIAI_MR_CLCustomization* config, const char* opName)
{
    if (config == NULL || opName == NULL) {
        return;
    }

    HIAI_CLCustomizationV1* configV1 = (HIAI_CLCustomizationV1*)config;
    if (configV1->opName != NULL) {
        FMK_LOGW("opName set repeatedly.", configV1->opName);
        return;
    }

    configV1->opName = HIAI_CopyName(opName);
}

const char* HIAI_CLCustomizationV1_GetOpName(const HIAI_MR_CLCustomization* config)
{
    return config == NULL ? NULL : ((HIAI_CLCustomizationV1*)config)->opName;
}

void HIAI_CLCustomizationV1_SetCustomization(HIAI_MR_CLCustomization* config, const char* customization)
{
    if (config == NULL || customization == NULL) {
        return;
    }

    HIAI_CLCustomizationV1* configV1 = (HIAI_CLCustomizationV1*)config;
    if (configV1->customization != NULL) {
        FMK_LOGW("customization set repeatedly.");
        return;
    }

    size_t customizationLen = strlen(customization);
    if (customizationLen >= (UINT_MAX - 1)) {
        FMK_LOGW("customizationLen set too large.");
        return;
    }
    configV1->customization = (char*)malloc(customizationLen + 1);
    if (configV1->customization == NULL) {
        FMK_LOGW("malloc faied.");
        return;
    }
    if (memcpy_s(configV1->customization, customizationLen + 1, customization, customizationLen) != 0) {
        FMK_LOGW("memcpy faied.");
        if (configV1->customization != NULL) {
            free(configV1->customization);
            configV1->customization = NULL;
        }
        return;
    }
    configV1->customization[customizationLen] = '\0';
}

const char* HIAI_CLCustomizationV1_GetCustomization(const HIAI_MR_CLCustomization* config)
{
    return config == NULL ? NULL : ((HIAI_CLCustomizationV1*)config)->customization;
}

void HIAI_CLCustomizationV1_Destroy(HIAI_MR_CLCustomization** config)
{
    if (config == NULL || *config == NULL) {
        return;
    }

    HIAI_CLCustomizationV1* tmp = (HIAI_CLCustomizationV1*)(*config);
    if (tmp->opName != NULL) {
        free(tmp->opName);
    }

    if (tmp->customization != NULL) {
        free(tmp->customization);
    }
    free(*config);
    *config = NULL;
}

typedef struct HIAI_ModelDeviceConfigV1 {
    HIAI_DEVICE_CONFIG_MODE deviceConfigMode;
    HIAI_FALLBACK_MODE fallBackMode;
    size_t configModelNum;
    HIAI_EXECUTE_DEVICE* modelDeviceOrder;
    size_t configOpNum;
    HIAI_MR_OpDeviceOrder** opDeviceOrder;
    HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan;
    HIAI_MR_CLCustomization** clCustomization;
} HIAI_ModelDeviceConfigV1;

HIAI_MR_ModelDeviceConfig* HIAI_ModelDeviceConfigV1_Create(void)
{
    HIAI_ModelDeviceConfigV1* config = (HIAI_ModelDeviceConfigV1*)malloc(sizeof(HIAI_ModelDeviceConfigV1));
    MALLOC_NULL_CHECK_RET_VALUE(config, NULL);
    (void)memset_s(config, sizeof(HIAI_ModelDeviceConfigV1), 0, sizeof(HIAI_ModelDeviceConfigV1));

    config->deviceConfigMode = HIAI_DEVICE_CONFIG_MODE_AUTO;
    config->fallBackMode = HIAI_FALLBACK_MODE_ENABLE;
    return (HIAI_MR_ModelDeviceConfig*)config;
}

void HIAI_ModelDeviceConfigV1_SetDeviceConfigMode(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_CONFIG_MODE deviceConfigMode)
{
    if (config == NULL || deviceConfigMode < HIAI_DEVICE_CONFIG_MODE_AUTO ||
        deviceConfigMode > HIAI_DEVICE_CONFIG_MODE_OP_LEVEL) {
        return;
    }
    ((HIAI_ModelDeviceConfigV1*)config)->deviceConfigMode = deviceConfigMode;
}

HIAI_DEVICE_CONFIG_MODE HIAI_ModelDeviceConfigV1_GetDeviceConfigMode(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? HIAI_DEVICE_CONFIG_MODE_AUTO : ((HIAI_ModelDeviceConfigV1*)config)->deviceConfigMode;
}

void HIAI_ModelDeviceConfigV1_SetFallBackMode(HIAI_MR_ModelDeviceConfig* config, HIAI_FALLBACK_MODE fallBackMode)
{
    if (config == NULL || fallBackMode < HIAI_FALLBACK_MODE_ENABLE || fallBackMode > HIAI_FALLBACK_MODE_DISABLE) {
        return;
    }
    ((HIAI_ModelDeviceConfigV1*)config)->fallBackMode = fallBackMode;
}

HIAI_FALLBACK_MODE HIAI_ModelDeviceConfigV1_GetFallBackMode(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? HIAI_FALLBACK_MODE_ENABLE : ((HIAI_ModelDeviceConfigV1*)config)->fallBackMode;
}

void HIAI_ModelDeviceConfigV1_SetModelDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configModelNum, HIAI_EXECUTE_DEVICE* modelDeviceOrder)
{
    if (config == NULL || modelDeviceOrder == NULL) {
        return;
    }

    for (size_t i = 0; i < configModelNum; i++) {
        if (modelDeviceOrder[i] < HIAI_EXECUTE_DEVICE_NPU || modelDeviceOrder[i] > HIAI_EXECUTE_DEVICE_CPU) {
            FMK_LOGE("modelDeviceOrder invalid.");
            return;
        }
    }

    HIAI_ModelDeviceConfigV1* configV1 = (HIAI_ModelDeviceConfigV1*)config;
    if (configV1->modelDeviceOrder != NULL) {
        FMK_LOGW("modelDeviceOrder set repeatedly.");
        return;
    }
    configV1->configModelNum = configModelNum;
    configV1->modelDeviceOrder = modelDeviceOrder;
}

size_t HIAI_ModelDeviceConfigV1_GetConfigModelNum(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? 0 : ((HIAI_ModelDeviceConfigV1*)config)->configModelNum;
}

HIAI_EXECUTE_DEVICE* HIAI_ModelDeviceConfigV1_GetModelDeviceOrder(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? NULL : ((HIAI_ModelDeviceConfigV1*)config)->modelDeviceOrder;
}

void HIAI_ModelDeviceConfigV1_SetOpDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configOpNum, HIAI_MR_OpDeviceOrder** opDeviceOrder)
{
    if (config == NULL || opDeviceOrder == NULL) {
        return;
    }

    HIAI_ModelDeviceConfigV1* configV1 = (HIAI_ModelDeviceConfigV1*)config;
    if (configV1->opDeviceOrder != NULL) {
        FMK_LOGW("opDeviceOrder set repeatedly.");
        return;
    }
    configV1->configOpNum = configOpNum;
    configV1->opDeviceOrder = opDeviceOrder;
}

size_t HIAI_ModelDeviceConfigV1_GetConfigOpNum(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? 0 : ((HIAI_ModelDeviceConfigV1*)config)->configOpNum;
}

HIAI_MR_OpDeviceOrder** HIAI_ModelDeviceConfigV1_GetOpDeviceOrder(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? NULL : ((HIAI_ModelDeviceConfigV1*)config)->opDeviceOrder;
}

void HIAI_ModelDeviceConfigV1_SetDeviceMemoryReusePlan(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan)
{
    if (config == NULL || deviceMemoryReusePlan < HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET ||
        deviceMemoryReusePlan > HIAI_DEVICE_MEMORY_REUSE_PLAN_HIGH) {
        return;
    }
    ((HIAI_ModelDeviceConfigV1*)config)->deviceMemoryReusePlan = deviceMemoryReusePlan;
}

HIAI_DEVICE_MEMORY_REUSE_PLAN HIAI_ModelDeviceConfigV1_GetDeviceMemoryReusePlan(const HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET :
                            ((HIAI_ModelDeviceConfigV1*)config)->deviceMemoryReusePlan;
}

void HIAI_ModelDeviceConfigV1_SetCLCustomization(
    HIAI_MR_ModelDeviceConfig* config, HIAI_MR_CLCustomization** clCustomization)
{
    if (config == NULL || clCustomization == NULL) {
        return;
    }

    HIAI_ModelDeviceConfigV1* configV1 = (HIAI_ModelDeviceConfigV1*)config;
    if (configV1->clCustomization != NULL) {
        FMK_LOGW("clCustomization set repeatedly.");
        return;
    }
    configV1->clCustomization = clCustomization;
}

HIAI_MR_CLCustomization** HIAI_ModelDeviceConfigV1_GetCLCustomization(HIAI_MR_ModelDeviceConfig* config)
{
    return config == NULL ? NULL : ((HIAI_ModelDeviceConfigV1*)config)->clCustomization;
}

void HIAI_ModelDeviceConfigV1_Destroy(HIAI_MR_ModelDeviceConfig** config)
{
    if (config == NULL || *config == NULL) {
        return;
    }

    HIAI_ModelDeviceConfigV1* tmp = (HIAI_ModelDeviceConfigV1*)(*config);
    if (tmp->opDeviceOrder != NULL) {
        for (size_t i = 0; i < tmp->configOpNum; i++) {
            HIAI_OpDeviceOrderV1_Destroy(&tmp->opDeviceOrder[i]);
        }
        free(tmp->opDeviceOrder);
        tmp->opDeviceOrder = NULL;
    }

    if (tmp->clCustomization != NULL) {
        for (size_t i = 0; i < tmp->configOpNum; i++) {
            HIAI_CLCustomizationV1_Destroy(&tmp->clCustomization[i]);
        }
        free(tmp->clCustomization);
        tmp->clCustomization = NULL;
    }

    if (tmp->modelDeviceOrder != NULL) {
        free(tmp->modelDeviceOrder);
        tmp->modelDeviceOrder = NULL;
    }

    free(*config);
    *config = NULL;
}

typedef struct HIAI_ConfigBufferV1 {
    uint8_t* data;
    size_t dataSize;
} HIAI_ConfigBufferV1;

typedef struct HIAI_ModelBuildOptionsV1 {
    size_t inputNum;
    HIAI_NDTensorDesc** inputTensorDescs;

    HIAI_FORMAT_MODE_OPTION formatMode;

    HIAI_PRECISION_MODE_OPTION precisionMode;

    HIAI_MR_DynamicShapeConfig* dynamicShapeConfig;

    HIAI_MR_ModelDeviceConfig* modelDeviceConfig;
    HIAI_TUNING_STRATEGY tuningStrategy;
    size_t estimatedOutputSize;
    HIAI_MR_ConfigBuffer* quantizeConfig;
} HIAI_ModelBuildOptionsV1;

HIAI_MR_ModelBuildOptions* HIAI_ModelBuildOptionsV1_Create(void)
{
    HIAI_ModelBuildOptionsV1* option = (HIAI_ModelBuildOptionsV1*)malloc(sizeof(HIAI_ModelBuildOptionsV1));
    MALLOC_NULL_CHECK_RET_VALUE(option, NULL);
    (void)memset_s(option, sizeof(HIAI_ModelBuildOptionsV1), 0, sizeof(HIAI_ModelBuildOptionsV1));

    return (HIAI_MR_ModelBuildOptions*)option;
}

void HIAI_ModelBuildOptionsV1_SetInputTensorDescs(
    HIAI_MR_ModelBuildOptions* options, size_t inputNum, HIAI_NDTensorDesc** inputTensorDescs)
{
    if (options == NULL || inputTensorDescs == NULL) {
        return;
    }

    HIAI_ModelBuildOptionsV1* optionV1 = (HIAI_ModelBuildOptionsV1*)options;
    if (optionV1->inputTensorDescs != NULL) {
        FMK_LOGW("inputTensorDescs set repeatedly.");
        return;
    }

    optionV1->inputNum = inputNum;
    optionV1->inputTensorDescs = inputTensorDescs;
}

size_t HIAI_ModelBuildOptionsV1_GetInputSize(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? 0 : ((HIAI_ModelBuildOptionsV1*)options)->inputNum;
}

HIAI_NDTensorDesc** HIAI_ModelBuildOptionsV1_GetInputTensorDescs(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? NULL : ((HIAI_ModelBuildOptionsV1*)options)->inputTensorDescs;
}

void HIAI_ModelBuildOptionsV1_SetFormatModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_FORMAT_MODE_OPTION formatMode)
{
    if (options == NULL) {
        return;
    }

    if (formatMode < HIAI_FORMAT_MODE_USE_NCHW || formatMode > HIAI_FORMAT_MODE_USE_ORIGIN) {
        FMK_LOGE("set format mode error.");
        return;
    }
    ((HIAI_ModelBuildOptionsV1*)options)->formatMode = formatMode;
}

HIAI_FORMAT_MODE_OPTION HIAI_ModelBuildOptionsV1_GetFormatModeOption(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? HIAI_FORMAT_MODE_USE_NCHW : ((HIAI_ModelBuildOptionsV1*)options)->formatMode;
}

HIAI_Status HIAI_ModelBuildOptionsV1_SetPrecisionModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_PRECISION_MODE_OPTION precisionMode)
{
    if (options == NULL || precisionMode < HIAI_PRECISION_MODE_FP32 || precisionMode > HIAI_PRECISION_MODE_FP16) {
        FMK_LOGE("options or precisionMode invalid.");
        return HIAI_FAILURE;
    }
    ((HIAI_ModelBuildOptionsV1*)options)->precisionMode = precisionMode;
    return HIAI_SUCCESS;
}

HIAI_PRECISION_MODE_OPTION HIAI_ModelBuildOptionsV1_GetPrecisionModeOption(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? HIAI_PRECISION_MODE_FP32 : ((HIAI_ModelBuildOptionsV1*)options)->precisionMode;
}

void HIAI_ModelBuildOptionsV1_SetDynamicShapeConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_DynamicShapeConfig* dynamicShapeConfig)
{
    if (options == NULL || dynamicShapeConfig == NULL) {
        return;
    }

    HIAI_ModelBuildOptionsV1* optionV1 = (HIAI_ModelBuildOptionsV1*)options;
    if (optionV1->dynamicShapeConfig != NULL) {
        FMK_LOGW("dynamicShapeConfig set repeatedly.");
        return;
    }

    optionV1->dynamicShapeConfig = dynamicShapeConfig;
}

HIAI_MR_DynamicShapeConfig* HIAI_ModelBuildOptionsV1_GetDynamicShapeConfig(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? NULL : ((HIAI_ModelBuildOptionsV1*)options)->dynamicShapeConfig;
}

void HIAI_ModelBuildOptionsV1_SetModelDeviceConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_ModelDeviceConfig* modelDeviceConfig)
{
    if (options == NULL || modelDeviceConfig == NULL) {
        return;
    }

    HIAI_ModelBuildOptionsV1* optionV1 = (HIAI_ModelBuildOptionsV1*)options;
    if (optionV1->modelDeviceConfig != NULL) {
        FMK_LOGW("modelDeviceConfig set repeatedly.");
        return;
    }

    optionV1->modelDeviceConfig = modelDeviceConfig;
}

HIAI_MR_ModelDeviceConfig* HIAI_ModelBuildOptionsV1_GetModelDeviceConfig(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? NULL : ((HIAI_ModelBuildOptionsV1*)options)->modelDeviceConfig;
}

HIAI_Status HIAI_ModelBuildOptionsV1_SetTuningStrategy(
    HIAI_MR_ModelBuildOptions* options, HIAI_TUNING_STRATEGY tuningStrategy)
{
    if (options == NULL || tuningStrategy < HIAI_TUNING_STRATEGY_OFF ||
        tuningStrategy > HIAI_TUNING_STRATEGY_ON_CLOUD_TUNING) {
        FMK_LOGE("options or tuningStrategy invalid.");
        return HIAI_FAILURE;
    }
    ((HIAI_ModelBuildOptionsV1*)options)->tuningStrategy = tuningStrategy;
    return HIAI_SUCCESS;
}

HIAI_TUNING_STRATEGY HIAI_ModelBuildOptionsV1_GetTuningStrategy(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? HIAI_TUNING_STRATEGY_OFF : ((HIAI_ModelBuildOptionsV1*)options)->tuningStrategy;
}

void HIAI_ModelBuildOptionsV1_SetEstimatedOutputSize(HIAI_MR_ModelBuildOptions* options, size_t size)
{
    if (options == NULL) {
        FMK_LOGE("model build options is nullptr.");
        return;
    }
    ((HIAI_ModelBuildOptionsV1*)options)->estimatedOutputSize = size;
}
size_t HIAI_ModelBuildOptionsV1_GetEstimatedOutputSize(const HIAI_MR_ModelBuildOptions* options)
{
    return options == NULL ? 0 : ((HIAI_ModelBuildOptionsV1*)options)->estimatedOutputSize;
}

void HIAI_ModelBuildOptionsV1_Destroy(HIAI_MR_ModelBuildOptions** options)
{
    if (options == NULL || (*options) == NULL) {
        return;
    }

    HIAI_ModelBuildOptionsV1* tmp = (HIAI_ModelBuildOptionsV1*)(*options);

    if (tmp->inputTensorDescs != NULL) {
        for (size_t i = 0; i < tmp->inputNum; i++) {
            HIAI_NDTensorDesc_Destroy(&tmp->inputTensorDescs[i]);
        }
        free(tmp->inputTensorDescs);
    }

    if (tmp->dynamicShapeConfig != NULL) {
        HIAI_DynamicShapeConfigV1_Destroy(&tmp->dynamicShapeConfig);
    }

    if (tmp->modelDeviceConfig != NULL) {
        HIAI_ModelDeviceConfigV1_Destroy(&tmp->modelDeviceConfig);
    }

    if (tmp->quantizeConfig != NULL) {
        free(tmp->quantizeConfig);
    }

    free(*options);
    *options = NULL;
}

HIAI_Status HIAI_ModelBuildOptionsV1_SetQuantizeConfig(HIAI_MR_ModelBuildOptions* options, uint8_t* data, size_t size)
{
    if (options == NULL || data == NULL || size == 0) {
        FMK_LOGE("input param is invalid, please check.");
        return HIAI_FAILURE;
    }
    if (((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig == NULL) {
        ((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig =
            (HIAI_MR_ConfigBuffer*)malloc(sizeof(HIAI_ConfigBufferV1));
        MALLOC_NULL_CHECK_RET_VALUE(((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig, HIAI_FAILURE);
    }
    ((HIAI_ConfigBufferV1*)((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig)->data = data;
    ((HIAI_ConfigBufferV1*)((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig)->dataSize = size;
    return HIAI_SUCCESS;
}

uint8_t* HIAI_ModelBuildOptionsV1_GetQuantizeConfigData(const HIAI_MR_ModelBuildOptions* options)
{
    if (options == NULL || ((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig == NULL) {
        return NULL;
    }
    return ((HIAI_ConfigBufferV1*)((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig)->data;
}

size_t HIAI_ModelBuildOptionsV1_GetQuantizeConfigSize(const HIAI_MR_ModelBuildOptions* options)
{
    if (options == NULL || ((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig == NULL) {
        return 0;
    }
    return ((HIAI_ConfigBufferV1*)((HIAI_ModelBuildOptionsV1*)options)->quantizeConfig)->dataSize;
}
