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
#ifndef _OM_OPTIONS_H_
#define _OM_OPTIONS_H_

#include <string>
#include <map>

namespace ge {
const char* const OM_MODEL_NAME = "model_name";
const char* const OM_ENABLE_PMU = "open_pmu";
enum class OMType { NONE, PROFILING, DUMP };

enum class OMAction {
    ACTION_OFF = 0,
    ACTION_ON = 1,
    ACTION_INVALID,
};

struct OMOptions {
    OMType type = OMType::NONE;
    OMAction action = OMAction::ACTION_OFF;
    std::map<std::string, std::string> config;
};
} // namespace ge
#endif
