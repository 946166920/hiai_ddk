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

#include "control_c.h"
#include "framework/infra/log/log.h"

namespace hiai {
ControlC& ControlC::GetInstance()
{
    static ControlC instance;
    return instance;
}

void ControlC::Clear()
{
    expectStatusMap_.clear();
    expectWhichTimeErrorMap_.clear();
    nowTime_ = 0;
    static ModelBuildOptions buildOptions;
    expectBuildOptions_ = buildOptions;
    static ModelInitOptions initOptions;
    expectInitOptions_ = initOptions;
}

HIAI_Status ControlC::GetExpectStatus(CKey key)
{
    if (expectStatusMap_.find(key) != expectStatusMap_.end()) {
        if (expectWhichTimeErrorMap_.find(key) != expectWhichTimeErrorMap_.end()) {
            if (expectWhichTimeErrorMap_[key] == nowTime_) {
                return expectStatusMap_[key];
            } else {
                return HIAI_SUCCESS;
            }
        } else {
            return expectStatusMap_[key];
        }
    }
    return HIAI_SUCCESS;
}

void ControlC::SetExpectStatus(CKey key, HIAI_Status expectStatus, int witchTimeError)
{
    expectStatusMap_[key] = expectStatus;
    if (witchTimeError != -1) {
        expectWhichTimeErrorMap_[key] = witchTimeError;
    }
}

void ControlC::SetBuildOptions(ModelBuildOptions& buildOptions)
{
    expectBuildOptions_ = buildOptions;
}

bool ControlC::CheckBuildOptions(const HIAI_MR_ModelBuildOptions* options)
{
    // check static shape
    size_t inputNum = HIAI_MR_ModelBuildOptions_GetInputSize(options);
    HIAI_NDTensorDesc** inputTensorDescs = HIAI_MR_ModelBuildOptions_GetInputTensorDescs(options);
    if (expectBuildOptions_.inputTensorDescs.size() != inputNum) {
        FMK_LOGE("ERROR");
        return false;
    }
    for (size_t i = 0; i < inputNum; i++) {
        size_t num = HIAI_NDTensorDesc_GetDimNum(inputTensorDescs[i]);
        if (expectBuildOptions_.inputTensorDescs[i].dims.size() != num) {
            FMK_LOGE("ERROR");
            return false;
        }
        for (int j = 0; j < num; j++) {
            if (expectBuildOptions_.inputTensorDescs[i].dims[j] != HIAI_NDTensorDesc_GetDim(inputTensorDescs[i], j)) {
                FMK_LOGE("ERROR");
                return false;
            }
        }

        auto dataType = HIAI_NDTensorDesc_GetDataType(inputTensorDescs[i]);
        if (expectBuildOptions_.inputTensorDescs[i].dataType != static_cast<DataType>(dataType)) {
            FMK_LOGE("ERROR");
            return false;
        }
        auto format = HIAI_NDTensorDesc_GetFormat(inputTensorDescs[i]);
        if (expectBuildOptions_.inputTensorDescs[i].format != static_cast<Format>(format)) {
            FMK_LOGE("ERROR");
            return false;
        }
    }

    // check dynamic shape
    auto dynamicConfig = HIAI_MR_ModelBuildOptions_GetDynamicShapeConfig(options);
    auto dynamicEnable = HIAI_MR_DynamicShapeConfig_GetEnableMode(dynamicConfig);
    size_t dynamicMaxCacheNum = HIAI_MR_DynamicShapeConfig_GetMaxCacheNum(dynamicConfig);
    auto dynamicCacheMode = HIAI_MR_DynamicShapeConfig_GetCacheMode(dynamicConfig);
    if (expectBuildOptions_.dynamicShapeConfig.enable != dynamicEnable) {
        FMK_LOGE("ERROR");
        return false;
    }
    if (expectBuildOptions_.dynamicShapeConfig.maxCachedNum != dynamicMaxCacheNum) {
        FMK_LOGE("ERROR");
        return false;
    }
    if (expectBuildOptions_.dynamicShapeConfig.cacheMode != (hiai::CacheMode)dynamicCacheMode) {
        FMK_LOGE("ERROR");
        return false;
    }

    // check model device config
    auto modelDeviceConfig = HIAI_MR_ModelBuildOptions_GetModelDeviceConfig(options);
    auto deviceConfigMode = HIAI_MR_ModelDeviceConfig_GetDeviceConfigMode(modelDeviceConfig);
    if (expectBuildOptions_.modelDeviceConfig.deviceConfigMode != static_cast<DeviceConfigMode>(deviceConfigMode)) {
        FMK_LOGE("ERROR");
        return false;
    }
    auto fallBackMode = HIAI_MR_ModelDeviceConfig_GetFallBackMode(modelDeviceConfig);
    if (expectBuildOptions_.modelDeviceConfig.fallBackMode != static_cast<FallBackMode>(fallBackMode)) {
        FMK_LOGE("ERROR");
        return false;
    }
    size_t modelNum = HIAI_MR_ModelDeviceConfig_GetConfigModelNum(modelDeviceConfig);
    if (expectBuildOptions_.modelDeviceConfig.modelDeviceOrder.size() != modelNum) {
        FMK_LOGE("ERROR");
        return false;
    }
    auto modelDeviceOrder = HIAI_MR_ModelDeviceConfig_GetModelDeviceOrder(modelDeviceConfig);
    for (size_t i = 0; i < modelNum; i++) {
        if (expectBuildOptions_.modelDeviceConfig.modelDeviceOrder[i] !=
            static_cast<ExecuteDevice>(modelDeviceOrder[i])) {
            FMK_LOGE("ERROR");
            return false;
        }
    }
    size_t opNum = HIAI_MR_ModelDeviceConfig_GetConfigOpNum(modelDeviceConfig);
    if (expectBuildOptions_.modelDeviceConfig.opDeviceOrder.size() != opNum) {
        FMK_LOGE("ERROR");
        return false;
    }
    return true;
}

void ControlC::SetInitOptions(ModelInitOptions& initOptions)
{
    expectInitOptions_ = initOptions;
}

bool ControlC::CheckInitOptions(const HIAI_MR_ModelInitOptions* options)
{
    expectBuildOptions_ = expectInitOptions_.buildOptions;
    return CheckBuildOptions(HIAI_MR_ModelInitOptions_GetBuildOptions(
        const_cast<HIAI_MR_ModelInitOptions *>(options)));
}

void ControlC::ChangeNowTimes()
{
    nowTime_++;
}

int ControlC::GetNowTime()
{
    return nowTime_;
}
} // namespace hiai