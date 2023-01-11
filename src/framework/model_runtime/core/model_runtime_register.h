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
#ifndef FRAMEWORK_MODEL_MANAGER_MODEL_RUNTIME_REGISTER_H
#define FRAMEWORK_MODEL_MANAGER_MODEL_RUNTIME_REGISTER_H
#include <functional>

#include "hiai_model_runtime.h"
#include "hiai_model_runtime_repo.h"

namespace hiai {
using InitModelRuntimeFunc = std::function<void(HIAI_ModelRuntime* runtime)>;
class ModelRuntimeRegister {
public:
    ModelRuntimeRegister(InitModelRuntimeFunc initModelRuntime, HIAI_ModelRuntimeType runtimeType) noexcept;
    ~ModelRuntimeRegister() = default;
};
} // namespace hiai
#endif
