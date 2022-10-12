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
#include "model_runtime_register.h"
#include "hiai_model_runtime_repo.h"
#include "securec.h"
#include "framework/infra/log/log.h"

namespace hiai {
ModelRuntimeRegister::ModelRuntimeRegister(
    InitModelRuntimeFunc initModelRuntime, HIAI_ModelRuntimeType runtimeType) noexcept
{
    HIAI_ModelRuntime* runtime = (HIAI_ModelRuntime*)malloc(sizeof(HIAI_ModelRuntime));
    if (runtime == nullptr) {
        FMK_LOGE("malloc failed.");
        return;
    }
    runtime->handle = nullptr;
    (void)memset_s(runtime, sizeof(HIAI_ModelRuntime), 0, sizeof(HIAI_ModelRuntime));
    runtime->runtimeType = runtimeType;

    initModelRuntime(runtime);

    (void)HIAI_ModelRuntimeRepo_Add(runtimeType, runtime);
}
} // namespace hiai
