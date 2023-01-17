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
#include "MembufferUtil.h"

#include <vector>
#include <map>
#include <string>
#include <cstring>
#include <limits.h>

#include "framework/infra/log/log.h"

namespace hiai {

int32_t ReadFileFromPath(const std::string& path, FILE*& fp, uint32_t& fileLength)
{
    if (path.length() == 0) {
        FMK_LOGE("file path is null.");
        return -1;
    }

    char pathTemp[PATH_MAX + 1] = {0x00};
    if (path.length() > PATH_MAX || realpath(path.c_str(), pathTemp) == nullptr) {
        FMK_LOGE("path size is too long or realpath return nullptr.");
        return -1;
    }

    fp = fopen(pathTemp, "r+");
    if (fp == nullptr) {
        FMK_LOGE("open err: %s", path.c_str());
        return -1;
    }

    // get file length
    if (fseek(fp, 0, SEEK_END) != 0) {
        FMK_LOGE("fseek SEEK_END error.");
        fclose(fp);
        return -1;
    }

    long fileSize = ftell(fp);
    if (fileSize <= 0) {
        FMK_LOGE("ReadFileFromPath ERROR: fileLength is 0!");
        fclose(fp);
        return -1;
    }
    fileLength = static_cast<uint32_t>(fileSize);
    if (fseek(fp, 0, SEEK_SET)) {
        FMK_LOGE("fseek SEEK_SET error.");
        fclose(fp);
        return -1;
    }
    return 0;
}

MemBuffer* MembufferUtil::InputMemBufferCreate(void* data, uint32_t size)
{
    if (data == nullptr || size == 0) {
        FMK_LOGE("InputMemBufferCreate error: invalid parameters");
        return nullptr;
    }
    MemBuffer* membuffer = nullptr;

    membuffer = new (std::nothrow) MemBuffer();
    if (membuffer == nullptr) {
        FMK_LOGE("InputMemBufferCreate error: malloc MemBuffer failed");
        return nullptr;
    }
    membuffer->SetMemBufferData(reinterpret_cast<void*>(data));
    membuffer->SetMemBufferSize(size);
    membuffer->SetAppAllocFlag(true);
    membuffer->SetServerMem(nullptr);
    return membuffer;
}

MemBuffer* MembufferUtil::InputMemBufferCreate(const std::string& path)
{
    FILE* fp = nullptr;
    uint32_t fileLength = 0;
    void* data = nullptr;
    MemBuffer* membuffer = nullptr;
    uint32_t readSize = 0;

    if (ReadFileFromPath(path, fp, fileLength) != 0) {
        FMK_LOGE("ReadFileFromPath failed");
        return nullptr;
    }

    data = malloc(fileLength);
    if (data == nullptr) {
        FMK_LOGE("InputMemBufferCreate ERROR: malloc fail!");
        goto STATE1_ERR_BACK;
    }

    readSize = static_cast<uint32_t>(fread(data, 1, fileLength, fp));
    if (readSize != fileLength) {
        FMK_LOGE("InputMemBufferCreate ERROR: readSize(%u) != fileLength(%u)", readSize, fileLength);
        goto STATE2_ERR_BACK;
    }

    membuffer = new (std::nothrow) MemBuffer();
    if (membuffer == nullptr) {
        FMK_LOGE("InputMemBufferCreate from file error: malloc MemBuffer failed");
        goto STATE2_ERR_BACK;
    }
    membuffer->SetMemBufferData(reinterpret_cast<void*>(data));
    membuffer->SetMemBufferSize(readSize);
    membuffer->SetAppAllocFlag(false);
    membuffer->SetServerMem(nullptr);

    fclose(fp);
    FMK_LOGI("new InputMemBufferCreate from file success");
    return membuffer;

STATE2_ERR_BACK:
    free(data);
    data = nullptr;
    fclose(fp);
    return nullptr;
STATE1_ERR_BACK:
    fclose(fp);
    return nullptr;
}

MemBuffer* MembufferUtil::OutputMemBufferCreate(int32_t framework, const std::vector<MemBuffer*>& pinputMemBuffer)
{
    (void)framework;
    (void)pinputMemBuffer;
    MemBuffer* membuffer = nullptr;
    void* newData = nullptr;
    uint32_t size = 200 * 1024 * 1024; // 200 : 200MByte; 1024 : in byte;

    membuffer = new (std::nothrow) MemBuffer();
    if (membuffer == nullptr) {
        FMK_LOGE("InputMemBufferCreate error: malloc MemBuffer failed");
        return nullptr;
    }
    newData = malloc(size);
    if (newData == nullptr) {
        FMK_LOGE("InputMemBufferCreate error: malloc data buffer failed");
        goto STATE1_ERR_BACK;
    }

    membuffer->SetMemBufferData(reinterpret_cast<void*>(newData));
    membuffer->SetMemBufferSize(size);
    membuffer->SetAppAllocFlag(false);
    membuffer->SetServerMem(nullptr);

    return membuffer;

STATE1_ERR_BACK:
    delete membuffer;
    return nullptr;
}

void MembufferUtil::MemBufferDestroy(MemBuffer* membuf)
{
    if (membuf == nullptr) {
        return;
    }
    if (!membuf->GetAppAllocFlag()) {
        void* data = membuf->GetMemBufferData();
        if (data != nullptr) {
            free(data);
        }
    }
    delete membuf;
    membuf = nullptr;
}

AIStatus MembufferUtil::MemBufferExportFile(MemBuffer* membuf, uint32_t pbuildSize, const std::string& pbuildPath)
{
    if (membuf == nullptr || membuf->GetMemBufferData() == nullptr) {
        FMK_LOGE("HIAI_MemBuffer_export_file ERROR: membuf is nullptr");
        return AI_FAILED;
    }
    if (pbuildSize > membuf->GetMemBufferSize()) {
        FMK_LOGE("HIAI_MemBuffer_export_file ERROR: pbuildSize(%d) is greater than MemBuffer allocSize(%d)", pbuildSize,
            membuf->GetMemBufferSize());
        return AI_FAILED;
    }
    const char* cPath = pbuildPath.c_str();
    if (strlen(cPath) > PATH_MAX) {
        FMK_LOGE("HIAI_MemBuffer_export_file error: path size is too long.");
        return AI_FAILED;
    }

    FILE* fp = fopen(cPath, "wb");
    if (fp == nullptr) {
        FMK_LOGE("HIAI_MemBuffer_export_file ERROR: open %s fail", cPath);
        return AI_FAILED;
    }
    uint32_t count = pbuildSize;
    uint32_t writeSize = static_cast<uint32_t>(fwrite(membuf->GetMemBufferData(), 1, count, fp));
    if (writeSize != pbuildSize) {
        FMK_LOGE("HIAI_MemBuffer_export_file ERROR: writeSize(%d) != size(%d)", writeSize, pbuildSize);
        fclose(fp);
        return AI_FAILED;
    }
    fclose(fp);
    return AI_SUCCESS;
}

} // namespace hiai
