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
#ifndef FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_COMMON_UTIL_H
#define FRAMEWORK_MODEL_MANAGER_DIRECT_MODEL_RUNTIME_COMMON_UTIL_H
#include <string>

#include "framework/c/compatible/HIAIModelManager.h"
#include "framework/c/hiai_model_builder_types.h"
#include "base/error_types.h"
#include "framework/util/base_buffer.h"

namespace hiai {
constexpr uint32_t MAX_COMPILED_TARGET_SIZE = 200 * 1024 * 1024;

class DirectCommonUtil {
public:
    DirectCommonUtil() = default;

    ~DirectCommonUtil() = default;

    static bool CheckBuildOptions(const HIAI_ModelBuildOptions* options);

    static Status MakeCompatibleModel(HIAI_MemBuffer* input, HIAI_MemBuffer** output, bool& isNeedRelease);

    static void DestroyBuffer(HIAI_MemBuffer* membuf);

    static bool IsSupportIRBuild(void);
};
} // namespace hiai

#endif