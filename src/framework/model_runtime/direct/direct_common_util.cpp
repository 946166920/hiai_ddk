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

// api/framework
#include "util/version_util.h"

// inc
#include "framework/util/rom_version_util.h"
#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

// src/framework
#include "util/hiai_foundation_dl_helper.h"
#ifdef USE_STATIC_LIB
#include "model_runtime/direct/model_compatible/hiai_model_compatible.h"
#else
#include "direct_model_compatible_proxy.h"
#endif

namespace hiai {
bool DirectCommonUtil::IsSupportBuildOptions(const HIAI_MR_ModelBuildOptions* options)
{
    if (options == nullptr) {
        return true;
    }

    if (HIAI_MR_ModelBuildOptions_GetInputSize(options) != 0) {
        return false;
    }

    if (HIAI_MR_DynamicShapeConfig_GetEnableMode(HIAI_MR_ModelBuildOptions_GetDynamicShapeConfig(options)) !=
        HIAI_DYNAMIC_SHAPE_ENABLE_MODE::HIAI_DYNAMIC_SHAPE_DISABLE) {
        return false;
    }

    if (HIAI_MR_ModelDeviceConfig_GetDeviceConfigMode(HIAI_MR_ModelBuildOptions_GetModelDeviceConfig(options)) !=
        HIAI_DEVICE_CONFIG_MODE::HIAI_DEVICE_CONFIG_MODE_AUTO) {
        return false;
    }

    if (HIAI_MR_ModelBuildOptions_GetTuningStrategy(options) != HIAI_TUNING_STRATEGY::HIAI_TUNING_STRATEGY_OFF) {
        return false;
    }

    if (HIAI_MR_ModelBuildOptions_GetQuantizeConfigData(options) != nullptr) {
        return false;
    }

    return true;
}

std::shared_ptr<BaseBuffer> DirectCommonUtil::MakeCompatibleBuffer(const std::shared_ptr<BaseBuffer>& input)
{
    HIAI_MemBuffer cInput {static_cast<unsigned int>(input->GetSize()), static_cast<void*>(input->MutableData())};
    HIAI_MemBuffer* compatibleBuffer = nullptr;

    using CompatibleFunc = HIAI_Status (*)(const HIAI_MemBuffer*, HIAI_MemBuffer**);
    CompatibleFunc func = nullptr;
#ifdef USE_STATIC_LIB
    func = HIAI_MakeDirectCompatibleModel;
#else
    func = reinterpret_cast<CompatibleFunc>(DirectModelCompatibleProxy::GetSymbol("HIAI_MakeDirectCompatibleModel"));
#endif
    if (func != nullptr && func(&cInput, &compatibleBuffer) != HIAI_SUCCESS) {
        FMK_LOGE("The model isn't supported.");
        return nullptr;
    }
    if (compatibleBuffer != nullptr) {
        std::function<void (uint8_t*)> freeFunc = [](uint8_t* p) {
            HIAI_EXPECT_NOT_NULL_VOID(p);
            free(p);
        };
        std::shared_ptr<BaseBuffer> output = make_shared_nothrow<BaseBuffer>(
            static_cast<uint8_t*>(compatibleBuffer->data), compatibleBuffer->size, freeFunc);

        void* cData = compatibleBuffer->data;
        free(compatibleBuffer);
        if (output == nullptr) {
            free(cData);
        }
        return output;
    }

    return input;
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
