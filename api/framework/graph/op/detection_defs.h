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

#ifndef INC_API_GRAPH_OP_DETECTION_DEFS_H
#define INC_API_GRAPH_OP_DETECTION_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Permutes the dimensions of the input according to a given pattern.
 * <Input>
 *    x : Input tensor.
 * <Output>
 *    y : Has the same shape as the input, but with the dimensions re-ordered according to the specified pattern.
 * <Attr>
 *    order : Tuple of dimension indices indicating the permutation pattern, list of dimension indices.
 *            When order is -1, it means reverse order.
 * <Added in HiAI version>
 *    100.300.010.011
 * <Examples>
 *    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
 *    hiai::op::Data x = hiai::op::Data("x");
 *    x.update_input_desc_x(xDesc);
 *
 *    auto permute = hiai::op::Permute("permute")
 *                   .set_input_x(x)
 *                   .set_attr_order({0, 3, 1, 2});
 */
REG_OP(Permute)
.INPUT(x, TensorType({ DT_FLOAT, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_UINT8, DT_INT32, DT_INT64, DT_BOOL }))
.ATTR(order, AttrValue::LIST_INT ({ 0 }))
.OP_END()

/*
 * A layer in Faster R-CNN net, the role of FSRDetectionOutput is to generate the  number and coordinate of label boxes
 * according to position offset of prior box and adjusting parameters(nms threshold and confidence threshold).
 * <Input>
 *    rois : The coordinate of prior boxes.
 *    bbox_delta : The position offset.
 *    score : The probability score of all prior boxes.
 *    im_info : The image information. Const Op. (img_height, img_width)
 *    actual_rois_num : Reserved.
 * <Output>
 *    actual_bbox_num : The number of output box.
 *    box : The coordinate of output box.
 * <Attr>
 *    num_classes : The number of classes, and must be larger than 1.
 *    score_threshold : Confidence threshold parameter, and must be between 0 and 1.
 *    iou_threshold : Non maximum suppression threshold and must be between 0 and 1.
 *    batch_rois : The parameter of batch_size, and must be between 1 and 65535.
 *                 N (number of rois) / batch_size should be a multiple of 16.
 * <Added in HiAI version>
 *    100.320.010.010
 */
REG_OP(FSRDetectionOutput)
.INPUT(rois, TensorType({ DT_FLOAT }))
.INPUT(bbox_delta, TensorType({ DT_FLOAT }))
.INPUT(score, TensorType({ DT_FLOAT }))
.INPUT(im_info, TensorType({ DT_INT32 }))
.OPTIONAL_INPUT(actual_rois_num, TensorType({ DT_FLOAT }))
.OUTPUT(actual_bbox_num, TensorType({ DT_INT32 }))
.OUTPUT(box, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(num_classes, AttrValue::INT)
.REQUIRED_ATTR(score_threshold, AttrValue::FLOAT)
.REQUIRED_ATTR(iou_threshold, AttrValue::FLOAT)
.ATTR(batch_rois, AttrValue::INT { 1 })
.OP_END()

/*
 * A layer in SSD net, the role of SSDDetectionOutput is to generate the  number and coordinate of label boxes
 * according to position offset of prior box and adjusting parameters(nms threshold and confidence threshold).
 * only support in CPUCL
 * <Input>
 *    bbox_delta : Frame position offset data.
 *    score : Confidence data.
 *    anchors : Preselection box data.
 * <Output>
 *    out_boxnum : The number of output box.
 *    y : Output box data.
 * <Attr>
 *    num_classes : The number of classes.
 *    share_location : if true, bounding box are shared among different classes.
 *    background_label_id : Background label id. If there is no background class,set it as -1.
 *    iou_threshold : Non maximum suppression threshold and must be between 0 and 1.
 *    top_k : Number of bboxes to be considered for per class before NMS.
 *    eta : Parameter for adaptive nms.
 *    variance_encoded_in_target : If true, variance is encoded in target;
 *    code_type : Type of coding method for bbox.
 *    keep_top_k : Number of total bboxes to be kept per image after nms step.
 *    confidence_threshold : Confidence threshold parameter, and must be between 0 and 1.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(SSDDetectionOutput)
.INPUT(bbox_delta, TensorType({ DT_FLOAT }))
.INPUT(score, TensorType({ DT_FLOAT }))
.INPUT(anchors, TensorType({ DT_FLOAT }))
.OUTPUT(out_boxnum, TensorType({ DT_INT32 }))
.OUTPUT(y, TensorType({ DT_FLOAT }))
.REQUIRED_ATTR(num_classes, AttrValue::INT)
.ATTR(share_location, AttrValue::BOOL { true })
.ATTR(background_label_id, AttrValue::INT { 0 })
.ATTR(iou_threshold, AttrValue::FLOAT { 0.3f })
.ATTR(top_k, AttrValue::INT { 200 })
.ATTR(eta, AttrValue::FLOAT { 1.0f })
.ATTR(variance_encoded_in_target, AttrValue::BOOL { false })
.ATTR(code_type, AttrValue::INT { 1 })
.ATTR(keep_top_k, AttrValue::INT { 200 })
.ATTR(confidence_threshold, AttrValue::FLOAT { 0.0f })
.OP_END()
} // namespace hiai
// clang-format on

#endif