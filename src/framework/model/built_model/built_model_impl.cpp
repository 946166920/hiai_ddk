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
#include "built_model_impl.h"
// inc
#include "infra/base/securestl.h"
#include "framework/c/hiai_nd_tensor_desc.h"
// src/framework/inc
#include "infra/base/assertion.h"
#include "util/file_util.h"
#include "framework/infra/log/log.h"
// src/framework
#include "model/built_model/customdata_util.h"
#ifdef AI_SUPPORT_AIPP_API
#include "model/aipp/aipp_input_converter.h"
#include "framework/c/hiai_built_model_aipp.h"
#include "framework/c/hiai_tensor_aipp_para.h"
#endif
#include "framework/c/hiai_built_model.h"

namespace hiai {

BuiltModelImpl::BuiltModelImpl(std::shared_ptr<HIAI_BuiltModel> builtModel) : builtModelImpl_(std::move(builtModel))
{
}

std::shared_ptr<HIAI_BuiltModel> BuiltModelImpl::GetBuiltModelImpl()
{
    return builtModelImpl_;
}

Status BuiltModelImpl::SaveToExternalBuffer(std::shared_ptr<IBuffer>& buffer, size_t& realSize) const
{
#ifdef AI_SUPPORT_BUILT_MODEL_SAVE
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);
    const size_t MAX_External_Buffer_SIZE = 2147483648UL; // Max size of 2GB.
    HIAI_EXPECT_NOT_NULL(buffer);
    HIAI_EXPECT_NOT_NULL(buffer->GetData());
    HIAI_EXPECT_TRUE(buffer->GetSize() < MAX_External_Buffer_SIZE);

    size_t offset = 0;
    if (!customModelData_.type.empty()) {
        HIAI_EXPECT_EXEC(CustomDataUtil::CopyCustomDataToBuffer(buffer, offset, customModelData_));
    }

    if (HIAI_BuiltModel_SaveToExternalBuffer(builtModelImpl_.get(),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + offset), buffer->GetSize() - offset,
        &realSize) == HIAI_SUCCESS) {
        realSize += offset;
        return SUCCESS;
    }
    return FAILURE;
#else
    (void)buffer;
    (void)realSize;
    FMK_LOGE("Not supported.");
    return FAILURE;
#endif
}

Status BuiltModelImpl::SaveToBuffer(std::shared_ptr<IBuffer>& buffer) const
{
#ifdef AI_SUPPORT_BUILT_MODEL_SAVE
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);
    void* data = nullptr;
    size_t size = 0;
    HIAI_EXPECT_EXEC(HIAI_BuiltModel_Save(builtModelImpl_.get(), &data, &size));
    buffer = CustomDataUtil::SaveCustomDataToBuffer(data, size, customModelData_);
    HIAI_EXPECT_NOT_NULL(buffer);

    return SUCCESS;
#else
    FMK_LOGE("Not supported.");
    return FAILURE;
#endif
}

Status BuiltModelImpl::SaveToFile(const char* file) const
{
#ifdef AI_SUPPORT_BUILT_MODEL_SAVE
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);
    HIAI_EXPECT_NOT_NULL(file);
    HIAI_EXPECT_EXEC(FileUtil::CreateEmptyFile(file));
    HIAI_EXPECT_EXEC(CustomDataUtil::WriteCustomDataToFile(file, customModelData_));
    if (HIAI_BuiltModel_SaveToFile(builtModelImpl_.get(), file) != HIAI_SUCCESS) {
        FMK_LOGE("save to file failed.");
        return FAILURE;
    }

    return SUCCESS;
#else
    FMK_LOGE("Not supported.");
    return FAILURE;
#endif
}

Status BuiltModelImpl::RestoreFromBuffer(const std::shared_ptr<IBuffer>& buffer)
{
    HIAI_EXPECT_NOT_NULL(buffer);
    HIAI_EXPECT_NOT_NULL(buffer->GetData());
    HIAI_EXPECT_TRUE(buffer->GetSize() > 0);
    HIAI_EXPECT_TRUE(builtModelImpl_ == nullptr);

    const std::shared_ptr<IBuffer> outBuffer = CustomDataUtil::GetModelData(buffer, customModelData_);
    HIAI_EXPECT_NOT_NULL(outBuffer);

    builtModelImpl_.reset(HIAI_BuiltModel_Restore(outBuffer->GetData(), outBuffer->GetSize()),
        [](HIAI_BuiltModel* p) { HIAI_BuiltModel_Destroy(&p); });
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);

    return SUCCESS;
}

Status BuiltModelImpl::RestoreFromFile(const char* file)
{
    HIAI_EXPECT_NOT_NULL(file);
    HIAI_EXPECT_TRUE(builtModelImpl_ == nullptr);

    if (CustomDataUtil::HasCustomData(file)) {
        buffer_ = FileUtil::LoadToBuffer(file);
        HIAI_EXPECT_NOT_NULL(buffer_);

        std::shared_ptr<hiai::IBuffer> buffer =
            CreateLocalBuffer(static_cast<void*>(buffer_->MutableData()), buffer_->GetSize(), false);

        return RestoreFromBuffer(buffer);
    }

    builtModelImpl_.reset(HIAI_BuiltModel_RestoreFromFile(file),
        [](HIAI_BuiltModel* p) { HIAI_BuiltModel_Destroy(&p); });

    HIAI_EXPECT_NOT_NULL(builtModelImpl_);

    return SUCCESS;
}

Status BuiltModelImpl::CheckCompatibility(bool& compatible) const
{
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);

    compatible = false;
    HIAI_BuiltModel_Compatibility compatibleValue;
    HIAI_EXPECT_EXEC(HIAI_BuiltModel_CheckCompatibility(builtModelImpl_.get(), &compatibleValue));
    compatible = (compatibleValue == HIAI_BUILTMODEL_COMPATIBLE);

    return SUCCESS;
}

namespace {
NDTensorDesc ConvertToNDTensorDesc(const HIAI_NDTensorDesc* desc)
{
    NDTensorDesc tmpDesc;
    size_t dimNum = HIAI_NDTensorDesc_GetDimNum(desc);
    for (size_t i = 0; i < dimNum; i++) {
        tmpDesc.dims.emplace_back(HIAI_NDTensorDesc_GetDim(desc, i));
    }
    tmpDesc.dataType = static_cast<DataType>(HIAI_NDTensorDesc_GetDataType(desc));
    tmpDesc.format = static_cast<Format>(HIAI_NDTensorDesc_GetFormat(desc));
    return tmpDesc;
}

using GetTenorNumFunc = int32_t (*)(const HIAI_BuiltModel*);
using GetTensorDescFunc = HIAI_NDTensorDesc* (*)(const HIAI_BuiltModel*, size_t);

std::vector<NDTensorDesc> GetTensorDescs(
    HIAI_BuiltModel* builtModel, GetTenorNumFunc getTenorNumFunc, GetTensorDescFunc getTensorDescFunc)
{
    auto num = getTenorNumFunc(builtModel);
    if (num <= 0) {
        FMK_LOGE("getTenorNumFunc failed.");
        return std::vector<NDTensorDesc>();
    }
    std::vector<NDTensorDesc> tensorDescVec(num);
    for (size_t i = 0; i < static_cast<size_t>(num); i++) {
        HIAI_NDTensorDesc* tensorDesc = getTensorDescFunc(builtModel, i);
        if (tensorDesc == nullptr) {
            FMK_LOGE("get tensor[%zu] failed.", i);
            return std::vector<NDTensorDesc>();
        }
        tensorDescVec[i] = ConvertToNDTensorDesc(tensorDesc);
        HIAI_NDTensorDesc_Destroy(&tensorDesc);
    }

    return tensorDescVec;
}

} // namespace

std::vector<NDTensorDesc> BuiltModelImpl::GetInputTensorDescs() const
{
    if (builtModelImpl_ == nullptr) {
        FMK_LOGE("please restore or build first.");
        return std::vector<NDTensorDesc>();
    }

    std::vector<NDTensorDesc> inputTensorDescVec =
        GetTensorDescs(builtModelImpl_.get(), HIAI_BuiltModel_GetInputTensorNum, HIAI_BuiltModel_GetInputTensorDesc);

#ifdef AI_SUPPORT_AIPP_API
    AippInputConverter::ConvertInputTensorDesc(customModelData_, inputTensorDescVec);
#endif
    return inputTensorDescVec;
}

std::vector<NDTensorDesc> BuiltModelImpl::GetOutputTensorDescs() const
{
    if (builtModelImpl_ == nullptr) {
        FMK_LOGE("please restore or build first.");
        return std::vector<NDTensorDesc>();
    }

    return GetTensorDescs(
        builtModelImpl_.get(), HIAI_BuiltModel_GetOutputTensorNum, HIAI_BuiltModel_GetOutputTensorDesc);
}

std::string BuiltModelImpl::GetName() const
{
    if (builtModelImpl_ == nullptr) {
        FMK_LOGE("please restore or build first.");
        return "";
    }
    return HIAI_BuiltModel_GetName(builtModelImpl_.get());
}

void BuiltModelImpl::SetName(const std::string& name)
{
    if (builtModelImpl_ == nullptr) {
        FMK_LOGE("please restore or build first.");
        return;
    }

    HIAI_BuiltModel_SetName(builtModelImpl_.get(), name.data());
}

void BuiltModelImpl::SetCustomData(const CustomModelData& customModelData)
{
    customModelData_ = customModelData;
}
const CustomModelData& BuiltModelImpl::GetCustomData()
{
    return customModelData_;
}

#ifdef AI_SUPPORT_AIPP_API
Status BuiltModelImpl::GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);
    return HIAI_BuiltModel_GetTensorAippInfo(builtModelImpl_.get(), index, aippParaNum, batchCount);
}

Status BuiltModelImpl::GetTensorAippPara(int32_t index, std::vector<void*>& aippParas) const
{
    HIAI_EXPECT_NOT_NULL(builtModelImpl_);

    int32_t inputNum = HIAI_BuiltModel_GetInputTensorNum(builtModelImpl_.get());
    HIAI_EXPECT_TRUE(inputNum > 0);

    for (int32_t i = 0; i < inputNum; ++i) {
        if (i != index && index != -1) {
            continue; // skip not specified tensor
        }

        uint32_t aippParaNum = 0;
        uint32_t batchCount = 0;
        HIAI_EXPECT_EXEC(HIAI_BuiltModel_GetTensorAippInfo(builtModelImpl_.get(), i, &aippParaNum, &batchCount));
        if (aippParaNum == 0) {
            continue;
        }

        std::vector<HIAI_TensorAippPara*> tmpAippParaVec(aippParaNum);
        HIAI_EXPECT_EXEC(HIAI_BuiltModel_GetTensorAippPara(
            builtModelImpl_.get(), i, tmpAippParaVec.data(), aippParaNum, batchCount));

        for (const auto& tmpPara : tmpAippParaVec) {
            void* bufferPara = HIAI_TensorAippPara_GetRawBuffer(tmpPara);
            if (bufferPara == nullptr) {
                continue;
            }
            HIAI_TensorAippCommPara* commPara = reinterpret_cast<HIAI_TensorAippCommPara*>(bufferPara);
            commPara->batchNum = batchCount;
        }
        aippParas.insert(aippParas.cend(), tmpAippParaVec.cbegin(), tmpAippParaVec.cend());
    }
    return SUCCESS;
}
#endif

std::shared_ptr<IBuiltModel> CreateBuiltModel()
{
    return make_shared_nothrow<BuiltModelImpl>();
}
} // namespace hiai
