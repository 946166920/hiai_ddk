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

#ifndef DOMI_COMMON_REGISTER_OP_TYPE_H__
#define DOMI_COMMON_REGISTER_OP_TYPE_H__

#if defined(HOST_VISIBILITY) || defined(DEV_VISIBILITY)
#define GE_API_VISIBILITY __attribute__((visibility("default")))
#else
#ifdef _MSC_VER
#ifdef LIBAI_FMK_OP_DEF_EXPORTS
#define GE_API_VISIBILITY __declspec(dllexport)
#else
#define GE_API_VISIBILITY __declspec(dllimport)
#endif
#else
#define GE_API_VISIBILITY
#endif
#endif
#endif // DOMI_COMMON_REGISTER_OP_TYPE_H__
