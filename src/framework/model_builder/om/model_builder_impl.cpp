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
#include "model_builder_impl.h"

#include <climits>

#include "securec.h"
#include "infra/base/assertion.h"
#include "framework/infra/log/log_fmk_interface.h"
#include "model_build_options_util.h"
#include "model/built_model/built_model_impl.h"
#include "infra/base/securestl.h"

#include "framework/infra/log/log.h"
#include "util/file_util.h"
#include "framework/common/types.h"
#include "model/built_model/customdata_util.h"
#include "framework/util/model_type_util.h"
#include "framework/c/hiai_model_builder.h"

namespace hiai {

static Status BuildModel(const ModelBuildOptions& options, const std::string& modelName,
    const std::shared_ptr<IBuffer>& modelBuffer, std::shared_ptr<IBuiltModel>& builtModel)
{
    auto buildOptions = ModelBuildOptionsUtil::ConvertToCBuildOptions(options);
    HIAI_EXPECT_NOT_NULL(buildOptions);

    HIAI_BuiltModel* builtModelImpl = nullptr;
    auto ret = HIAI_ModelBuilder_Build(
        buildOptions, modelName.c_str(), modelBuffer->GetData(), modelBuffer->GetSize(), &builtModelImpl);
    HIAI_ModelBuildOptions_Destroy(&buildOptions);
    if (ret != HIAI_SUCCESS || builtModelImpl == nullptr) {
        FMK_LOGE("build model failed.");
        return FAILURE;
    }

    builtModel = make_shared_nothrow<BuiltModelImpl>(
        std::shared_ptr<HIAI_BuiltModel>(builtModelImpl, [](HIAI_BuiltModel* p) { HIAI_BuiltModel_Destroy(&p); }));
    if (builtModel != nullptr) {
        FMK_LOGI("build model success.");
        return SUCCESS;
    }

    return FAILURE;
}

#ifdef AI_SUPPORT_AIPP_API
static Status GetCustomData(
    CustomModelData& customModelData, const std::shared_ptr<IBuffer>& buffer, uint32_t customDataOffset)
{
    customDataOffset += strlen(CUST_DATA_TAG);
    customDataOffset += sizeof(int32_t);

    if (buffer->GetSize() - customDataOffset < sizeof(int32_t)) {
        FMK_LOGE("buffer size is not enough big");
        return FAILURE;
    }
    int customDataTypeLen;
    HIAI_EXPECT_TRUE(memcpy_s(&customDataTypeLen, sizeof(int32_t),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + customDataOffset),
        sizeof(int32_t)) == EOK);

    customDataOffset += sizeof(int32_t);

    std::string type((reinterpret_cast<char*>(buffer->GetData()) + customDataOffset), customDataTypeLen);
    customModelData.type = type;

    customDataOffset += static_cast<uint32_t>(customDataTypeLen);

    int customDataValueLen;
    HIAI_EXPECT_TRUE(memcpy_s(&customDataValueLen, sizeof(int32_t),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + customDataOffset),
        sizeof(int32_t)) == EOK);

    customDataOffset += sizeof(int32_t);

    std::string value((reinterpret_cast<char*>(buffer->GetData()) + customDataOffset), customDataValueLen);
    customModelData.value = value;

    return SUCCESS;
}

static Status MakeCompatibleBuffer(std::shared_ptr<IBuffer>& compatibleModelBuffer, CustomModelData& customData,
    const std::shared_ptr<IBuffer>& modelBuffer)
{
    // ir api 编译后的模型
    compatibleModelBuffer = CustomDataUtil::GetModelData(modelBuffer, customData);
    if (compatibleModelBuffer == nullptr) {
        FMK_LOGE("GetModelData failed.");
        return FAILURE;
    }
    if (compatibleModelBuffer != modelBuffer) {
        return SUCCESS;
    }

    ModelType modelType = IR_API_GRAPH_MODEL;
    ge::BaseBuffer baseBuffer(const_cast<void*>(modelBuffer->GetData()), modelBuffer->GetSize());
    if (ModelTypeUtil::GetModelType(baseBuffer, modelType) != ge::SUCCESS) {
        FMK_LOGE("GetModelType failed");
        return HIAI_FAILURE;
    }

    if (modelType == STANDARD_IR_GRAPH_MODEL || modelType == HCS_PARTITION_MODEL) {
        ModelFileHeader* fileHeader = static_cast<ModelFileHeader*>(modelBuffer->GetData());
        if (fileHeader->customDataLen > 0) { // omg生成的模型
            if (modelBuffer->GetSize() - fileHeader->customDataLen < 0) {
                return FAILURE;
            }
            uint32_t customDataOffset = static_cast<uint32_t>(modelBuffer->GetSize() - fileHeader->customDataLen);
            compatibleModelBuffer = modelBuffer;

            if (GetCustomData(customData, modelBuffer, customDataOffset) != SUCCESS) {
                return FAILURE;
            }
        }
    }
    return SUCCESS;
}
#endif

Status ModelBuiderImpl::Build(const ModelBuildOptions& options, const std::string& modelName,
    const std::shared_ptr<IBuffer>& modelBuffer, std::shared_ptr<IBuiltModel>& builtModel)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (modelBuffer == nullptr) {
        FMK_LOGE("modelBuffer is nullptr.");
        return INVALID_PARAM;
    }

    if (modelName.length() > PATH_MAX) {
        FMK_LOGE("modelName length is too long.");
        return INVALID_PARAM;
    }

#ifdef AI_SUPPORT_AIPP_API
    std::shared_ptr<IBuffer> compatibleModelBuffer = nullptr;
    CustomModelData customData;
    if (MakeCompatibleBuffer(compatibleModelBuffer, customData, modelBuffer) != SUCCESS) {
        return FAILURE;
    }

    if (compatibleModelBuffer != nullptr) {
        if (BuildModel(options, modelName, compatibleModelBuffer, builtModel) != SUCCESS) {
            return FAILURE;
        }
        if (builtModel != nullptr) {
            builtModel->SetCustomData(customData);
        }
        return SUCCESS;
    }
#endif
    return BuildModel(options, modelName, modelBuffer, builtModel);
}

Status ModelBuiderImpl::Build(const ModelBuildOptions& options, const std::string& modelName,
    const std::string& modelFile, std::shared_ptr<IBuiltModel>& builtModel)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    buffer_ = FileUtil::LoadToBuffer(modelFile);
    if (buffer_ == nullptr) {
        return FAILURE;
    }

    std::shared_ptr<IBuffer> localBuffer =
        CreateLocalBuffer(static_cast<void*>(buffer_->MutableData()), buffer_->GetSize(), false);

    return Build(options, modelName, localBuffer, builtModel);
}

std::shared_ptr<IModelBuilder> CreateModelBuilder()
{
    return make_shared_nothrow<ModelBuiderImpl>();
}

} // namespace hiai
