/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: execute types
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