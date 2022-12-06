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

#include "framework/infra/log/log.h"
#include "securec.h"
#include "infra/base/securestl.h"
#include "direct_built_model_impl.h"
#include "direct_common_util.h"
#include "util/file_util.h"
#include "framework/util/model_type_util.h"
#include "common/scope_guard.h"

namespace hiai {
static bool IsBufferValid(DirectBuiltModelImpl*& imp)
{
    if (imp->modelBuffer_ == nullptr || imp->modelBuffer_->MutableData() == nullptr ||
        imp->modelBuffer_->GetSize() == 0) {
        FMK_LOGE("please restore model first.");
        return false;
    }

    return true;
}

static void LoadToBuffer(DirectBuiltModelImpl*& imp)
{
    if (imp->modelBuffer_ == nullptr) {
        imp->modelBuffer_ = FileUtil::LoadToBuffer(imp->modelFile_);
    }
}

HIAI_Status HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(
    const HIAI_BuiltModel* model, void* data, size_t size, size_t* realSize)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return HIAI_FAILURE;
    }

    // restore from file scene
    LoadToBuffer(builtModelImpl);

    if (!IsBufferValid(builtModelImpl)) {
        return HIAI_FAILURE;
    }

    size_t bufferSize = builtModelImpl->modelBuffer_->GetSize();
    if (data == nullptr || size < bufferSize) {
        FMK_LOGE("data or size is invalid.");
        return HIAI_FAILURE;
    }

    if (memcpy_s(data, bufferSize, builtModelImpl->modelBuffer_->MutableData(), bufferSize) != EOK) {
        FMK_LOGE("SaveToExternalBuffer copy targets failed!");
        return HIAI_FAILURE;
    }

    *realSize = bufferSize;

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_BuiltModel_Save(const HIAI_BuiltModel* model, void** data, size_t* size)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl null.");
        return HIAI_FAILURE;
    }

    // restore from file scene
    LoadToBuffer(builtModelImpl);

    if (!IsBufferValid(builtModelImpl)) {
        return HIAI_FAILURE;
    }
    size_t modelBufferSize = builtModelImpl->modelBuffer_->GetSize();
    auto dstData = new (std::nothrow) uint8_t[modelBufferSize];
    if (dstData == nullptr) {
        FMK_LOGE("new dstData failed.");
        return HIAI_FAILURE;
    }

    void* srcData = builtModelImpl->modelBuffer_->MutableData();
    if (memcpy_s(dstData, modelBufferSize, srcData, modelBufferSize) != EOK) {
        FMK_LOGE("SaveToExternalBuffer copy targets failed!");
        delete[] dstData;
        dstData = nullptr;
        return HIAI_FAILURE;
    }

    *size = modelBufferSize;
    *data = dstData;

    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_BuiltModel_SaveToFile(const HIAI_BuiltModel* model, const char* file)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return HIAI_FAILURE;
    }

    if (file == nullptr) {
        FMK_LOGE("file is invalid.");
        return HIAI_FAILURE;
    }

    // restore from file scene
    LoadToBuffer(builtModelImpl);

    if (!IsBufferValid(builtModelImpl)) {
        return HIAI_FAILURE;
    }

    if (FileUtil::WriteBufferToFile(builtModelImpl->modelBuffer_->MutableData(),
        builtModelImpl->modelBuffer_->GetSize(), file) != HIAI_SUCCESS) {
        FMK_LOGE("WriteBufferToFile failed!");
        return HIAI_FAILURE;
    }

    return HIAI_SUCCESS;
}

static HIAI_BuiltModel* BuiltModel_Restore(std::shared_ptr<BaseBuffer>& buffer)
{
    if (buffer == nullptr || buffer->MutableData() == nullptr || buffer->GetSize() == 0) {
        FMK_LOGE("input is invalid.");
        return nullptr;
    }

    ModelType modelType = IR_API_GRAPH_MODEL;
    if (ModelTypeUtil::GetModelType(buffer->GetData(), buffer->GetSize(), modelType) ==
        ge::FAIL || modelType == IR_API_GRAPH_MODEL) {
        FMK_LOGE("This model isn't supported.");
        return nullptr;
    }

    ModelFileHeader* header = reinterpret_cast<ModelFileHeader*>(buffer->MutableData());
    char* modelName = reinterpret_cast<char*>(header->name);
    std::string defaultName = std::string("default_") + modelName;

    // 兼容性处理
    HIAI_MemBuffer memBuffer {static_cast<unsigned int>(buffer->GetSize()), static_cast<void*>(buffer->MutableData())};
    HIAI_MemBuffer* aferCompatibleBuffer = nullptr;
    bool isNeedRelease = false;
    if (DirectCommonUtil::MakeCompatibleModel(&memBuffer, &aferCompatibleBuffer, isNeedRelease) != HIAI_SUCCESS) {
        return nullptr;
    }

    std::shared_ptr<BaseBuffer> modelBuffer {nullptr};
    if (isNeedRelease) {
        modelBuffer = make_shared_nothrow<BaseBuffer>(
            static_cast<uint8_t*>(aferCompatibleBuffer->data), aferCompatibleBuffer->size, false);
        hiai::ScopeGuard guard([&aferCompatibleBuffer, &modelBuffer] {
            if (modelBuffer == nullptr) {
                free(aferCompatibleBuffer->data);
            }
            free(aferCompatibleBuffer);
        });
        if (modelBuffer == nullptr) {
            FMK_LOGE("modelBuffer is null.");
            return nullptr;
        }
    } else {
        modelBuffer = buffer;
    }

    auto builtModelImpl = new (std::nothrow) DirectBuiltModelImpl(modelBuffer, defaultName, isNeedRelease);
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return nullptr;
    }

    return reinterpret_cast<HIAI_BuiltModel*>(builtModelImpl);
}

HIAI_BuiltModel* HIAI_DIRECT_BuiltModel_Restore(const void* data, size_t size)
{
    auto buffer = static_cast<uint8_t*>(const_cast<void*>(data));
    std::shared_ptr<BaseBuffer> baseBuffer = make_shared_nothrow<BaseBuffer>(buffer, size);
    return BuiltModel_Restore(baseBuffer);
}

HIAI_BuiltModel* HIAI_DIRECT_BuiltModel_RestoreFromFile(const char* file)
{
    if (file == nullptr) {
        FMK_LOGE("file null.");
        return nullptr;
    }

    ModelType modelType = IR_API_GRAPH_MODEL;
    if (ModelTypeUtil::GetModelTypeFromFile(file, modelType) == ge::FAIL || modelType != hiai::HCS_PARTITION_MODEL) {
        std::shared_ptr<BaseBuffer> baseBuffer = FileUtil::LoadToBuffer(file);
        return BuiltModel_Restore(baseBuffer);
    }

    auto builtModelImpl = new (std::nothrow) DirectBuiltModelImpl(file);
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return nullptr;
    }

    return reinterpret_cast<HIAI_BuiltModel*>(builtModelImpl);
}

const char* HIAI_DIRECT_BuiltModel_GetName(const HIAI_BuiltModel* model)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return nullptr;
    }

    return builtModelImpl->modelName_.data();
}

HIAI_Status HIAI_DIRECT_BuiltModel_SetName(const HIAI_BuiltModel* model, const char* name)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return HIAI_FAILURE;
    }

    if (name == nullptr) {
        FMK_LOGE("name is invalid.");
        return HIAI_FAILURE;
    }

    builtModelImpl->modelName_ = name;
    return HIAI_SUCCESS;
}

HIAI_Status HIAI_DIRECT_BuiltModel_CheckCompatibility(
    const HIAI_BuiltModel* model, HIAI_BuiltModel_Compatibility* compatibility)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return HIAI_FAILURE;
    }

    bool isModelCompatibility = false;
    if (builtModelImpl->CheckCompatibility(isModelCompatibility) == HIAI_SUCCESS) {
        *compatibility = (isModelCompatibility ? HIAI_BUILTMODEL_COMPATIBLE : HIAI_BUILTMODEL_INCOMPATIBLE);
        return HIAI_SUCCESS;
    }

    FMK_LOGE("CheckCompatibility fail.");
    return HIAI_FAILURE;
}

static HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetIOTensorDesc(
    const HIAI_BuiltModel* model, size_t index, bool isInput)
{
    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return nullptr;
    }

    std::vector<HIAI_NDTensorDesc*> inputDescs;
    std::vector<HIAI_NDTensorDesc*> outputDescs;
    auto ret = builtModelImpl->GetModelIOTensorDescs(inputDescs, outputDescs);
    if (ret != SUCCESS) {
        FMK_LOGE("GetModelIOTensorDescs fail.");
        return nullptr;
    }

    if (isInput) {
        if (index >= inputDescs.size()) {
            FMK_LOGE("index is invalid.");
            return nullptr;
        }
        return HIAI_NDTensorDesc_Clone(inputDescs[index]);
    } else {
        if (index >= outputDescs.size()) {
            FMK_LOGE("index is invalid.");
            return nullptr;
        }
        return HIAI_NDTensorDesc_Clone(outputDescs[index]);
    }
}

int32_t HIAI_DIRECT_BuiltModel_GetInputTensorNum(const HIAI_BuiltModel* model)
{
    uint32_t inputCount = 0;
    uint32_t outputCount = 0;

    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return inputCount;
    }

    if (builtModelImpl->GetModelIOTensorNum(inputCount, outputCount) != HIAI_SUCCESS) {
        FMK_LOGE("GetModelIOTensorNum fail.");
    }

    return inputCount;
}

HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetInputTensorDesc(const HIAI_BuiltModel* model, size_t index)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorDesc(model, index, true);
}

int32_t HIAI_DIRECT_BuiltModel_GetOutputTensorNum(const HIAI_BuiltModel* model)
{
    uint32_t inputCount = 0;
    uint32_t outputCount = 0;

    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(const_cast<HIAI_BuiltModel*>(model));
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return outputCount;
    }

    if (builtModelImpl->GetModelIOTensorNum(inputCount, outputCount) != HIAI_SUCCESS) {
        FMK_LOGE("GetModelIOTensorNum fail.");
    }

    return outputCount;
}

HIAI_NDTensorDesc* HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(const HIAI_BuiltModel* model, size_t index)
{
    return HIAI_DIRECT_BuiltModel_GetIOTensorDesc(model, index, false);
}

void HIAI_DIRECT_BuiltModel_Destroy(HIAI_BuiltModel** model)
{
    if (model == nullptr || *model == nullptr) {
        FMK_LOGI("input is nullptr.");
        return;
    }

    auto builtModelImpl = reinterpret_cast<DirectBuiltModelImpl*>(*model);
    if (builtModelImpl == nullptr) {
        FMK_LOGE("builtModelImpl is null.");
        return;
    }

    delete builtModelImpl;
    builtModelImpl = nullptr;
    *model = nullptr;
}
} // namespace hiai