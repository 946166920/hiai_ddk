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
#ifndef FRAMEWORK_TENSOR_ND_TENSOR_DESC_H
#define FRAMEWORK_TENSOR_ND_TENSOR_DESC_H
#include <vector>

#include "util/base_types.h"

namespace hiai {
struct NDTensorDesc {
    std::vector<int32_t> dims;
    DataType dataType = DataType::FLOAT32;
    Format format = Format::NCHW;
};
} // namespace hiai
#endif
