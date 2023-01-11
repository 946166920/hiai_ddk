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
#ifndef FRAMEWORK_C_HIAI_AIPP_PARA_LOCAL_H
#define FRAMEWORK_C_HIAI_AIPP_PARA_LOCAL_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* HIAI_TensorAippPara_CreateLocal(uint32_t batchNum);

void* HIAI_TensorAippPara_GetRawBufferLocal(const void* handle);

int32_t HIAI_TensorAippPara_GetRawBufferSizeLocal(const void* handle);

void* HIAI_TensorAippPara_GetHandleLocal(const void* handle);

int32_t HIAI_TensorAippPara_GetInputIndexLocal(const void* handle);

void HIAI_TensorAippPara_SetInputIndexLocal(void* handle, uint32_t inputIndex);

int32_t HIAI_TensorAippPara_GetInputAippIndexLocal(const void* handle);

void HIAI_TensorAippPara_SetInputAippIndexLocal(void* handle, uint32_t inputAippIndex);

void HIAI_TensorAippPara_DestroyLocal(void* handle);

#ifdef __cplusplus
}
#endif

#endif