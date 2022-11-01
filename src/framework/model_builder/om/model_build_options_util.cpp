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
#include "model_build_options_util.h"
#include "securec.h"
#include "infra/base/assertion.h"
#include "framework/infra/log/log.h"

namespace hiai {
static HIAI_DynamicShapeConfig* ConvertCDynamicShapeConfig(const DynamicShapeConfig& config)
{
    HIAI_DynamicShapeConfig* dynamicShapeConfig = HIAI_DynamicShapeConfig_Create();
    HIAI_EXPECT_NOT_NULL_R(dynamicShapeConfig, nullptr);

    HIAI_DynamicShapeConfig_SetEnableMode(
        dynamicShapeConfig, config.enable ? HIAI_DYNAMIC_SHAPE_ENABLE : HIAI_DYNAMIC_SHAPE_DISABLE);

    HIAI_DynamicShapeConfig_SetMaxCacheNum(dynamicShapeConfig, config.maxCachedNum);
    HIAI_DynamicShapeConfig_SetCacheMode(
        dynamicShapeConfig, static_cast<HIAI_DYNAMIC_SHAPE_CACHE_MODE>(config.cacheMode));
    return dynamicShapeConfig;
}

static HIAI_OpDeviceOrder* ConvertToCOpDeviceConfig(
    const std::string& opName, const std::vector<ExecuteDevice>& deviceList)
{
    HIAI_OpDeviceOrder* opDeviceOrder = HIAI_OpDeviceOrder_Create();
    HIAI_EXPECT_NOT_NULL_R(opDeviceOrder, nullptr);

    HIAI_OpDeviceOrder_SetOpName(opDeviceOrder, opName.c_str());

    HIAI_EXECUTE_DEVICE* cDeviceList = (HIAI_EXECUTE_DEVICE*)malloc(sizeof(HIAI_EXECUTE_DEVICE) * deviceList.size());
    if (cDeviceList == nullptr) {
        FMK_LOGE("malloc failed.");
        goto FREE_ALL;
    }
    for (size_t i = 0; i < deviceList.size(); i++) {
        cDeviceList[i] = static_cast<HIAI_EXECUTE_DEVICE>(deviceList[i]);
    }
    HIAI_OpDeviceOrder_SetDeviceOrder(opDeviceOrder, deviceList.size(), cDeviceList);
    return opDeviceOrder;

FREE_ALL:
    HIAI_OpDeviceOrder_Destroy(&opDeviceOrder);
    return nullptr;
}

static HIAI_OpDeviceOrder** ConvertToCOpDeviceConfigList(
    const std::map<std::string, std::vector<ExecuteDevice>>& opDeviceConfigs)
{
    size_t size = sizeof(HIAI_OpDeviceOrder*) * opDeviceConfigs.size();
    HIAI_OpDeviceOrder** opDeviceOrders =
        (HIAI_OpDeviceOrder**)malloc(size);
    HIAI_EXPECT_NOT_NULL_R(opDeviceOrders, nullptr);
    (void)memset_s(opDeviceOrders, size, 0, size);

    size_t i = 0;
    for (auto it = opDeviceConfigs.begin(); it != opDeviceConfigs.end(); it++, i++) {
        opDeviceOrders[i] = ConvertToCOpDeviceConfig(it->first, it->second);
        if (opDeviceOrders[i] == nullptr) {
            goto FREE_ALL;
        }
    }
    return opDeviceOrders;

FREE_ALL:
    for (i = 0; i < opDeviceConfigs.size(); i++) {
        if (opDeviceOrders[i] != nullptr) {
            HIAI_OpDeviceOrder_Destroy(&opDeviceOrders[i]);
        }
    }
    free(opDeviceOrders);
    opDeviceOrders = nullptr;
    return nullptr;
}

#ifdef AI_SUPPORT_CL_CUSTOMIZATION
static HIAI_CLCustomization* ConvertToCclCustomization(const std::string& opName, const std::string& customization)
{
    HIAI_CLCustomization* cLCustomization = HIAI_CLCustomization_Create();
    HIAI_EXPECT_NOT_NULL_R(cLCustomization, nullptr);

    HIAI_CLCustomization_SetOpName(cLCustomization, opName.c_str());
    HIAI_CLCustomization_SetCustomization(cLCustomization, customization.c_str());
    return cLCustomization;
}

static HIAI_CLCustomization** ConvertToCclCustomizationList(const std::map<std::string, std::string>& clCustomizations)
{
    size_t size = sizeof(HIAI_CLCustomization*) * clCustomizations.size();
    HIAI_CLCustomization** cLCustomization = (HIAI_CLCustomization**)malloc(size);
    HIAI_EXPECT_NOT_NULL_R(cLCustomization, nullptr);
    (void)memset_s(cLCustomization, size, 0, size);

    size_t i = 0;
    for (auto it = clCustomizations.begin(); it != clCustomizations.end(); it++, i++) {
        cLCustomization[i] = ConvertToCclCustomization(it->first, it->second);
        if (cLCustomization[i] == nullptr) {
            goto FREE_ALL;
        }
    }
    return cLCustomization;

FREE_ALL:
    for (i = 0; i < clCustomizations.size(); i++) {
        if (cLCustomization[i] != nullptr) {
            HIAI_CLCustomization_Destroy(&cLCustomization[i]);
        }
    }
    free(cLCustomization);
    cLCustomization = nullptr;
    return nullptr;
}
#endif

static HIAI_EXECUTE_DEVICE* ConvertToCModelDeviceOrder(const std::vector<ExecuteDevice>& modelDeviceOrder)
{
    HIAI_EXPECT_TRUE_R(!modelDeviceOrder.empty(), nullptr);

    HIAI_EXECUTE_DEVICE* cDeviceTypeList =
        (HIAI_EXECUTE_DEVICE*)malloc(sizeof(HIAI_EXECUTE_DEVICE) * modelDeviceOrder.size());
    HIAI_EXPECT_NOT_NULL_R(cDeviceTypeList, nullptr);

    for (size_t i = 0; i < modelDeviceOrder.size(); ++i) {
        cDeviceTypeList[i] = static_cast<HIAI_EXECUTE_DEVICE>(modelDeviceOrder[i]);
    }
    return cDeviceTypeList;
}

static HIAI_ModelDeviceConfig* ConvertCModelDeviceConfig(const ModelDeviceConfig& config)
{
    HIAI_ModelDeviceConfig* modelDeviceConfig = HIAI_ModelDeviceConfig_Create();
    HIAI_EXPECT_NOT_NULL_R(modelDeviceConfig, nullptr);

    HIAI_ModelDeviceConfig_SetDeviceConfigMode(
        modelDeviceConfig, static_cast<HIAI_DEVICE_CONFIG_MODE>(config.deviceConfigMode));
    HIAI_ModelDeviceConfig_SetFallBackMode(modelDeviceConfig, static_cast<HIAI_FALLBACK_MODE>(config.fallBackMode));
    if (config.deviceConfigMode == DeviceConfigMode::MODEL_LEVEL) {
        HIAI_EXECUTE_DEVICE* cDeviceTypeList = ConvertToCModelDeviceOrder(config.modelDeviceOrder);
        if (cDeviceTypeList == nullptr) {
            HIAI_ModelDeviceConfig_Destroy(&modelDeviceConfig);
            return nullptr;
        }
        HIAI_ModelDeviceConfig_SetModelDeviceOrder(modelDeviceConfig, config.modelDeviceOrder.size(), cDeviceTypeList);
    } else if (config.deviceConfigMode == DeviceConfigMode::OP_LEVEL) {
        if (!config.opDeviceOrder.empty()) {
            HIAI_OpDeviceOrder** cOpDeviceConfigList = ConvertToCOpDeviceConfigList(config.opDeviceOrder);
            if (cOpDeviceConfigList == nullptr) {
                HIAI_ModelDeviceConfig_Destroy(&modelDeviceConfig);
                return nullptr;
            }
            HIAI_ModelDeviceConfig_SetOpDeviceOrder(
                modelDeviceConfig, config.opDeviceOrder.size(), cOpDeviceConfigList);
        }
        HIAI_ModelDeviceConfig_SetDeviceMemoryReusePlan(
            modelDeviceConfig, static_cast<HIAI_DEVICE_MEMORY_REUSE_PLAN>(config.deviceMemoryReusePlan));
#ifdef AI_SUPPORT_CL_CUSTOMIZATION
        if (config.clCustomizations.size() > 0) {
            if (config.clCustomizations.size() != config.opDeviceOrder.size()) {
                HIAI_ModelDeviceConfig_Destroy(&modelDeviceConfig);
                return nullptr;
            }
            HIAI_CLCustomization** cCLCustomizationList = ConvertToCclCustomizationList(config.clCustomizations);
            HIAI_ModelDeviceConfig_SetCLCustomization(modelDeviceConfig, cCLCustomizationList);
        }
#endif
    }
    return modelDeviceConfig;
}

static HIAI_NDTensorDesc** ConvertToCTensorDescs(const std::vector<NDTensorDesc>& descs)
{
    HIAI_NDTensorDesc** ndTensorDescs = (HIAI_NDTensorDesc**)malloc(sizeof(HIAI_NDTensorDesc*) * descs.size());
    HIAI_EXPECT_NOT_NULL_R(ndTensorDescs, nullptr);
    (void)memset_s(
        ndTensorDescs, sizeof(HIAI_NDTensorDesc*) * descs.size(), 0, sizeof(HIAI_NDTensorDesc*) * descs.size());

    for (size_t i = 0; i < descs.size(); i++) {
        if (descs[i].dims.empty()) {
            goto FREE_ALL;
        }
        ndTensorDescs[i] = HIAI_NDTensorDesc_Create(descs[i].dims.data(), descs[i].dims.size(),
            static_cast<HIAI_DataType>(descs[i].dataType), static_cast<HIAI_Format>(descs[i].format));
        if (ndTensorDescs[i] == nullptr) {
            goto FREE_ALL;
        }
    }
    return ndTensorDescs;

FREE_ALL:
    for (size_t i = 0; i < descs.size(); i++) {
        if (ndTensorDescs[i] != nullptr) {
            HIAI_NDTensorDesc_Destroy(&ndTensorDescs[i]);
        }
    }
    free(ndTensorDescs);
    return nullptr;
}

HIAI_ModelBuildOptions* ModelBuildOptionsUtil::ConvertToCBuildOptions(const ModelBuildOptions& options)
{
    HIAI_ModelBuildOptions* buildOptions = HIAI_ModelBuildOptions_Create();
    HIAI_EXPECT_NOT_NULL_R(buildOptions, nullptr);

    HIAI_DynamicShapeConfig* cDynamicShapeConfig = nullptr;
    HIAI_ModelDeviceConfig* cModelDeviceConfig = nullptr;
    if (!options.inputTensorDescs.empty()) {
        HIAI_NDTensorDesc** inputDescs = ConvertToCTensorDescs(options.inputTensorDescs);
        if (inputDescs == nullptr) {
            goto FREE_ALL;
        }
        HIAI_ModelBuildOptions_SetInputTensorDescs(buildOptions, options.inputTensorDescs.size(), inputDescs);
    }

    HIAI_ModelBuildOptions_SetFormatModeOption(buildOptions, static_cast<HIAI_FORMAT_MODE_OPTION>(options.formatMode));

    HIAI_ModelBuildOptions_SetPrecisionModeOption(
        buildOptions, static_cast<HIAI_PRECISION_MODE_OPTION>(options.precisionMode));

    cDynamicShapeConfig = ConvertCDynamicShapeConfig(options.dynamicShapeConfig);
    if (cDynamicShapeConfig == nullptr) {
        goto FREE_ALL;
    }
    HIAI_ModelBuildOptions_SetDynamicShapeConfig(buildOptions, cDynamicShapeConfig);

    cModelDeviceConfig = ConvertCModelDeviceConfig(options.modelDeviceConfig);
    if (cModelDeviceConfig == nullptr) {
        goto FREE_ALL;
    }
    HIAI_ModelBuildOptions_SetModelDeviceConfig(buildOptions, cModelDeviceConfig);

    HIAI_ModelBuildOptions_SetTuningStrategy(buildOptions, static_cast<HIAI_TUNING_STRATEGY>(options.tuningStrategy));

    (void)HIAI_ModelBuildOptions_SetEstimatedOutputSize(buildOptions, options.estimatedOutputSize);
    if (options.quantizeConfig != "") {
        (void)HIAI_ModelBuildOptions_SetQuantizeConfig(buildOptions,
            reinterpret_cast<uint8_t*>(const_cast<char*>(options.quantizeConfig.data())),
            options.quantizeConfig.size());
    }
    return buildOptions;

FREE_ALL:
    HIAI_ModelBuildOptions_Destroy(&buildOptions);
    return nullptr;
}

bool ModelBuildOptionsUtil::IsHasBuildOptions(const ModelBuildOptions& options)
{
    if (options.formatMode != FormatMode::USE_NCHW) {
        return true;
    }
    if (options.precisionMode != PrecisionMode::PRECISION_MODE_FP32) {
        return true;
    }
    if (!options.inputTensorDescs.empty()) {
        return true;
    }
    if (options.dynamicShapeConfig.enable) {
        return true;
    }
    if (options.modelDeviceConfig.fallBackMode != FallBackMode::ENABLE ||
        !options.modelDeviceConfig.modelDeviceOrder.empty() || !options.modelDeviceConfig.opDeviceOrder.empty()) {
        return true;
    }
    if (options.tuningStrategy != TuningStrategy::OFF) {
        return true;
    }
    if (options.estimatedOutputSize != 0) {
        return true;
    }
    if (!options.quantizeConfig.empty()) {
        return true;
    }
    return false;
}
} // namespace hiai
