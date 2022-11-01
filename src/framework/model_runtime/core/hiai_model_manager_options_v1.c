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
#include "hiai_model_manager_options_v1.h"
#include "securec.h"
#include "framework/infra/log/log.h"

typedef struct HIAI_ModelInitOptionsV1 {
    HIAI_PerfMode perfMode;
    HIAI_ModelBuildOptions* buildOptions;
} HIAI_ModelInitOptionsV1;

HIAI_ModelInitOptions* HIAI_ModelInitOptionsV1_Create(void)
{
    HIAI_ModelInitOptionsV1* option = (HIAI_ModelInitOptionsV1*)malloc(sizeof(HIAI_ModelInitOptionsV1));
    MALLOC_NULL_CHECK_RET_VALUE(option, NULL);
    (void)memset_s(option, sizeof(HIAI_ModelInitOptionsV1), 0, sizeof(HIAI_ModelInitOptionsV1));

    option->perfMode = HIAI_PERFMODE_NORMAL;
    option->buildOptions = NULL;
    return (HIAI_ModelInitOptions*)option;
}

void HIAI_ModelInitOptionsV1_SetPerfMode(HIAI_ModelInitOptions* options, HIAI_PerfMode devPerf)
{
    if (options == NULL) {
        return;
    }
    if (devPerf < HIAI_PERFMODE_UNSET || devPerf > HIAI_PERFMODE_EXTREME) {
        FMK_LOGW("devPerf is invalid. set devPerf is HIAI_PERFMODE_NORMAL");
        ((HIAI_ModelInitOptionsV1*)options)->perfMode = HIAI_PERFMODE_NORMAL;
        return;
    }
    ((HIAI_ModelInitOptionsV1*)options)->perfMode = devPerf;
}

HIAI_PerfMode HIAI_ModelInitOptionsV1_GetPerfMode(const HIAI_ModelInitOptions* options)
{
    return options == NULL ? HIAI_PERFMODE_UNSET : ((HIAI_ModelInitOptionsV1*)options)->perfMode;
}

void HIAI_ModelInitOptionsV1_SetBuildOptions(HIAI_ModelInitOptions* options, HIAI_ModelBuildOptions* buildOptions)
{
    if (options == NULL) {
        return;
    }

    if (((HIAI_ModelInitOptionsV1*)options)->buildOptions != NULL) {
        FMK_LOGW("buildOptions set repeatedly.");
        return;
    }
    ((HIAI_ModelInitOptionsV1*)options)->buildOptions = buildOptions;
}

HIAI_ModelBuildOptions* HIAI_ModelInitOptionsV1_GetBuildOptions(const HIAI_ModelInitOptions* options)
{
    return options == NULL ? NULL : ((HIAI_ModelInitOptionsV1*)options)->buildOptions;
}

void HIAI_ModelInitOptionsV1_Destroy(HIAI_ModelInitOptions** options)
{
    if (options == NULL || *options == NULL) {
        return;
    }

    HIAI_ModelInitOptionsV1* tmp = (HIAI_ModelInitOptionsV1*)(*options);
    if (tmp->buildOptions != NULL) {
        HIAI_ModelBuildOptionsV1_Destroy(&tmp->buildOptions);
    }

    free(*options);
    *options = NULL;
}
