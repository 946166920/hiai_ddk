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
#ifndef DOMI_OMG_COMMON_FILE_UTIL_H_
#define DOMI_OMG_COMMON_FILE_UTIL_H_

#include <string>

namespace hiai {
const int32_t DOMI_MAX_PATH_LEN = 256;
/*
 * @ingroup: domi_common
 * @brief: 获取文件长度
 * @param [in] inputFile: 文件路径
 * @return long： 文件长度，如果获取文件长度失败，返回-1
 */
int GetFileLength(const std::string& inputFile);

/*  @ingroup domi_common
 *  @brief 递归创建目录
 *  @param [in] directoryPath  路径，可以为多级目录
 *  @return 0 成功
 *  @return -1 失败
 */
int CreateDir(const std::string& directoryPath);

/*
 *  @ingroup domi_common
 *  @brief 获取文件的绝对路径
 *  @param [in] 输入的文件路径
 *  @param [out] 文件的绝对路径。无法获取绝对路径时，返回空字符串
 */
std::string RealPath(const char* path);

/*
 *  @ingroup domi_common
 *  @brief 读取文件, 内存可读写, 通过delete[]释放内存
 *  @param [in] filePath 文件路径
 *  @param [out] buffer 和 size
 */
int ReadFile(const char* filePath, uint8_t*& addr, size_t& size);

/*
 *  @ingroup domi_common
 *  @brief 读取文件, 内存只读, 通过ReleaseFileMemory释放内存
 *  @param [in] filePath 文件路径
 *  @param [out] buffer 和 size
 */
int ReadFileOnly(const char* filePath, uint8_t*& addr, size_t& size);

/*
 *  @ingroup domi_common
 *  @brief 释放ReadFileOnly的文件内存
 *  @param [in] addr 文件内存地址
 *  @param [in] size 文件内存大小
 *  @param [out] void
 */
void ReleaseFileMemory(uint8_t* addr, size_t size);

/*
 *  @ingroup domi_common
 *  @brief 检查指定的输入文件路径是否合法
 *  1. 指定路径不为空
 *  2. 路径可以转换成绝对路径
 *  3. 文件路径存在且可读
 *  @param [in] filePath 输入的文件路径
 *  @param [out] 校验结果
 */
bool CheckInputPathValid(const std::string& filePath);

/*
 *  @ingroup domi_common
 *  @brief 检查指定的输出文件路径是否合法
 *  @param [in] filePath 输出的文件路径
 *  @param [out] 校验结果
 */
bool CheckOutputPathValid(const std::string& filePath);

/*
 *  @ingroup domi_common
 *  @brief 检查文件路径是否符合白名单校验
 *  @param [in] filePath 文件路径
 *  @param [out] 校验结果
 */
bool ValidateStr(const std::string& filePath, const std::string& mode);
} // namespace hiai
#endif // DOMI_OMG_COMMON_FILE_UTIL_H_
