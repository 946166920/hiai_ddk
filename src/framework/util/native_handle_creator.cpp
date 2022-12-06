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
#include "util/native_handle_creator.h"
#include <map>

#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log_fmk_interface.h"
#include "hiai_foundation_dl_helper.h"

namespace hiai {
#if defined(__ANDROID__) || defined(ANDROID)
namespace {
struct PrivateHandleInfo {
    int fd;
    int size;
    int offset;
    int format;
    int w;
    int h;
    int strideW;
    int strideH;
};
}

Status CreateNativeHandle(NativeHandle& nativeHandle, const buffer_handle_t handle, int offset, int size)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    PrivateHandleInfo* (*getHandleInfoFunc)(buffer_handle_t) =
        (PrivateHandleInfo* (*)(buffer_handle_t)) HIAI_Foundation_GetSymbol("HIAI_GetHandleInfo_From_BufferHandle");
    void (*destroyHandleFunc)(PrivateHandleInfo*) =
        (void (*)(PrivateHandleInfo*))HIAI_Foundation_GetSymbol("HIAI_DestroyPrivateHandle");
    if (getHandleInfoFunc == nullptr || destroyHandleFunc == nullptr) {
        FMK_LOGE("unsupported version.");
        return FAILURE;
    }

    std::shared_ptr<PrivateHandleInfo> bufferHandleT(
        getHandleInfoFunc(handle), [&destroyHandleFunc](PrivateHandleInfo* p) { destroyHandleFunc(p); });
    if (bufferHandleT == nullptr) {
        FMK_LOGE("Get PrivateHandleInfo failed!");
        return FAILURE;
    }

    nativeHandle.fd = bufferHandleT->fd;
    nativeHandle.size = size == -1 ? bufferHandleT->size : size;
    nativeHandle.offset = offset == -1 ? bufferHandleT->offset : offset;
    return SUCCESS;
}
#endif
} // namespace hiai