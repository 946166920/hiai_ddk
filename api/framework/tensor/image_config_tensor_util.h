/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: image config tensor util
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