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
#include "direct_built_model.h"
#include "securec.h"
// inc
#include "framework/infra/log/log.h"
#include "framework/model/model_type_util.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

// src/framework
#include "inc/common/scope_guard.h"
#include "inc/util/file_util.h"

#include "direct_built_model_impl.h"
#include "direct_common_util.h"

namespace hiai {

HIAI_Status HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(
    const HIAI_MR_BuiltModel* model, void* data, size_t size, size_t* realSize)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);

    return static_cast<HIAI_Status>(builtModelImpl->SaveToExternalBuffer(data, size, realSize));
}

HIAI_Status HIAI_DIRECT_BuiltModel_Save(const HIAI_MR_BuiltModel* model, void** data, size_t* size)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);

    return static_cast<HIAI_Status>(builtModelImpl->Save(data, size));
}

HIAI_Status HIAI_DIRECT_BuiltModel_SaveToFile(const HIAI_MR_BuiltModel* model, const char* file)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);

    return static_cast<HIAI_Status>(builtModelImpl->SaveToFile(file));
}

namespace {
HIAI_MR_BuiltModel* CreateBuiltModelImpl(std::shared_ptr<BaseBuffer>& modelBuffer)
{
    char modelName[MODEL_NAME_LENGTH] = {0};
    ModelFileHeader* header = reinterpret_cast<ModelFileHeader*>(modelBuffer->MutableData());
    (void)strcpy_s(modelName, MODEL_NAME_LENGTH, reinterpret_cast<char*>(header->name));
    std::string defaultName = std::string("default_") + modelName;
    auto builtModelImpl = new (std::nothrow) DirectBuiltModelImpl(modelBuffer, defaultName);
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, nullptr);

    return reinterpret_cast<HIAI_MR_BuiltModel*>(builtModelImpl);
}

HIAI_MR_BuiltModel* BuiltModel_Restore(std::shared_ptr<BaseBuffer>& buffer)
{
    if (buffer == nullptr || buffer->MutableData() == nullptr || buffer->GetSize() == 0) {
        FMK_LOGE("input is invalid.");
        return nullptr;
    }
    ModelType modelType = IR_API_GRAPH_MODEL;
    if (ModelTypeUtil::GetModelType(buffer->GetData(), buffer->GetSize(), modelType) == ge::FAIL
        || modelType == IR_API_GRAPH_MODEL) {
        FMK_LOGE("This model isn't supported.");
        return nullptr;
    }

    std::shared_ptr<BaseBuffer> output = DirectCommonUtil::MakeCompatibleBuffer(buffer);
    HIAI_EXPECT_NOT_NULL_R(output, nullptr);

    return CreateBuiltModelImpl(output);
}
}

HIAI_MR_BuiltModel* HIAI_DIRECT_BuiltModel_Restore(const void* data, size_t size)
{
    auto buffer = static_cast<uint8_t*>(const_cast<void*>(data));
    std::shared_ptr<BaseBuffer> baseBuffer = make_shared_nothrow<BaseBuffer>(buffer, size);
    return BuiltModel_Restore(baseBuffer);
}

HIAI_MR_BuiltModel* HIAI_DIRECT_BuiltModel_RestoreFromFile(const char* file)
{
    HIAI_EXPECT_NOT_NULL_R(file, nullptr);

    ModelType modelType = IR_API_GRAPH_MODEL;
    if (ModelTypeUtil::GetModelTypeFromFile(file, modelType) != ge::SUCCESS) {
        FMK_LOGE("get model type failed");
        return nullptr;
    }
    if (modelType != hiai::HCS_PARTITION_MODEL) {
        std::shared_ptr<BaseBuffer> baseBuffer = FileUtil::LoadToBuffer(file);
        return BuiltModel_Restore(baseBuffer);
    }

    auto builtModelImpl = new (std::nothrow) DirectBuiltModelImpl(file);
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, nullptr);

    return reinterpret_cast<HIAI_MR_BuiltModel*>(builtModelImpl);
}

const char* HIAI_DIRECT_BuiltModel_GetName(const HIAI_MR_BuiltModel* model)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, nullptr);

    return builtModelImpl->GetModelName().data();
}

HIAI_Status HIAI_DIRECT_BuiltModel_SetName(const HIAI_MR_BuiltModel* model, const char* name)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);
    HIAI_EXPECT_NOT_NULL_R(name, HIAI_FAILURE);

    builtModelImpl->SetModelName(name);
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_BuiltModel_CheckCompatibility(
    const HIAI_MR_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, HIAI_FAILURE);

    bool isModelCompatibility = false;
    if (builtModelImpl->CheckCompatibility(isModelCompatibility) != HIAI_SUCCESS) {
        FMK_LOGE("CheckCompatibility fail.");
        return HIAI_FAILURE;
    }

    *compatibility = (isModelCompatibility ? HIAI_BUILTMODEL_COMPATIBLE : HIAI_BUILTMODEL_INCOMPATIBLE);
    return HIAI_SUCCESS;
}

namespace {
int32_t HIAI_DIRECT_BuiltModel_GetIOTensorNum(const HIAI_MR_BuiltModel* model, bool isInput)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, 0);

    return builtModelImpl->GetModelIOTensorNum(isInput);
}

HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetIOTensorDesc(
    const HIAI_MR_BuiltModel* model, size_t index, bool isInput)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_MR_BuiltModel*>(model));
    HIAI_EXPECT_NOT_NULL_R(builtModelImpl, nullptr);

    return builtModelImpl->GetModelIOTensorDesc(index, isInput);
}
}

int32_t HIAI_DIRECT_BuiltModel_GetInputTensorNum(const HIAI_MR_BuiltModel* model)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorNum(model, true);
}

HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetInputTensorDesc(const HIAI_MR_BuiltModel* model, size_t index)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorDesc(model, index, true);
}

int32_t HIAI_DIRECT_BuiltModel_GetOutputTensorNum(const HIAI_MR_BuiltModel* model)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorNum(model, false);
}

HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(const HIAI_MR_BuiltModel* model, size_t index)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorDesc(model, index, false);
}

void HIAI_DIRECT_BuiltModel_Destroy(HIAI_MR_BuiltModel** model)
{
    HIAI_EXPECT_NOT_NULL_VOID(model);
    HIAI_EXPECT_NOT_NULL_VOID(*model);

    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(*model);
    delete builtModelImpl;
    builtModelImpl = nullptr;
    *model = nullptr;
}
} // namespace hiai