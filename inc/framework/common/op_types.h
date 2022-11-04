/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: common op_types
 */

#ifndef DOMI_COMMON_OP_TYPES_H_
#define DOMI_COMMON_OP_TYPES_H_

#include <cstdint>
#include <string>

#include "framework/common/register_op_types.h"

namespace hiai {
/*
 * @ingroup domi_omg
 * @brief DATA节点类型
 */
GE_API_VISIBILITY extern const char* const GRAPH_OP_TYPE;

/*
 * @ingroup domi_omg
 * @brief 默认卷积Group Size
 */
GE_API_VISIBILITY extern const uint32_t DEFAULT_CONV_GROUP;

/*
 * @ingroup domi_omg
 * @brief dim size默认值
 */
GE_API_VISIBILITY extern const int32_t DIM_DEFAULT_SIZE;

GE_API_VISIBILITY extern const int32_t DIM_5D_DEFAULT_SIZE;

/*
 * @ingroup domi_omg
 * @brief dim 扩展默认值
 */
GE_API_VISIBILITY extern const int32_t DIM_DEFAULT_VALUE;

/*
 * @ingroup domi_omg
 * @brief NCHW索引默认值
 */
GE_API_VISIBILITY extern const int32_t NCHW_DIM_N;
GE_API_VISIBILITY extern const int32_t NCHW_DIM_C;
GE_API_VISIBILITY extern const int32_t NCHW_DIM_H;
GE_API_VISIBILITY extern const int32_t NCHW_DIM_W;

/*
 * @ingroup domi_omg
 * @brief HWCK索引默认值
 */
GE_API_VISIBILITY extern const int32_t HWCK_DIM_H;
GE_API_VISIBILITY extern const int32_t HWCK_DIM_W;
GE_API_VISIBILITY extern const int32_t HWCK_DIM_C;
GE_API_VISIBILITY extern const int32_t HWCK_DIM_K;

/*
 * @ingroup domi_omg
 * @brief NHWC索引默认值
 */
GE_API_VISIBILITY extern const int32_t NHWC_DIM_N;
GE_API_VISIBILITY extern const int32_t NHWC_DIM_H;
GE_API_VISIBILITY extern const int32_t NHWC_DIM_W;
GE_API_VISIBILITY extern const int32_t NHWC_DIM_C;

/*
 * @ingroup domi_omg
 * @brief CHWN索引默认值
 */
GE_API_VISIBILITY extern const int32_t CHWN_DIM_N;
GE_API_VISIBILITY extern const int32_t CHWN_DIM_C;
GE_API_VISIBILITY extern const int32_t CHWN_DIM_H;
GE_API_VISIBILITY extern const int32_t CHWN_DIM_W;

/*
 * @ingroup domi_omg
 * @brief CHW索引默认值
 */
GE_API_VISIBILITY extern const int32_t CHW_DIM_C;
GE_API_VISIBILITY extern const int32_t CHW_DIM_H;
GE_API_VISIBILITY extern const int32_t CHW_DIM_W;

/*
 * @ingroup domi_omg
 * @brief HWC索引默认值
 */
GE_API_VISIBILITY extern const int32_t HWC_DIM_H;
GE_API_VISIBILITY extern const int32_t HWC_DIM_W;
GE_API_VISIBILITY extern const int32_t HWC_DIM_C;

/*
 * @ingroup domi_omg
 * @brief NCDHW索引默认值
 */
GE_API_VISIBILITY extern const int32_t NCDHW_DIM_N;
GE_API_VISIBILITY extern const int32_t NCDHW_DIM_C;
GE_API_VISIBILITY extern const int32_t NCDHW_DIM_D;
GE_API_VISIBILITY extern const int32_t NCDHW_DIM_H;
GE_API_VISIBILITY extern const int32_t NCDHW_DIM_W;

/*
 * @ingroup domi_omg
 * @brief NDHWC索引默认值
 */
GE_API_VISIBILITY extern const int32_t NDHWC_DIM_N;
GE_API_VISIBILITY extern const int32_t NDHWC_DIM_D;
GE_API_VISIBILITY extern const int32_t NDHWC_DIM_H;
GE_API_VISIBILITY extern const int32_t NDHWC_DIM_W;
GE_API_VISIBILITY extern const int32_t NDHWC_DIM_C;

/*
 * @ingroup domi_omg
 * @brief DHWCN索引默认值
 */
GE_API_VISIBILITY extern const int32_t DHWCN_DIM_D;
GE_API_VISIBILITY extern const int32_t DHWCN_DIM_H;
GE_API_VISIBILITY extern const int32_t DHWCN_DIM_W;
GE_API_VISIBILITY extern const int32_t DHWCN_DIM_C;
GE_API_VISIBILITY extern const int32_t DHWCN_DIM_N;

/*
 * @ingroup domi_omg
 * @brief Pad索引默认值
 */
GE_API_VISIBILITY extern const int32_t PAD_H_HEAD;
GE_API_VISIBILITY extern const int32_t PAD_H_TAIL;
GE_API_VISIBILITY extern const int32_t PAD_W_HEAD;
GE_API_VISIBILITY extern const int32_t PAD_W_TAIL;

/*
 * @ingroup domi_omg
 * @brief window索引默认值
 */
GE_API_VISIBILITY extern const int32_t WINDOW_H;
GE_API_VISIBILITY extern const int32_t WINDOW_W;

/*
 * @ingroup domi_omg
 * @brief stride索引默认值
 */
GE_API_VISIBILITY extern const int32_t STRIDE_H;
GE_API_VISIBILITY extern const int32_t STRIDE_W;

/*
 * @ingroup domi_omg
 * @brief dilation索引默认值
 */
GE_API_VISIBILITY extern const int32_t DILATION_H;
GE_API_VISIBILITY extern const int32_t DILATION_W;

GE_API_VISIBILITY extern const uint32_t MODEL_VERSION; /* * 模型版本 1.0 */

// elem定义的常量
/*
 * @ingroup domi
 * @brief original tensor type
 */
enum DomiTensorFormat {
    DOMI_TENSOR_NCHW = 0, /* NCHW */
    DOMI_TENSOR_NHWC, /* NHWC */
    DOMI_TENSOR_ND, /* Nd Tensor */
    DOMI_TENSOR_NC1HWC0, /* NC1HWC0 */
    DOMI_TENSOR_FRACTAL_Z, /* FRACTAL_Z */
    DOMI_TENSOR_NC1C0HWPAD,
    DOMI_TENSOR_NHWC1C0,
    DOMI_TENSOR_FSR_NCHW,
    DOMI_TENSOR_FRACTAL_DECONV,
    DOMI_TENSOR_BN_WEIGHT,
    DOMI_TENSOR_CHWN, /* Android NN Depth CONV */
    DOMI_TENSOR_FILTER_HWCK, /* filter input tensor format */
    DOMI_TENSOR_NCDHW = 40,
    DOMI_TENSOR_NDHWC,
    DOMI_TENSOR_NDC1HWC0,
    DOMI_TENSOR_DHWCN,
    DOMI_TENSOR_FRACTAL_Z_5D,
    DOMI_TENSOR_RESERVED
};

/*
 * @ingroup domi
 * @brief Filter format
 */
enum DomiFilterFormat {
    DOMI_FILTER_KCHW, /* KCHW */
    DOMI_FILTER_HWCK, /* HWCK */
    DOMI_FILTER_DHWCK, /* DHWCK */
    DOMI_FILTER_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of activation
 */
enum DomiActivationMode {
    DOMI_ACTIVATION_SIGMOID = 0, /* sigmoid */
    DOMI_ACTIVATION_RELU, /* ReLU */
    DOMI_ACTIVATION_TANH, /* tanh */
    DOMI_ACTIVATION_CLIPPED_RELU, /* clipped ReLU */
    DOMI_ACTIVATION_ELU, /* ELU */
    DOMI_ACTIVATION_LEAKY_RELU,
    DOMI_ACTIVATION_ABS, /* Abs */
    DOMI_ACTIVATION_RELU1, /* relu1 */
    DOMI_ACTIVATION_SOFTSIGN, /* softsign */
    DOMI_ACTIVATION_SOFTPLUS, /* softplus */
    DOMI_ACTIVATION_HARDSIGMOID, /* hardsigmoid */
    DOMI_ACTIVATION_THRESHOLD_RELU, /* threshold */
    DOMI_ACTIVATION_SELU, /* selu */
    DOMI_ACTIVATION_LINEAR, /* linear */
    DOMI_ACTIVATION_RELU6, /* ReLU6 */
    DOMI_ACTIVATION_GELU, /* gelu */
    DOMI_ACTIVATION_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of batchnorm
 */
enum DomiBatchNormMode {
    DOMI_BATCHNORM_PER_ACTIVATION = 0, /* bnScale, bnBias tensor dims are 1xCxHxW */
    DOMI_BATCHNORM_SPATIAL, /* bnScale, bnBias tensor dims are 1xCx1x1 */
    DOMI_BATCHNORM_RESERVED
};

/*
 * @ingroup domi
 * @brief eltwise mode
 */
enum DomiEltwiseMode {
    DOMI_ELTWISE_PROD = 0, /* prod */
    DOMI_ELTWISE_SUM, /* sum */
    DOMI_ELTWISE_MAX, /* max */
    DOMI_ELTWISE_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of padding
 */
enum DomiPaddingMode {
    DOMI_PADDING_CEIL = 0, /* Default padding mode, same with caffe, same with MxNet full mode */
    DOMI_PADDING_DIRECTASSIGN, /* Default padding mode: NOTSET */
    DOMI_PADDING_VALID, /* VALID padding mode, same with tensorflow VALID mode */
    DOMI_PADDING_SAME, /* Padding values of 0 are always used */
    DOMI_PADDING_CEIL_NEW, /* Padding values of 0 are always used */
    DOMI_PADDING_VALID_NEW, /* Padding values of 0 are always used */
    DOMI_PADDING_SAME_NEW, /* Padding values of 0 are always used */
    DOMI_PADDING_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of convolution
 */
enum DomiConvolutionMode {
    DOMI_CONV_CONVOLUTION = 0, /* math convolution */
    DOMI_CONV_CROSS_CORRELATION, /* cross-correlation convolution */
    DOMI_CONV_DECONVOLUTION, /* deconvolution, also named transposed convolution */
    DOMI_CONV_MODE_DEPTHWISE, /* depthwise convolution */
    DOMI_CONV_MODE_RESERVED
};

/*
 * @ingroup domi
 * @brief original data type
 */
enum DomiDataType {
    DOMI_DATA_FLOAT = 0, /* float type */
    DOMI_DATA_HALF, /* fp16 type */
    DOMI_DATA_INT8, /* int8 type */
    DOMI_DATA_INT32, /* int32 type */
    DOMI_DATA_UINT8, /* uint8 type */
    DOMI_DATA_HALF_UINT16_PROPOSAL, /* mixed type for proposal */
    DOMI_DATA_INT16, /* int16 type */
    DOMI_DATA_UINT16, /* uint16 type */
    DOMI_DATA_UINT32, /* uint32 type */
    DOMI_DATA_INT64, /* int64 type */
    DOMI_DATA_UINT64, /* uint64 type */
    DOMI_DATA_DOUBLE, /* double type */
    DOMI_DATA_BOOL, /* bool type */
    DOMI_DATA_STRING, /* tring type */
    DOMI_DATA_RESOURCE, /* resource type */
    DOMI_DATA_INT64_REF = 101, /* int64 ref */
    DOMI_DATA_INT32_REF = 103, /* int64 ref */
    DOMI_DATA_FLOAT_REF = 109, /* float ref */
    DOMI_DATA_VARIANT, /* variant type */
    DOMI_DATA_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of pooling
 */
enum DomiPoolingMode {
    DOMI_POOLING_MAX = 0, /* max pooling */
    DOMI_POOLING_AVG, /* average pooling */
    DOMI_POOLING_L2, /* L2 pooling */
    DOMI_POOLING_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of pad
 */
enum DomiPadMode {
    DOMI_PAD_CONSTANT = 0, /* CONSTANT */
    DOMI_PAD_REFLECT, /* REFLECT */
    DOMI_PAD_SYMMETRIC, /* SYMMETRIC */
    DOMI_PAD_EDGE, /* EDGE */
    DOMI_PAD_MODE_RESERVED
};

/*
 * @ingroup domi
 * @brief mode of cropandresize
 */
enum DomiCropAndResizeMode {
    DOMI_RESIZE_METHOD_BILINEAR = 0, /* resize bilinear */
    DOMI_RESIZE_METHOD_NEAREST, /* resize nearest */
    DOMI_RESIZE_RESERVED
};

enum DomiPoolingCeilMod {
    DOMI_POOLING_FLOOR = 0,
    DOMI_POOLING_CEIL
};

enum DomiPermuteType {
    DOMI_NCHW_TO_NHWC_PERMUTE = 0,
    DOMI_NHWC_TO_NCHW_PERMUTE,
    DOMI_PERMUTE_RESERVED
};
} // namespace hiai
#endif // DOMI_OMG_COMMON_OP_TYPES_H_
