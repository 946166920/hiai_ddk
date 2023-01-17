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
#ifndef _HIAI_IMAGE_CONFIG_TENSOR_H_
#define _HIAI_IMAGE_CONFIG_TENSOR_H_

#include "tensor_api_export.h"
#include "nd_tensor_buffer.h"

namespace hiai {
template <typename T>
std::shared_ptr<INDTensorBuffer> CreateImageConfigTensor(T& para)
{
    NDTensorDesc desc;
    int channel = sizeof(T);
    desc.dims = {1, channel, 1, 1};
    desc.dataType = DataType::UINT8;
    desc.format = Format::NCHW;

    return CreateNDTensorBuffer(desc, &para, sizeof(T));
}
}; // namespace hiai
#endif