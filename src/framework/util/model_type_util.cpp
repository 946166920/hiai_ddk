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
#include "framework/util/model_type_util.h"
#include <fstream>
#include <string>
#include "framework/common/types.h"
#include "framework/infra/log/log.h"
#include "common/file_util.h"
#include "securec.h"

using namespace std;
using namespace hiai;
using namespace ge;

namespace hiai {
ge::Status ModelTypeUtil::GetModelType(const void* data, size_t size, ModelType& type)
{
    if (data == nullptr || size == 0) {
        FMK_LOGE("data or size is invalid. size:%zu", size);
        return FAIL;
    }

#if defined(AI_SUPPORT_SPECIAL_3RD_MODEL)
    bool isSpecial = false;
    if (IsSpecial3rdModel(data, size, isSpecial) == ge::SUCCESS && isSpecial) {
        type = ModelType::SPECIAL_3RD_MODEL;
        return ge::SUCCESS;
    }
#endif

    if (size < sizeof(ModelFileHeader)) {
        FMK_LOGI("Input model size:%zu", size);
        type = IR_API_GRAPH_MODEL;
        return ge::SUCCESS;
    }

#ifdef PLATFORM_FPGA
    const int MODEL_SIZE_LIMIT = 209715200; // 200M: 209715200 = 200 * 1024 * 1024
    if (size > MODEL_SIZE_LIMIT) {
        FMK_LOGE("Load model size:%zu is big for FPGA platform", size);
        return ge::FAIL;
    }
#endif

    const ModelFileHeader* modelHeader = reinterpret_cast<const ModelFileHeader*>(data);
    type = static_cast<ModelType>(modelHeader->modeltype);
#ifndef __AICP_TA__
    if (modelHeader->magic != MODEL_FILE_MAGIC_NUM) {
        type = IR_API_GRAPH_MODEL;
    }
#endif
    FMK_LOGI("The model type is %d", type);
    return ge::SUCCESS;
}

#ifndef __AICP_TA__
namespace {
int ReadModelHeader(const char* filePath, uint8_t*& addr)
{
    std::string realPath = RealPath(filePath);
    if (realPath.empty()) {
        return -1;
    }
    std::ifstream fs(realPath.c_str(), std::ifstream::binary);
    if (!fs.is_open()) {
        return -1;
    }
    // get length of file:
    fs.seekg(0, fs.end);
    int64_t len = fs.tellg();
    if (len <= 0) {
        FMK_LOGE("len is invalids");
        fs.close();
        return -1;
    }

    if (static_cast<uint64_t>(len) <= sizeof(ModelFileHeader)) {
        fs.close();
        FMK_LOGE("file length invalid:%jd.", len);
        return -1;
    }

    fs.seekg(0, fs.beg);
    auto* data = new (std::nothrow) uint8_t[sizeof(ModelFileHeader)];
    if (data == nullptr) {
        fs.close();
        FMK_LOGE("create file data fail");
        return -1;
    }
    // read data as a block:
    fs.read(reinterpret_cast<char*>(data), sizeof(ModelFileHeader));
    fs.close();
    // 设置model_data参数
    addr = data;

    return 0;
}
} // namespace

ge::Status ModelTypeUtil::GetModelTypeFromFile(const std::string& modelPath, ModelType& type)
{
    // 解析文件
    uint8_t* data = nullptr;
    if (ReadModelHeader(modelPath.c_str(), data) != 0) {
        FMK_LOGE("Read file failed");
        return FAIL;
    }

    if (ModelTypeUtil::GetModelType(data, sizeof(ModelFileHeader), type) != ge::SUCCESS) {
        FMK_LOGE("GetModelType failed");
        delete[] data;
        return FAIL;
    }
    delete[] data;
    return ge::SUCCESS;
}

#if defined(AI_SUPPORT_SPECIAL_3RD_MODEL)
ge::Status ModelTypeUtil::IsSpecial3rdModelFromFile(const std::string& modelPath, bool& isSpecial)
{
    // 解析文件
    uint8_t* data = nullptr;
    if (ReadModelHeader(modelPath.c_str(), data) != 0) {
        FMK_LOGE("Read file failed");
        return FAIL;
    }

    if (IsSpecial3rdModel(data, sizeof(ModelFileHeader), isSpecial) != ge::SUCCESS) {
        FMK_LOGE("GetModelType failed");
        delete[] data;
        return FAIL;
    }
    delete[] data;
    return ge::SUCCESS;
}

#if defined(AI_SUPPORT_3RD_PLATFORM_IR_MODEL_INFERENCE) || defined(AI_SUPPORT_3RD_PLATFORM_IR_API_ONLINE_BUILD)
Status ModelTypeUtil::IsSpecial3rdModel(const void* data, size_t size, bool& isSpecial)
{
    isSpecial = false;
    static const size_t modelFileHeadLength = sizeof(ModelFileHeader);
    if (size < modelFileHeadLength) {
        FMK_LOGE("model is invalid, size is %zu.", size);
        return FAIL;
    }
    const vector<vector<uint8_t>> special3rdFlagMap = {
        {0x1A, 0xFD, 0x4A, 0x31}, // 2 core model head
        {0xE0, 0xF3, 0x02, 0x00} // 1 core model head
    };
    static const std::size_t special3rdFlagLength = 4;
    for (auto it : special3rdFlagMap) {
        if (memcmp((const void*)(&it[0]), data, special3rdFlagLength) == 0) {
            isSpecial = true;
            FMK_LOGI("IsSpecial3rdModel, model is 3rd.");
            return ge::SUCCESS;
        }
    }

    auto modelHead = new (std::nothrow) char[modelFileHeadLength];
    if (modelHead == nullptr) {
        FMK_LOGE("modelHead is null.");
        return FAIL;
    }
    if (memcpy_s(modelHead, modelFileHeadLength, buff.data(), modelFileHeadLength) != EOK) {
        FMK_LOGE("buffer memset failed.");
        delete[] modelHead;
        return FAIL;
    }
    auto flag = strstr(modelHead, "cambricon_offline");
    if (flag == nullptr) {
        flag = strstr(modelHead, "MEF");
    }
    if (flag != nullptr) {
        isSpecial = true;
        FMK_LOGI("IsSpecial3rdModel, model is 3rd.");
    }
    delete[] modelHead;
    return ge::SUCCESS;
}
#else
Status ModelTypeUtil::IsSpecial3rdModel(const void* data, size_t size, bool& isSpecial)
{
    isSpecial = false;
    const std::size_t special3rdFlagLength = 4;
    if (size < special3rdFlagLength) {
        FMK_LOGE("model is invalid, size is %zu.", size);
        return ge::FAIL;
    }
    const vector<vector<uint8_t>> special3rdFlagMap = {
        {0x1A, 0xFD, 0x4A, 0x31}, // 2 core model head
        {0xE0, 0xF3, 0x02, 0x00} // 1 core model head
    };
    for (auto it : special3rdFlagMap) {
        if (memcmp(&it[0], data, special3rdFlagLength) == 0) {
            isSpecial = true;
        }
    }
    return ge::SUCCESS;
}
#endif
#endif
#endif
} // namespace hiai
