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

#ifndef ALL_TENSOR_UTILS_H
#define ALL_TENSOR_UTILS_H

#include "graph/tensor.h"
#include "framework/graph/core/def_types.h"
#include "graph/debug/ge_error_codes.h"
#include "framework/graph/core/op/op_desc.h"

namespace ge {
class GRAPH_API_EXPORT TensorUtils {
public:
    static ge::GraphErrCodeStatus GetSize(const TensorDesc& tensorDesc, uint32_t& size);

    static ge::GraphErrCodeStatus GetSize(TensorDescPtr tensorDescPtr, uint32_t& size);

    static void SetSize(TensorDesc& tensorDesc, uint32_t size);

    static void SetSize(TensorDescPtr tensorDescPtr, uint32_t size);

    static uint32_t GetWeightSize(ConstTensorPtr tensorPtr);

    static uint32_t GetWeightSize(const Tensor& tensor);

    static uint32_t GetWeightSize(const TensorDesc& tensorDesc);

    static const uint8_t* GetWeightAddr(ConstTensorPtr tensorPtr, const uint8_t* base);

    static const uint8_t* GetWeightAddr(const Tensor& tensor, const uint8_t* base);

    static void SetWeightSize(TensorDesc& tensorDesc, uint32_t size);

    static ge::GraphErrCodeStatus GetReuseInput(const TensorDesc& tensorDesc, bool& flag);

    static void SetReuseInput(TensorDesc& tensorDesc, bool flag);

    static ge::GraphErrCodeStatus GetOutputTensor(const TensorDesc& tensorDesc, bool& flag);

    static ge::GraphErrCodeStatus GetOutputTensor(const TensorDescPtr& tensorDescPtr, bool& flag);

    static void SetOutputTensor(TensorDesc& tensorDesc, bool flag);

    static GraphErrCodeStatus GetDeviceType(const TensorDesc& tensorDesc, DeviceType& type);

    static void SetDeviceType(TensorDesc& tensorDesc, DeviceType type);

    static ge::GraphErrCodeStatus GetInputTensor(const TensorDesc& tensorDesc, bool& flag);

    static void SetInputTensor(TensorDesc& tensorDesc, bool flag);

    static ge::GraphErrCodeStatus GetRealDimCnt(const TensorDesc& tensorDesc, uint32_t& cnt);

    static ge::GraphErrCodeStatus GetRealDimCnt(const TensorDescPtr& tensorDesc, uint32_t& cnt);

    static void SetRealDimCnt(TensorDesc& tensorDesc, uint32_t cnt);

    static ge::GraphErrCodeStatus GetReuseInputIndex(const TensorDesc& tensorDesc, uint32_t& idx);

    static void SetReuseInputIndex(TensorDesc& tensorDesc, uint32_t idx);

    static ge::GraphErrCodeStatus GetDataOffset(const TensorDesc& tensorDesc, int64_t& offset);

    static void SetDataOffset(TensorDesc& tensorDesc, int64_t offset);

    static bool HasAlloffsetQuantizeInfo(const TensorDesc& tensorDesc);

    static bool HasAlloffsetQuantizeInfo(const TensorDescPtr& tensorPtr);

    static ge::GraphErrCodeStatus GetAlloffsetQuantizeInfo(const TensorDesc& tensorDesc, AllOffsetQuantizeInfo& info);

    static ge::GraphErrCodeStatus GetAlloffsetQuantizeInfo(const TensorDescPtr& tensorPtr, AllOffsetQuantizeInfo& info);

    static void SetAlloffsetQuantizeInfo(TensorDesc& tensorDesc, const AllOffsetQuantizeInfo& info);

    static bool GetSecondRankPointer(const TensorDesc& tensorDesc);

    static void SetSecondRankPointer(TensorDesc& tensorDesc, const bool& isSecondPointer);

    static void DeleteAttr(TensorDesc& tensorDesc, const std::string& name);

    static void SetTensorArrayHandle(TensorDesc& tensorDesc, const string& handle);

    static ge::GraphErrCodeStatus GetTensorArrayHandle(const TensorDesc& tensorDesc, string& handle);
};
} // namespace ge
#endif // ALL_TENSOR_UTILS_H
