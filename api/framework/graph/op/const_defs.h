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
#ifndef INC_API_GRAPH_OP_CONST_DEFS_H
#define INC_API_GRAPH_OP_CONST_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Constant tensor
 * <Output>
 *    y : Output tensor containing the same value of the provided tensor
 * <Attr>
 *    value : Value for the elements of the output tensor
 * <Added in HiAI version>
 *    100.300.010.011
 * <Examples>
 *    TensorDesc constTensorDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    TensorPtr constTensor = std::make_shared<hiai::Tensor>(constTensorDesc);
 *    vector<float> dataValue(4* 5 * 6 * 7, 0);
 *    constTensor->SetData((uint8_t*)dataValue.data(), 4* 5 * 6 * 7 * sizeof(float));
 *    auto constTemp = hiai::op::Const("constTemp").set_attr_value(constTensor);
 */
REG_OP(Const)
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT8, DT_INT32, DT_BOOL }))
.ATTR(value, AttrValue::TENSOR(new (std::nothrow) Tensor(TensorDesc())))
.OP_END()

/*
 * Quantized constant tensor.Used only for quantized const weight for quantize ops scenarios.
 * <Output>
 *    y : Output tensor containing the same value of the provided tensor.Supported data type:
 *        DT_INT8, DT_INT4, DT_2BIT.
 *        DT_INT4 and DT_2BIT can only be supported by Omg tools with quantize config currently.
 * <Attr>
 *    value : Value for the elements of the output tensor
 *    scale : Quantize scale values. If quant Per Layer, scale value nums is 1.
 *    offset : Quantize offset values. offset value nums is equal to scale values nums.
 * <Examples>
 *    TensorDesc constTensorDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_INT8);
 *    TensorPtr constTensor = std::make_shared<hiai::Tensor>(constTensorDesc);
 *    vector<int8_t> dataValue(4* 5 * 6 * 7, 0);
 *    constTensor->SetData((uint8_t*)dataValue.data(), 4* 5 * 6 * 7 * sizeof(int8_t));
 *    vector<float> scales(4, 0.1f);
 *    vector<float> offsets(4, 0.0f);
 *    auto constTemp = hiai::op::QuantizedConst("constTemp")
 *                         .set_attr_value(constTensor)
 *                         .set_attr_scale(scales)
 *                         .set_attr_offset(offsets);
 * <Added in HiAI version>
 *    100.515.020.100
 */
REG_OP(QuantizedConst)
.OUTPUT(y, TensorType({ DT_INT8, DT_INT4, DT_2BIT }))
.ATTR(value, AttrValue::TENSOR(new (std::nothrow) Tensor(TensorDesc())))
.REQUIRED_ATTR(scale, AttrValue::LIST_FLOAT)
.REQUIRED_ATTR(offset, AttrValue::LIST_FLOAT)
.OP_END()
} // namespace hiai
// clang-format on

#endif