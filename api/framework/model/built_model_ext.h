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
#ifndef HIAI_API_BUILT_MODEL_EXT_H
#define HIAI_API_BUILT_MODEL_EXT_H

#include "built_model_aipp.h"
#include "model_api_export.h"

namespace hiai {
class IBuiltModelExt : public IBuiltModelAipp {
public:
    HIAI_M_API_EXPORT static std::shared_ptr<IBuiltModelExt> RestoreFromFile(const char* file, uint8_t shapeIndex);
};
} // namespace hiai
#endif // HIAI_API_BUILT_MODEL_EXT_H
