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

#include "common/file_util.h"
#include <fcntl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>
#include <cstdlib>
#include <climits>
#include <regex>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include "infra/base/assertion.h"
#include "framework/infra/log/log.h"
#include "framework/common/fmk_types.h"

using namespace std;

namespace hiai {
static const int MAX_FILE_SIZE_LIMIT =
    INT_MAX; // 输入文件的最大长度。基于安全编码规范和目前实际(protobuf)模型大小，定为2G。

// 获取文件长度
int GetFileLength(const std::string& inputFile)
{
    HIAI_EXPECT_TRUE_R(!(inputFile.empty()), -1);
    string realPath = RealPath(inputFile.c_str());

    HIAI_EXPECT_TRUE_R(!(realPath.empty()), -1);

    struct stat buf;
    if (stat(realPath.c_str(), &buf) != 0) {
        FMK_LOGE("stat file failed.");
        return -1;
    }
    if ((buf.st_mode & S_IFDIR) == S_IFDIR) {
        FMK_LOGE("%s is not file", inputFile.c_str());
        return -1;
    }
#ifdef _MSC_VER
    std::ifstream file;
    file.open(realPath.c_str(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        FMK_LOGE("open file failed.");
        return -1;
    }

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.close();

    HIAI_EXPECT_TRUE_R((size > 0), -1);

    HIAI_EXPECT_TRUE_R((size <= MAX_FILE_SIZE_LIMIT), -1);

    return size;
#else

    HIAI_EXPECT_TRUE_R((buf.st_size > 0), -1);

    HIAI_EXPECT_TRUE_R((buf.st_size <= MAX_FILE_SIZE_LIMIT), -1);

    return static_cast<int>(buf.st_size);
#endif
}

/**
 *  @ingroup domi_common
 *  @brief 创建目录，支持创建多级目录
 *  @param [in] directoryPath  路径，可以为多级目录
 *  @return -1 失败
 *  @return 0 成功
 */
FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY int CreateDir(const std::string& directoryPath)
{
    HIAI_EXPECT_TRUE_R(!(directoryPath.empty()), -1);
    uint32_t dirPathLen = directoryPath.length();
    if (dirPathLen >= DOMI_MAX_PATH_LEN) {
        FMK_LOGE("directory path is too long.");
        return -1;
    }
    char tmpDirPath[DOMI_MAX_PATH_LEN] = {0};
    for (uint32_t i = 0; i < dirPathLen; i++) {
        tmpDirPath[i] = directoryPath[i];
        if ((tmpDirPath[i] == '\\') || (tmpDirPath[i] == '/')) {
            if (access(tmpDirPath, F_OK) == 0) {
                continue;
            }
            int32_t ret = mkdir(tmpDirPath, S_IRUSR | S_IWUSR | S_IXUSR);
            if (ret != 0 && errno != EEXIST) {
                FMK_LOGE("Cannot create directory %s. Make sure that the directory exists and writable.",
                    directoryPath.c_str());
                return ret;
            }
        }
    }
    if ((access(directoryPath.c_str(), F_OK) != 0)) {
        int32_t ret = mkdir(directoryPath.c_str(), S_IRUSR | S_IWUSR | S_IXUSR); // 700
        if (ret != 0 && errno != EEXIST) {
            FMK_LOGE("Cannot create directory %s. Make sure that the directory exists and writable.",
                directoryPath.c_str());
            return ret;
        }
    }
    return 0;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY string RealPath(const char* path)
{
    HIAI_EXPECT_NOT_NULL_R(path, "");
    HIAI_EXPECT_TRUE_R((strlen(path) < PATH_MAX), "");
    // PATH_MAX使系统自带的宏，表示支持的最大文件路径长度
    char* resolvedPath = new (std::nothrow) char[PATH_MAX] {0};
    HIAI_EXPECT_NOT_NULL_R(resolvedPath, "");

    string res = "";

    // 路径不存在，或者没有权限时，会返回nullptr
    // 路径可访问时，返回绝对路径
    if (realpath(path, resolvedPath) != nullptr) {
        res = resolvedPath;
    }

    if (resolvedPath != nullptr) {
        delete[] resolvedPath;
    }
    return res;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY bool CheckInputPathValid(const string& filePath)
{
    // 指定路径为空
    if (filePath.empty()) {
        FMK_LOGE("path is empty.");
        return false;
    }

    // 校验输入文件路径是否合法的正则匹配表达式
    // ^(/|./|(../)+|)([.]?[A-Za-z0-9_-]+/)*[A-Za-z0-9_+.-]+$
    // 路径部分：支持大小写字母、数字，下划线
    // 文件名部分：支持大小写字母、数字，下划线和点(.)
#ifdef _MSC_VER
    //  confirm the path mode for windows
    string mode = "^(([a-zA-Z]\:\/\/)+|([a-zA-Z]\:\\\\\\\\)+|(\/)+|(.\/)+|(..\/+)+|(\\\\)+|(.\\\\)+|(..\\\\+)+|)"
                  "((..\/)|(..\\\\)|[.]?[a-zA-Z0-9_-]+(\/|\\\\)+)*([A-Za-z0-9_+.-])+$";
#else
    string mode = "^(/+|./+|(../+)+|)(../|([.]?[A-Za-z0-9_-]+)/+)*[A-Za-z0-9_+.-]+$";
#endif
    HIAI_EXPECT_TRUE_R((ValidateStr(filePath, mode)), false);

    string realPath = RealPath(filePath.c_str());
    // 无法获取绝对路径(不存在，或者无权限访问)
    if (realPath.empty()) {
        FMK_LOGE("can not get real path for %s.", filePath.c_str());
        return false;
    }

#ifdef _MSC_VER
    if (_access(realPath.c_str(), 0) != 0) {
        FMK_LOGE("can not get real path for %s.", filePath.c_str());
        return false;
    }
#else
    // 绝对路径指向的文件不可读
    if (access(realPath.c_str(), R_OK) != 0) {
        FMK_LOGE("can not read file in %s.", filePath.c_str());
        return false;
    }
#endif

    return true;
}

FMK_FUNC_HOST_VISIBILITY bool CheckOutputPathValid(const string& filePath)
{
    // 指定路径为空
    if (filePath.empty()) {
        FMK_LOGE("path is empty.");
        return false;
    }

    // 校验输入文件路径是否合法的正则匹配表达式
    // ^(/|./|(../)+|)([.]?[A-Za-z0-9_-]+/)*[A-Za-z0-9_+.-]+$
    // 路径部分：支持大小写字母、数字，下划线
    // 文件名部分：支持大小写字母、数字，下划线和点(.)
#ifdef _MSC_VER
    string mode = "^(([a-zA-Z]\:\/\/)+|([a-zA-Z]\:\\\\\\\\)+|(\/)+|(.\/)+|(..\/+)+|(\\\\)+|(.\\\\)+|(..\\\\+)+|)"
                  "((..\/)|(..\\\\)|[.]?[a-zA-Z0-9_-]+(\/|\\\\)+)*([A-Za-z0-9_+.-])+$";
#else
    string mode = "^(/+|./+|(../+)+|)(../|([.]?[A-Za-z0-9_-]+)/+)*[A-Za-z0-9_+.-]+$";
#endif
    HIAI_EXPECT_TRUE_R((ValidateStr(filePath, mode)), false);
    string realPath = RealPath(filePath.c_str());
    // 可以获取绝对路径(文件存在)
    if (!realPath.empty()) {
#if !defined _MSC_VER
        // 文件不可读写
        if (access(realPath.c_str(), R_OK | W_OK | F_OK) != 0) {
            FMK_LOGE("path[ %s ] exists, but can not be write.", filePath.c_str());
            return false;
        }
#endif
    } else { // 无法获取绝对路径。 1. 无权限 2. 文件路径不存在。通过创建路径的方式，判断目录是否有权访问
        // 寻找最后一个分割符
        int pathSplitPos = static_cast<int>(filePath.size()) - 1;
        for (; pathSplitPos >= 0; pathSplitPos--) {
            if (filePath[pathSplitPos] == '\\' || filePath[pathSplitPos] == '/') {
                break;
            }
        }
        if (pathSplitPos == 0) {
            return true;
        }
        if (pathSplitPos != -1) { // 可以找到路径分隔符
            string prefixPath = std::string(filePath).substr(0, pathSplitPos);
            // 通过创建路径的方式，判断指定路径是否有效
            if (CreateDir(prefixPath) != 0) { // 没有权限创建路径
                FMK_LOGE("can not create prefix path for path[ %s ].", filePath.c_str());
                return false;
            }
        }
    }

    return true;
}

FMK_FUNC_HOST_VISIBILITY bool ValidateStr(const std::string& filePath, const std::string& mode)
{
    regex reg(mode);

    // 匹配上的字符串部分
    smatch match;

    bool res = regex_match(filePath, match, reg);

#ifdef _MSC_VER
    res = !regex_search(filePath, regex("[`!@#$%^&*()|{}';',\\[\\]<>?]"));
#else
    res = !regex_search(filePath, regex("[`!@#$%^&*()|{}':;',\\[\\]<>?]"));
#endif

    return res && (filePath.size() == match.str().size());
}
FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY int ReadFile(const char* filePath, uint8_t*& addr, size_t& size)
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
        fs.close();
        return -1;
    }
    fs.seekg(0, fs.beg);
    auto* data = new (std::nothrow) uint8_t[len];
    if (data == nullptr) {
        fs.close();
        return -1;
    }
    // read data as a block:
    fs.read(reinterpret_cast<char*>(data), len);
    fs.close();
    // 设置model_data参数
    addr = data;
    size = static_cast<size_t>(len);

    return 0;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY int ReadFileOnly(const char* filePath, uint8_t*& addr, size_t& size)
{
    std::string realPath = RealPath(filePath);
    if (realPath.empty()) {
        return -1;
    }

    int fd = open(realPath.c_str(), O_RDONLY);
    if (fd == -1) {
        FMK_LOGE("open file[%s] fail", filePath);
        return -1;
    }
    int len = lseek(fd, 0, SEEK_END);
    if (len <= 0) {
        close(fd);
        return -1;
    }

    void* data = mmap(nullptr, len, PROT_READ, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        FMK_LOGE("mmap fail");
        close(fd);
        return -1;
    }
    close(fd);

    addr = (uint8_t*)data;
    size = static_cast<size_t>(len);

    return 0;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY void ReleaseFileMemory(uint8_t* addr, size_t size)
{
    (void)munmap(addr, size);
}
} // namespace hiai
