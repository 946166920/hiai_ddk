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

#include "framework/c/hiai_model_manager_types.h"
#include "securec.h"
#include "framework/infra/log/log.h"

typedef struct HIAI_MR_ModelInitOptions {
    HIAI_PerfMode perfMode;
    HIAI_MR_ModelBuildOptions* buildOptions;
} HIAI_MR_ModelInitOptions;

HIAI_MR_ModelInitOptions* HIAI_MR_ModelInitOptions_Create(void)
{
    HIAI_MR_ModelInitOptions* option = (HIAI_MR_ModelInitOptions*)malloc(sizeof(HIAI_MR_ModelInitOptions));
    MALLOC_NULL_CHECK_RET_VALUE(option, NULL);
    (void)memset_s(option, sizeof(HIAI_MR_ModelInitOptions), 0, sizeof(HIAI_MR_ModelInitOptions));

    option->perfMode = HIAI_PERFMODE_NORMAL;
    option->buildOptions = NULL;
    return option;
}

void HIAI_MR_ModelInitOptions_SetPerfMode(HIAI_MR_ModelInitOptions* options, HIAI_PerfMode devPerf)
{
    if (options == NULL) {
        return;
    }
    if (devPerf < HIAI_PERFMODE_UNSET || devPerf > HIAI_PERFMODE_EXTREME) {
        FMK_LOGW("devPerf is invalid. set devPerf is HIAI_PERFMODE_NORMAL");
        options->perfMode = HIAI_PERFMODE_NORMAL;
        return;
    }
    options->perfMode = devPerf;
}

HIAI_PerfMode HIAI_MR_ModelInitOptions_GetPerfMode(const HIAI_MR_ModelInitOptions* options)
{
    return options == NULL ? HIAI_PERFMODE_UNSET : options->perfMode;
}

void HIAI_MR_ModelInitOptions_SetBuildOptions(
    HIAI_MR_ModelInitOptions* options, HIAI_MR_ModelBuildOptions* buildOptions)
{
    if (options == NULL) {
        return;
    }

    if (options->buildOptions != NULL) {
        FMK_LOGW("buildOptions set repeatedly.");
        return;
    }
    options->buildOptions = buildOptions;
}

HIAI_MR_ModelBuildOptions* HIAI_MR_ModelInitOptions_GetBuildOptions(HIAI_MR_ModelInitOptions* options)
{
    return options == NULL ? NULL : options->buildOptions;
}

void HIAI_MR_ModelInitOptions_Destroy(HIAI_MR_ModelInitOptions** options)
{
    if (options == NULL || *options == NULL) {
        return;
    }

    HIAI_MR_ModelInitOptions* tmp = (HIAI_MR_ModelInitOptions*)(*options);
    if (tmp->buildOptions != NULL) {
        HIAI_MR_ModelBuildOptions_Destroy(&tmp->buildOptions);
    }

    free(*options);
    *options = NULL;
}
