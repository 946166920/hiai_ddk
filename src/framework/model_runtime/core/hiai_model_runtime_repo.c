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
#include "hiai_model_runtime_repo.h"

#include <pthread.h>
#include <stdbool.h>
#include <dlfcn.h>

#include "hiai_model_builder_impl.h"
#include "framework/infra/log/log.h"

static pthread_mutex_t g_runtimesMutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct HIAI_ModelRuntimeRepo {
    HIAI_ModelRuntime* runtimes[HIAI_MAX_MODEL_RUNTIME_NUM];
} HIAI_ModelRuntimeRepo;

static HIAI_ModelRuntimeRepo* HIAI_ModelRuntimeRepo_Inst(void)
{
    static HIAI_ModelRuntimeRepo repo;
    return &repo;
}

void HIAI_ModelRuntimeRepo_DeInit(void)
{
    HIAI_ModelRuntimeRepo* repo = HIAI_ModelRuntimeRepo_Inst();
    for (size_t i = 0; i < HIAI_MAX_MODEL_RUNTIME_NUM; i++) {
        HIAI_ModelRuntime_UnLoad(repo->runtimes[i]);
    }
}

void HIAI_ModelRuntimeRepo_Add(HIAI_ModelRuntimeType type, HIAI_ModelRuntime* runtime)
{
    if (type >= HIAI_MAX_MODEL_RUNTIME_NUM || runtime == NULL) {
        return;
    }

    HIAI_ModelRuntimeRepo* repo = HIAI_ModelRuntimeRepo_Inst();
    repo->runtimes[type] = runtime;
}

static HIAI_ModelRuntime* HIAI_ModelRuntimeRepo_LoadByType(HIAI_ModelRuntimeType type)
{
    HIAI_ModelRuntimeRepo* repo = HIAI_ModelRuntimeRepo_Inst();
    if (repo->runtimes[type] == NULL) {
        pthread_mutex_lock(&g_runtimesMutex);
        if (repo->runtimes[type] == NULL) {
            HIAI_ModelRuntime* modelRuntime = HIAI_ModelRuntime_Load(type);
            HIAI_ModelRuntimeRepo_Add(type, modelRuntime);
        }
        pthread_mutex_unlock(&g_runtimesMutex);
    }
    return repo->runtimes[type];
}

static bool HIAI_ModelRuntime_IsSupportedModelRuntime(HIAI_ModelRuntimeType type)
{
    if (type >= HIAI_MAX_MODEL_RUNTIME_NUM) {
        return false;
    }

    char* list = "PLUGIN,LOCALHCL,DIRECT,ROMHCL,BINARY";
#ifdef AI_SW_SPEC_MODEL_RUNTIME_LIST
    list = AI_SW_SPEC_MODEL_RUNTIME_LIST;
#endif
    if (list == NULL) {
        return false;
    }

    static const char* runtimeList[HIAI_MAX_MODEL_RUNTIME_NUM] = {
        [PLUGIN_MODEL_RUNTIME_HCL]  = "PLUGIN",
        [HIAI_MODEL_RUNTIME_HCL]    = "LOCALHCL",
        [HIAI_MODEL_RUNTIME_DIRECT] = "DIRECT",
        [FMK_MODEL_RUNTIME_HCL]     = "ROMHCL",
        [HIAI_MODEL_RUNTIME_BINARY] = "BINARY",
    };

    if (strstr(list, runtimeList[type]) != NULL) {
        return true;
    }

    return false;
}

static HIAI_ModelRuntime* ModelRuntimeRepo_GetRuntimeFromIndex(size_t index, uint32_t* isHCLTried)
{
    if (!HIAI_ModelRuntime_IsSupportedModelRuntime(index)) {
        return NULL;
    }

    if (*isHCLTried == 1 &&
        (index == PLUGIN_MODEL_RUNTIME_HCL || index == HIAI_MODEL_RUNTIME_HCL || index == FMK_MODEL_RUNTIME_HCL)) {
        return NULL;
    }

    (void)HIAI_ModelRuntimeRepo_LoadByType(index);

    HIAI_ModelRuntimeRepo* repo = HIAI_ModelRuntimeRepo_Inst();
    if (repo->runtimes[index] != NULL &&
        (index == PLUGIN_MODEL_RUNTIME_HCL || index == HIAI_MODEL_RUNTIME_HCL || index == FMK_MODEL_RUNTIME_HCL)) {
        *isHCLTried = 1;
    }
    return repo->runtimes[index];
}

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryBuild(
    const HIAI_ModelBuildOptions* options, const char* modelName, const void* modelData, size_t modelSize)
{
#ifdef HIAI_DDK
    FMK_LOGI("DDK version is beta.");
#endif

    uint32_t isHCLTried = 0;
    for (size_t i = 0; i < HIAI_MAX_MODEL_RUNTIME_NUM; i++) {
        HIAI_ModelRuntime* modelRuntime = ModelRuntimeRepo_GetRuntimeFromIndex(i, &isHCLTried);
        HIAI_BuiltModel_Impl* builtModelImpl =
            HIAI_ModelBuilder_BuildOnRuntime(options, modelName, modelData, modelSize, modelRuntime);
        if (builtModelImpl != NULL) {
            return builtModelImpl;
        }
    }

    FMK_LOGE("no runtime support the Model.");
    return NULL;
}

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestore(const void* modelData, size_t modelSize)
{
#ifdef HIAI_DDK
    FMK_LOGI("DDK version is beta.");
#endif

    uint32_t isHCLTried = 0;
    for (size_t i = 0; i < HIAI_MAX_MODEL_RUNTIME_NUM; i++) {
        HIAI_ModelRuntime* modelRuntime = ModelRuntimeRepo_GetRuntimeFromIndex(i, &isHCLTried);
        HIAI_BuiltModel_Impl* builtModelImpl = HIAI_BuiltModel_RestoreOnRuntime(modelData, modelSize, modelRuntime);
        if (builtModelImpl != NULL) {
            return builtModelImpl;
        }
    }

    FMK_LOGE("no runtime support the Model.");
    return NULL;
}

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestoreFromFile(const char* file)
{
    uint32_t isHCLTried = 0;
    for (size_t i = 0; i < HIAI_MAX_MODEL_RUNTIME_NUM; ++i) {
        HIAI_ModelRuntime* modelRuntime = ModelRuntimeRepo_GetRuntimeFromIndex(i, &isHCLTried);
        HIAI_BuiltModel_Impl* builtModelImpl = HIAI_BuiltModel_RestoreFromFileOnRuntime(file, modelRuntime);
        if (builtModelImpl != NULL) {
            return builtModelImpl;
        }
    }

    FMK_LOGE("no runtime support the Model.");
    return NULL;
}

HIAI_BuiltModel_Impl* ModelRuntimeRepo_TryRestoreFromFileWithShapeIndex(const char* file, uint8_t shapeIndex)
{
    uint32_t isHCLTried = 0;
    for (size_t i = 0; i < HIAI_MAX_MODEL_RUNTIME_NUM; ++i) {
        HIAI_ModelRuntime* modelRuntime = ModelRuntimeRepo_GetRuntimeFromIndex(i, &isHCLTried);
        HIAI_BuiltModel_Impl* builtModelImpl =
            HIAI_BuiltModel_RestoreFromFileWithShapeIndexOnRuntime(file, shapeIndex, modelRuntime);
        if (builtModelImpl != NULL) {
            return builtModelImpl;
        }
    }

    FMK_LOGE("no runtime support the Model.");
    return NULL;
}

HIAI_ModelRuntime* HIAI_ModelRuntimeRepo_GetSutiableHclRuntime(void)
{
    HIAI_ModelRuntimeRepo* repo = HIAI_ModelRuntimeRepo_Inst();

    HIAI_ModelRuntimeType type = PLUGIN_MODEL_RUNTIME_HCL;
    if (HIAI_ModelRuntime_IsSupportedModelRuntime(type) && HIAI_ModelRuntimeRepo_LoadByType(type) != NULL) {
        return repo->runtimes[type];
    }

    type = HIAI_MODEL_RUNTIME_HCL;
    if (HIAI_ModelRuntime_IsSupportedModelRuntime(type) && HIAI_ModelRuntimeRepo_LoadByType(type) != NULL) {
        return repo->runtimes[type];
    }

    type = FMK_MODEL_RUNTIME_HCL;
    if (HIAI_ModelRuntime_IsSupportedModelRuntime(type) && HIAI_ModelRuntimeRepo_LoadByType(type) != NULL) {
        return repo->runtimes[type];
    }

    return NULL;
}

bool HIAI_ModelRuntimeRepo_IsOldHclModelRuntime(const HIAI_ModelRuntime* runtime)
{
    void* hcl_buildV2 = dlsym(runtime->handle, "HIAI_HCL_ModelBuilder_BuildV2");
    if (hcl_buildV2 == NULL) {
        void* hcl_build = dlsym(runtime->handle, "HIAI_HCL_ModelBuilder_Build");
        if (hcl_build == NULL) {
            return true;
        }
    }
    return false;
}

#ifdef AI_SUPPORT_LEGACY_APP_COMPATIBLE
bool IsHclModelRuntimeCanBuild(void)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime == NULL) {
        return false;
    }
    if (runtime->handle == NULL) { // static lib
        return true;
    }

    bool isOldHclModelRuntime = HIAI_ModelRuntimeRepo_IsOldHclModelRuntime(runtime);
    if (isOldHclModelRuntime) {
        return false;
    }

    return true;
}
#endif