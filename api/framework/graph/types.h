/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: types
 */
#ifndef GE_PUBLIC_TYPES_H
#define GE_PUBLIC_TYPES_H

#include <memory>
#include <vector>

namespace ge {
enum DataType {
    DT_UNDEFINED = 17, // Used to indicate a DataType field has not been set.
    DT_FLOAT = 0, // float type
    DT_FLOAT16 = 1, // fp16 type
    DT_INT8 = 2, // int8 type
    DT_INT16 = 6, // int16 type
    DT_UINT16 = 7, // uint16 type
    DT_UINT8 = 4, // uint8 type
    DT_INT32 = 3,
    DT_INT64 = 9, // int64 type
    DT_UINT32 = 8, // unsigned int32
    DT_UINT64 = 10, // unsigned int64
    DT_BOOL = 12, // bool type
    DT_DOUBLE = 11, // double type
    DT_DUAL = 13, /* dual output type */
    DT_DUAL_SUB_INT8 = 14, /* dual output int8 type */
    DT_DUAL_SUB_UINT8 = 15, /* dual output uint8 type */
    DT_2BIT = 21, // 2BIT type
    DT_INT4 = 22, // int4 type
    DT_QUINT8 = 23, // quint8 for ann
    DT_RESOURCE = 24, // dataflow resource flow
};

enum Format {
    FORMAT_NCHW = 0, /* NCHW */
    FORMAT_NHWC, /* NHWC */
    FORMAT_ND, /* Nd Tensor */
    FORMAT_NC1HWC0, /* NC1HWC0 */
    FORMAT_FRACTAL_Z, /* FRACTAL_Z */
    FORMAT_NC1C0HWPAD,
    FORMAT_NHWC1C0,
    FORMAT_FSR_NCHW,
    FORMAT_FRACTAL_DECONV,
    FORMAT_C1HWNC0,
    FORMAT_FRACTAL_DECONV_TRANSPOSE,
    FORMAT_FRACTAL_DECONV_SP_STRIDE_TRANS,
    FORMAT_NC1HWC0_C04, /* NC1HWC0, C0 =4 */
    FORMAT_FRACTAL_Z_C04, /* FRACZ格式，C0 =4 */
    FORMAT_CHWN,
    FORMAT_FRACTAL_DECONV_SP_STRIDE8_TRANS,
    FORMAT_HWCN,
    FORMAT_NC1KHKWHWC0, /* KH,KW kernel h& kernel w maxpooling max output format */
    FORMAT_BN_WEIGHT,
    FORMAT_FILTER_HWCK, /* filter input tensor format */
    FORMAT_HASHTABLE_LOOKUP_LOOKUPS = 20,
    FORMAT_HASHTABLE_LOOKUP_KEYS,
    FORMAT_HASHTABLE_LOOKUP_VALUE,
    FORMAT_HASHTABLE_LOOKUP_OUTPUT,
    FORMAT_HASHTABLE_LOOKUP_HITS = 24,
    FORMAT_FRACTAL_Z_SPARSE,
    FORMAT_WINO = 27,
    FORMAT_NC4HW4, /* For CPUCL Kernel */
    FORMAT_FRACTAL_Z_N_TRANS,
    FORMAT_FRACTAL_Z_N_NORMAL,
    FORMAT_NC8HW8, /* For CPUCL fp16 Kernel */
    FORMAT_NCDHW = 40,
    FORMAT_NDHWC,
    FORMAT_NDC1HWC0,
    FORMAT_DHWCN,
    FORMAT_FRACTAL_Z_5D,
    FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2,
    FORMAT_WINO_V2,
    FORMAT_FRACTAL_Z_S16, /* for S16S8 filter */
    FORMAT_WINO_V2_S16, /* for wino S16S8 filter */
    FORMAT_FRACTAL_Z_DW_DIAG, // for dw diagonal
    FORMAT_FRACTAL_Z_DW_V2, // for dw v2 kernel
    FRACTAL_NZ, /* L0C format */
    FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0,
    FORMAT_FRACTAL_Z_DECONV_SP_K2S2P0_S16S8,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P0_S16S8,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P1_S16S8,
    FORMAT_FRACTAL_Z_DECONV_SP_K4S2P2_S16S8,
    FORMAT_FRACTAL_Z_DECONV_SP_K3S3P0_S16S8,
    FORMAT_RESERVED
};

enum DeviceType {
    NPU = 0,
    CPU = 1,
    GPU = 2,
    DEVICE_TYPE_RESERVED,
    // DeviceType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
    // DeviceType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};

enum WeightDataType {
    FP32,
    FP16
};

enum AnchorStatus {
    ANCHOR_SUSPEND = 0, // dat null
    ANCHOR_CONST = 1,
    ANCHOR_DATA = 2, // 有效的
    ANCHOR_RESERVED = 3
};

enum PrecisionMode {
    PRECISION_MODE_FP32 = 0, // 设置精度配置选项为FP32，当前对CPUCL生效
    PRECISION_MODE_PREFER_FP16, // 设置精度配置选项为FP16，当算子不支持时，回退到FP32计算，当前对CPUCL生效
};

enum class TuningStrategy {
    OFF = 0,
    ON_DEVICE_TUNING,
    ON_DEVICE_PREPROCESS_TUNING,
    ON_CLOUD_TUNING
};

enum class ExecuteDeviceConfigMode {
    AUTO,
    MODEL_LEVEL,
    OP_LEVEL
};

enum class ExecuteFallBackMode {
    ENABLE,
    DISABLE
};

enum class DeviceMemoryReusePlan {
    UNSET,
    LOW,
    HIGH
};

struct TensorType {
    explicit TensorType(DataType dt)
    {
        dtVec.push_back(dt);
    };

    TensorType(std::initializer_list<DataType> types)
    {
        dtVec = types;
    };

    std::vector<DataType> dtVec;
};
} // namespace ge

#endif // GE_PUBLIC_TYPES_H