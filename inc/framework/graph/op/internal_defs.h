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
#ifndef INC_FRAMEWORK_GRAPH_OP_INTERNAL_DEFS_H
#define INC_FRAMEWORK_GRAPH_OP_INTERNAL_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Return element-wise remainder of division.
 * <Input>
 *    x1 : The input tensor.
 *    x2 : The input tensor has same element type as x.
 * <Output>
 *    y : Result has same element type as x.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(TruncateMod)
.INPUT(x1, TensorType({ DT_INT32, DT_FLOAT }))
.INPUT(x2, TensorType({ DT_INT32, DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_INT32, DT_FLOAT }))
.OP_END()

/*
 * Switch Operator
 * <Input>
 *    data : Input tensor.
 *           The tensor to be forwarded to the appropriate output.
 *    pred : A scalar that specifies which output port will receive data.
 * <Output>
 *    outputFalse : If pred is false, data will be forwarded to this output.
 *    outputTrue : If pred is true, data will be forwarded to this output.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Switch)
.INPUT(data, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.INPUT(pred, TensorType({ DT_BOOL }))
.OUTPUT(outputFalse, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(outputTrue, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OP_END()

/*
 * Merge Operator
 * <Input>
 *    x : The input tensors, exactly one of which will become available.
 * <Output>
 *    y : Will be set to the available input tensor.
 *    value_index : The index of the chosen input tensor in inputs.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Merge)
.DYNAMIC_INPUT(x, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(value_index, TensorType({ DT_INT32 }))
.OP_END()

/*
 * Enter Operator
 * <Input>
 *    x : input tensor.
 * <Output>
 *    y : output tensor.
 * <Attr>
 *    frame_name : the frame name of the loop body.
 *    is_constant : Describes whether the input of the Enter operator is a const operator.The default value is false.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Enter)
.INPUT(x, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.REQUIRED_ATTR(frame_name, AttrValue::STR)
.ATTR(is_constant, AttrValue::BOOL { false })
.OP_END()

/*
 * Exit Operator
 * <Input>
 *    x : input tensor.
 * <Output>
 *    y : output tensor.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Exit)
.INPUT(x, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OP_END()

/*
 * NextIteration Operator
 * <Input>
 *    x : The tensor to be made available to the next iteration.
 * <Output>
 *    y : The same tensor as data.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(NextIteration)
.INPUT(x, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OP_END()

/*
 * LoopCond Operator
 * <Input>
 *    x : A boolean scalar, representing the branch predicate of the Switch op.
 * <Output>
 *    y : The same tensor as input.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(LoopCond)
.INPUT(x, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT16, DT_FLOAT, DT_DOUBLE, DT_INT8, DT_INT16, DT_INT32, DT_INT64, DT_UINT8,
    DT_UINT16, DT_UINT32, DT_UINT64, DT_BOOL }))
.OP_END()

/*
 * Returns a tensor of the same shape and type with all elements set to one.
 * <Input>
 *    x : Input tensor, of the same type as Data
 * <Output>
 *    y : A Tensor with all elements set to 1.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(OnesLike)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT8, DT_UINT8, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT8, DT_UINT8, DT_BOOL }))
.OP_END()

/*
 * Consumes an input tensor and a filter, and computes the output.
 * <Input>
 *    x : Input tensor with size [N, Ci, Hi, Wi].
 *    filter : With shape [Co, Ci/group, Hk, Wk].
 *    bias : With shape [Co], must be a Const-OP.
 *    offset_w : Reserved. For quantized.
 * <Output>
 *    y : Output tensor
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
 *    offset_x : Reserved. For quantized.
 * <Added in HiAI version>
 *    100.500.010.011
 */
REG_OP(Correlation)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(filter, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(bias, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(offset_w, TensorType({ DT_INT8 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(strides, AttrValue::LIST_INT)
.ATTR(dilations, AttrValue::LIST_INT ({ 1, 1 }))
.ATTR(pads, AttrValue::LIST_INT ({ 0, 0, 0, 0 }))
.ATTR(pad_mode, AttrValue::STR { "SPECIFIC" })
.ATTR(groups, AttrValue::INT { 1 })
.ATTR(data_format, AttrValue::STR { "NCHW" })
.ATTR(offset_x, AttrValue::INT { 0 })
.OP_END()

/*
 * Multi frame joint the input image tensor, It use bilinear or nearest neighbor algorithm to support scale up and down
 * <Input>
 *    x : the input tensor.
 * <Output>
 *    y : the output tensor
 * <Attr>
 *    frame_num : frame number
 * <Added in HiAI version>
 *    100.xxx.xxx.xxx
 */
REG_OP(ImageMultiFrameJoint)
.INPUT(x, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_INT8, DT_FLOAT }))
.REQUIRED_ATTR(frame_num, AttrValue::INT)
.OP_END()

/*
 * Permutes the dimensions of the input according to a given pattern.
 * <Input>
 *    x : Input tensor.
 *    perm : Tuple of dimension indices indicating the permutation pattern, list of dimension indices.
 *           When order is -1, it means reverse order.
 * <Output>
 *    y : Has the same shape as the input, but with the dimensions re-ordered according to the specified pattern.
 * <Added in HiAI version>
 *    100.500.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc permTensorDesc(Shape({4}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr permTensor = std::make_shared<hiai::Tensor>(permTensorDesc);
 *    vector<int32_t> permDataValue = {0, 3, 1, 2};
 *    permTensor->SetData((uint8_t*)permDataValue.data(), 4 * sizeof(int32_t));
 *    auto strides = hiai::op::Const("perm").set_attr_value(permTensor);
 *
 *    auto Transpose = hiai::op::Transpose("transpose")
 *                   .set_input_x(x)
 *                   .set_input_perm(perm)
 */
REG_OP(Transpose)
.INPUT(x, TensorType({ DT_FLOAT, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.INPUT(perm, TensorType({ DT_INT32, DT_INT64 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.OP_END()

/*
 * Computes the 'logical and' of elements across dimensions of a tensor. Reduces 'input_tensor'
 * along the dimensions given in 'axis'.
 * <Input>
 *    x : Boolean tensor
 * <Output>
 *    y : Reduced tensor
 * <Attr>
 *    axes : The dimensions to reduce. If None (the default), reduces all dimensions.
 *           Must be in the range [-rank(x), rank(x)).
 *    keep_dims : If false,the rank of the tensor is reduced by 1 for each entry in axis.
 *                If true, the reduced dimensions are retained with length 1.
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_BOOL);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    auto reduceAllD = hiai::op::ReduceAllD("ReduceAllD")
 *                      .set_input_x(x)
 *                      .set_attr_axes({0, 1})
 *                      .set_attr_keep_dims(false);
 */
REG_OP(ReduceAllD)
.INPUT(x, TensorType({ DT_BOOL }))
.OUTPUT(y, TensorType({ DT_BOOL }))
.REQUIRED_ATTR(axes, AttrValue::LIST_INT)
.ATTR(keep_dims, AttrValue::BOOL { false })
.OP_END()

/*
 * Apply postprocessing steps to bounding box detections
 * <Input>
 *    score : the score of each anchor with each class.
 *    bbox_delta : the bounding box deltas.
 *    anchors : the shape of each predefined anchor.
 * <Output>
 *    detect_scores : the score of each output detections.
 *    rois : the coordinates of each output bounding box,with format [detect_scores, score, rois, bbox_delta].
 *    detect_class : the class label for each output detection.
 *    actual_rois_num : the number of valid output detections for each batch.
 * <Attr>
 *    scale_y : the scaling factor for dy in bounding box deltas.
 *    scale_x : the scaling factor for dx in bounding box deltas.
 *    scale_h : the scaling factor for dh in bounding box deltas.
 *    scale_w : the scaling factor for dw in bounding box deltas.
 *    max_num_detections : the maxmum number of boxes for the output Boxes with the lowest scores are descarded to
 *                         meet the limit.
 *    score_threshold : the score threshold, boxes with scores lower than the threshold are filtered before sending
 *                      to the NMS algorithm.
 *    iou_threshold : the iou threshold for hard NMS.
 *    use_regular_nms : setting to true to use regular multi-class NMS algorithm that do NMS separately for each class
 *                      and false for a faster algorithm that only do one single NMS using the highest class score
 *    max_classes_per_detection : using when use_regular_nms is set to false, specifying the maximum number of classes
 *                                per detection.
 *    max_detections_per_class : using when use_regular_nms is set to true, specifying the maximum number of detections
 *                               when applying NMS algorithm for each single class.
 *    is_bg_in_label : set to true to include background class in the list of label map for the output, set fo false
 *                     to not include the background.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(DetectionPostprocessing)
.INPUT(score, TensorType({ DT_FLOAT }))
.INPUT(bbox_delta, TensorType({ DT_FLOAT }))
.INPUT(anchors, TensorType({ DT_FLOAT }))
.OUTPUT(detect_scores, TensorType({ DT_FLOAT }))
.OUTPUT(rois, TensorType({ DT_FLOAT }))
.OUTPUT(detect_class, TensorType({ DT_INT32 }))
.OUTPUT(actual_rois_num, TensorType({ DT_INT32 }))
.REQUIRED_ATTR(scale_y, AttrValue::FLOAT)
.REQUIRED_ATTR(scale_x, AttrValue::FLOAT)
.REQUIRED_ATTR(scale_h, AttrValue::FLOAT)
.REQUIRED_ATTR(scale_w, AttrValue::FLOAT)
.REQUIRED_ATTR(max_num_detections, AttrValue::INT)
.REQUIRED_ATTR(score_threshold, AttrValue::FLOAT)
.REQUIRED_ATTR(iou_threshold, AttrValue::FLOAT)
.ATTR(use_regular_nms, AttrValue::BOOL { false })
.ATTR(max_classes_per_detection, AttrValue::INT { 1 })
.ATTR(max_detections_per_class, AttrValue::INT { 1 })
.ATTR(is_bg_in_label, AttrValue::BOOL { false })
.OP_END()

/*
 * Computes inverse hyperbolic cosine of the input tensor 'x' element-wise.
 * <Input>
 *    x : Input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Acosh)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes inverse hyperbolic sine of the input tensor 'x' element-wise.
 * <Input>
 *    x : Input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Asinh)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes and returns hyperbolic cosine of 'x' element-wise
 * <Input>
 *    x : Input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Cosh)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes and returns hyperbolic sine of 'x' element-wise.
 * <Input>
 *    x : Input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Sinh)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Returns a new tensor with the inverse hyperbolic tangent of input 'x'.
 * <Input>
 *    x : Input tensor, of range [-1, 1]
 * <Output>
 *    y : Tensor of the same type as 'x'
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Atanh)
.INPUT(x, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * z = a*x + y. x,y are the same dimension vectors,
 * a is the coefficient, a*x + y represents two vectors, and adds them by elements.
 * <Input>
 *    a : The input tensor,x1H,x1W must be 1.
 *    x : The input tensor,x2N,x2C must be the same with x1N,x1C.
 *    y : The input tensor,dimension vectors must be the same with x.
 * <Output>
 *    z : the output tensor
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(ChannelAxpy)
.INPUT(a, TensorType({ DT_FLOAT }))
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(y, TensorType({ DT_FLOAT }))
.OUTPUT(z, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes the maximum along segments of a tensor.
 * <Input>
 *    x : The input tensor.
 *    segment_ids : A 1-D const,not supports negative index,starts from 0
 *                  and should be sorted in ascending order and can be repeated.
 *                  whose size is equal to the size of x first dimension.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(SegmentMax)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(segment_ids, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes the minimum along segments of a tensor.
 * <Input>
 *    x : The input tensor.
 *    segment_ids : A 1-D const,not supports negative index,starts from 0
 *                  and should be sorted in ascending order and can be repeated.
 *                  whose size is equal to the size of x's first dimension.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(SegmentMin)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(segment_ids, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes the mean along segments of a tensor.
 * <Input>
 *    x : The input tensor.
 *    segment_ids : A 1-D const,not supports negative index,starts from 0
 *                  and should be sorted in ascending order and can be repeated.
 *                  whose size is equal to the size of x's first dimension.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(SegmentMean)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(segment_ids, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes the sum along segments of a tensor.
 * <Input>
 *    x : The input tensor.
 *    segment_ids : A 1-D const,not supports negative index,starts from 0
 *                  and should be sorted in ascending order and can be repeated.
 *                  whose size is equal to the size of x's first dimension.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(SegmentSum)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(segment_ids, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Computes the product along segments of a tensor.
 * <Input>
 *    x : The input tensor.
 *    segment_ids : A 1-D const,not supports negative index,starts from 0
 *                  and should be sorted in ascending order and can be repeated.
 *                  whose size is equal to the size of x's first dimension.
 * <Output>
 *    y : The output tensor.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(SegmentProd)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(segment_ids, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Returns the inverse permutation of a tensor.
 * <Input>
 *    x : Input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4}), FORMAT_NCHW, DT_INT32);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    auto invertPermutation = hiai::op::InvertPermutation("invertPermutation")
 *                             .set_input_x(x);
 */
REG_OP(InvertPermutation)
.INPUT(x, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_INT32 }))
.OP_END()

/*
 * Reverses variable length slices.
 * <Input>
 *    x : A rank R >0 tensor to reverse
 *    seq_lengths : 1-D with length input, seq_lengths.dimension[0] == x.dimension[batch_dim]
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    seq_dim : dimension partially reversed. 'seq_dim' should not equal to 'batch_dim'.
 *              'seq_dim' must be in range [-R, R).
 *    batch_dim : dimension along which reversal is performed, default to 0.
 *                'batch_dim' must not equal to 'seq_dim'.
 *                'batch_dim' must be in range [-R, R).
 * <Added in HiAI version>
 *    100.310.010.013
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    TensorDesc seqLengthsTensorDesc(Shape({5}), FORMAT_NCHW, DT_INT32);
 *    TensorPtr seqLengthsTensor = std::make_shared<hiai::Tensor>(seqLengthsTensorDesc);
 *    vector<int32_t> dataValue(5, 1);
 *    seqLengthsTensor->SetData((uint8_t*)dataValue.data(), 5 * sizeof(int32_t));
 *    auto seqLengths = hiai::op::Const("seqLengths").set_attr_value(seqLengthsTensor);
 *
 *    auto reverseSequence = hiai::op::ReverseSequence("reverseSequence")
 *                           .set_input_x(x)
 *                           .set_input_seq_lengths(seqLengths)
 *                           .set_attr_batch_dim(1)
 *                           .set_attr_seq_dim(2);
 */
REG_OP(ReverseSequence)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8 }))
.INPUT(seq_lengths, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT32, DT_UINT8 }))
.REQUIRED_ATTR(seq_dim, AttrValue::INT)
.ATTR(batch_dim, AttrValue::INT { 0 })
.OP_END()

/*
 * Extracts, rearranges, and combines input data.
 * <Input>
 *    x : Input tensor
 *    reindex : Index of input tensor
 * <Output>
 *    y : Output tensor
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(BatchReindex)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(reindex, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Scatter 'updates' into a new tensor according to 'indices'.
 * <Input>
 *    var : Input tensor.
 *    indices : An Index tensor.
 *    updates : A tensor.
 * <Output>
 *    var : A Tensor. Has the same type and format as input "var".
 * <Attr>
 *    use_locking : An optional bool. Defaults to "False". If "True", the operation will be protected by a lock.
 *    axis : Which axis to scatter on. Negative value means counting dimensions from the back.Accepted range is
 *           [-r, r-1] where r = rank(data).
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(ScatterUpdate)
.INPUT(var, TensorType({ DT_FLOAT, DT_INT8, DT_UINT8, DT_BOOL }))
.INPUT(indices, TensorType({ DT_INT32, DT_INT64 }))
.INPUT(updates, TensorType({ DT_FLOAT, DT_INT8, DT_UINT8, DT_BOOL }))
.OUTPUT(var, TensorType({ DT_FLOAT, DT_INT8, DT_UINT8, DT_BOOL }))
.ATTR(use_locking, AttrValue::BOOL { false })
.ATTR(axis, AttrValue::INT { 0 })
.OP_END()

/*
 * Performs fractional average or max pooling on the input.
 * <Input>
 *    x : 4-D Tensor with shape [batch, height, width, channels]
 * <Output>
 *    y : Output tensor
 *    row_pooling_sequence : Tensor of type int64
 *    col_pooling_sequence : Tensor of type int64
 * <Attr>
 *    mode : Either 0 (Max pooling) or 1 (Avg pooling)
 *    pooling_ratio : List of floats (>= 1.0) that has length 4. Only the row <= height  and column <= width
 *                    dimensions are supported. For example, a valid pooling ratio looks like [1.0, 1.44, 1.73, 1.0]
 *    pseudo_random : (Optional) bool. Defaults to False.
 *                    If True, generates the pooling sequence in a pseudorandom fashion, otherwise, in a
 *                    random fashion.
 *    overlapping : (Optional) bool. Defaults to False.
 *                  If True, it means when pooling, the values at the boundary of adjacent pooling cells
 *                  are used by both cells.
 *    deterministic : (Optional) bool. Defaults to False.
 *                    If True, a fixed pooling region will be used when iterating over a FractionalMaxPool
 *                    node in the computation graph.
 *    seed : (Optional) int. Defaults to 0. If either 'seed' or 'seed2' is set to be non-zero,
 *           the random number generator is seeded by the given seed. Otherwise, it is seeded
 *           by a random seed.
 *    seed2 : (Optional) int. Defaults to 1. Used to avoid seed collision.
 * <Added in HiAI version>
 *    100.320.010.010
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    auto permute = hiai::op::Permute("permute")
 *                   .set_input_x(x)
 *                   .set_attr_order({0, 2, 3, 1});
 *
 *    auto fractionalPooling = hiai::op::FractionalPooling("fractionalPooling")
 *                             .set_input_x(permute)
 *                             .set_attr_mode(1)
 *                             .set_attr_pooling_ratio({1.0, 1.3, 1.4, 1.0})
 *                             .set_attr_overlapping(true)
 *                             .set_attr_deterministic(true);
 */
REG_OP(FractionalPooling)
.INPUT(x, TensorType({ DT_FLOAT, DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_INT32 }))
.OUTPUT(row_pooling_sequence, TensorType({ DT_INT64 }))
.OUTPUT(col_pooling_sequence, TensorType({ DT_INT64 }))
.ATTR(mode, AttrValue::INT { 0 })
.ATTR(pooling_ratio, AttrValue::LIST_FLOAT ({}))
.ATTR(pseudo_random, AttrValue::BOOL { false })
.ATTR(overlapping, AttrValue::BOOL { false })
.ATTR(deterministic, AttrValue::BOOL { false })
.ATTR(seed, AttrValue::INT { 0 })
.ATTR(seed2, AttrValue::INT { 1 })
.OP_END()

/*
 * Pooling operations for some position sensitive areas of interest.
 * <Input>
 *    x : feature map, must be non const op, roisN can be devided by xN.
 *    rois : region of interest, must be non const op and roisC, roisH, roisW must be equal to 5, 1, 1.
 * <Output>
 *    y : the output tensor, all input and output must have same datatype.
 * <Attr>
 *    spatial_scale : spatial scale, must be greater than 0.
 *    output_dim : number of output channels, must be greater than 0.
 *    group_size : number of groups encoding position sensitive score maps, must be greater than 0.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(PSROIPooling)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(rois, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(spatial_scale, AttrValue::FLOAT)
.REQUIRED_ATTR(output_dim, AttrValue::INT)
.REQUIRED_ATTR(group_size, AttrValue::INT)
.OP_END()

/*
 * Bidirection Long Short-Term Memory layer, using an internal network unrolled in time.
 * <Input>
 *    x : The input tensor for LSTM cell. Shape is [B, T, X].
 *    seq_len : A batch element's sequence length. Shape is [B].
 *    w_fw : Forward weight tensor, must be const op. Two dimensions [x(t -1) + h(t -1), 4 * hidden-size].
 *    w_bw : Backward weight tensor, must be const op. Two dimensions [x(t -1) + h(t -1), 4 * hidden-size].
 *    c_0_fw : The forward initial memory cells for next input layer. Shape is [4 * hidden-size].
 *    h_0_fw : The forward initial state of memory cells for next input layer. Shape is [4 * hidden-size].
 *    c_0_bw : The backward initial memory cells for next input layer. Shape is [4 * hidden-size].
 *    h_0_bw : The backward initial state of memory cells for next input layer. Shape is [4 * hidden-size].
 * <Output>
 *    y_fw : Output of forward.
 *    y_bw : Output of backward.
 *    h_t_fw : Output state of forward hidden.
 *    c_t_fw : Output state of forward cell.
 *    h_t_bw : Output state of backward cell.
 *    c_t_bw : Output state of backward hidden.
 * <Attr>
 *    forget_bias : The forget gate bias.
 *    num_layers : Number of lstm layers, only support 1 now.
 *    activation : The activation function. "Sigmoid" "Tanh" "ReLU" "ReLU1" "ReLU6"
 *    cell_type : Reserved. Only supprot 'LSTM' now.
 *    state_is_tuple : Reserved. Only support true now.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(BidirectionLSTM)
.INPUT(x, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(seq_len, TensorType({ DT_INT32 }))
.INPUT(w_fw, TensorType({ DT_FLOAT }))
.INPUT(w_bw, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(c_0_fw, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(h_0_fw, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(c_0_bw, TensorType({ DT_FLOAT }))
.OPTIONAL_INPUT(h_0_bw, TensorType({ DT_FLOAT }))
.OUTPUT(y_fw, TensorType({ DT_FLOAT }))
.OUTPUT(y_bw, TensorType({ DT_FLOAT }))
.OUTPUT(h_t_fw, TensorType({ DT_FLOAT }))
.OUTPUT(c_t_fw, TensorType({ DT_FLOAT }))
.OUTPUT(h_t_bw, TensorType({ DT_FLOAT }))
.OUTPUT(c_t_bw, TensorType({ DT_FLOAT }))
.ATTR(forget_bias, AttrValue::FLOAT { 1.0 })
.ATTR(num_layers, AttrValue::INT { 1 })
.ATTR(activation, AttrValue::STR { "Tanh" })
.ATTR(cell_type, AttrValue::STR { "LSTM" })
.ATTR(state_is_tuple, AttrValue::BOOL { true })
.OP_END()

/*
 * Outputs random values from a uniform distribution.
 * <Input>
 *    shape : 1-D const, shape of the output tensor. Only a 1D-4D tensor can be generated.
 *    minval : 0-D const, lower bound on the range of random values to generate.
 *    maxval : 0-D const, upper bound on the range of random values to generate.
 * <Output>
 *    y : Result tensor of the same element shape as 'shape'.
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(RandomUniformNoSeed)
.INPUT(shape, TensorType({ DT_INT32 }))
.INPUT(minval, TensorType({ DT_FLOAT }))
.INPUT(maxval, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * generate random values from a normal distributions.
 * <Input>
 *    shape : 1-D const, shape of the output tensor. Only a 1D-4D tensor can be generated.
 *    mean : 0-D const. The mean of the normal distribution.
 *    stddev : 0-D const. The standard deviation of the normal distribution.
 * <Output>
 *    y : Result tensor of the same element shape as 'shape'.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(RandomNormalNoSeed)
.INPUT(shape, TensorType({ DT_INT32 }))
.INPUT(mean, TensorType({ DT_FLOAT }))
.INPUT(stddev, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Randomly shuffles a tensor along its first dimension
 * <Input>
 *    x : A tensor.
 * <Output>
 *    y : A tensor
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(RandomShuffleNoSeed)
.INPUT(x, TensorType({ DT_UINT8, DT_FLOAT, DT_BOOL, DT_INT32 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_FLOAT, DT_BOOL, DT_INT32 }))
.OP_END()

/*
 * Aipp
 * <Input>
 *    x : A tensor
 *    config : a aipp v2 param operator
 * <Output>
 *    y : A tensor
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(Aipp)
.INPUT(x, TensorType({ DT_UINT8, DT_FLOAT, DT_BOOL, DT_INT32 }))
.INPUT(config, TensorType({ DT_UINT8, DT_FLOAT, DT_BOOL, DT_INT32 }))
.OUTPUT(y, TensorType({ DT_UINT8, DT_FLOAT, DT_BOOL, DT_INT32 }))
.OP_END()

/*
 * AippConfigFusion
 * <Input>
 *    crop : A cropv2 param operator
 *    swap : A swapv2 param operator
 *    csc : A cscv2 param operator
 *    resize : A resizev2 param operator
 *    dtc : A dtcv2 param operator
 *    rotate : A rotatev2 param operator
 *    pad : A padv2 param operator
 * <Output>
 *    y : A tensor
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(AippConfigFusion)
.OPTIONAL_INPUT(crop, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(swap, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(csc, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(resize, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(dtc, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(rotate, TensorType({ DT_UINT8 }))
.OPTIONAL_INPUT(pad, TensorType({ DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_UINT8 }))
.OP_END()
} // namespace hiai
// clang-format on

#endif