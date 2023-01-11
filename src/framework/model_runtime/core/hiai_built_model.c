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
#include "framework/c/hiai_built_model.h"
#include "hiai_built_model_impl.h"
#include "hiai_model_runtime_repo.h"
#include "securec.h"
#include "framework/infra/log/log.h"

typedef HIAI_Status (*HIAI_BuiltModel_SaveToExternalBuffer_Ptr)(
    const HIAI_MR_BuiltModel* model, void* data, size_t size, size_t* realSize);
HIAI_Status HIAI_MR_BuiltModel_SaveToExternalBuffer(
    const HIAI_MR_BuiltModel* model, void* data, size_t size, size_t* realSize)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_SaveToExternalBuffer_Ptr save =
        (HIAI_BuiltModel_SaveToExternalBuffer_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_SAVE_TO_EXTERNAL_BUFFER];
    if (save != NULL) {
        return save(impl->runtimeBuiltModel, data, size, realSize);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_SAVE_TO_EXTERNAL_BUFFER);
    return HIAI_UNSUPPORTED;
}

typedef HIAI_Status (*HIAI_BuiltModel_Save_Ptr)(const HIAI_MR_BuiltModel* model, void** data, size_t* size);
HIAI_Status HIAI_MR_BuiltModel_Save(const HIAI_MR_BuiltModel* model, void** data, size_t* size)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_Save_Ptr save = (HIAI_BuiltModel_Save_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_SAVE];
    if (save != NULL) {
        return save(impl->runtimeBuiltModel, data, size);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_SAVE);
    return HIAI_UNSUPPORTED;
}

typedef HIAI_Status (*HIAI_BuiltModel_SaveToFile_Ptr)(const HIAI_MR_BuiltModel* model, const char* file);
HIAI_Status HIAI_MR_BuiltModel_SaveToFile(const HIAI_MR_BuiltModel* model, const char* file)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_SaveToFile_Ptr save =
        (HIAI_BuiltModel_SaveToFile_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_SAVE_TO_FILE];
    if (save != NULL) {
        return save(impl->runtimeBuiltModel, file);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_SAVE_TO_FILE);
    return HIAI_UNSUPPORTED;
}

HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_Restore(const void* data, size_t size)
{
    return (HIAI_MR_BuiltModel*)ModelRuntimeRepo_TryRestore(data, size);
}

HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_RestoreFromFile(const char* file)
{
    return (HIAI_MR_BuiltModel*)ModelRuntimeRepo_TryRestoreFromFile(file);
}

HIAI_MR_BuiltModel* HIAI_MR_BuiltModel_RestoreFromFileWithShapeIndex(const char* file, uint8_t shapeIndex)
{
    if (file == NULL || (shapeIndex < 1 || shapeIndex > 16)) {
        FMK_LOGE("param is invalid.");
        return NULL;
    }
    return (HIAI_MR_BuiltModel*)ModelRuntimeRepo_TryRestoreFromFileWithShapeIndex(file, shapeIndex);
}

typedef HIAI_Status (*HIAI_BuiltModel_CheckCompatibility_Ptr)(
    const HIAI_MR_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility);
HIAI_Status HIAI_MR_BuiltModel_CheckCompatibility(
    const HIAI_MR_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_CheckCompatibility_Ptr check =
        (HIAI_BuiltModel_CheckCompatibility_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_CHECK_COMPATIBILITY];
    if (check != NULL) {
        return check(impl->runtimeBuiltModel, compatibility);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_CHECK_COMPATIBILITY);
    return HIAI_UNSUPPORTED;
}

typedef int32_t (*HIAI_BuiltModel_GetInputTensorNum_Ptr)(const HIAI_MR_BuiltModel* model);
int32_t HIAI_MR_BuiltModel_GetInputTensorNum(const HIAI_MR_BuiltModel* model)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return 0;
    }

    HIAI_BuiltModel_GetInputTensorNum_Ptr get =
        (HIAI_BuiltModel_GetInputTensorNum_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_GET_INPUT_TENSOR_NUM];
    if (get != NULL) {
        return get(impl->runtimeBuiltModel);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_INPUT_TENSOR_NUM);
    return 0;
}

typedef HIAI_NDTensorDesc* (*HIAI_BuiltModel_GetInputTensorDesc_Ptr)(const HIAI_MR_BuiltModel* model, size_t index);
HIAI_NDTensorDesc* HIAI_MR_BuiltModel_GetInputTensorDesc(const HIAI_MR_BuiltModel* model, size_t index)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_GetInputTensorDesc_Ptr get =
        (HIAI_BuiltModel_GetInputTensorDesc_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_GET_INPUT_TENSOR_DESC];
    if (get != NULL) {
        return get(impl->runtimeBuiltModel, index);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_INPUT_TENSOR_DESC);
    return NULL;
}

typedef int32_t (*HIAI_BuiltModel_GetOutputTensorNum_Ptr)(const HIAI_MR_BuiltModel* model);
int32_t HIAI_MR_BuiltModel_GetOutputTensorNum(const HIAI_MR_BuiltModel* model)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return 0;
    }

    HIAI_BuiltModel_GetOutputTensorNum_Ptr get =
        (HIAI_BuiltModel_GetOutputTensorNum_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_NUM];
    if (get != NULL) {
        return get(impl->runtimeBuiltModel);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_NUM);
    return 0;
}

typedef HIAI_NDTensorDesc* (*HIAI_BuiltModel_GetOutputTensorDesc_Ptr)(const HIAI_MR_BuiltModel* model, size_t index);
HIAI_NDTensorDesc* HIAI_MR_BuiltModel_GetOutputTensorDesc(const HIAI_MR_BuiltModel* model, size_t index)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_GetOutputTensorDesc_Ptr get =
        (HIAI_BuiltModel_GetOutputTensorDesc_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_DESC];
    if (get != NULL) {
        return get(impl->runtimeBuiltModel, index);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_DESC);
    return NULL;
}

typedef const char* (*HIAI_BuiltModel_GetName_Ptr)(const HIAI_MR_BuiltModel* model);
const char* HIAI_MR_BuiltModel_GetName(const HIAI_MR_BuiltModel* model)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return NULL;
    }

    HIAI_BuiltModel_GetName_Ptr get = impl->runtime->symbolList[HRANI_BUILTMODEL_GET_NAME];
    if (get != NULL) {
        return get(impl->runtimeBuiltModel);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_GET_NAME);
    return NULL;
}

typedef HIAI_Status (*HIAI_BuiltModel_SetName_Ptr)(const HIAI_MR_BuiltModel* model, const char* name);
HIAI_Status HIAI_MR_BuiltModel_SetName(const HIAI_MR_BuiltModel* model, const char* name)
{
    const HIAI_BuiltModel_Impl* impl = HIAI_BuiltModel_ToBuiltModelImpl(model);
    if (impl == NULL) {
        return HIAI_INVALID_PARAM;
    }

    HIAI_BuiltModel_SetName_Ptr set = (HIAI_BuiltModel_SetName_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_SET_NAME];
    if (set != NULL) {
        return set(impl->runtimeBuiltModel, name);
    }

    FMK_LOGW("sym %d not found.", HRANI_BUILTMODEL_SET_NAME);
    return HIAI_UNSUPPORTED;
}

typedef void (*HIAI_BuiltModel_Destroy_Ptr)(HIAI_MR_BuiltModel** model);
void HIAI_MR_BuiltModel_Destroy(HIAI_MR_BuiltModel** model)
{
    if (model == NULL || *model == NULL) {
        return;
    }

    HIAI_BuiltModel_Impl* impl = (HIAI_BuiltModel_Impl*)(*model);
    if (impl == NULL || impl->runtimeBuiltModel == NULL || impl->runtime == NULL) {
        goto FREE;
    }

    HIAI_BuiltModel_Destroy_Ptr destroy =
        (HIAI_BuiltModel_Destroy_Ptr)impl->runtime->symbolList[HRANI_BUILTMODEL_DESTROY];
    if (destroy == NULL) {
        FMK_LOGE("sym %d not found.", HRANI_BUILTMODEL_DESTROY);
        goto FREE;
    }

    destroy(&impl->runtimeBuiltModel);

FREE:
    free(*model);
    *model = NULL;
}