/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * Description: tensor_utils.cpp
 */
#include "framework/graph/utils/tensor_utils.h"
#include "graph/tensor.h"
#include "framework/graph/debug/ge_log.h"
#include "graph/persistance/interface/tensor_desc_def.h"

namespace ge {
const char* const TENSOR_UTILS_ALLOFFSET_QUANTIZE_INFO = "alloffset_quantize_info";
const char* const TENSOR_UTILS_IS_SECOND_RANK_POINTER = "is_second_rank_pointer";
const char* const TENSOR_UTILS_TENSORARRAY_HANDLE = "tensorarray_handle";

GraphErrCodeStatus TensorUtils::GetSize(const TensorDesc& tensorDesc, uint32_t& size)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg == nullptr) {
        return GRAPH_FAILED;
    }
    size = static_cast<uint32_t>(tensorDescriptorMsg->size());
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus TensorUtils::GetSize(TensorDescPtr tensorDescPtr, uint32_t& size)
{
    GE_CHECK_NOTNULL(tensorDescPtr);
    return GetSize(*tensorDescPtr, size);
}

void TensorUtils::SetSize(TensorDesc& tensorDesc, uint32_t size)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_size(static_cast<int64_t>(size));
    }
}

void TensorUtils::SetSize(TensorDescPtr tensorDescPtr, uint32_t size)
{
    if (tensorDescPtr != nullptr) {
        SetSize(*tensorDescPtr, size);
    }
}

uint32_t TensorUtils::GetWeightSize(const TensorDesc& tensorDesc)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        return static_cast<uint32_t>(tensorDescriptorMsg->weight_size());
    }
    return 0;
}

uint32_t TensorUtils::GetWeightSize(const Tensor& tensor)
{
    return GetWeightSize(tensor.GetTensorDesc());
}

uint32_t TensorUtils::GetWeightSize(ConstTensorPtr tensorPtr)
{
    if (!tensorPtr) {
        return 0;
    }
    return GetWeightSize(*tensorPtr);
}

uint8_t* TensorUtils::GetWeightAddr(ConstTensorPtr tensorPtr, uint8_t* base)
{
    GE_CHK_BOOL_EXEC(base != nullptr, return nullptr, "base is null.");
    if (!tensorPtr) {
        return nullptr;
    }
    return GetWeightAddr(*tensorPtr, base);
}

uint8_t* TensorUtils::GetWeightAddr(const Tensor& tensor, uint8_t* base)
{
    GE_CHK_BOOL_EXEC(base != nullptr, return nullptr, "base is null.");
    int64_t weightDataOffset = 0;
    if (GetDataOffset(tensor.GetTensorDesc(), weightDataOffset) != GRAPH_SUCCESS) {
        return nullptr;
    }
    if (weightDataOffset == 0) {
        // offset为0的权值仍然放在const op里面，没有清空，还是从ATTR_NAME_WEIGHTS中获取
        return const_cast<uint8_t*>(tensor.GetData().GetData());
    }

    return base + weightDataOffset;
}

void TensorUtils::SetWeightSize(TensorDesc& tensorDesc, uint32_t size) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_weight_size(size);
    }
}

GraphErrCodeStatus TensorUtils::GetReuseInput(const TensorDesc& tensorDesc, bool& flag)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        flag = tensorDescriptorMsg->reuse_input();
    }
    return GRAPH_SUCCESS;
}

void TensorUtils::SetReuseInput(TensorDesc& tensorDesc, bool flag)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_reuse_input(flag);
    }
}

GraphErrCodeStatus TensorUtils::GetOutputTensor(const TensorDesc& tensorDesc, bool& flag)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        flag = tensorDescriptorMsg->output_tensor();
    }
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus TensorUtils::GetOutputTensor(const TensorDescPtr& tensorDescPtr, bool& flag)
{
    if (tensorDescPtr == nullptr) {
        return GRAPH_FAILED;
    }
    return GetOutputTensor(*tensorDescPtr, flag);
}

void TensorUtils::SetOutputTensor(TensorDesc& tensorDesc, bool flag) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_output_tensor(flag);
    }
}

static map<uint32_t, string> deviceToStrMap {
    {0, "NPU"},
    {1, "CPU"},
};
static map<string, uint32_t> strToDeviceMap {
    {"NPU", 0},
    {"CPU", 1},
};

GraphErrCodeStatus TensorUtils::GetDeviceType(const TensorDesc& tensorDesc, DeviceType& type)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        std::string typeStr = *(tensorDescriptorMsg->mutable_device_type());
        type = DeviceType(strToDeviceMap[typeStr]);
    }
    return GRAPH_SUCCESS;
}

void TensorUtils::SetDeviceType(TensorDesc& tensorDesc, DeviceType type) //lint !e1764
{
    auto typeStr = deviceToStrMap[type];
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_device_type(typeStr);
    }
}

GraphErrCodeStatus TensorUtils::GetInputTensor(const TensorDesc& tensorDesc, bool& flag)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        flag = tensorDescriptorMsg->input_tensor();
    }
    return GRAPH_SUCCESS;
}

void TensorUtils::SetInputTensor(TensorDesc& tensorDesc, bool flag) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_input_tensor(flag);
    }
}

GraphErrCodeStatus TensorUtils::GetRealDimCnt(const TensorDesc& tensorDesc, uint32_t& cnt)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        cnt = static_cast<uint32_t>(tensorDescriptorMsg->real_dim_cnt());
    }
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus TensorUtils::GetRealDimCnt(const TensorDescPtr& tensorDesc, uint32_t& cnt)
{
    if (tensorDesc == nullptr) {
        return GRAPH_FAILED;
    }
    return GetRealDimCnt(*tensorDesc, cnt);
}

void TensorUtils::SetRealDimCnt(TensorDesc& tensorDesc, uint32_t cnt) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_real_dim_cnt(cnt);
    }
}

GraphErrCodeStatus TensorUtils::GetReuseInputIndex(const TensorDesc& tensorDesc, uint32_t& idx)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        idx = static_cast<uint32_t>(tensorDescriptorMsg->reuse_input_index());
    }
    return GRAPH_SUCCESS;
}

void TensorUtils::SetReuseInputIndex(TensorDesc& tensorDesc, uint32_t idx) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_reuse_input_index(idx);
    }
}

GraphErrCodeStatus TensorUtils::GetDataOffset(const TensorDesc& tensorDesc, int64_t& offset)
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        offset = tensorDescriptorMsg->data_offset();
    }
    return GRAPH_SUCCESS;
}

void TensorUtils::SetDataOffset(TensorDesc& tensorDesc, int64_t offset) //lint !e1764
{
    auto tensorDescriptorMsg = tensorDesc.tensorDescDef_;
    if (tensorDescriptorMsg != nullptr) {
        tensorDescriptorMsg->set_data_offset(offset);
    }
}

bool TensorUtils::HasAlloffsetQuantizeInfo(const TensorDesc& tensorDesc)
{
    return tensorDesc.HasAttr(TENSOR_UTILS_ALLOFFSET_QUANTIZE_INFO);
}

bool TensorUtils::HasAlloffsetQuantizeInfo(const TensorDescPtr& tensorPtr)
{
    if (tensorPtr == nullptr) {
        FMK_LOGE("tensorDescPtr is null.");
        return false;
    }
    return tensorPtr->HasAttr(TENSOR_UTILS_ALLOFFSET_QUANTIZE_INFO);
}

GraphErrCodeStatus TensorUtils::GetAlloffsetQuantizeInfo(const TensorDesc& tensorDesc, AllOffsetQuantizeInfo& info)
{
    AttrValue attrValue;
    if (tensorDesc.GetAttr(TENSOR_UTILS_ALLOFFSET_QUANTIZE_INFO, attrValue) != GRAPH_SUCCESS) {
        return GRAPH_FAILED;
    }
    AttrValue::NamedAttrs namedAttrs = attrValue.GetNamedAttrs();
    info.scale = namedAttrs.GetItem("scale").GetFloat();
    info.offset = namedAttrs.GetItem("offset").GetInt();
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus TensorUtils::GetAlloffsetQuantizeInfo(const TensorDescPtr& tensorPtr, AllOffsetQuantizeInfo& info)
{
    if (tensorPtr == nullptr) {
        FMK_LOGE("tensorDescPtr is null.");
        return GRAPH_FAILED;
    }
    return GetAlloffsetQuantizeInfo(*tensorPtr, info);
}

void TensorUtils::SetAlloffsetQuantizeInfo(TensorDesc& tensorDesc, const AllOffsetQuantizeInfo& info)
{
    AttrValue::NamedAttrs namedAttrs;
    namedAttrs.SetAttr("scale", AttrValue::CreateFrom(info.scale));
    namedAttrs.SetAttr("offset", AttrValue::CreateFrom(static_cast<int64_t>(info.offset)));

    (void)tensorDesc.SetAttr(TENSOR_UTILS_ALLOFFSET_QUANTIZE_INFO, AttrValue::CreateFrom(namedAttrs));
}

bool TensorUtils::GetSecondRankPointer(const TensorDesc& tensorDesc)
{
    AttrValue attrValue;
    if (tensorDesc.GetAttr(TENSOR_UTILS_IS_SECOND_RANK_POINTER, attrValue) != GRAPH_SUCCESS) {
        return false;
    }
    bool result = attrValue.GetBool();
    return result;
}

void TensorUtils::SetSecondRankPointer(TensorDesc& tensorDesc, const bool& isSecondPointer)
{
    (void)tensorDesc.SetAttr(TENSOR_UTILS_IS_SECOND_RANK_POINTER, AttrValue::CreateFrom(isSecondPointer));
}

void TensorUtils::DeleteAttr(TensorDesc& tensorDesc, const std::string& name)
{
    (void)tensorDesc.DelAttr(name);
}

void TensorUtils::SetTensorArrayHandle(TensorDesc& tensorDesc, const string& handle)
{
    (void)tensorDesc.SetAttr(TENSOR_UTILS_TENSORARRAY_HANDLE, AttrValue::CreateFrom(handle));
}

GraphErrCodeStatus TensorUtils::GetTensorArrayHandle(const TensorDesc& tensorDesc, string& handle)
{
    AttrValue attrValue;
    if (tensorDesc.GetAttr(TENSOR_UTILS_TENSORARRAY_HANDLE, attrValue) != GRAPH_SUCCESS) {
        return GRAPH_FAILED;
    }
    handle = attrValue.GetString();
    return GRAPH_SUCCESS;
}
} // namespace ge
