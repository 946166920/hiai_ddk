/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: ge_graph_attr_define, graph attribute define
 */
#ifndef GE_GRAPH_ATTR_DEFINE_H_
#define GE_GRAPH_ATTR_DEFINE_H_

namespace hiai {
//  cpucl, framework, npucl, ann
const char * const ATTR_NAME_FORMAT = "format";

// cpucl, framework, npucl
const char * const ATTR_NAME_AIPP = "aipp";
const char * const ATTR_NAME_WEIGHTS = "value";
const char * const MERGED_OFFSET = "merged_offset";
const char * const ATTR_NAME_INFERRED_FORMAT = "inferred_format";

// cpucl, npucl, ann
const char * const ATTR_NAME_ORG_FORMAT = "original_format";

// framework, npucl, ann
const char * const ATTR_NAME_INPUT_FORMAT = "input_format";
const char * const ATTR_NAME_OUTPUT_FORMAT = "output_format";
const char * const ATTR_NAME_IS_OUTPUT_DATATYPE_SET = "is_output_datatype_set";

// cpucl, npucl
const char * const ATTR_NAME_RELUFLAG = "relu_flag";

// cpucl, framework
const char * const ATTR_NAME_X_QUANT_TYPE = "x_QuantType";
const char * const WEIGHT_MERGED = "weight_merged";
const char * const SRC_MERGED_WEIGHT_ADDR = "src_merged_weight_addr";
const char * const SRC_MERGED_WEIGHT_SIZE = "src_merged_weight_size";

// framework, npucl
const char * const ATTR_MODEL_MEMORY_SIZE = "memory_size";
const char * const ATTR_MODEL_INPUT_OUTPUT_SIZE = "input_output_size";
const char * const ATTR_MODEL_WEIGHT_SIZE = "weight_size";
const char * const ATTR_MODEL_WEIGHT_REAL_SIZE = "weight_real_size";
const char * const TARGET_TYPE_LITE = "LITE";
const char * const TARGET_TYPE_SECURITY = "SECURITY";
const char * const AIPP_MODEL_DATA_DIM_W = "aipp_model_data_dim_w";
const char * const AIPP_MODEL_DATA_DIM_H = "aipp_model_data_dim_h";
const char * const AIPP_CONV_FLAG = "Aipp_Conv_Flag";
const char * const AIPP_DYN_INPUT = "aipp_dyn_input";
const char * const SUBGRAPH_NAME = "subgraph_name";
const char * const SUBGRAPH = "subgraph";
const char * const ATTR_ORIGINAL_OP_NAME_LIST = "original_op_name_list";
const char * const GRAPHOP_TASK_SIZE = "graphop_task_size";
const char * const GRAPHOP_WEIGHT_SIZE = "graphop_weight_size";
const char * const GRAPHOP_TASK_OFFSET = "graphop_task_offset";
const char * const GRAPHOP_WEIGHT_OFFSET = "graphop_weight_offset";
const char * const SSD_RESHAPE_SLICE_CONCAT_FUSION = "reshape_slice_concat_fusion";
const char * const SSD_MBOX_LOC_FUSION = "permute_flatten_fusion";
const char * const SSD_MBOX_FUSION_BOX_TYPE_NUM = "ssd_mbox_fusion_box_type_num";
const char * const SSD_USE_POLYGON_FUSION = "use_polygon_fusion";
const char * const SSD_MBOX_CONF_FUSION = "permute_flatten_reshape_flatten_fusion";
const char * const DETECTIONOUTPUT_ATTR_USE_POLYGON = "use_polygon";
const char * const FASTRCNN_PREDICTIONS_ATTR_TOPK = "fsr_topk";
const char * const FASTRCNN_PREDICTIONS_ATTR_SCORE_THRESHOLD = "fsr_score_thres";
const char * const FASTRCNN_PREDICTIONS_ATTR_NMS_THRESHOLD = "fsr_nms_thres";
const char * const ATTR_NAME_IS_OUTPUT_FORMAT_SET = "is_output_format_set";
const char * const FILLER_VALUE = "filler_value";

// npucl, ann
const char * const RESIZE_BILINEAR_ATTR_HEIGHT = "height";
const char * const RESIZE_BILINEAR_ATTR_WIDTH = "width";

// cpucl
const char * const AIPP_MODE = "aipp_mode";
const char * const ATTR_RUN_FORMAT = "run_format";
const char * const ATTR_NAME_W_QUANT_TYPE = "w_QuantType";

// framework
const char * const AIPP_INPUT_NAME = "aipp_input_name";
const char * const ATTR_MODEL_TARGET_TYPE = "target_type";
const char * const ATTR_MODEL_STREAM_NUM = "stream_num";
const char * const ATTR_MODEL_EVENT_NUM = "event_num";
const char * const ATTR_NAME_WEIGHTS_DATA = "weights_data";
const char * const ATTR_MODEL_CACHE_OFFSET = "model_cache_offset";
const char * const TARGET_TYPE_MINI = "MINI";
const char * const MERGE_DEAD_INDEX = "merge_dead_index";
const char * const DST_WEIGHT_MEMORY_ADDR = "dst_weight_memory_addr";
const char * const ATTR_NAME_FILTER_FORMAT = "filter_format";
const char * const NOT_NET_OUTPUT = "not_net_output";
} // namespace hiai
#endif