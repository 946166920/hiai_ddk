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
#include "hiai_built_model_impl.h"
#include "securec.h"
#include "framework/infra/log/log.h"

HIAI_BuiltModel_Impl* HIAI_BuiltModel_ToBuiltModelImpl(const HIAI_MR_BuiltModel* model)
{
    if (model == NULL) {
        FMK_LOGE("input model is null.");
        return NULL;
    }

    HIAI_BuiltModel_Impl* impl = (HIAI_BuiltModel_Impl*)model;
    if (impl->runtimeBuiltModel == NULL || impl->runtime == NULL) {
        FMK_LOGE("input model is invalid.");
        return NULL;
    }
    return impl;
}

HIAI_BuiltModel_Impl* HIAI_BuiltModel_CreateModelImpl(HIAI_MR_BuiltModel* model, const HIAI_ModelRuntime* runtime)
{
    if (model == NULL || runtime == NULL) {
        FMK_LOGE("create built model failed.");
        return NULL;
    }

    HIAI_BuiltModel_Impl* modelImpl = malloc(sizeof(HIAI_BuiltModel_Impl));
    MALLOC_NULL_CHECK_RET_VALUE(modelImpl, NULL);

    modelImpl->runtimeBuiltModel = model;
    modelImpl->runtime = runtime;
    return modelImpl;
}

typedef HIAI_MR_BuiltModel* (*HIAI_BuiltModel_Restore_Ptr)(const void* data, size_t size);
HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreOnRuntime(const void* data, size_t size, const HIAI_ModelRuntime* runtime)
{
    if (runtime == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_Restore_Ptr restore = (HIAI_BuiltModel_Restore_Ptr)runtime->symbolList[HRANI_BUILTMODEL_RESTORE];
    if (restore == NULL) {
        FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_RESTORE);
        return NULL;
    }
    HIAI_MR_BuiltModel* runtimeBuiltModel = restore(data, size);

    return HIAI_BuiltModel_CreateModelImpl(runtimeBuiltModel, runtime);
}

typedef HIAI_MR_BuiltModel* (*HIAI_BuiltModel_RestoreFromFile_Ptr)(const char* file);
HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreFromFileOnRuntime(const char* file, const HIAI_ModelRuntime* runtime)
{
    if (runtime == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_RestoreFromFile_Ptr restore =
        (HIAI_BuiltModel_RestoreFromFile_Ptr)runtime->symbolList[HRANI_BUILTMODEL_RESTORE_FROM_FILE];
    if (restore == NULL) {
        FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_RESTORE_FROM_FILE);
        return NULL;
    }
    HIAI_MR_BuiltModel* runtimeBuiltModel = restore(file);

    return HIAI_BuiltModel_CreateModelImpl(runtimeBuiltModel, runtime);
}

typedef HIAI_MR_BuiltModel* (*HIAI_BuiltModel_RestoreFromFileWithShapeIndex_Ptr)(const char* file, uint8_t shapeIndex);
HIAI_BuiltModel_Impl* HIAI_BuiltModel_RestoreFromFileWithShapeIndexOnRuntime(
    const char* file, uint8_t shapeIndex, const HIAI_ModelRuntime* runtime)
{
    if (runtime == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_RestoreFromFileWithShapeIndex_Ptr restore =
        (HIAI_BuiltModel_RestoreFromFileWithShapeIndex_Ptr)
            runtime->symbolList[HRANI_BUILTMODEL_RESTORE_FROM_FILE_WITH_SHAPE_INDEX];
    if (restore == NULL) {
        FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_RESTORE_FROM_FILE_WITH_SHAPE_INDEX);
        return NULL;
    }
    HIAI_MR_BuiltModel* runtimeBuiltModel = restore(file, shapeIndex);

    return HIAI_BuiltModel_CreateModelImpl(runtimeBuiltModel, runtime);
}
