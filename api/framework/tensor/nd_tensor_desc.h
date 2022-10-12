/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: nd tensor buffer
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
