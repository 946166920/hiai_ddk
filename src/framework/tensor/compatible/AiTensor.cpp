/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: AiTensor implementation
 */

#include "compatible/AiTensor.h"

#include <map>

#include "tensor/aipp_para.h"
#include "tensor/image_tensor_buffer.h"
#include "util/base_types.h"
#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log_fmk_interface.h"

namespace hiai {
TensorDimension::TensorDimension()
{
}

TensorDimension::~TensorDimension()
{
}

TensorDimension::TensorDimension(uint32_t number, uint32_t channel, uint32_t height, uint32_t width)
    : n(number), c(channel), h(height), w(width)
{
}

void TensorDimension::SetNumber(const uint32_t number)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    n = number;
}

uint32_t TensorDimension::GetNumber() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return n;
}

void TensorDimension::SetChannel(const uint32_t channel)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    c = channel;
}

uint32_t TensorDimension::GetChannel() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return c;
}

void TensorDimension::SetHeight(const uint32_t height)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    h = height;
}

uint32_t TensorDimension::GetHeight() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return h;
}

void TensorDimension::SetWidth(const uint32_t width)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    w = width;
}

uint32_t TensorDimension::GetWidth() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return w;
}

bool TensorDimension::IsEqual(const TensorDimension& dim)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (this == &dim) {
        return true;
    }

    return n == dim.n && c == dim.c && h == dim.h && w == dim.w;
}

AiTensor::AiTensor() : tensor_(nullptr)
{
}

AiTensor::~AiTensor()
{
    if (tensor_ != nullptr) {
        tensor_.reset();
        tensor_ = nullptr;
    }
}

AIStatus AiTensor::Init(const TensorDimension* dim, HIAI_DataType type)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dim == nullptr) {
        return AI_INVALID_PARA;
    }
    if (type < HIAI_DATATYPE_UINT8 || type > HIAI_DATATYPE_DOUBLE) {
        FMK_LOGE("invalid data type.");
        return AI_INVALID_PARA;
    }
    desc_.dims = {static_cast<int32_t>(dim->GetNumber()), static_cast<int32_t>(dim->GetChannel()),
        static_cast<int32_t>(dim->GetHeight()), static_cast<int32_t>(dim->GetWidth())};
    desc_.dataType = static_cast<DataType>(type);
    desc_.format = Format::NCHW;

    tensor_ = CreateNDTensorBuffer(desc_);
    if (tensor_ == nullptr) {
        return AI_FAILED;
    }
    return AI_SUCCESS;
}

static size_t GetTotalDimNum(const NDTensorDesc tensorDesc)
{
    size_t totalDimSize = 1;
    for (size_t i = 0; i < tensorDesc.dims.size(); i++) {
        int32_t dimValue = tensorDesc.dims[i];
        if (dimValue <= 0) {
            FMK_LOGE("invalid dim value, dimValue = %d", dimValue);
            return 0;
        }

        if (totalDimSize > static_cast<size_t>(SIZE_MAX / dimValue)) {
            FMK_LOGE("too large totalDimSize, current totalDimSize = %d, dimValue = %d", totalDimSize, dimValue);
            return 0;
        }
        totalDimSize *= static_cast<size_t>(dimValue);
    }

    return totalDimSize;
}

static size_t GetElementSize(DataType type)
{
    std::map<DataType, int32_t> dataTypeElementSize = {
        {DataType::UINT8, sizeof(uint8_t)},
        {DataType::FLOAT32, sizeof(float)},
        {DataType::FLOAT16, sizeof(float) / 2},
        {DataType::INT32, sizeof(int32_t)},
        {DataType::INT8, sizeof(int8_t)},
        {DataType::INT16, sizeof(int16_t)},
        {DataType::BOOL, sizeof(char)},
        {DataType::INT64, sizeof(int64_t)},
        {DataType::UINT32, sizeof(uint32_t)},
        {DataType::DOUBLE, sizeof(double)},
    };

    auto it = dataTypeElementSize.find(type);
    if (it == dataTypeElementSize.end()) {
        FMK_LOGE("invalid DataType, type = %d", static_cast<int32_t>(type));
        return 0;
    }

    return it->second;
}

AIStatus AiTensor::Init(const void* data, const TensorDimension* dim, HIAI_DataType pdataType)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dim == nullptr || data == nullptr) {
        return AI_INVALID_PARA;
    }
    if (pdataType < HIAI_DATATYPE_UINT8 || pdataType > HIAI_DATATYPE_DOUBLE) {
        FMK_LOGE("invalid data type, pdataType = %d.", pdataType);
        return AI_INVALID_PARA;
    }
    desc_.dims = {static_cast<int32_t>(dim->GetNumber()), static_cast<int32_t>(dim->GetChannel()),
        static_cast<int32_t>(dim->GetHeight()), static_cast<int32_t>(dim->GetWidth())};
    desc_.dataType = static_cast<DataType>(pdataType);
    desc_.format = Format::NCHW;

    size_t totalSize = 0;

    size_t totalDimNum = GetTotalDimNum(desc_);
    if (totalDimNum == 0) {
        FMK_LOGE("GetTotalDimNum error.");
        return AI_INVALID_PARA;
    }

    size_t elementSize = GetElementSize(desc_.dataType);
    if (elementSize == 0) {
        FMK_LOGE("GetElementSize error.");
        return AI_INVALID_PARA;
    }

    if (totalDimNum > SIZE_MAX / elementSize) {
        FMK_LOGE("too large totalSize, current totalSize = %d, elementSize = %d", totalDimNum, elementSize);
        return AI_INVALID_PARA;
    }

    totalSize = totalDimNum * elementSize;
    tensor_ = CreateNDTensorBufferNoCopy(desc_, data, totalSize);
    if (tensor_ == nullptr) {
        FMK_LOGE("tensor_ is nullptr.");
        return AI_FAILED;
    }

    return AI_SUCCESS;
}

AIStatus AiTensor::Init(const TensorDimension* dim)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dim == nullptr) {
        return AI_INVALID_PARA;
    }
    HIAI_DataType dataType = static_cast<HIAI_DataType>(DataType::FLOAT32);
    return Init(dim, dataType);
}

AIStatus AiTensor::Init(const NativeHandle& handle, const TensorDimension* dim, HIAI_DataType dataType)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dim == nullptr) {
        return AI_INVALID_PARA;
    }
    if (dataType < HIAI_DATATYPE_UINT8 || dataType > HIAI_DATATYPE_DOUBLE) {
        FMK_LOGE("invalid data type.");
        return AI_INVALID_PARA;
    }
    desc_.dims = {static_cast<int32_t>(dim->GetNumber()), static_cast<int32_t>(dim->GetChannel()),
        static_cast<int32_t>(dim->GetHeight()), static_cast<int32_t>(dim->GetWidth())};
    desc_.dataType = static_cast<DataType>(dataType);
    desc_.format = Format::NCHW;
    tensor_ = CreateNDTensorBuffer(desc_, handle);
    if (tensor_ == nullptr) {
        FMK_LOGE("tensor_ is nullptr");
        return AI_FAILED;
    }
    return AI_SUCCESS;
}

AIStatus AiTensor::Init(uint32_t number, uint32_t height, uint32_t width, AiTensorImage_Format format)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::shared_ptr<IImageTensorBuffer> tensor =
        CreateImageTensorBuffer(static_cast<int32_t>(number), static_cast<int32_t>(height), static_cast<int32_t>(width),
        (ImageFormat)(format), ImageColorSpace::BT_601_NARROW, 0);
    if (tensor == nullptr) {
        return AI_FAILED;
    }
    tensor_ = tensor;
    desc_ = tensor->GetTensorDesc();
    return AI_SUCCESS;
}

uint32_t AiTensor::GetSize() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (tensor_ == nullptr) {
        FMK_LOGE("tensor is not inited");
        return 0;
    }
    return tensor_->GetSize();
}

void* AiTensor::GetBuffer() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (tensor_ == nullptr) {
        FMK_LOGE("tensor is not inited");
        return nullptr;
    }
    return tensor_->GetData();
}

void* AiTensor::GetTensorBuffer() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return GetBuffer();
}

AIStatus AiTensor::SetTensorDimension(const TensorDimension* dim)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (dim == nullptr) {
        return AI_FAILED;
    }

    desc_.dims = {static_cast<int32_t>(dim->GetNumber()), static_cast<int32_t>(dim->GetChannel()),
        static_cast<int32_t>(dim->GetHeight()), static_cast<int32_t>(dim->GetWidth())};
    return AI_SUCCESS;
}

TensorDimension AiTensor::GetTensorDimension() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (desc_.dims.size() == 4) {
        return TensorDimension(desc_.dims[0], desc_.dims[1], desc_.dims[2], desc_.dims[3]);
    }

    return TensorDimension();
}

} // namespace hiai
