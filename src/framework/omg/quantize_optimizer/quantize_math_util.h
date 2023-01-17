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
#ifndef FRAMEWORK_OMG_QUANTIZE_OPTIMIZER_QUANTIZE_MATH_UTIL_H
#define FRAMEWORK_OMG_QUANTIZE_OPTIMIZER_QUANTIZE_MATH_UTIL_H

#include "base/error_types.h"
#include "framework/graph/core/node/node.h"
#include "graph/buffer.h"
#include "graph/tensor.h"

namespace hiai {
class QuantizeMathUtil {
public:
    /**
     *  @brief  获取权值的kernel信息
     */
    static Status CalculateKernelInfo(const ge::Node& node, const ge::Tensor& filter, uint32_t& kernelSize,
        uint32_t& kernelNum, uint32_t& kernelDataCount);
    /**
     *  @brief  校验权值参数信息与量化信息是否一致
     */
    static Status CheckWeightParams(
        const ge::Tensor& filter, const std::vector<float>& weightScale, uint32_t kernelNum, uint32_t weightDataSize);
    /*
     *  @brief  计算INT8权值数据
     */
    static Status CalculateINT8Data(
        const ge::Node& node, ge::Tensor& filter, const std::vector<float>& weightScale, int8_t* weightDataNew);
    /*
     *  @brief  计算FP32权值数据
     */
    static Status CalculateFP32Data(
        const ge::Node& node, ge::Tensor& filter, const std::vector<float>& weightScale, float* weightDataNew);

    static Status CompressWeightToINT8(ge::Tensor& weight, ge::DataType dataType);
};
} // namespace hiai

#endif
