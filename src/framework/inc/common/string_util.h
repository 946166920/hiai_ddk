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
#ifndef DOMI_OMG_PARSER_STRING_UTIL_H_
#define DOMI_OMG_PARSER_STRING_UTIL_H_

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <functional>
#include <cctype>

namespace hiai {
class StringUtils {
public:
    static std::string& Ltrim(std::string& s)
    {
#if __cplusplus >= 201103L
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
#else
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
#endif
        return s;
    }

    // lint -esym(551,*)
    static std::string& Rtrim(std::string& s) // lint !e618
    {
#if __cplusplus >= 201103L
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int c) { return !std::isspace(c); }));
#else
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
#endif
        return s;
        // lint +esym(551,*)
    }

    /*  @ingroup domi_common
     *  @brief 去除字符串头尾的空格
     *  @link http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
     *  @param [in] str 待trim的字符串
     *  @return trim后的字符串
     */
    static std::string& Trim(std::string& s)
    {
        return Ltrim(Rtrim(s));
    }

    /*  @ingroup domi_common
     *  @brief 字符串分割
     *  @link http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
     *  @param [in] str 待分割的字符串
     *  @param [in] delim  分割字符
     *  @return 分割后的字符串数组
     */
    static std::vector<std::string> Split(const std::string& str, char delim)
    {
        std::vector<std::string> elems;

        if (str.empty()) {
            elems.emplace_back("");
            return elems;
        }

        std::stringstream ss(str);
        std::string item;

        while (getline(ss, item, delim)) {
            elems.push_back(item);
        }
        auto strSize = str.size();
        if (strSize > 0 && str[strSize - 1] == delim) {
            elems.emplace_back("");
        }

        return elems;
    }
    /*  @ingroup domi_common
     *  @brief 获取带后缀名的文件名
     *  @param [in] s 路径名
     *  @return 文件名
     */
    static std::string GetFileName(const std::string& s)
    {
        if (s.empty()) {
            return "";
        }
        std::vector<std::string> files = StringUtils::Split(s, '/');

        return files.empty() ? "" : files[files.size() - 1];
    }
    /* @ingroup domi_common
     *  @brief 全量替换
     *  @link
     *  @param [in] str 待替换的字符串
     *  @param [in] oldValue  替换前旧字符
     *  @param [in] newValue  替换为新字符
     *  @return 替换后的字符串
     */
    static std::string ReplaceAll(std::string str, const std::string& oldValue, const std::string& newValue)
    {
        for (std::string::size_type pos(0); pos != std::string::npos; pos += newValue.length()) {
            if ((pos = str.find(oldValue, pos)) != std::string::npos) {
                str.replace(pos, oldValue.length(), newValue);
            } else {
                break;
            }
        }
        return str;
    }

    /*  @ingroup domi_common
     *  @brief 判断字符串是否是以某字符串开头（前缀）
     *  @link
     *  @param [in] str 待比较的字符串
     *  @param [in] strX  前缀字符串
     *  @return 如果是前缀返回true，否则返回false
     */
    static bool StartWith(const std::string& str, const std::string strX)
    {
        return ((str.size() >= strX.size()) && (str.compare(0, strX.size(), strX) == 0));
    }
};
} // namespace hiai

#endif // DOMI_OMG_PARSER_STRING_UTIL_H_
