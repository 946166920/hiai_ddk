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
