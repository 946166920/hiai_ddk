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

#ifndef _INC_INFRA_LOG_INTERFACE_H_
#define _INC_INFRA_LOG_INTERFACE_H_

#ifndef BUILD_VERSION_ENG
#define H_LOG_INTERFACE(...)
#define H_LOG_INTERFACE_FILTER(...)
#else
#define H_LOG_INTERFACE() \
    AI_Log_Print(AI_LOG_INFO, "interface_test:", "%s|%s|%s|%d", "hiai", ITF_COMPONENT_NAME, __FUNCTION__, __LINE__)
#define H_LOG_INTERFACE_FILTER(N) \
    do { \
        static int _count = 0; \
        if (_count == 0) { \
            H_LOG_INTERFACE(); \
        } \
        if (++_count == N) { \
            _count = 0; \
        } \
    } while (0)
#endif

#endif // _INC_INFRA_LOG_INTERFACE_H_
