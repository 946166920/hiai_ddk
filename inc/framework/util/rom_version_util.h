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
#ifndef ROM_VERSION_H
#define ROM_VERSION_H
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static inline bool MatchVersion(const char versionList[][16], size_t versionCount, const char* version, size_t matchLen)
{
    for (size_t i = 0; i < versionCount; i++) {
        if (strncmp(version, versionList[i], matchLen) == 0) {
            return true;
        }
    }
    return false;
}

static inline bool Is3rdRomVersion(const char* version)
{
    if (version == NULL) {
        return false;
    }
    const char is3rdRomVersion[][16] = {
        "100.100.010.010",
        "100.151.010.010",
        "100.211.010.010",
    };
    // only compare the first 8 characters.
    return MatchVersion(
        is3rdRomVersion, sizeof(is3rdRomVersion) / sizeof(is3rdRomVersion[0]), version, strlen("100.100."));
}

static inline bool Is3rdOldRomVersion(const char* version)
{
    if (version == NULL) {
        return false;
    }
    const char is3rdOldRomVersion[][16] = {
        "100.150.010.010",
        "100.210.010.010",
        "100.211.010.010",
    };

    return MatchVersion(
        is3rdOldRomVersion, sizeof(is3rdOldRomVersion) / sizeof(is3rdOldRomVersion[0]), version, strlen(version));
}
#endif
