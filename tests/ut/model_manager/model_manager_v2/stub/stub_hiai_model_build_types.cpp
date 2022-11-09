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
#include "framework/c/hiai_model_builder_types.h"

#include <climits>
#include "securec.h"
#include "framework/infra/log/log.h"
#include "control_c.h"

using namespace hiai;

struct HIAI_DynamicShapeConfig {
    HIAI_DYNAMIC_SHAPE_ENABLE_MODE enableMode;
    size_t maxCachedNum;
    HIAI_DYNAMIC_SHAPE_CACHE_MODE cacheMode;
};

HIAI_DynamicShapeConfig* HIAI_DynamicShapeConfig_Create(void)
{
    HIAI_Status expectStatus =
        ControlC::GetInstance().GetExpectStatus(CKey::C_BUILD_OPTIONS_DYNAMIC_SHAPE_CONFIG_CREATE);
    if (expectStatus != HIAI_SUCCESS) {
        return nullptr;
    }
    HIAI_DynamicShapeConfig* config = (HIAI_DynamicShapeConfig*)malloc(sizeof(HIAI_DynamicShapeConfig));
    MALLOC_NULL_CHECK_RET_VALUE(config, nullptr);

    config->enableMode = HIAI_DYNAMIC_SHAPE_DISABLE;
    config->maxCachedNum = 0;
    config->cacheMode = HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL;
    return config;
}

void HIAI_DynamicShapeConfig_SetEnableMode(HIAI_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_ENABLE_MODE mode)
{
    if (config == nullptr) {
        return;
    }
    if (mode < HIAI_DYNAMIC_SHAPE_DISABLE || mode > HIAI_DYNAMIC_SHAPE_ENABLE) {
        FMK_LOGE("set enable mode error.");
        return;
    }
    config->enableMode = mode;
}

HIAI_DYNAMIC_SHAPE_ENABLE_MODE HIAI_DynamicShapeConfig_GetEnableMode(const HIAI_DynamicShapeConfig* config)
{
    return config == nullptr ? HIAI_DYNAMIC_SHAPE_DISABLE : config->enableMode;
}

void HIAI_DynamicShapeConfig_SetMaxCacheNum(HIAI_DynamicShapeConfig* config, size_t maxCacheNum)
{
    if (config == nullptr) {
        return;
    }

    config->maxCachedNum = maxCacheNum;
}

size_t HIAI_DynamicShapeConfig_GetMaxCacheNum(const HIAI_DynamicShapeConfig* config)
{
    return config == nullptr ? 0 : config->maxCachedNum;
}

void HIAI_DynamicShapeConfig_SetCacheMode(HIAI_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_CACHE_MODE mode)
{
    if (config == nullptr) {
        return;
    }

    if (mode < HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL || mode > HIAI_DYNAMIC_SHAPE_CACHE_LOADED_MODEL) {
        FMK_LOGE("set cache mode error.");
        return;
    }
    config->cacheMode = mode;
}

HIAI_DYNAMIC_SHAPE_CACHE_MODE HIAI_DynamicShapeConfig_GetCacheMode(const HIAI_DynamicShapeConfig* config)
{
    return config == nullptr ? HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL : config->cacheMode;
}

void HIAI_DynamicShapeConfig_Destroy(HIAI_DynamicShapeConfig** config)
{
    if (config == nullptr || *config == nullptr) {
        return;
    }
    free(*config);
    *config = nullptr;
}

struct HIAI_OpDeviceOrder {
    char* opName;
    size_t supportedDeviceNum;
    HIAI_EXECUTE_DEVICE* supportedDevices;
};

HIAI_OpDeviceOrder* HIAI_OpDeviceOrder_Create(void)
{
    hiai::ControlC::GetInstance().ChangeNowTimes();
    auto status = hiai::ControlC::GetInstance().GetExpectStatus(C_BUILD_OPTIONS_OPDEVICE_CREATE);
    if (status != HIAI_SUCCESS) {
        return nullptr;
    }

    HIAI_OpDeviceOrder* config = (HIAI_OpDeviceOrder*)malloc(sizeof(HIAI_OpDeviceOrder));
    MALLOC_NULL_CHECK_RET_VALUE(config, nullptr);
    (void)memset_s(config, sizeof(HIAI_OpDeviceOrder), 0, sizeof(HIAI_OpDeviceOrder));

    return config;
}

static char* HIAI_CopyName(const char* name)
{
    size_t nameLen = strlen(name);
    if (nameLen >= (UINT_MAX - 1)) {
        FMK_LOGW("nameLen set too large.");
        return nullptr;
    }
    char* dstName = (char*)malloc(nameLen + 1);
    if (dstName == nullptr) {
        FMK_LOGW("malloc faied.");
        return nullptr;
    }
    if (memcpy_s(dstName, nameLen + 1, name, nameLen) != 0) {
        FMK_LOGW("memcpy faied.");
        free(dstName);
        dstName = nullptr;
        return nullptr;
    }
    dstName[nameLen] = '\0';
    return dstName;
}

void HIAI_OpDeviceOrder_SetOpName(HIAI_OpDeviceOrder* config, const char* opName)
{
    if (config == nullptr || opName == nullptr) {
        return;
    }

    if (config->opName != nullptr) {
        FMK_LOGW("opName set repeatedly.");
        return;
    }

    config->opName = HIAI_CopyName(opName);
}

const char* HIAI_OpDeviceOrder_GetOpName(const HIAI_OpDeviceOrder* config)
{
    return config == nullptr ? nullptr : config->opName;
}

void HIAI_OpDeviceOrder_SetDeviceOrder(
    HIAI_OpDeviceOrder* config, size_t supportedDeviceNum, HIAI_EXECUTE_DEVICE* supportedDevices)
{
    if (config == nullptr || supportedDevices == nullptr) {
        return;
    }

    if (config->supportedDevices != nullptr) {
        FMK_LOGW("supportedDevices set repeatedly.");
        return;
    }
    config->supportedDeviceNum = supportedDeviceNum;
    config->supportedDevices = supportedDevices;
}

size_t HIAI_OpDeviceOrder_GetSupportedDeviceNum(const HIAI_OpDeviceOrder* config)
{
    return config == nullptr ? 0 : config->supportedDeviceNum;
}

HIAI_EXECUTE_DEVICE* HIAI_OpDeviceOrder_GetSupportedDevices(const HIAI_OpDeviceOrder* config)
{
    return config == nullptr ? nullptr : config->supportedDevices;
}

void HIAI_OpDeviceOrder_Destroy(HIAI_OpDeviceOrder** config)
{
    if (config == nullptr || *config == nullptr) {
        return;
    }

    HIAI_OpDeviceOrder* tmp = static_cast<HIAI_OpDeviceOrder*>(*config);
    if (tmp->opName != nullptr) {
        free(tmp->opName);
    }

    if (tmp->supportedDevices != nullptr) {
        free(tmp->supportedDevices);
    }
    free(tmp);
    *config = nullptr;
}

struct HIAI_CLCustomization {
    char* opName;
    char* customization;
};

HIAI_CLCustomization* HIAI_CLCustomization_Create(void)
{
    HIAI_CLCustomization* config = (HIAI_CLCustomization*)malloc(sizeof(HIAI_CLCustomization));
    MALLOC_NULL_CHECK_RET_VALUE(config, nullptr);
    (void)memset_s(config, sizeof(HIAI_CLCustomization), 0, sizeof(HIAI_CLCustomization));
    return config;
}

void HIAI_CLCustomization_SetOpName(HIAI_CLCustomization* config, const char* opName)
{
    if (config == nullptr || opName == nullptr) {
        return;
    }

    if (config->opName != nullptr) {
        FMK_LOGW("opName set repeatedly.", config->opName);
        return;
    }

    config->opName = HIAI_CopyName(opName);
}

const char* HIAI_CLCustomization_GetOpName(const HIAI_CLCustomization* config)
{
    return config == nullptr ? nullptr : config->opName;
}

void HIAI_CLCustomization_SetCustomization(HIAI_CLCustomization* config, const char* customization)
{
    if (config == nullptr || customization == nullptr) {
        return;
    }

    if (config->customization != nullptr) {
        FMK_LOGW("customization set repeatedly.");
        return;
    }

    size_t customizationLen = strlen(customization);
    if (customizationLen >= (UINT_MAX - 1)) {
        FMK_LOGW("customizationLen set too large.");
        return;
    }
    config->customization = (char*)malloc(customizationLen + 1);
    if (config->customization == nullptr) {
        FMK_LOGW("malloc faied.");
        return;
    }
    if (memcpy_s(config->customization, customizationLen + 1, customization, customizationLen) != 0) {
        FMK_LOGW("memcpy faied.");
        if (config->customization != nullptr) {
            free(config->customization);
            config->customization = nullptr;
        }
        return;
    }
    config->customization[customizationLen] = '\0';
}

const char* HIAI_CLCustomization_GetCustomization(const HIAI_CLCustomization* config)
{
    return config == nullptr ? nullptr : config->customization;
}

void HIAI_CLCustomization_Destroy(HIAI_CLCustomization** config)
{
    if (config == nullptr || *config == nullptr) {
        return;
    }

    HIAI_CLCustomization* tmp = static_cast<HIAI_CLCustomization*>(*config);
    if (tmp->opName != nullptr) {
        free(tmp->opName);
    }

    if (tmp->customization != nullptr) {
        free(tmp->customization);
    }
    free(*config);
    *config = nullptr;
}
struct HIAI_ModelDeviceConfig {
    HIAI_DEVICE_CONFIG_MODE deviceConfigMode;
    HIAI_FALLBACK_MODE fallBackMode;
    size_t configModelNum;
    HIAI_EXECUTE_DEVICE* modelDeviceOrder;
    size_t configOpNum;
    HIAI_OpDeviceOrder** opDeviceOrder;
    HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan;
    HIAI_CLCustomization** clCustomization;
};

HIAI_ModelDeviceConfig* HIAI_ModelDeviceConfig_Create(void)
{
    HIAI_ModelDeviceConfig* config = (HIAI_ModelDeviceConfig*)malloc(sizeof(HIAI_ModelDeviceConfig));
    MALLOC_NULL_CHECK_RET_VALUE(config, nullptr);
    (void)memset_s(config, sizeof(HIAI_ModelDeviceConfig), 0, sizeof(HIAI_ModelDeviceConfig));

    config->deviceConfigMode = HIAI_DEVICE_CONFIG_MODE_AUTO;
    config->fallBackMode = HIAI_FALLBACK_MODE_ENABLE;
    return config;
}

void HIAI_ModelDeviceConfig_SetDeviceConfigMode(
    HIAI_ModelDeviceConfig* config, HIAI_DEVICE_CONFIG_MODE deviceConfigMode)
{
    if (config == nullptr || deviceConfigMode < HIAI_DEVICE_CONFIG_MODE_AUTO ||
        deviceConfigMode > HIAI_DEVICE_CONFIG_MODE_OP_LEVEL) {
        return;
    }
    config->deviceConfigMode = deviceConfigMode;
}

HIAI_DEVICE_CONFIG_MODE HIAI_ModelDeviceConfig_GetDeviceConfigMode(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? HIAI_DEVICE_CONFIG_MODE_AUTO : config->deviceConfigMode;
}

void HIAI_ModelDeviceConfig_SetFallBackMode(HIAI_ModelDeviceConfig* config, HIAI_FALLBACK_MODE fallBackMode)
{
    if (config == nullptr || fallBackMode < HIAI_FALLBACK_MODE_ENABLE || fallBackMode > HIAI_FALLBACK_MODE_DISABLE) {
        return;
    }
    config->fallBackMode = fallBackMode;
}

HIAI_FALLBACK_MODE HIAI_ModelDeviceConfig_GetFallBackMode(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? HIAI_FALLBACK_MODE_ENABLE : config->fallBackMode;
}

void HIAI_ModelDeviceConfig_SetModelDeviceOrder(
    HIAI_ModelDeviceConfig* config, size_t configModelNum, HIAI_EXECUTE_DEVICE* modelDeviceOrder)
{
    if (config == nullptr || modelDeviceOrder == nullptr) {
        return;
    }

    for (size_t i = 0; i < configModelNum; i++) {
        if (modelDeviceOrder[i] < HIAI_EXECUTE_DEVICE_NPU || modelDeviceOrder[i] > HIAI_EXECUTE_DEVICE_CPU) {
            FMK_LOGE("modelDeviceOrder invalid.");
            return;
        }
    }

    if (config->modelDeviceOrder != nullptr) {
        FMK_LOGW("modelDeviceOrder set repeatedly.");
        return;
    }
    config->configModelNum = configModelNum;
    config->modelDeviceOrder = modelDeviceOrder;
}

size_t HIAI_ModelDeviceConfig_GetConfigModelNum(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? 0 : config->configModelNum;
}

HIAI_EXECUTE_DEVICE* HIAI_ModelDeviceConfig_GetModelDeviceOrder(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? nullptr : config->modelDeviceOrder;
}

void HIAI_ModelDeviceConfig_SetOpDeviceOrder(
    HIAI_ModelDeviceConfig* config, size_t configOpNum, HIAI_OpDeviceOrder** opDeviceOrder)
{
    if (config == nullptr || opDeviceOrder == nullptr) {
        return;
    }

    if (config->opDeviceOrder != nullptr) {
        FMK_LOGW("opDeviceOrder set repeatedly.");
        return;
    }
    config->configOpNum = configOpNum;
    config->opDeviceOrder = opDeviceOrder;
}

size_t HIAI_ModelDeviceConfig_GetConfigOpNum(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? 0 : config->configOpNum;
}

HIAI_OpDeviceOrder** HIAI_ModelDeviceConfig_GetOpDeviceOrder(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? nullptr : config->opDeviceOrder;
}

void HIAI_ModelDeviceConfig_SetDeviceMemoryReusePlan(
    HIAI_ModelDeviceConfig* config, HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan)
{
    if (config == nullptr || deviceMemoryReusePlan < HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET ||
        deviceMemoryReusePlan > HIAI_DEVICE_MEMORY_REUSE_PLAN_HIGH) {
        return;
    }
    ((HIAI_ModelDeviceConfig*)config)->deviceMemoryReusePlan = deviceMemoryReusePlan;
}

HIAI_DEVICE_MEMORY_REUSE_PLAN HIAI_ModelDeviceConfig_GetDeviceMemoryReusePlan(const HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET : config->deviceMemoryReusePlan;
}

void HIAI_ModelDeviceConfig_SetCLCustomization(HIAI_ModelDeviceConfig* config, HIAI_CLCustomization** clCustomization)
{
    if (config == nullptr || clCustomization == nullptr) {
        return;
    }

    if (config->clCustomization != nullptr) {
        FMK_LOGW("clCustomization set repeatedly.");
        return;
    }
    config->clCustomization = clCustomization;
}

HIAI_CLCustomization** HIAI_ModelDeviceConfig_GetCLCustomization(HIAI_ModelDeviceConfig* config)
{
    return config == nullptr ? nullptr : config->clCustomization;
}

void HIAI_ModelDeviceConfig_Destroy(HIAI_ModelDeviceConfig** config)
{
    if (config == nullptr || *config == nullptr) {
        return;
    }

    HIAI_ModelDeviceConfig* tmp = static_cast<HIAI_ModelDeviceConfig*>(*config);
    if (tmp->opDeviceOrder != nullptr) {
        for (size_t i = 0; i < tmp->configOpNum; i++) {
            HIAI_OpDeviceOrder_Destroy(&tmp->opDeviceOrder[i]);
        }
        free(tmp->opDeviceOrder);
        tmp->opDeviceOrder = nullptr;
    }

    if (tmp->clCustomization != nullptr) {
        for (size_t i = 0; i < tmp->configOpNum; i++) {
            HIAI_CLCustomization_Destroy(&tmp->clCustomization[i]);
        }
        free(tmp->clCustomization);
        tmp->clCustomization = nullptr;
    }

    if (tmp->modelDeviceOrder != nullptr) {
        free(tmp->modelDeviceOrder);
        tmp->modelDeviceOrder = nullptr;
    }

    free(*config);
    *config = nullptr;
}

struct HIAI_ConfigBuffer {
    uint8_t* data;
    size_t dataSize;
};

struct HIAI_ModelBuildOptions {
    size_t inputNum;
    HIAI_NDTensorDesc** inputTensorDescs;

    HIAI_FORMAT_MODE_OPTION formatMode;

    HIAI_PRECISION_MODE_OPTION precisionMode;

    HIAI_DynamicShapeConfig* dynamicShapeConfig;

    HIAI_ModelDeviceConfig* modelDeviceConfig;
    HIAI_TUNING_STRATEGY tuningStrategy;
    size_t estimatedOutputSize;
    HIAI_ConfigBuffer* quantizeConfig;
};

HIAI_ModelBuildOptions* HIAI_ModelBuildOptions_Create(void)
{
    HIAI_Status expectStatus = ControlC::GetInstance().GetExpectStatus(CKey::C_BUILD_OPTIONS_CREATE);
    if (expectStatus != HIAI_SUCCESS) {
        return nullptr;
    }
    HIAI_ModelBuildOptions* option = (HIAI_ModelBuildOptions*)malloc(sizeof(HIAI_ModelBuildOptions));
    MALLOC_NULL_CHECK_RET_VALUE(option, nullptr);
    (void)memset_s(option, sizeof(HIAI_ModelBuildOptions), 0, sizeof(HIAI_ModelBuildOptions));

    return option;
}

void HIAI_ModelBuildOptions_SetInputTensorDescs(
    HIAI_ModelBuildOptions* options, size_t inputNum, HIAI_NDTensorDesc** inputTensorDescs)
{
    if (options == nullptr || inputTensorDescs == nullptr) {
        return;
    }

    if (options->inputTensorDescs != nullptr) {
        FMK_LOGW("inputTensorDescs set repeatedly.");
        return;
    }

    options->inputNum = inputNum;
    options->inputTensorDescs = inputTensorDescs;
}

size_t HIAI_ModelBuildOptions_GetInputSize(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? 0 : options->inputNum;
}

HIAI_NDTensorDesc** HIAI_ModelBuildOptions_GetInputTensorDescs(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? nullptr : options->inputTensorDescs;
}

void HIAI_ModelBuildOptions_SetFormatModeOption(HIAI_ModelBuildOptions* options, HIAI_FORMAT_MODE_OPTION formatMode)
{
    if (options == nullptr) {
        return;
    }

    if (formatMode < HIAI_FORMAT_MODE_USE_NCHW || formatMode > HIAI_FORMAT_MODE_USE_ORIGIN) {
        FMK_LOGE("set format mode error.");
        return;
    }
    options->formatMode = formatMode;
}

HIAI_FORMAT_MODE_OPTION HIAI_ModelBuildOptions_GetFormatModeOption(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? HIAI_FORMAT_MODE_USE_NCHW : options->formatMode;
}

HIAI_Status HIAI_ModelBuildOptions_SetPrecisionModeOption(
    HIAI_ModelBuildOptions* options, HIAI_PRECISION_MODE_OPTION precisionMode)
{
    if (options == nullptr || precisionMode < HIAI_PRECISION_MODE_FP32 || precisionMode > HIAI_PRECISION_MODE_FP16) {
        FMK_LOGE("options or precisionMode invalid.");
        return HIAI_FAILURE;
    }
    options->precisionMode = precisionMode;
    return HIAI_SUCCESS;
}

HIAI_PRECISION_MODE_OPTION HIAI_ModelBuildOptions_GetPrecisionModeOption(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? HIAI_PRECISION_MODE_FP32 : options->precisionMode;
}

void HIAI_ModelBuildOptions_SetDynamicShapeConfig(
    HIAI_ModelBuildOptions* options, HIAI_DynamicShapeConfig* dynamicShapeConfig)
{
    if (options == nullptr || dynamicShapeConfig == nullptr) {
        return;
    }

    if (options->dynamicShapeConfig != nullptr) {
        FMK_LOGW("dynamicShapeConfig set repeatedly.");
        return;
    }

    options->dynamicShapeConfig = dynamicShapeConfig;
}

HIAI_DynamicShapeConfig* HIAI_ModelBuildOptions_GetDynamicShapeConfig(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? nullptr : options->dynamicShapeConfig;
}

void HIAI_ModelBuildOptions_SetModelDeviceConfig(
    HIAI_ModelBuildOptions* options, HIAI_ModelDeviceConfig* modelDeviceConfig)
{
    if (options == nullptr || modelDeviceConfig == nullptr) {
        return;
    }

    if (options->modelDeviceConfig != nullptr) {
        FMK_LOGW("modelDeviceConfig set repeatedly.");
        return;
    }

    options->modelDeviceConfig = modelDeviceConfig;
}

HIAI_ModelDeviceConfig* HIAI_ModelBuildOptions_GetModelDeviceConfig(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? nullptr : options->modelDeviceConfig;
}

HIAI_Status HIAI_ModelBuildOptions_SetTuningStrategy(
    HIAI_ModelBuildOptions* options, HIAI_TUNING_STRATEGY tuningStrategy)
{
    if (options == nullptr || tuningStrategy < HIAI_TUNING_STRATEGY_OFF ||
        tuningStrategy > HIAI_TUNING_STRATEGY_ON_CLOUD_TUNING) {
        FMK_LOGE("options or tuningStrategy invalid. tuningStrategy = %u", tuningStrategy);
        return HIAI_FAILURE;
    }
    options->tuningStrategy = tuningStrategy;
    return HIAI_SUCCESS;
}

HIAI_TUNING_STRATEGY HIAI_ModelBuildOptions_GetTuningStrategy(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? HIAI_TUNING_STRATEGY_OFF : options->tuningStrategy;
}

void HIAI_ModelBuildOptions_SetEstimatedOutputSize(HIAI_ModelBuildOptions* options, size_t size)
{
    if (options == nullptr) {
        FMK_LOGE("model build options is nullptr.");
        return;
    }
    options->estimatedOutputSize = size;
}
size_t HIAI_ModelBuildOptions_GetEstimatedOutputSize(const HIAI_ModelBuildOptions* options)
{
    return options == nullptr ? 0 : options->estimatedOutputSize;
}

void HIAI_ModelBuildOptions_Destroy(HIAI_ModelBuildOptions** options)
{
    if (options == nullptr || (*options) == nullptr) {
        return;
    }

    HIAI_ModelBuildOptions* tmp = static_cast<HIAI_ModelBuildOptions*>(*options);

    if (tmp->inputTensorDescs != nullptr) {
        for (size_t i = 0; i < tmp->inputNum; i++) {
            HIAI_NDTensorDesc_Destroy(&tmp->inputTensorDescs[i]);
        }
        free(tmp->inputTensorDescs);
    }

    if (tmp->dynamicShapeConfig != nullptr) {
        HIAI_DynamicShapeConfig_Destroy(&tmp->dynamicShapeConfig);
    }

    if (tmp->modelDeviceConfig != nullptr) {
        HIAI_ModelDeviceConfig_Destroy(&tmp->modelDeviceConfig);
    }

    if (tmp->quantizeConfig != nullptr) {
        free(tmp->quantizeConfig);
    }

    free(*options);
    *options = nullptr;
}

HIAI_Status HIAI_ModelBuildOptions_SetQuantizeConfig(HIAI_ModelBuildOptions* options, uint8_t* data, size_t size)
{
    if (options == nullptr || data == nullptr || size == 0) {
        FMK_LOGE("input param is invalid, please check.");
        return HIAI_FAILURE;
    }
    if (options->quantizeConfig == nullptr) {
        options->quantizeConfig = (HIAI_ConfigBuffer*)malloc(sizeof(HIAI_ConfigBuffer));
        MALLOC_NULL_CHECK_RET_VALUE(options->quantizeConfig, HIAI_FAILURE);
    }
    options->quantizeConfig->data = data;
    options->quantizeConfig->dataSize = size;
    return HIAI_SUCCESS;
}

uint8_t* HIAI_ModelBuildOptions_GetQuantizeConfigData(const HIAI_ModelBuildOptions* options)
{
    if (options == nullptr || options->quantizeConfig == nullptr) {
        return nullptr;
    }
    return options->quantizeConfig->data;
}

size_t HIAI_ModelBuildOptions_GetQuantizeConfigSize(const HIAI_ModelBuildOptions* options)
{
    if (options == nullptr || options->quantizeConfig == nullptr) {
        return 0;
    }
    return options->quantizeConfig->dataSize;
}
