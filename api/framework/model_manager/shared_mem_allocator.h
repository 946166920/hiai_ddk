/**
 * Copyright 2022-2022 Huawei Technologies Co., Ltd
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
#ifndef FRAMEWORK_MODEL_MANAGER_SHARED_MEM_ALLOCATOR_H
#define FRAMEWORK_MODEL_MANAGER_SHARED_MEM_ALLOCATOR_H

#include <vector>
#include <memory>

#include "model_manager/model_manager_api_export.h"
#include "util/base_types.h"

namespace hiai {
using HIAI_ModelManagerMemAllocator_AllocateFun = void (*)(uint32_t requiredSize, hiai::NativeHandle handles[],
    uint32_t handlesSize, void* config, uint32_t* allocatedHandlesSize);
using HIAI_ModelManagerMemAllocator_FreeFun = void (*)(
    hiai::NativeHandle handles[], uint32_t handlesSize, void* config);

class ISharedMemAllocator {
public:
    HIAI_MM_API_EXPORT static std::shared_ptr<ISharedMemAllocator> Create(
        HIAI_ModelManagerMemAllocator_AllocateFun allocateFun, HIAI_ModelManagerMemAllocator_FreeFun freeFun,
        void* config);

public:
    virtual ~ISharedMemAllocator() = default;
    virtual std::vector<NativeHandle> Allocate(size_t size) = 0;
    virtual void Free(std::vector<NativeHandle>& handle) = 0;
};
} // namespace hiai
#endif
