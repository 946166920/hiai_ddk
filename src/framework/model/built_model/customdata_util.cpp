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
#include "customdata_util.h"

#include "securec.h"
#include "framework/infra/log/log.h"
#include "framework/c/hiai_built_model_aipp.h"
#include "framework/util/base_buffer.h"
#include "util/file_util.h"
// src/framework/inc
#include "infra/base/assertion.h"

namespace hiai {
void CustomDataUtil::CopyDataToBuffer(
    std::shared_ptr<hiai::IBuffer>& outBuffer, size_t& offset, const void* data, size_t size)
{
    HIAI_EXPECT_NOT_NULL_VOID(data);
    if (memcpy_s(reinterpret_cast<void*>(reinterpret_cast<char*>(outBuffer->GetData()) + offset),
        outBuffer->GetSize() - offset, data, size) != 0) {
        FMK_LOGE("memcpy data failed.");
        return;
    }
    offset += size;
}

Status CustomDataUtil::CopyCustomDataToBuffer(
    std::shared_ptr<hiai::IBuffer>& buffer, size_t& offset, const CustomModelData& customModelData)
{
    size_t customDataSize = strlen(CUST_DATA_TAG) + sizeof(int32_t) + sizeof(int32_t) + customModelData.type.size() +
        sizeof(int32_t) + customModelData.value.size();
    HIAI_EXPECT_TRUE(buffer->GetSize() > customDataSize);

    (void)CopyDataToBuffer(buffer, offset, CUST_DATA_TAG, strlen(CUST_DATA_TAG));

    auto customDataLen = customModelData.type.size() + customModelData.value.size();
    (void)CopyDataToBuffer(buffer, offset, &customDataLen, sizeof(int32_t));

    auto customDataTypeLen = customModelData.type.size();
    (void)CopyDataToBuffer(buffer, offset, &customDataTypeLen, sizeof(int32_t));

    (void)CopyDataToBuffer(
        buffer, offset, reinterpret_cast<const void*>(customModelData.type.data()), customModelData.type.size());

    auto customDataValueLen = customModelData.value.size();
    (void)CopyDataToBuffer(buffer, offset, &customDataValueLen, sizeof(int));

    (void)CopyDataToBuffer(
        buffer, offset, reinterpret_cast<const void*>(customModelData.value.data()), customModelData.value.size());

    return SUCCESS;
}

Status CustomDataUtil::WriteCustomDataToFile(const char* file, const CustomModelData& customModelData)
{
    if (customModelData.type.empty()) {
        return SUCCESS;
    }
    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(CUST_DATA_TAG, strlen(CUST_DATA_TAG), file));

    auto customDataLen = customModelData.type.size() + customModelData.value.size();
    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(&customDataLen, sizeof(int32_t), file));

    auto customDataTypeLen = customModelData.type.size();
    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(&customDataTypeLen, sizeof(int32_t), file));

    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(
        reinterpret_cast<const void*>(customModelData.type.data()), customModelData.type.size(), file));

    auto customDataValueLen = customModelData.value.size();
    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(&customDataValueLen, sizeof(int), file));

    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(
        reinterpret_cast<const void*>(customModelData.value.data()), customModelData.value.size(), file));

    return SUCCESS;
}

std::shared_ptr<hiai::IBuffer> CustomDataUtil::SaveCustomDataToBuffer(
    void* data, size_t size, const CustomModelData& customModelData)
{
    HIAI_EXPECT_NOT_NULL_R(data, nullptr);
    if (customModelData.type.empty()) {
        return CreateLocalBuffer(data, size, true);
    }

    size_t totalOutputLength = strlen(CUST_DATA_TAG) + sizeof(int32_t) + sizeof(int32_t) + customModelData.type.size() +
        sizeof(int32_t) + customModelData.value.size() + size;
    std::shared_ptr<hiai::IBuffer> outBuffer = hiai::CreateLocalBuffer(totalOutputLength);
    HIAI_EXPECT_NOT_NULL_R(outBuffer, nullptr);

    size_t offset = 0;
    if (CopyCustomDataToBuffer(outBuffer, offset, customModelData) != SUCCESS) {
        FMK_LOGE("CopyCustomDataToBuffer failed");
        return nullptr;
    }
    (void)CopyDataToBuffer(outBuffer, offset, data, size);
    delete[] static_cast<uint8_t*>(data);

    return outBuffer;
}

bool CheckOffsetValid(const std::shared_ptr<IBuffer>& buffer, size_t offset)
{
    return offset <= buffer->GetSize();
}

std::shared_ptr<IBuffer> SplitCustomData(const std::shared_ptr<IBuffer>& buffer, CustomModelData& customModelData)
{
    size_t modelDataOffset = strlen(CUST_DATA_TAG) + sizeof(int32_t);

    int customDataTypeLen;
    HIAI_EXPECT_TRUE_R(CheckOffsetValid(buffer, modelDataOffset + sizeof(int32_t)), nullptr);

    if (memcpy_s(&customDataTypeLen, sizeof(int32_t),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + modelDataOffset), sizeof(int32_t)) != 0) {
        FMK_LOGE("memcpy data failed.");
        return nullptr;
    }

    modelDataOffset += sizeof(int32_t);

    HIAI_EXPECT_TRUE_R(customDataTypeLen > 0, nullptr);
    HIAI_EXPECT_TRUE_R(CheckOffsetValid(buffer, modelDataOffset + customDataTypeLen), nullptr);

    std::string type((reinterpret_cast<char*>(buffer->GetData()) + modelDataOffset), customDataTypeLen);
    customModelData.type = type;

    modelDataOffset += static_cast<size_t>(customDataTypeLen);

    int customDataValueLen;
    if (!CheckOffsetValid(buffer, modelDataOffset + sizeof(int32_t))) {
        return nullptr;
    }
    if (memcpy_s(&customDataValueLen, sizeof(int32_t),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + modelDataOffset), sizeof(int32_t)) != 0) {
        FMK_LOGE("memcpy data failed.");
        return nullptr;
    }
    modelDataOffset += sizeof(int32_t);

    HIAI_EXPECT_TRUE_R(customDataValueLen > 0, nullptr);
    HIAI_EXPECT_TRUE_R(CheckOffsetValid(buffer, modelDataOffset + static_cast<size_t>(customDataValueLen)), nullptr);

    std::string value((reinterpret_cast<char*>(buffer->GetData()) + modelDataOffset), customDataValueLen);
    customModelData.value = value;

    modelDataOffset += static_cast<size_t>(customDataValueLen);

    size_t modelDataSize = buffer->GetSize() - modelDataOffset;
    HIAI_EXPECT_TRUE_R(modelDataSize > 0, nullptr);

    std::shared_ptr<IBuffer> outBuffer = CreateLocalBuffer(modelDataSize);
    HIAI_EXPECT_NOT_NULL_R(outBuffer, nullptr);

    if (memcpy_s(outBuffer->GetData(), outBuffer->GetSize(),
        reinterpret_cast<void*>(reinterpret_cast<char*>(buffer->GetData()) + modelDataOffset), modelDataSize) != 0) {
        FMK_LOGE("memcpy data failed.");
        return nullptr;
    }
    return outBuffer;
}

std::shared_ptr<IBuffer> CustomDataUtil::GetModelData(
    const std::shared_ptr<IBuffer>& buffer, CustomModelData& customModelData)
{
    HIAI_EXPECT_TRUE_R(buffer->GetSize() > strlen(CUST_DATA_TAG), nullptr);

    if (strncmp(static_cast<const char*>(buffer->GetData()), static_cast<const char*>(CUST_DATA_TAG),
        strlen(CUST_DATA_TAG)) == 0) {
        return SplitCustomData(buffer, customModelData);
    }

    return buffer;
}

bool CustomDataUtil::HasCustomData(const char* file)
{
    std::shared_ptr<BaseBuffer> buffer = FileUtil::LoadToBuffer(file, strlen(CUST_DATA_TAG));
    HIAI_EXPECT_NOT_NULL_R(buffer, false);
    HIAI_EXPECT_TRUE(buffer->GetSize() == strlen(CUST_DATA_TAG));

    if (strncmp(reinterpret_cast<const char*>(buffer->MutableData()), static_cast<const char*>(CUST_DATA_TAG),
        strlen(CUST_DATA_TAG)) != 0) {
        return false;
    }

    return true;
}
} // namespace hiai
