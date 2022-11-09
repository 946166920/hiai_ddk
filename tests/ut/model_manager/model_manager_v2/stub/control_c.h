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

#ifndef CONTROL_C_H
#define CONTROL_C_H

#include <map>
#include <string>
#include "framework/c/hiai_error_types.h"
#include "framework/c/hiai_model_builder_types.h"
#include "framework/c/hiai_model_manager_types.h"
#include "model_builder/model_builder_types.h"
#include "model_manager/model_manager_types.h"

namespace hiai {

enum CKey {
    C_BUILD_OPTIONS_CREATE,
    C_ND_TENSOR_DESC_CREATE,
    C_BUILD_OPTIONS_DYNAMIC_SHAPE_CONFIG_CREATE,
    C_RUN_AIPP_MODEL_V2_FAILED,
    C_BUILD_OPTIONS_OPDEVICE_CREATE,
};

class ControlC {
public:
    static ControlC& GetInstance();

    void Clear();

    HIAI_Status GetExpectStatus(CKey key);
    void SetExpectStatus(CKey key, HIAI_Status expectStatus = HIAI_FAILURE, int witchTimeError = -1);
    void SetBuildOptions(ModelBuildOptions& buildOptions);
    bool CheckBuildOptions(const HIAI_ModelBuildOptions* options);

    void SetInitOptions(ModelInitOptions& initOptions);
    bool CheckInitOptions(const HIAI_ModelInitOptions* options);

    void ChangeNowTimes();
    int GetNowTime();

private:
    std::map<CKey, HIAI_Status> expectStatusMap_;
    std::map<CKey, int> expectWhichTimeErrorMap_;
    int nowTime_ {0};
    ModelBuildOptions expectBuildOptions_;
    ModelInitOptions expectInitOptions_;
};
} // namespace hiai
#endif // CONTROL_CLIENT_H