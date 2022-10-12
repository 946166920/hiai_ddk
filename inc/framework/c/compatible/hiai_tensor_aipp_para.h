/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: tensor aipp para(deprecated)
 */
#ifndef FRAMEWORK_C_LEGACY_HIAI_TENSOR_AIPP_PARA_H
#define FRAMEWORK_C_LEGACY_HIAI_TENSOR_AIPP_PARA_H

#include "framework/c/hiai_c_api_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HIAI_TensorAippPara HIAI_TensorAippPara;

AICP_C_API_EXPORT HIAI_TensorAippPara* HIAI_TensorAipp_create(unsigned int batchCount);

AICP_C_API_EXPORT void* HIAI_TensorAipp_getRawBuffer(HIAI_TensorAippPara* tensorAipp);

AICP_C_API_EXPORT int HIAI_TensorAipp_getInputIndex(HIAI_TensorAippPara* tensorAipp);
AICP_C_API_EXPORT void HIAI_TensorAipp_setInputIndex(HIAI_TensorAippPara* tensorAipp, unsigned int inputIndex);

AICP_C_API_EXPORT int HIAI_TensorAipp_getInputAippIndex(HIAI_TensorAippPara* tensorAipp);
AICP_C_API_EXPORT void HIAI_TensorAipp_setInputAippIndex(HIAI_TensorAippPara* tensorAipp, unsigned int inputAippIndex);

AICP_C_API_EXPORT void HIAI_TensorAipp_destroy(HIAI_TensorAippPara* aippParas);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FRAMEWORK_C_LEGACY_HIAI_TENSOR_AIPP_PARA_H
