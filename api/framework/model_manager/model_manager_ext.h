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
#ifndef FRAMEWORK_MODEL_MANAGER_MODEL_MANAGER_EXT_H
#define FRAMEWORK_MODEL_MANAGER_MODEL_MANAGER_EXT_H

#include "model_manager/model_manager_aipp.h"
#include "shared_mem_allocator.h"

namespace hiai {
class IModelManagerExt : public IModelManagerAipp {
public:
    HIAI_MM_API_EXPORT static std::shared_ptr<IModelManagerExt> CreateModelManagerExt();

public:
    using IModelManagerAipp::Init;
    virtual Status Init(const ModelInitOptions& options, const std::shared_ptr<IBuiltModel>& builtModel,
        const std::shared_ptr<IModelManagerListener>& listener,
        const std::shared_ptr<ISharedMemAllocator>& allocator) = 0;
};
} // namespace hiai
#endif
