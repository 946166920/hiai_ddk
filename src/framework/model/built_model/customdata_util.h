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
#ifndef FRAMEWORK_MODEL_MANAGER_CUSTOMDATA_UTIL_H
#define FRAMEWORK_MODEL_MANAGER_CUSTOMDATA_UTIL_H

// api/framework
#include "model/built_model.h"

namespace hiai {
const char *const CUST_DATA_TAG = "CUST";
class CustomDataUtil {
public:
    static void CopyDataToBuffer(
        std::shared_ptr<hiai::IBuffer>& outBuffer, size_t& offset, const void* data, size_t size);

    static Status CopyCustomDataToBuffer(
        std::shared_ptr<hiai::IBuffer>& buffer, size_t& offset, const CustomModelData& customModelData);

    static Status WriteCustomDataToFile(const char* file, const CustomModelData& customModelData);

    static std::shared_ptr<hiai::IBuffer> SaveCustomDataToBuffer(
        void* data, size_t size, const CustomModelData& customModelData);

    static std::shared_ptr<IBuffer> GetModelData(
        const std::shared_ptr<IBuffer>& buffer, CustomModelData& customModelData);

    static bool HasCustomData(const char* file);
};
} // namespace hiai
#endif // FRAMEWORK_MODEL_MANAGER_CUSTOMDATA_UTIL_H
