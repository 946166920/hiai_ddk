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
#ifndef FRAMEWORK_C_HIAI_EXECUTE_OPTION_TYPES_H
#define FRAMEWORK_C_HIAI_EXECUTE_OPTION_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HIAI_PERFMODE_UNSET = 0,
    HIAI_PERFMODE_LOW,
    HIAI_PERFMODE_NORMAL,
    HIAI_PERFMODE_HIGH,
    HIAI_PERFMODE_EXTREME,
#ifdef AI_SUPPORT_PREFMODE_EXTEND
    HIAI_HIGH_ONLY_NPU = 103
#endif
} HIAI_PerfMode;

typedef enum {
#ifdef AI_SUPPORT_PRIORITY_EXTEND
    HIAI_PRIORITY_HIGH_PLATFORM1 = 1,
    HIAI_PRIORITY_HIGH_PLATFORM2,
    HIAI_PRIORITY_HIGHEST,
    HIAI_PRIORITY_HIGHER,
#endif
    HIAI_PRIORITY_HIGH = 5,
    HIAI_PRIORITY_MIDDLE,
    HIAI_PRIORITY_LOW
} HIAI_ModelPriority;

#ifdef __cplusplus
}
#endif

#endif /* FRAMEWORK_C_HIAI_EXECUTE_OPTION_TYPES_H */