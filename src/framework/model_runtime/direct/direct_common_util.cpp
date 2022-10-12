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
#include "direct_common_util.h"

#include <climits>
#include <vector>

#ifdef USE_STATIC_LIB
#include "model_runtime/direct/model_compatible/hiai_model_compatible.h"
#else
#include "direct_model_compatible_proxy.h"
#endif
#include "util/version_util.h"
#include "framework/util/rom_version_util.h"
#include "framework/infra/log/log.h"

#include "util/hiai_foundation_dl_helper.h"

namespace hiai {
bool DirectCommonUtil::CheckBuildOptions(const HIAI_ModelBuildOptions* options)
{
    if (options == nullptr) {
        return true;
    }

    if (HIAI_ModelBuildOptions_GetInputSize(options) != 0) {
        return false;
    }

    if (HIAI_DynamicShapeConfig_GetEnableMode(HIAI_ModelBuildOptions_GetDynamicShapeConfig(options)) !=
        HIAI_DYNAMIC_SHAPE_ENABLE_MODE::HIAI_DYNAMIC_SHAPE_DISABLE) {
        return false;
    }

    if (HIAI_ModelDeviceConfig_GetDeviceConfigMode(HIAI_ModelBuildOptions_GetModelDeviceConfig(options)) !=
        HIAI_DEVICE_CONFIG_MODE::HIAI_DEVICE_CONFIG_MODE_AUTO) {
        return false;
    }

    if (HIAI_ModelBuildOptions_GetTuningStrategy(options) != HIAI_TUNING_STRATEGY::HIAI_TUNING_STRATEGY_OFF) {
        return false;
    }

    if (HIAI_ModelBuildOptions_GetQuantizeConfigData(options) != NULL) {
        return false;
    }

    return true;
}

Status DirectCommonUtil::MakeCompatibleModel(HIAI_MemBuffer* input, HIAI_MemBuffer** output, bool& isNeedRelease)
{
    using CompatibleFunc = HIAI_Status (*)(const HIAI_MemBuffer*, HIAI_MemBuffer**);
    CompatibleFunc func = nullptr;
#ifdef USE_STATIC_LIB
    func = HIAI_MakeDirectCompatibleModel;
#else
    func = reinterpret_cast<CompatibleFunc>(DirectModelCompatibleProxy::GetSymbol("HIAI_MakeDirectCompatibleModel"));
#endif
    if (func != nullptr && func(input, output) != HIAI_SUCCESS) {
        FMK_LOGE("The model isn't supported.");
        return FAILURE;
    }
    if (*output != nullptr) {
        isNeedRelease = true;
    } else {
        *output = input;
    }
    return SUCCESS;
}

void DirectCommonUtil::DestroyBuffer(HIAI_MemBuffer* membuf)
{
    void* destroyFun = HIAI_Foundation_GetSymbol("HIAI_MemBuffer_destroy");
    if (destroyFun == nullptr) {
        return;
    }
    ((void (*)(HIAI_MemBuffer*))destroyFun)(membuf);
}

bool DirectCommonUtil::IsSupportIRBuild(void)
{
    const char* romVersion = VersionUtil::GetVersion();
    if (romVersion == nullptr) {
        FMK_LOGE("GetVersion is null");
        return false;
    }

    std::string baseVersion = "100.320.011.000";
    if (strcmp(romVersion, baseVersion.c_str()) <= 0 && !Is3rdRomVersion(romVersion)) {
        FMK_LOGE("current version is %s which is not support IR Build.", romVersion);
        return false;
    }
    return true;
}
} // namespace hiai
