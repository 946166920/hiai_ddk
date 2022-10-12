/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: ge_attr_define, common attribute define
 */
#ifndef GE_ATTR_DEFINE_H_
#define GE_ATTR_DEFINE_H_
#include "framework/graph/debug/ge_graph_attr_define.h"
#include "infra/alias/aicp.h"
namespace hiai {
/*
 * 公共属性
 */

// npucl ann
const char * const ATTR_NAME_RELUMODE = "relu_mode";
const char * const ATTR_NAME_MULTIPLES = "multiples";
const char * const FULL_CONNECTION_ATTR_RELU_FLAG = "relu_flag";
const char * const SOFTMAX_ATTR_MODE = "mode";
const char * const COMPARE_OP_TYPE = "compare_type";
const char * const PRELU_ATTR_WEIGHT_DATATYPE = "weight_data_type";
const char * const PRELU_ATTR_ANN_DATA = "ann_data";
const char * const ANN_MEAN_KEEPDIMS = "AnnMeanKeepDims";
const char * const PAD_ATTR_PADDINGDS = "paddings";
const char * const PAD_ATTR_CONSTANT_VALUE = "padvalue";
const char * const LSTM_CELL_CLIP = "lstm_cell_clip";
const char * const LSTM_PROJ_CLIP = "lstm_proj_clip";
const char * const LSTM_USE_PROJECTION = "use_projection";
const char * const LSTM_USE_PROJECTION_BIAS = "use_projection_bias";
const char * const LSTM_MODE = "mode";
const char * const LSTM_BASIC_MODE = "Basic";
const char * const LSTM_UNIDIRECTIONAL_MODE = "UniDirectional";
const char * const CHANNEL_SHUFFLE_NUM_GROUP = "group";
const char * const LSTM_MERGE_OUTPUT = "merge_output";

// framework npucl ann
const char * const ATTR_NAME_ALPHA = "alpha";
const char * const ATTR_NAME_BETA = "beta";
const char * const ATTR_NAME_AXIS = "axis";
const char * const CONV_ATTR_NAME_RELU_FLAG = "relu_flag";
const char * const PRELU_ATTR_CHANNEL_SHARED = "channel_shared";
const char * const LSTM_ACTIVATE = "activation";
const char * const CONV_ATTR_NAME_HAS_BIAS = "has_bias";

// framework npucl
const char * const ATTR_IMG_H = "img_h";
const char * const ATTR_IMG_W = "img_w";
const char * const ATTR_NAME_MODE = "mode";
const char * const ATTR_NAME_NC1HWC0_TO_NCHW = "nc1hwc0_to_nchw";
const char * const ATTR_AIPP_COMPATIBLE = "aipp_compatible_flag";
const char * const ATTR_NAME_COEF = "coef";
const char * const ROIPOOLING_ATTR_NAME_SPATIAL_SCALE = "spatial_scale";
const char * const ROIPOOLING_ATTR_NAME_RIO_POOLING_MODE = "rio_pooling_mode";
const char * const DETECTIONOUTPUT_ATTR_NUM_CLASSES = "num_classes";
const char * const DETECTIONOUTPUT_ATTR_NMS_THRESHOLD = "nms_threshold";
const char * const DETECTIONOUTPUT_ATTR_IMG_H = "img_h";
const char * const DETECTIONOUTPUT_ATTR_IMG_W = "img_w";
const char * const DETECTIONOUTPUT_ATTR_YOLO_VERSION = "yolo_version";
const char * const SPTIALTF_ATTR_NAME_OUTPUT_H = "output_h";
const char * const SPTIALTF_ATTR_NAME_OUTPUT_W = "output_w";
const char * const SPTIALTF_ATTR_NAME_AFFINE_TRANSFORM = "affine_transform";
const char * const SSD_PRIOR_BOX_ATTR_VARIANCE_NUM = "variance_num";
const char * const SPLIT_ATTR_NAME_SLICE_POINT = "slice_point";
const char * const GENERATE_RPN_PROPOSAL_ATTR_PRE_NMS_TOPK = "pre_nms_topk";
const char * const GENERATE_RPN_PROPOSAL_ATTR_POST_NMS_TOPK = "post_nms_topk";
const char * const GENERATE_RPN_PROPOSAL_ATTR_RPN_MINI_SIZE = "rpn_mini_size";
const char * const GENERATE_RPN_PROPOSAL_ATTR_RPN_PROPOSAL_NMS_THRESH = "rpn_proposal_nms_thresh";
const char * const DECODE_BBOX_ATTR_DECODECLIP = "decodeClip";
const char * const SUM_ATTR_TIDX = "Tidx";
const char * const FLATTEN_END_AXIS = "end_axis";
const char * const RNN_TENSORFLOW = "rnn_tensorflow";
const char * const RNN_MODE_STATIC = "rnn_static";
const char * const MUTI_RNN = "multi_rnn";
const char * const CELL_MODE = "mode";
const char * const RNN_HT = "ht";
const char * const RNN_XT_HT = "xt_ht";
const char * const RNN_BATCH_SIZE = "batch_size";
const char * const FILLER_TYPE = "filler_type";
const char * const DETECTIONOUTPUT_ATTR_BIASES = "biases";

// framework
const char * const SSD_PRIOR_BOX_ATTR_MIN_SIZE_NUM = "min_size_num";
const char * const SSD_PRIOR_BOX_ATTR_MAX_SIZE_NUM = "max_size_num";
const char * const SSD_PRIOR_BOX_ATTR_ASPECT_RATIO_NUM = "aspect_ratio_num";

// ann
const char * const CONST_ATTR_NAME_OUTPUT_FORMAT = "output_format";
} // namespace hiai
#endif