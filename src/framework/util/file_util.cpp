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
#include "util/file_util.h"

#include <climits>
#include <functional>

#include "infra/base/assertion.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"

namespace hiai {
static int32_t MAX_SUPPORTED_FILE_SIZE = 1 * 1024 * 1024 * 1024; // 1GB

FILE* FileUtil::OpenFile(const std::string& fileName, const std::string& mode)
{
    if (fileName.empty()) {
        FMK_LOGE("fileName is null.");
        return nullptr;
    }

    char pathTemp[PATH_MAX + 1] = {0x00};
    if (fileName.size() > PATH_MAX || realpath(fileName.c_str(), pathTemp) == nullptr) {
        FMK_LOGE("fileName is invalid.");
        return nullptr;
    }

    return fopen(pathTemp, mode.c_str());
}

void FileUtil::CloseFile(FILE* fp)
{
    if (fp == nullptr) {
        return;
    }
    fclose(fp);
}

long FileUtil::GetFileSize(FILE* fp)
{
    if (fp == nullptr) {
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        FMK_LOGE("fseek SEEK_END error.");
        return -1;
    }

    long fileSize = ftell(fp);
    if (fileSize <= 0) {
        FMK_LOGE("ftell failed.");
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        FMK_LOGW("fseek SEEK_SET error.");
    }
    return fileSize;
}

long FileUtil::GetFileSize(const std::string& fileName)
{
    FILE* fp = OpenFile(fileName, "r");
    long fileSize = GetFileSize(fp);
    CloseFile(fp);

    return fileSize;
}

static std::shared_ptr<BaseBuffer> LoadToBufferBySize(FILE* fp, size_t size)
{
    std::shared_ptr<BaseBuffer> buffer = make_shared_nothrow<BaseBuffer>();
    if (buffer == nullptr) {
        FMK_LOGE("make shared failed.");
        return nullptr;
    }

    uint8_t* data = new (std::nothrow) uint8_t[size];
    HIAI_EXPECT_TRUE_R(data != nullptr, nullptr);
    buffer->SetData(data, size, true);

    size_t readSize = fread(buffer->MutableData(), sizeof(uint8_t), size, fp);
    if (readSize != size) {
        FMK_LOGE("read failed.");
        return nullptr;
    }

    return buffer;
}

std::shared_ptr<BaseBuffer> FileUtil::LoadToBuffer(const std::string& fileName)
{
    std::unique_ptr<FILE, std::function<void(FILE*)>> fp(OpenFile(fileName, "r"), [](FILE* p) { fclose(p); });

    if (fp == nullptr) {
        FMK_LOGE("open file failed.");
        return nullptr;
    }
    long fileSize = GetFileSize(fp.get());
    if (fileSize <= 0 || fileSize > MAX_SUPPORTED_FILE_SIZE) {
        FMK_LOGE("unsupported file size[%ld].", fileSize);
        return nullptr;
    }

    return LoadToBufferBySize(fp.get(), static_cast<size_t>(fileSize));
}

std::shared_ptr<BaseBuffer> FileUtil::LoadToBuffer(const std::string& fileName, size_t size)
{
    std::unique_ptr<FILE, std::function<void(FILE*)>> fp(OpenFile(fileName, "r"), [](FILE* p) { fclose(p); });
    if (fp == nullptr) {
        FMK_LOGE("open file failed.");
        return nullptr;
    }
    long fileSize = GetFileSize(fp.get());
    if (fileSize <= 0 || static_cast<size_t>(fileSize) < size) {
        FMK_LOGE("insufficient size[%zu].", size);
        return nullptr;
    }

    return LoadToBufferBySize(fp.get(), size);
}

static Status CheckFilePath(const char* file, std::string& resolvedPathStr)
{
    if (file == nullptr) {
        FMK_LOGE("file point is null!");
        return FAILURE;
    }

    size_t lastSplitIndex = resolvedPathStr.find_last_of("/\\");
    std::string dirName = resolvedPathStr.substr(0, lastSplitIndex + 1);
    std::string outputFileName = resolvedPathStr.substr(lastSplitIndex + 1);
    char resolvedOutputPath[PATH_MAX + 1] = {0x00};
    if (dirName == "") {
        dirName += "./";
    }
    if (realpath(dirName.c_str(), resolvedOutputPath) == nullptr) {
        FMK_LOGE("invalid output file path");
        return FAILURE;
    }
    resolvedPathStr = std::string(resolvedOutputPath) + "/" + outputFileName;
    return SUCCESS;
}

Status FileUtil::CreateEmptyFile(const char* file)
{
    std::string resolvedPathStr = file;
    HIAI_EXPECT_EXEC(CheckFilePath(file, resolvedPathStr));
    FILE* fp = fopen(resolvedPathStr.c_str(), "wb");
    if (fp == nullptr) {
        FMK_LOGE("open model fail, because not found file path");
        return FAILURE;
    }

    fclose(fp);
    return SUCCESS;
}

Status FileUtil::WriteBufferToFile(const void* data, uint32_t size, const char* file)
{
    std::string resolvedPathStr = file;
    HIAI_EXPECT_EXEC(CheckFilePath(file, resolvedPathStr));
    FILE* fp = fopen(resolvedPathStr.c_str(), "ab");
    if (fp == nullptr) {
        FMK_LOGE("open model fail, because not found file path");
        return FAILURE;
    }

    uint32_t writeSize = static_cast<uint32_t>(fwrite(data, 1, size, fp));
    if (writeSize != size) {
        fclose(fp);
        FMK_LOGE("WriteBufferToFile ERROR: writeSize(%u) != size(%u)", writeSize, size);
        return FAILURE;
    }

    fclose(fp);
    return SUCCESS;
}
} // namespace hiai
