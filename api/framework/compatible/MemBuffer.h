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

#ifndef FRAMEWORK_MODEL_MANAGER_COMPATIBLE_MEMBUFFER_H
#define FRAMEWORK_MODEL_MANAGER_COMPATIBLE_MEMBUFFER_H
#include "model_manager/model_manager_api_export.h"
#include "compatible/hiai_base_types_cpt.h"

namespace hiai {
class HIAI_MM_API_EXPORT MemBuffer {
public:
    MemBuffer() = default;
    ~MemBuffer() = default;

    /*
     * @brief 获取通用MEMBuffer的内存地址
     * @return MEMBuffer的内存地址
     */
    void* GetMemBufferData();

    /*
     * @brief 获取通用MEMBuffer的内存大小
     * @return MEMBuffer的内存大小
     */
    uint32_t GetMemBufferSize();

private:
    friend class MembufferUtil;

    void SetMemBufferSize(uint32_t size);
    void SetMemBufferData(void* data);

    void SetServerMem(void* serverMem);
    void SetAppAllocFlag(bool isAppAlloc);

    void* GetServerMem();
    bool GetAppAllocFlag();

    void* data_ {nullptr};
    uint32_t size_ {0};
    void* servermem_ {nullptr};
    bool isAppAlloc_ {0};
};
} // namespace hiai
#endif
