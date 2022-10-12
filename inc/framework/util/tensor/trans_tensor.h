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

#ifndef TRANS_TENSOR_H
#define TRANS_TENSOR_H

#include <iostream>
#include "framework/common/hcs_types.h"
#include "framework/common/fmk_error_codes.h"
#include "graph/tensor.h"

#ifdef _MSC_VER
#include "common_win.h"
#endif

namespace ge {
/*
 * @ingroup fmk
 * @brief cube size
 */
const int CC_CUBE_SIZE = 16;

/*
 * @ingroup fmk
 * @brief original data type
 */
typedef enum tagDataType {
    CC_DATA_FLOAT = 0, /* *< float type */
    CC_DATA_HALF, /* *< fp16 type */
    CC_DATA_INT8, /* *< int8 type */
    CC_DATA_INT32, /* *< int32 type */
    CC_DATA_UINT8, /* *< uint8 type */
    CC_DATA_HALF_UINT16_PROPOSAL, /* *<mixed type for proposal*/
    CC_DATA_INT16, /* *< int16 type */
    CC_DATA_UINT16, /* *< uint16 type */
    CC_DATA_UINT32, /* *< uint32 type */
    CC_DATA_INT64, /* *< int64 type */
    CC_DATA_UINT64, /* *< uint64 type */
    CC_DATA_DOUBLE, /* *< double type */
    CC_DATA_BOOL, /* *< bool type */
    CC_DATA_DUAL, /* *< dual output type */
    CC_DATA_DUAL_SUB_INT8, /* *< dual output int8 type */
    CC_DATA_DUAL_SUB_UINT8, /* *< dual output uint8 type */
    CC_DATA_QINT8, /* *< quantization int8 type */
    CC_DATA_QUINT8, /* *< quantization uint8 type */
    CC_DATA_QINT16, /* *< quantization int16 type */
    CC_DATA_QUINT16, /* *< quantization uint16 type */
    CC_DATA_QINT32, /* *< quantization int32 type */
    CC_DATA_2BITS, /* *< 2bit type */
    CC_DATA_RESERVED
} DataType_t;

const int CC_DIM_MAX = 8;
typedef struct tagTensor {
    Format format;
    DataType_t dataType;
    int32_t dimCnt;
    int32_t realDimCnt;
    uint32_t dataSize;
    int32_t dim[CC_DIM_MAX]; // 8代表数组的大小
    int32_t stride[CC_DIM_MAX]; // 8代表数组的大小
} ccTensor_t;

/*
 * @ingroup fmk
 * @brief mode of data type transform
 */
typedef enum tagDataTypeTransMode {
    CC_DATATYPE_TRANS_FLOAT_NO_TRANS = 0, /* *< origin data is float, no trans */
    CC_DATATYPE_TRANS_FP16_NO_TRANS, /* *< origin data is fp16, no trans */
    CC_DATATYPE_TRANS_INT8_NO_TRANS, /* *< origin data is int8, no trans */
    CC_DATATYPE_TRANS_FLOAT_TO_FP16, /* *< data type float trans to fp16 */
    CC_DATATYPE_TRANS_FP16_TO_FLOAT, /* *< data type fp16 trans to float */
    CC_DATATYPE_TRANS_FLOAT_TO_INT8, /* *< data type float trans to int8 */
    CC_DATATYPE_TRANS_INT8_TO_FLOAT, /* *< data type int8 trans to float */
    CC_DATATYPE_TRANS_UINT8_TO_FLOAT, /* *< data type uint8 trans to float */
    CC_DATATYPE_TRANS_UINT8_NO_TRANS, /* *< origin data is uint8, no trans */
    CC_DATATYPE_TRANS_INT32_NO_TRANS, /* *< data type uint8 trans to float */
    CC_DATATYPE_TRANS_INT64_NO_TRANS, /* *< data type int64 trans to int64 */
    CC_DATATYPE_TRANS_MODE_RESERVED
} DataTypeTransMode_t;

hiai::Status TransTensorFloatToHALF(const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y);

HCS_API_EXPORT hiai::Status TransTensorHALFToFloat(
    const ccTensor_t& xDesc, const void* x, const ccTensor_t& yDesc, void* y);

HCS_API_EXPORT hiai::Status InitTensorDescriptor(const ge::TensorDesc& tensor, ccTensor_t& ccTensor);

HCS_API_EXPORT hiai::Status TransTensor(
    const ge::TensorDesc& xDesc, const void* x, const ge::TensorDesc& yDesc, void* y);
} // namespace ge

#endif
