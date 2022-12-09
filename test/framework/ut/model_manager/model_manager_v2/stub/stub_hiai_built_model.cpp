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
#include <vector>
#include "framework/c/hiai_built_model.h"
#include "framework/c/hiai_nd_tensor_desc.h"

HIAI_Status HIAI_BuiltModel_SaveToExternalBuffer(
    const HIAI_BuiltModel* model, void* data, size_t size, size_t* realSize)
{
    *realSize = 100;
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_BuiltModel_Save(const HIAI_BuiltModel* model, void** data, size_t* size)
{
    *data = new uint8_t[200];
    *size = 200;
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_BuiltModel_SaveToFile(const HIAI_BuiltModel* model, const char* file)
{
    return HIAI_SUCCESS;
}

HIAI_BuiltModel* HIAI_BuiltModel_Restore(const void* data, size_t size)
{
    HIAI_BuiltModel* builtModel = (HIAI_BuiltModel*)malloc(1);
    return builtModel;
}

HIAI_BuiltModel* HIAI_BuiltModel_RestoreFromFile(const char* file)
{
    HIAI_BuiltModel* builtModel = (HIAI_BuiltModel*)malloc(1);
    return builtModel;
}

HIAI_Status HIAI_BuiltModel_CheckCompatibility(
    const HIAI_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility)
{
    return HIAI_SUCCESS;
}

int32_t HIAI_BuiltModel_GetInputTensorNum(const HIAI_BuiltModel* model)
{
    return 2;
}

HIAI_NDTensorDesc* HIAI_BuiltModel_GetInputTensorDesc(const HIAI_BuiltModel* model, size_t index)
{
    std::vector<int32_t> dims = {1, 2, 255, 255};
    return HIAI_NDTensorDesc_Create(dims.data(), 4, HIAI_DataType(HIAI_DATATYPE_UINT8), HIAI_Format(HIAI_FORMAT_NCHW));
}

int32_t HIAI_BuiltModel_GetOutputTensorNum(const HIAI_BuiltModel* model)
{
    return 0;
}

HIAI_NDTensorDesc* HIAI_BuiltModel_GetOutputTensorDesc(const HIAI_BuiltModel* model, size_t index)
{
    return nullptr;
}

const char* HIAI_BuiltModel_GetName(const HIAI_BuiltModel* model)
{
    return nullptr;
}

HIAI_Status HIAI_BuiltModel_SetName(const HIAI_BuiltModel* model, const char* name)
{
    return HIAI_SUCCESS;
}

void HIAI_BuiltModel_Destroy(HIAI_BuiltModel** model)
{
    if (model == nullptr || *model == nullptr) {
        return;
    }
    free(*model);
    *model = nullptr;
}