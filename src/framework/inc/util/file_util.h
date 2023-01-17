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
#ifndef FRAMEWORK_UTIL_FILE_UTIL_H
#define FRAMEWORK_UTIL_FILE_UTIL_H
#include <cstdio>
#include <memory>
#include <string>

#include "infra/base/base_buffer.h"
#include "util/util_api_export.h"
#include "base/error_types.h"

namespace hiai {

class HIAI_UTIL_API_EXPORT FileUtil {
public:
    static long GetFileSize(const std::string& fileName);
    static std::shared_ptr<BaseBuffer> LoadToBuffer(const std::string& fileName);
    static std::shared_ptr<BaseBuffer> LoadToBuffer(const std::string& fileName, size_t size);
    static Status WriteBufferToFile(const void* data, uint32_t size, const char* file);
    static Status CreateEmptyFile(const char* file);

private:
    static FILE* OpenFile(const std::string& fileName, const std::string& mode);
    static void CloseFile(FILE* fp);
    static long GetFileSize(FILE* fp);
};

} // namespace hiai

#endif
