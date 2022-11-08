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
#ifndef INC_FRAMEWORK_GRAPH_OP_INTERNAL_NN_DEFS_H
#define INC_FRAMEWORK_GRAPH_OP_INTERNAL_NN_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * The operation pads input according to the paddings and constant_values.
 * <Input>
 *    x : The input tensor.
 *    paddings : The values of paddings, as a role of dimensions to be added on the input tensor x,
 *               must be a Const-OP.
 *    constant_values : A tensor of the same type as x, that indicates the value to use for padding input,
 *                      must be a Const-OP.
 * <Output>
 *    y : The output tensor.
 * <Attr>
 *    mode : constant, fill zero, only support ''constant'
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(PadV3)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT32 }))
.INPUT(paddings, TensorType({ DT_INT32 }))
.OPTIONAL_INPUT(constant_values, TensorType({ DT_FLOAT, DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT32 }))
.ATTR(mode, AttrValue::STR { "constant" })
.OP_END()

/*
 * Calculate values and indices of the k largest entries of the last dimension and output.
 * <Input>
 *    x : 1-D or higher Tensor with last dimension at least k
 *    assist_seq : reserved
 * <Output>
 *    values : k largest elements along each last dimensional slice
 *    indices : indices of values within the last dimension of input
 * <Attr>
 *    k : Number of top elements to look for along the last dimension
 *    dim : Dimension on which to do the sort
 *    largest : Whether to return the top-K largest or smallest elements
 *    sorted : If true, the resulting k elements will be sorted by the values in descending order.
 *             If it be set to false, just make sure the k elements no losing, the data set is whole.
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(TopKD)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8 }))
.INPUT(assist_seq, TensorType({ DT_INT32 }))
.OUTPUT(values, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8 }))
.OUTPUT(indices, TensorType({ DT_INT32 }))
.REQUIRED_ATTR(k, AttrValue::INT)
.ATTR(dim, AttrValue::INT { -1 })
.ATTR(largest, AttrValue::BOOL { true })
.ATTR(sorted, AttrValue::BOOL { true })
.OP_END()

/*
 * Long Short-Term Memory layer, using an internal network unrolled in time.
 * <Input>
 *    x : The input tensor for LSTM cell.
 *        when model attr is "Basic". A 2-D tensor of shape [batch_size, input_size],
 *        when model attr is "UniDirectional". A 3-D tensor of shape: If time-major:
 *        [max_time, batch_size, input_size] If batch-major: [batch_size, max_time, input_size].
 *    input_to_input_weight : A 2-D tensor of shape [num_units, input_size],
 *                            where "num_units" corresponds to the number of cell units.
 *    input_to_forget_weight : A 2-D tensor of shape [num_units, input_size].
 *    input_to_cell_weight : A 2-D tensor of shape [num_units, input_size].
 *    input_to_output_weight : A 2-D tensor of shape [num_units, input_size].
 *    recurrent_to_input_weight : A 2-D tensor of shape [num_units, output_size].
 *    recurrent_to_forget_weight : A 2-D tensor of shape [num_units, output_size].
 *    recurrent_to_cell_weight : A 2-D tensor of shape [num_units, output_size].
 *    recurrent_to_output_weight : A 2-D tensor of shape [num_units, output_size].
 *    cell_to_input_weight : A 1-D tensor of shape [num_units].
 *    cell_to_forget_weight : A 1-D tensor of shape [num_units].
 *    cell_to_output_weight : A 1-D tensor of shape [num_units].
 *    input_gate_bias : A 1-D tensor of shape [num_units].
 *    forget_gate_bias : A 1-D tensor of shape [num_units].
 *    cell_gate_bias : A 1-D tensor of shape [num_units].
 *    output_gate_bias : A 1-D tensor of shape [num_units].
 *    projection_weights : A 2-D tensor of shape [output_size, num_units].
 *    projection_bias : A 1-D tensor of shape [output_size].
 *    output_state_in : A 2-D tensor of shape [batch_size, output_size].
 *    cell_state_in : A 2-D tensor of shape [batch_size, num_units].
 *    input_normalization_weights : Reserved. A 1-D tensor of shape [num_units]. current is not support.
 *    forget_normalization_weights : Reserved. A 1-D tensor of shape [num_units]. current is not support.
 *    cell_normalization_weights : Reserved., A 1-D tensor of shape [num_units]. current is not support.
 *    output_normalization_weights : Reserved. A 1-D tensor of shape [num_units]. current is not support.
 * <Output>
 *    y : The output tensor. The output size are directed by "mode".
 * <Attr>
 *    mode : "Basic" mean BasicLstm. When mode set to "Basic" have four outputs :
 *           output 0: Reserved. The scratch buffer A 2-D tensor of shape [batch_size, num_units * 3] with CIFG,
 *           or [batch_size, num_units * 4] without CIFG.
 *           output 1: The output state (out). A 2-D tensor of shape [batch_size, output_size].
 *           output 2: The cell state (out). A 2-D tensor of shape [batch_size, num_units].
 *           output 3: The output. A 2-D tensor of shape [batch_size, output_size].
 *           "UniDirectional" mean UniDirectional LSTM, have one output :
 *           0: The output. A 3-D tensor of shape: If time-major: [max_time, batch_size, output_size]
 *           If batch-major: [batch_size, max_time, output_size].
 *    activation : LSTM activation mode, with options as follows:
 *                 1 : Tanh
 *                 3 : ReLU
 *                 4 : ReLU6
 *                 6 : Sigmoid
 *    lstm_cell_clip : clipping threshold for the cell state.
 *    lstm_proj_clip : clipping threshold for the output from the projection layer.
 *    use_cifg : True : enable CIFG feature; False: disable CIFG feature.
 *    use_peephole : True : enable peephole optimization; False: disable peephole optimization
 *    use_projection : True. enable projection weight; False: disable projection weight.
 *    use_projection_bias : True. enable projection bias; False: disable projection bias.
 *                          IF use_projection is False, use_projection_bias is disabled.
 *    time_major : only effect when model is "UniDirectional"
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(LSTMV2)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(input_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(input_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(input_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(input_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(recurrent_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(recurrent_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(recurrent_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(recurrent_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(cell_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(cell_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(cell_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(input_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(forget_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(cell_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(output_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(projection_weights, TensorType({ DT_FLOAT }))
.INPUT(projection_bias, TensorType({ DT_FLOAT }))
.INPUT(output_state_in, TensorType({ DT_FLOAT }))
.INPUT(cell_state_in, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(input_normalization_weights, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(forget_normalization_weights, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(cell_normalization_weights, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(output_normalization_weights, TensorType({ DT_FLOAT }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(mode, AttrValue::STR { "Basic" })
.ATTR(activation, AttrValue::INT { 4 })
.ATTR(lstm_cell_clip, AttrValue::FLOAT { 0.0 })
.ATTR(lstm_proj_clip, AttrValue::FLOAT { 0.0 })
.ATTR(use_cifg, AttrValue::BOOL { false })
.ATTR(use_peephole, AttrValue::BOOL { false })
.ATTR(use_projection, AttrValue::BOOL { false })
.ATTR(use_projection_bias, AttrValue::BOOL { false })
.ATTR(time_major, AttrValue::BOOL { true })
.OP_END()

/*
 * A recurrent neural network layer that applies an LSTM cell to a sequence of inputs
 * in forward and backward directions.
 * <Input>
 *    x : The input. A 3-D tensor of shape: If time-major: [max_time, batch_size, input_size]
 *        If batch-major: [batch_size, max_time, input_size].
 *    fw_input_to_input_weight : A 2-D tensor of shape [fw_num_units, input_size].
 *    fw_input_to_forget_weight : A 2-D tensor of shape [fw_num_units, input_size].
 *    fw_input_to_cell_weight : A 2-D tensor of shape [fw_num_units, input_size].
 *    fw_input_to_output_weight : A 2-D tensor of shape [fw_num_units, input_size].
 *    fw_recurrent_to_input_weight : A 2-D tensor of shape [fw_num_units, fw_output_size].
 *    fw_recurrent_to_forget_weight : A 2-D tensor of shape [fw_num_units, fw_output_size].
 *    fw_recurrent_to_cell_weight : A 2-D tensor of shape [fw_num_units, fw_output_size].
 *    fw_recurrent_to_output_weight : A 2-D tensor of shape [fw_num_units, fw_output_size].
 *    fw_cell_to_input_weight : A 1-D tensor of shape [fw_num_units].
 *    fw_cell_to_forget_weight : A 1-D tensor of shape [fw_num_units].
 *    fw_cell_to_output_weight : A 1-D tensor of shape [fw_num_units].
 *    fw_input_gate_bias : A 1-D tensor of shape [fw_num_units].
 *    fw_forget_gate_bias : A 1-D tensor of shape [fw_num_units].
 *    fw_cell_gate_bias : A 1-D tensor of shape [fw_num_units].
 *    fw_output_gate_bias : A 1-D tensor of shape [fw_num_units].
 *    bw_input_to_input_weight : A 2-D tensor of shape [bw_num_units, input_size].
 *    bw_input_to_forget_weight : A 2-D tensor of shape [bw_num_units, input_size].
 *    bw_input_to_cell_weight : A 2-D tensor of shape [bw_num_units, input_size].
 *    bw_input_to_output_weight : A 2-D tensor of shape [bw_num_units, input_size].
 *    bw_recurrent_to_input_weight : A 2-D tensor of shape [bw_num_units, bw_output_size].
 *    bw_recurrent_to_forget_weight : A 2-D tensor of shape [bw_num_units, bw_output_size].
 *    bw_recurrent_to_cell_weight : A 2-D tensor of shape [bw_num_units, bw_output_size].
 *    bw_recurrent_to_output_weight : A 2-D tensor of shape [bw_num_units, bw_output_size].
 *    bw_cell_to_input_weight : A 1-D tensor of shape [bw_num_units].
 *    bw_cell_to_forget_weight : A 1-D tensor of shape [bw_num_units].
 *    bw_cell_to_output_weight : A 1-D tensor of shape [bw_num_units].
 *    bw_input_gate_bias : A 1-D tensor of shape [bw_num_units].
 *    bw_forget_gate_bias : A 1-D tensor of shape [bw_num_units].
 *    bw_cell_gate_bias : A 1-D tensor of shape [bw_num_units].
 *    bw_output_gate_bias : A 1-D tensor of shape [bw_num_units].
 *    fw_activation_state : A 2-D tensor of shape [batch_size, bw_output_size].
 *    fw_cell_state : A 2-D tensor of shape [batch_size, bw_num_units].
 *    bw_activation_state : A 2-D tensor of shape [batch_size, bw_output_size].
 *    bw_cell_state : A 2-D tensor of shape [batch_size, bw_num_units].
 *    fw_proj_weight : A 2-D tensor of shape [fw_output_size, fw_num_units].
 *    fw_proj_bias : A 1-D tensor of shape [fw_output_size].
 *    bw_proj_weight : A 2-D tensor of shape [bw_output_size, bw_num_units].
 *    bw_proj_bias : A 1-D tensor of shape [bw_output_size].
 *    aux_input : Reserved.A 3-D tensor of shape [max_time, batch_size, aux_input_size].
 *    fw_aux_input_to_input_weight : Reserved. A 2-D tensor of shape [fw_num_units, aux_input_size].
 *    fw_aux_input_to_forget_weight : Reserved. A 2-D tensor of shape [fw_num_units, aux_input_size].
 *    fw_aux_input_to_cell_weight : Reserved. A 2-D tensor of shape [fw_num_units, aux_input_size].
 *    fw_aux_input_to_output_weight : Reserved. A 2-D tensor of shape [fw_num_units, aux_input_size].
 *    bw_aux_input_to_input_weight : Reserved. A 2-D tensor of shape [bw_num_units, aux_input_size].
 *    bw_aux_input_to_forget_weight : Reserved. A 2-D tensor of shape [bw_num_units, aux_input_size].
 *    bw_aux_input_to_cell_weight : Reserved. A 2-D tensor of shape [bw_num_units, aux_input_size].
 *    bw_aux_input_to_output_weight : Reserved. A 2-D tensor of shape [bw_num_units, aux_input_size].
 *    fw_input_layer_norm_weight : Reserved. A 1-D tensor of shape [fw_num_units].
 *    fw_forget_layer_norm_weight : Reserved. A 1-D tensor of shape [fw_num_units].
 *    fw_cell_layer_norm_weight : Reserved. A 1-D tensor of shape [fw_num_units].
 *    fw_output_layer_norm_weight : Reserved. A 1-D tensor of shape [fw_num_units].
 *    bw_input_layer_norm_weight : Reserved. A 1-D tensor of shape [bw_num_units].
 *    bw_forget_layer_norm_weight : Reserved. A 1-D tensor of shape [bw_num_units].
 *    bw_cell_layer_norm_weight : Reserved. A 1-D tensor of shape [bw_num_units].
 *    bw_output_layer_norm_weight : Reserved. A 1-D tensor of shape [bw_num_units].
 * <Output>
 *    y : The output tensor. Current support 2 outputs:
 *        output 0: The forward output. A 3-D tensor of shape:
 *                  If time-major and not merge_outputs: [max_time, batch_size, fw_output_size].
 *                  If time-major and merge_outputs: [max_time, batch_size, fw_output_size + bw_output_size].
 *                  If batch-major and not merge_outputs: [batch_size, max_time, fw_output_size]
 *                  If batch-major and merge_outputs: [batch_size, max_time, fw_output_size + bw_output_size]
 *        output 1: The backward output.Unused if merge_outputs is true. A 3-D tensor of shape:
 *                  If time-major: [max_time, batch_size, bw_output_size].
 *                  If batch-major: [batch_size, max_time, bw_output_size].
 * <Attr>
 *    activation : LSTM activation mode, with options as follows:
 *                 1 : Tanh
 *                 3 : ReLU
 *                 4 : ReLU6
 *                 6 : Sigmoid
 *    lstm_cell_clip : clipping threshold for the cell state.
 *    lstm_proj_clip : clipping threshold for the output from the projection layer.
 *    use_cifg : True : enable CIFG feature; False: disable CIFG feature.
 *    use_peephole : True : enable peephole optimization; False: disable peephole optimization
 *    use_projection : True. enable projection weight; False: disable projection weight.
 *    use_projection_bias : True. enable projection bias; False: disable projection bias.
 *                          IF use_projection is False, use_projection_bias is disabled.
 *    time_major : specifying the shape format of input and output tensors.
 *    merge_output : the input_gate_bias's offset.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(BidirectionalSequenceLstm)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(fw_input_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_input_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_input_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_input_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_recurrent_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_recurrent_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_recurrent_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_recurrent_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_cell_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_cell_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_cell_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(fw_input_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(fw_forget_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(fw_cell_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(fw_output_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(bw_input_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_input_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_input_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_input_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_recurrent_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_recurrent_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_recurrent_to_cell_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_recurrent_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_cell_to_input_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_cell_to_forget_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_cell_to_output_weight, TensorType({ DT_FLOAT }))
.INPUT(bw_input_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(bw_forget_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(bw_cell_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(bw_output_gate_bias, TensorType({ DT_FLOAT }))
.INPUT(fw_activation_state, TensorType({ DT_FLOAT }))
.INPUT(fw_cell_state, TensorType({ DT_FLOAT }))
.INPUT(bw_activation_state, TensorType({ DT_FLOAT }))
.INPUT(bw_cell_state, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_proj_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_proj_bias, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_proj_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_proj_bias, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(aux_input, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_aux_input_to_input_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_aux_input_to_forget_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_aux_input_to_cell_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_aux_input_to_output_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_aux_input_to_input_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_aux_input_to_forget_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_aux_input_to_cell_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_aux_input_to_output_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_input_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_forget_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_cell_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_output_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_input_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_forget_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_cell_layer_norm_weight, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_output_layer_norm_weight, TensorType({ DT_FLOAT }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(activation, AttrValue::INT { 4 })
.ATTR(lstm_cell_clip, AttrValue::FLOAT { 0.0 })
.ATTR(lstm_proj_clip, AttrValue::FLOAT { 0.0 })
.ATTR(use_cifg, AttrValue::BOOL { false })
.ATTR(use_peephole, AttrValue::BOOL { false })
.ATTR(use_projection, AttrValue::BOOL { false })
.ATTR(use_projection_bias, AttrValue::BOOL { false })
.ATTR(time_major, AttrValue::BOOL { true })
.ATTR(merge_output, AttrValue::BOOL { true })
.OP_END()

/*
 * A recurrent neural network layer.
 * <Input>
 *    x : input. A 2-D tensor of shape [batch_size, input_size].
 *    weights : A 2-D tensor of shape [num_units, input_size].
 *    recurrent_weights : A 2-D tensor of shape [num_units, num_units].
 *    bias : A 1-D tensor of shape [num_units].
 *    hidden_state : A 2-D tensor of shape [batch_size, num_units].
 * <Output>
 *    y : The output tensor. The output size are directed by "mode".
 * <Attr>
 *    rnn_activate : activation mode.
 *    mode : "Basic" mean BasicLstm, have four outputs :
 *                output 0: Reserved. The scratch buffer A 2-D tensor of shape [batch_size, num_units * 3] with CIFG,
 *                          or [batch_size, num_units * 4] without CIFG.
 *                output 1: The output state (out). A 2-D tensor of shape [batch_size, output_size].
 *                output 2: The cell state (out). A 2-D tensor of shape [batch_size, num_units].
 *                output 3: The output. A 2-D tensor of shape [batch_size, output_size].
 *           "UniDirectional" mean UniDirectional LSTM, have one output :
 *                output 0: The output. A 3-D tensor of shape:
 *                          If time-major: [max_time, batch_size, output_size].
 *                          If batch-major: [batch_size, max_time, output_size].
 *    time_major : only effect when model is "UniDirectional"
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(RNNV3)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(weights, TensorType({ DT_FLOAT }))
.INPUT(recurrent_weights, TensorType({ DT_FLOAT }))
.INPUT(bias, TensorType({ DT_FLOAT }))
.INPUT(hidden_state, TensorType({ DT_FLOAT }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(rnn_activate, AttrValue::INT { 4 })
.ATTR(mode, AttrValue::STR { "Basic" })
.ATTR(time_major, AttrValue::BOOL { true })
.OP_END()

/*
 * A recurrent neural network layer that applies a basic RNN cell to a sequence of inputs
 * in forward and backward directions
 * <Input>
 *    x : input. input. A 3-D tensor. input has a shape [maxTime, batchSize, inputSize].
 *    fw_weights : A 2-D tensor of shape [fwNumUnits, inputSize].
 *    fw_recurrent_weights : A 2-D tensor of shape [fwNumUnits, fwNumUnits].
 *    fw_bias : A 1-D tensor of shape [fwNumUnits].
 *    fw_hidden_state : A 2-D tensor of shape [batchSize, fwNumUnits].
 *    bw_weights : A 2-D tensor of shape [bwNumUnits, inputSize].
 *    bw_recurrent_weights : A 2-D tensor of shape [bwNumUnits, bwNumUnits].
 *    bw_bias : A 1-D tensor of shape [bwNumUnits].
 *    bw_hidden_state : A 2-D tensor of shape [batchSize, bwNumUnits].
 *    aux_input : A 3-D tensor, the input has a shape [batchSize, maxTime, auxInputSize].
 *    fw_aux_weights : A 2-D tensor of shape [fwNumUnits, auxInputSize].
 *    bw_aux_weights : A 2-D tensor of shape [bwNumUnits, auxInputSize].
 * <Output>
 *    y : The output tensor. The output size are directed.
 * <Attr>
 *    merge_output : LSTM activation mode, with options as follows:
 *    time_major : LSTM activation mode, with options as follows:
 *    bisequence_rnn_activate : The activation function.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(BidirectionalSequenceRnn)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(fw_weights, TensorType({ DT_FLOAT }))
.INPUT(fw_recurrent_weights, TensorType({ DT_FLOAT }))
.INPUT(fw_bias, TensorType({ DT_FLOAT }))
.INPUT(fw_hidden_state, TensorType({ DT_FLOAT }))
.INPUT(bw_weights, TensorType({ DT_FLOAT }))
.INPUT(bw_recurrent_weights, TensorType({ DT_FLOAT }))
.INPUT(bw_bias, TensorType({ DT_FLOAT }))
.INPUT(bw_hidden_state, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(aux_input, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(fw_aux_weights, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bw_aux_weights, TensorType({ DT_FLOAT }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(merge_output, AttrValue::BOOL { false })
.ATTR(time_major, AttrValue::BOOL { true })
.ATTR(bisequence_rnn_activate, AttrValue::INT { 1 })
.OP_END()

/*
 * Yolo2Reorg Operator
 * <Input>
 *    x : Input tensor of shape N*C*H*W
 * <Output>
 *    y : output tensor be Reorged
 * <Attr>
 *    stride : Zoom factor
 *    reverse : false  output shape N*(C*stride*stride)*(H/stride)*(W/stride)
 *              true   output shape N*(C/(stride*stride))*(H*stride)*(W*stride)
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(Yolo2Reorg)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.ATTR(stride, AttrValue::INT { 2 })
.ATTR(reverse, AttrValue::BOOL { false })
.OP_END()

/*
 * Performs Region of Interest (RoI) Pool operator.
 * <Input>
 *    x : the first input 4-D tensor.
 *    rois : the second input 2-D tensor, here respecting the roi.Shape is [num_rois, 5].
 *    roi_actual_num : Reserved.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    pooled_h : roi_pooling pooled_h must be greater than 0.
 *    pooled_w : roi_pooling pooled_w must be greater than 0.
 *    spatial_scale_h : specifying the ratio from the height of original image to the height of feature map.
 *                      must be greater than 0.
 *    spatial_scale_w : specifying the ratio from the width of original image to the width of feature map.
 *                      must be greater than 0
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(ROIPooling)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(rois, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(roi_actual_num, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(pooled_h, AttrValue::INT)
.REQUIRED_ATTR(pooled_w, AttrValue::INT)
.REQUIRED_ATTR(spatial_scale_h, AttrValue::FLOAT)
.REQUIRED_ATTR(spatial_scale_w, AttrValue::FLOAT)
.OP_END()

/*
 * Normalizes data. It is called Region on YOLO v2 and Yolo on YOLO v3.
 * <Input>
 *    x : An NCHW tensor with shape (N, boxes*(coords+obj+classes), H, W),
 *        "obj" indicates the confidence of an object, and only one confidence is supported.
 * <Output>
 *    coord_data : A tensor with shape [N, boxes*coords, ceilx(height*width*2+32, 32)/2].
 *                 "ceil" indicates that a detected box is aligned upwards with the second parameter.
 *                 Specifies the coordinates of a detected box.
 *    obj_prob : A tensor with shape [N, ceilx(boxes*height*width *2+32, 32)/2].
 *    classes_prob : A tensor with shape [N, classes, ceilx(boxes*height*width *2+32, 32)/2].
 * <Attr>
 *    boxes : specifying the number of anchor boxes. Defaults to "5" for V2 or "3" for V3.
 *    coords : specifying the number of parameters required for locating an object.
 *             The value is fixed at "4", corresponding to (x, y, w, h).
 *    classes : specifying the number of prediction classes. The value range is [1, 1024].
 *    yolo_version : A int, specifying the YOLO version, either "V2(1)" or "V3(2)".
 *    softmax : specifying whether to perform softmax, valid only when "yolo_version = V2".
 *    softmax_tree : Reserved. Only support false.
 *    background : specifying the operation types of the obj and classes,
 *                 used in conjunction with "softmax" and valid only when "yolo_version = V2".
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Yolo)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(coord_data, TensorType({ DT_FLOAT }))
.OUTPUT(obj_prob, TensorType({ DT_FLOAT }))
.OUTPUT(classes_prob, TensorType({ DT_FLOAT }))
.ATTR(boxes, AttrValue::INT { 3 })
.ATTR(coords, AttrValue::INT { 4 })
.ATTR(classes, AttrValue::INT { 80 })
.ATTR(yolo_version, AttrValue::INT { 2 })
.ATTR(softmax, AttrValue::BOOL { false })
.ATTR(softmax_tree, AttrValue::BOOL { false })
.ATTR(background, AttrValue::BOOL { false })
.OP_END()

/*
 * Performs YOLO V2 detection.
 * <Input>
 *    coord : Outputs of YOLO. For details, see the description of operator Yolo.
 *    obj : Outputs of YOLO. For details, see the description of operator Yolo.
 *    class : Outputs of YOLO. For details, see the description of operator Yolo.
 *    im_info : A float16, describing the image information including the required image height and width
 *              and the actual image height and width.
 * <Output>
 *    box_out : An NCHW tensor describing the information of each output box,
 *              including the coordinates, class, and confidence.
 *    box_out_num : An NCHW tensor specifying the number of output boxes.
 * <Attr>
 *    biases : Number of biases must be equal to twice of boxes.
 *    classes : specifying the number of prediction classes. The value range is [1, 1024].
 *    boxes : specifying the number of anchor boxes. Defaults to "5" for V2 or "3" for V3.
 *    relative : Defaults to and must be "true".
 *    objectness_threshold : specifying the confidence threshold for box filtering,
 *                           which is the output "obj" of operator Yolo. The value range is [0.0, 1.0].
 *    class_threshold : specifying the class score threshold for box filtering,
 *                      which is the output "class" of operator Yolo. The value range is [0.0, 1.0].
 *    post_top_k : The maximum number of boxes returning from the hard NMS algorithm.
 *    pre_nms_topn : This attribute is reserved.
 *    nms_threshold : specifying the intersection-over-union threshold for box filtering.
 *                    The value range is [0.0, 1.0].
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(YoloDetectionOutput)
.INPUT(coord, TensorType({ DT_FLOAT }))
.INPUT(obj, TensorType({ DT_FLOAT }))
.INPUT(class, TensorType({ DT_FLOAT }))
.INPUT(im_info, TensorType({ DT_FLOAT }))
.OUTPUT(box_out, TensorType({ DT_FLOAT }))
.OUTPUT(box_out_num, TensorType({ DT_INT32 }))
.REQUIRED_ATTR(biases, AttrValue::LIST_FLOAT)
.ATTR(classes, AttrValue::INT { 80 })
.ATTR(boxes, AttrValue::INT { 5 })
.ATTR(relative, AttrValue::BOOL { true })
.ATTR(objectness_threshold, AttrValue::FLOAT { 0.5 })
.ATTR(class_threshold, AttrValue::FLOAT { 0.5 })
.ATTR(post_top_k, AttrValue::INT { 512 })
.ATTR(pre_nms_topn, AttrValue::INT { 80 })
.ATTR(nms_threshold, AttrValue::FLOAT { 0.0 })
.OP_END()

/*
 * Consumes an 5D input tensor and a 5D filter, and computes the output.
 * <Input>
 *    x : Input 5D tensor with size [N, Ci, Di, Hi, Wi] or [N, Di, Hi, Wi, Ci].
 *    filter : 5D shape with size [Co, Ci/group, Dk, Hk, Wk].
 *    bias : With shape [Co], must be a Const-OP.
 *    offset_w : Reserved. For quantized.
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    strides : Stride along each input spatial axis [Di, Hi, Wi]. Strides size must be 3.
 *    pads : Padding for the beginning and ending along each axis [dh, dt, hh, ht, wh, wt].
 *    pad_mode : Pad mode, SPECIFIC(Default): not set, using pads; SAME, or VALID.
 *               If provided both pads and pad_mode, pad_mode has a higher priority than pads.
 *    dilations : Dilation value along each filter spatial axis [Dk, Hk, Wk]. Dilations size must be 3.
 *    groups : Number of groups input channels and output channels are divided into.
 *             When groups = 1, traditional convolution will be performed;
 *             When groups > 1, feature maps are grouped by group_count, and then each groups
 *             is convoluted separately. Specially, 'groups' equal to the number of input feature
 *             maps indicates Depthwise Convolution.
 *    data_format : Format of operator, 'NCDHW' or 'NDHWC'. Default is 'NCDHW'
 *    offset_x : Reserved. For quantized.
 * <Added in HiAI version>
 *    100.600.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 10, 224, 224, 3}), FORMAT_NDHWC, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *    x.update_output_desc_y(xDesc);
 *
 *    TensorDesc filterTensorDesc(Shape({16, 3, 1, 1, 1}), FORMAT_NCDHW, DT_FLOAT);
 *    TensorPtr filterTensor = std::make_shared<hiai::Tensor>(filterTensorDesc);
 *    vector<float> filterDataValue(16 * 3 , 0);
 *    filterTensor->SetData((uint8_t*)filterDataValue.data(), 16 * 3 * sizeof(float));
 *    auto filter = hiai::op::Const("filter").set_attr_value(filterTensor);
 *
 *    TensorDesc biasTensorDesc(Shape({16}), FORMAT_NCDHW, DT_FLOAT);
 *    TensorPtr biasTensor = std::make_shared<hiai::Tensor>(biasTensorDesc);
 *    vector<float> biasDataValue(16, 0);
 *    biasTensor->SetData((uint8_t*)biasDataValue.data(), 16 * sizeof(float));
 *    auto bias = hiai::op::Const("bias").set_attr_value(biasTensor);
 *
 *    auto convolution3d = hiai::op::Convolution3D("convolution3d")
 *                       .set_input_x(x)
 *                       .set_input_filter(filter)
 *                       .set_input_bias(bias)
 *                       .set_attr_strides({2, 2, 2})
 *                       .set_attr_dilations({1, 1, 1})
 *                       .set_attr_pads({0, 0, 0, 0, 0, 0})
 *                       .set_attr_pad_mode("VALID")
 *                       .set_attr_groups(1)
 *                       .set_attr_data_format("NDHWC");
 */
REG_OP(Convolution3D)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(filter, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bias, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(offset_w, TensorType({ DT_INT8 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.ATTR(pads, AttrValue::LIST_INT ({ 0, 0, 0, 0, 0, 0 }))
.ATTR(pad_mode, AttrValue::STR { "SPECIFIC" })
.ATTR(dilations, AttrValue::LIST_INT ({ 1, 1, 1 }))
.ATTR(groups, AttrValue::INT { 1 })
.ATTR(data_format, AttrValue::STR { "NCDHW" })
.ATTR(offset_x, AttrValue::INT { 0 })
.OP_END()

/*
 * Computing a 3D max pooling from a 5D input.
 * <Input>
 *    x : 5D tensor with shape [N, Ci, Di, Hi, Wi] or [N, Di, Hi, Wi, Ci,].
 * <Output>
 *    y : Output tensor.
 * <Attr>
 *    ksize : Tuple of 3 integers, which is downscale factor for [Di, Hi, Wi].
 *    strides : Stride along each input spatial dimension [Di, Hi, Wi]. Strides size must be 3.
 *    pads : Padding for the beginning and ending along each dimension [dh, dt, hh, ht, wh, wt].
 *    pad_mode : Pad mode, SPECIFIC(Default): not set, using pads; SAME, or VALID.
 *    dilations : Dilations along each dimension of the ksize. Dilations size must be 3.
 *    ceil_mode : When True, will use ceil instead of floor to compute the output shape. Default is false.
 *    data_format : Format of operator, 'NCDHW' or 'NDHWC'. Default is 'NCDHW'.
 * <Added in HiAI version>
 *    100.600.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 10, 224, 224, 3}), FORMAT_NDHWC, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *    x.update_output_desc_y(xDesc);
 *
 *    auto maxpooling3d = hiai::op::MaxPooling3D("maxpooling3d")
 *                       .set_input_x(x)
 *                       .set_attr_ksize({2, 2, 2})
 *                       .set_attr_strides({1, 1, 1})
 *                       .set_attr_pads({0, 0, 0, 0, 0, 0})
 *                       .set_attr_pad_mode("VALID")
 *                       .set_attr_dilations({1, 1, 1})
 *                       .set_attr_ceil_mode(false)
 *                       .set_attr_data_format("NDHWC");
 */
REG_OP(MaxPooling3D)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(ksize, AttrValue::LIST_INT)
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.ATTR(pads, AttrValue::LIST_INT ({ 0, 0, 0, 0, 0, 0 }))
.ATTR(pad_mode, AttrValue::STR { "SPECIFIC" })
.ATTR(dilations, AttrValue::LIST_INT ({ 1, 1, 1 }))
.ATTR(ceil_mode, AttrValue::BOOL { false })
.ATTR(data_format, AttrValue::STR { "NCDHW" })
.OP_END()

/*
 * Computing a 3D average pooling from a 5D input.
 * <Input>
 *    x : 5D tensor with shape [N, Ci, Di, Hi, Wi] or [N, Di, Hi, Wi, Ci,].
 * <Output>
 *    y : Output tensor.
 * <Attr>
 *    ksize : Tuple of 3 integers, which is downscale factor for [Di, Hi, Wi].
 *    strides : Stride along each input spatial dimension [Hi, Wi, Ci]. Strides size must be 3.
 *    pads : Padding for the beginning and ending along each dimension [dh, dt, hh, ht, wh, wt].
 *    pad_mode : Pad mode, SPECIFIC(Default): not set, using pads; SAME, or VALID.
 *    ceil_mode : When True, will use ceil instead of floor to compute the output shape. Default is false.
 *    count_include_pad : Reserved. If true, include the zero-padding in the averageing calculation. Default is false.
 *    divisor_override : Reserved. If specified, it will be used as divisor, otherwise the ksize will be used.
 *    data_format : Format of operator, 'NCDHW' or 'NDHWC'. Default is 'NCDHW'.
 * <Added in HiAI version>
 *    100.600.020.100
 * <Examples>
 *    TensorDesc xDesc(Shape({1, 10, 224, 224, 3}), FORMAT_NDHWC, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *    x.update_output_desc_y(xDesc);
 *
 *    auto avgpooling3d = hiai::op::AvgPooling3D("avgpooling3d")
 *                       .set_input_x(x)
 *                       .set_attr_ksize({2, 2, 2})
 *                       .set_attr_strides({1, 1, 1})
 *                       .set_attr_pads({0, 0, 0, 0, 0, 0})
 *                       .set_attr_pad_mode("VALID")
 *                       .set_attr_ceil_mode(false)
 *                       .set_attr_count_include_pad(false)
 *                       .set_attr_divisor_override(0)
 *                       .set_attr_data_format("NDHWC");
 */
REG_OP(AvgPooling3D)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(ksize, AttrValue::LIST_INT)
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.ATTR(pads, AttrValue::LIST_INT ({ 0, 0, 0, 0, 0, 0 }))
.ATTR(pad_mode, AttrValue::STR { "SPECIFIC" })
.ATTR(ceil_mode, AttrValue::BOOL { false })
.ATTR(count_include_pad, AttrValue::BOOL { false })
.ATTR(divisor_override, AttrValue::INT { 0 })
.ATTR(data_format, AttrValue::STR { "NCDHW" })
.OP_END()

/*
 * Performs max pooling on the input and outputs both max values and indices.
 * <Input>
 *    x : Input tensor with size [N, C, H, W].
 * <Output>
 *    y : Output tensor, has the same type as input.
 *    argmax : Output tensor, index of the maximum location.
 * <Attr>
 *    ksize : The size of the window to take a max over. A tuple of two ints.
 *    strides : The stride of the window. A tuple of two ints.
 *    pads : Implicit zero padding to be added on both sides. A tuple of two ints.
 *    dtype : Data type of the output argmax. Int32 or Int64.
 *    dilation : A parameter that controls the stride of elements in the window. A tuple of two ints.
 *    ceil_mode : When True, will use ceil instead of floor to compute the output shape.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(MaxPoolWithArgmaxV2)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OUTPUT(argmax, TensorType({ DT_INT32, DT_INT64 }))
.REQUIRED_ATTR(ksize, AttrValue::LIST_INT)
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.REQUIRED_ATTR(pads, AttrValue::LIST_INT)
.ATTR(dtype, AttrValue::INT { DT_INT32 })
.ATTR(dilation, AttrValue::LIST_INT ({ 1, 1 }))
.ATTR(ceil_mode, AttrValue::BOOL { false })
.OP_END()

/*
 * Computes a partial inverse in which all non-maximal values are set to zero.
 * <Input>
 *    x : Input tensor with size [N, C, H, W].
 *    argmax : Input tensor, index of the maximum location.
 * <Output>
 *    y : Output tensor.
 * <Attr>
 *    ksize : Size of the max pooling window. A tuple of two ints.
 *    strides : Stride of the max pooling window. A tuple of two ints.
 *    pads : Padding that was added to the input. A tuple of two ints.
 *    output_shape : The output shape. A tuple of two ints or four ints.
 *    data_format : Format of operator, 'NCHW' or 'NHWC'. Now only support NCHW
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(MaxUnpool2D)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(argmax, TensorType({ DT_INT32, DT_INT64 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(ksize, AttrValue::LIST_INT)
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.REQUIRED_ATTR(pads, AttrValue::LIST_INT)
.ATTR(output_shape, AttrValue::LIST_INT ({}))
.ATTR(data_format, AttrValue::STR { "NCHW" })
.OP_END()

/*
 * Consumes an input tensor and a filter, and computes the output.
 * <Input>
 *    x : Input tensor with size [N, Ci, Hi, Wi].
 *    filter : With shape [Co, Ci/group, Hk, Wk], must be a Const-OP.
 *    bias : With shape [Co], must be a Const-OP.
 * <Output>
 *    y : The output tensor.
 * <Attr>
 *    strides : Stride along each axis.
 *    dilations : Dilation value along each axis of the filter.
 *    pads : Padding for the beginning and ending along each axis [hh, ht, wh, wt].
 *    pad_mode : Pad mode, SPECIFIC(Default): not set, using pads; SAME, or VALID
 *    groups : Number of groups input channels and output channels are divided into.
 *             When groups = 1, traditional convolution will be performed;
 *             When groups > 1, feature maps are grouped by group_count, and then each groups
 *             is convoluted separately. Specially, 'groups' equal to the number of input feature
 *             maps indicates Depthwise Convolution.
 *    data_format : Format of operator, 'NCHW' or 'NHWC'. Default is 'NCHW'
 *    quantType : quantize type 1:INT8_8;2:INT8_2;3:INT4_4;4:OneSideQuant;6:INT16_8
 *    QuantizeInfo : Quantize info, including input scale/offset and weight scale/offset.
 *    quantizeInfoExt : Quantize extended info, using for compute library calculate.
 * <Added in HiAI version>
 *    100.520.010.000
 */
REG_OP(WinoConvolution)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT16, DT_UINT8 }))
.INPUT(filter, TensorType({ DT_INT8 }))
.OPTIONAL_INPUT(bias, TensorType({ DT_FLOAT, DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT32, DT_INT16 }))
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.ATTR(dilations, AttrValue::LIST_INT ({ 1, 1 }))
.ATTR(pads, AttrValue::LIST_INT ({ 0, 0, 0, 0 }))
.ATTR(pad_mode, AttrValue::STR { "SPECIFIC" })
.ATTR(groups, AttrValue::INT { 1 })
.ATTR(data_format, AttrValue::STR { "NCHW" })
.ATTR(quantType, AttrValue::INT { 1 })
.ATTR(QuantizeInfo, AttrValue::NamedAttrs())
.ATTR(quantizeInfoExt, AttrValue::STR { "" })
.OP_END()

/*
 * This operation outputs its input tensor such as Identity
 * <Input>
 *    x : The input tensor.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(StopGradient)
.INPUT(x, TensorType({ DT_FLOAT, DT_DOUBLE, DT_INT8, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_DOUBLE, DT_INT8, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.OP_END()
} // namespace hiai
// clang-format on

#endif