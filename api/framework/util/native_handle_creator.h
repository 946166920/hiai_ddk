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
#ifndef FRAMEWORK_UTIL_NATIVE_HANDLE_CREATOR_H
#define FRAMEWORK_UTIL_NATIVE_HANDLE_CREATOR_H

#include <memory>
#include "util_api_export.h"
#include "base_types.h"
#include "base/error_types.h"
#if (defined( __ANDROID__ ) || defined(ANDROID))
#include "native_handle.h"
#endif

namespace hiai {
#if (defined( __ANDROID__ ) || defined(ANDROID))
HIAI_UTIL_API_EXPORT Status CreateNativeHandle(NativeHandle& nativeHandle, const buffer_handle_t handle, int offset =
    -1, int size = -1);
#endif
} // namespace hiai
#endif // FRAMEWORK_UTIL_NATIVE_HANDLE_CREATOR_H