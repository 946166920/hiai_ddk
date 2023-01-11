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

#include <pthread.h>
#include <stdbool.h>

#include "hiai_model_manager_options_v1.h"
#include "hiai_model_runtime_repo.h"
#include "securec.h"
#include "framework/infra/log/log.h"

typedef struct ModelInitOptionsFuncs {
    HIAI_MR_ModelInitOptions* (*create)(void);

    void (*setPerfMode)(HIAI_MR_ModelInitOptions*, HIAI_PerfMode);
    HIAI_PerfMode (*getPerfMode)(const HIAI_MR_ModelInitOptions*);

    void (*setBuildOptions)(HIAI_MR_ModelInitOptions*, HIAI_MR_ModelBuildOptions*);
    HIAI_MR_ModelBuildOptions* (*getBuildOptions)(const HIAI_MR_ModelInitOptions*);

    void (*destroy)(HIAI_MR_ModelInitOptions**);
} ModelInitOptionsFuncs;

static void InitModelInitOptionsFuncs(ModelInitOptionsFuncs* modelInitOptionsFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_CREATE] != NULL) {
        modelInitOptionsFuncs->create = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_CREATE];
        modelInitOptionsFuncs->setPerfMode = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_SETPERFMODE];
        modelInitOptionsFuncs->getPerfMode = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_GETPERFMODE];
        modelInitOptionsFuncs->setBuildOptions = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_SETBUILDOPTIONS];
        modelInitOptionsFuncs->getBuildOptions = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_GETBUILDOPTIONS];
        modelInitOptionsFuncs->destroy = runtime->optionsSymbolList[HIAI_MODELINITOPTIONS_DESTROY];
    } else {
        // if you need add new func, don't add here.
        modelInitOptionsFuncs->create = HIAI_ModelInitOptionsV1_Create;
        modelInitOptionsFuncs->setPerfMode = HIAI_ModelInitOptionsV1_SetPerfMode;
        modelInitOptionsFuncs->getPerfMode = HIAI_ModelInitOptionsV1_GetPerfMode;
        modelInitOptionsFuncs->setBuildOptions = HIAI_ModelInitOptionsV1_SetBuildOptions;
        modelInitOptionsFuncs->getBuildOptions = HIAI_ModelInitOptionsV1_GetBuildOptions;
        modelInitOptionsFuncs->destroy = HIAI_ModelInitOptionsV1_Destroy;
    }
}

static pthread_mutex_t g_initOptionFuncsInitMutex = PTHREAD_MUTEX_INITIALIZER;
static ModelInitOptionsFuncs* GetModelInitOptionsFuncs(void)
{
    static ModelInitOptionsFuncs modelInitOptionsFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_initOptionFuncsInitMutex);
        if (!isInited) {
            InitModelInitOptionsFuncs(&modelInitOptionsFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_initOptionFuncsInitMutex);
    }

    return &modelInitOptionsFuncs;
}

HIAI_MR_ModelInitOptions* HIAI_MR_ModelInitOptions_Create(void)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->create != NULL) {
        return modelInitOptionsFuncs->create();
    }
    return NULL;
}

void HIAI_MR_ModelInitOptions_SetPerfMode(HIAI_MR_ModelInitOptions* options, HIAI_PerfMode devPerf)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->setPerfMode != NULL) {
        modelInitOptionsFuncs->setPerfMode(options, devPerf);
    }
}

HIAI_PerfMode HIAI_MR_ModelInitOptions_GetPerfMode(const HIAI_MR_ModelInitOptions* options)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->getPerfMode != NULL) {
        return modelInitOptionsFuncs->getPerfMode(options);
    }
    return HIAI_PERFMODE_UNSET;
}

void HIAI_MR_ModelInitOptions_SetBuildOptions(
    HIAI_MR_ModelInitOptions* options, HIAI_MR_ModelBuildOptions* buildOptions)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->setBuildOptions != NULL) {
        modelInitOptionsFuncs->setBuildOptions(options, buildOptions);
    }
}

HIAI_MR_ModelBuildOptions* HIAI_MR_ModelInitOptions_GetBuildOptions(HIAI_MR_ModelInitOptions* options)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->getBuildOptions != NULL) {
        return modelInitOptionsFuncs->getBuildOptions(options);
    }
    return NULL;
}

void HIAI_MR_ModelInitOptions_Destroy(HIAI_MR_ModelInitOptions** options)
{
    ModelInitOptionsFuncs* modelInitOptionsFuncs = GetModelInitOptionsFuncs();
    if (modelInitOptionsFuncs->destroy != NULL) {
        modelInitOptionsFuncs->destroy(options);
    }
}
