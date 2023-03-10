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

#ifndef GE_OP_TYPES_H_
#define GE_OP_TYPES_H_
#include <climits>

namespace ge {
#define GE_REGISTER_OPTYPE(varName, strName) static const char* varName __attribute__((unused)) = strName;

GE_REGISTER_OPTYPE(DATA, "Data");
GE_REGISTER_OPTYPE(FUSIONBATCHNORM, "FusionBatchNorm");
GE_REGISTER_OPTYPE(DROPOUT, "Dropout");
GE_REGISTER_OPTYPE(TRANSDATA, "TransData");
GE_REGISTER_OPTYPE(SSDPRIORBOX, "SSDPriorBox");
GE_REGISTER_OPTYPE(NETOUTPUT, "NetOutput");
GE_REGISTER_OPTYPE(ROIALIGN, "ROIAlign");
GE_REGISTER_OPTYPE(RPNPROPOSALS, "GenerateRpnProposals");
GE_REGISTER_OPTYPE(FASTRCNNPREDICTIONS, "FastrcnnPredictions");
GE_REGISTER_OPTYPE(REGION, "Region")
GE_REGISTER_OPTYPE(HIGHWAY, "HighWay");
GE_REGISTER_OPTYPE(RNN, "RNN");
GE_REGISTER_OPTYPE(ATTENTIONDECODER, "AttentionDecoder");
GE_REGISTER_OPTYPE(AIPP_CONFIG_OP, "AippConfig");
GE_REGISTER_OPTYPE(COPY, "Copy");
GE_REGISTER_OPTYPE(SSDPOSTPROCESSOR, "SSDPostProcessor");
GE_REGISTER_OPTYPE(ZEROSLIKE, "ZerosLike");
GE_REGISTER_OPTYPE(MOVE, "Move");
GE_REGISTER_OPTYPE(WHERE, "Where");
GE_REGISTER_OPTYPE(RELU, "ReLU");
GE_REGISTER_OPTYPE(TANH, "TanH");
GE_REGISTER_OPTYPE(NNREDUCESUM, "NNReduceSum");
GE_REGISTER_OPTYPE(DETECTIONOUTPUT, "DetectionOutput");
GE_REGISTER_OPTYPE(YOLOPOSTDETECTIONOUTPUT, "YoloPostDetectionOutput");
/* ANN专用算子 */
GE_REGISTER_OPTYPE(ANN_FULLCONNECTION, "AnnFullConnection");
GE_REGISTER_OPTYPE(ANN_CONVOLUTION, "AnnConvolution");
GE_REGISTER_OPTYPE(ANN_DATA, "AnnData");
/* 训练算子 */
GE_REGISTER_OPTYPE(MULTISHAPE, "MultiShape");
GE_REGISTER_OPTYPE(ASSERT, "Assert");
GE_REGISTER_OPTYPE(REQUIREDSPACETOBATCHPADDINGS, "RequiredSpaceToBatchPaddings");
GE_REGISTER_OPTYPE(IDENTITY, "Identity");
GE_REGISTER_OPTYPE(AIPP_CONFIG, "AippConfig");
GE_REGISTER_OPTYPE(GENERATEPROPOSAL, "GenerateProposal");

// framework
const char * const INPUT_TYPE = "Input";
const char * const OP_CONF_DELIMITER = ":";
const char * const AIPP_CONV_OP_NAME = "aipp_conv_op";

// framework, npucl
const char * const NODE_NAME_NET_OUTPUT = "Node_Output";
const char * const AIPP_DATA_FLAG = "aipp_data_flag";
const char * const AIPP_MODEL_DATA_DIM_N = "aipp_model_data_dim_n";
const char * const AIPP_MODEL_DATA_DIM_W = "aipp_model_data_dim_w";
const char * const AIPP_MODEL_DATA_DIM_H = "aipp_model_data_dim_h";
const char * const AIPP_MODEL_DATA_DIM_C = "aipp_model_data_dim_c";
const char * const AIPP_MODEL_DATA_TYPE = "aipp_model_data_type";
const char * const AIPP_ATTR_MAX_IMAGE_SIZE = "max_src_image_size";
}; // namespace ge

#endif // GE_OP_TYPES_H_