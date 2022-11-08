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
#include "compatible/AippTensor.h"

#include <map>
#include <vector>

#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log_fmk_interface.h"

#include "util/hiai_foundation_dl_helper.h"
namespace hiai {

using namespace std;

AippTensor::AippTensor(std::shared_ptr<AiTensor> tensor, std::vector<std::shared_ptr<AippPara>> aippParas)
    : tensor(tensor), aippParas(aippParas)
{
}

AippTensor::~AippTensor()
{
}

void* AippTensor::GetBuffer() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (tensor == nullptr) {
        FMK_LOGE("GetBuffer failed, tensor is null");
        return nullptr;
    }
    return tensor->GetBuffer();
}

uint32_t AippTensor::GetSize() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (tensor == nullptr) {
        FMK_LOGE("GetSize failed, tensor is null");
        return 0;
    }
    return tensor->GetSize();
}

std::shared_ptr<AiTensor> AippTensor::GetAiTensor() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return tensor;
}

std::vector<std::shared_ptr<AippPara>> AippTensor::GetAippParas() const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return aippParas;
}

std::shared_ptr<AippPara> AippTensor::GetAippParas(uint32_t index) const
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (index >= aippParas.size()) {
        FMK_LOGE("GetBuffer failed, index is out of the range of aippParas");
        return nullptr;
    }
    return aippParas[index];
}

struct PrivateHandleInfo {
    int fd;
    int size;
    int offset;
    int format;
    int w;
    int h;
    int strideW;
    int strideH;
};

std::shared_ptr<AippTensor> HIAI_CreateAiPPTensorFromHandle(
    void* handle, const TensorDimension* dim, AiTensorImage_Format imageFormat)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    PrivateHandleInfo* (*getHandleInfoFunc)(void*) =
        (PrivateHandleInfo * (*)(void*)) HIAI_Foundation_GetSymbol("HIAI_GetHandleInfo_From_BufferHandle");
    void (*destroyHandleFunc)(PrivateHandleInfo*) =
        (void (*)(PrivateHandleInfo*))HIAI_Foundation_GetSymbol("HIAI_DestroyPrivateHandle");
    if (getHandleInfoFunc == nullptr || destroyHandleFunc == nullptr) {
        FMK_LOGE("unsupported version.");
        return nullptr;
    }

    std::shared_ptr<PrivateHandleInfo> bufferHandleT(
        getHandleInfoFunc(handle), [&destroyHandleFunc](PrivateHandleInfo* p) { destroyHandleFunc(p); });
    if (bufferHandleT == nullptr) {
        FMK_LOGE("Get PrivateHandleInfo failed!");
        return nullptr;
    }

    // this map came from gralloc_priv.h base on graphics-base
    static const std::map<int32_t, AiTensorImage_Format> formatMap = {
        // rgb's fromats not supported now
        {16, AiTensorImage_Format::AiTensorImage_YUV422SP_U8},
        {17, AiTensorImage_Format::AiTensorImage_YUV420SP_U8},
        {20, AiTensorImage_Format::AiTensorImage_YUYV_U8},
        {538982489, AiTensorImage_Format::AiTensorImage_YUV400_U8},
    };

    auto ionFormat = formatMap.find(bufferHandleT->format);
    if (ionFormat == formatMap.end()) {
        FMK_LOGE("graphic_buffer's imageFormat [%u] not suppported", (uint32_t)bufferHandleT->format);
        return nullptr;
    }

    if (imageFormat == hiai::AiTensorImage_INVALID) {
        imageFormat = ionFormat->second;
    } else {
        if (imageFormat != ionFormat->second) {
            FMK_LOGE(
                "imageForamt is not equal to the graphic_buffer's imageFormat [%u]", (uint32_t)bufferHandleT->format);
            return nullptr;
        }
    }
    static const int imageChannel[AiTensorImage_Format::AiTensorImage_YVU444SP_U8 + 1] = {
        3, 4, 1, 4, 3, 3, 4, 3, 3, 3, 3};
    static const float imageSizeScale[AiTensorImage_Format::AiTensorImage_YVU444SP_U8 + 1] = {
        1.5, 4, 1, 4, 2, 2, 4, 3, 3, 3, 3};
    uint32_t channel = imageChannel[imageFormat];
    float sizeScale = imageSizeScale[imageFormat];

    int32_t fd = bufferHandleT->fd;
    int32_t offset = bufferHandleT->offset;
    int32_t size = static_cast<int32_t>(bufferHandleT->strideW * bufferHandleT->strideH * sizeScale + offset);
    NativeHandle nativate_handle;
    nativate_handle.fd = fd;
    nativate_handle.size = size;
    nativate_handle.offset = offset;
    int32_t ret = -1;
    std::shared_ptr<AiTensor> input = make_shared_nothrow<AiTensor>();
    if (input == nullptr) {
        FMK_LOGE("AiTensor create failed!");
        return nullptr;
    }
    TensorDimension dims(1, channel, bufferHandleT->strideH, bufferHandleT->strideW);
    ret = input->Init(nativate_handle, &dims, hiai::HIAI_DATATYPE_UINT8);
    if (ret != 0) {
        FMK_LOGE("AiTensor Init failed! ret:%d", ret);
        return nullptr;
    }

    vector<shared_ptr<AippPara>> aippParas;
    std::shared_ptr<AippPara> aippPara = hiai::make_shared_nothrow<AippPara>();
    if (aippPara == nullptr) {
        FMK_LOGE("AippPara create failed!");
        return nullptr;
    }

    AippInputShape aippInputShape;
    aippInputShape.srcImageSizeW = static_cast<uint32_t>(bufferHandleT->strideW);
    aippInputShape.srcImageSizeH = static_cast<uint32_t>(bufferHandleT->strideH);
    AippCropPara aippCropPara;
    aippCropPara.cropSizeW = (dim != nullptr ? dim->GetWidth() : static_cast<uint32_t>(bufferHandleT->w));
    aippCropPara.cropSizeH = (dim != nullptr ? dim->GetHeight() : static_cast<uint32_t>(bufferHandleT->h));
    aippCropPara.cropStartPosW = 0;
    aippCropPara.cropStartPosH = 0;
    aippCropPara.switch_ = true;
    if (aippPara->Init(1) != AI_SUCCESS) {
        FMK_LOGE("create aippPara failed.");
        return nullptr;
    }
    aippPara->SetInputFormat(imageFormat);
    aippPara->SetInputShape(aippInputShape);
    aippPara->SetCropPara(aippCropPara);
    aippParas.push_back(aippPara);

    std::shared_ptr<AippTensor> aippTensor = hiai::make_shared_nothrow<AippTensor>(input, aippParas);
    if (aippTensor == nullptr) {
        FMK_LOGE("AippTensor create failed!");
        return nullptr;
    }

    return aippTensor;
}

} // namespace hiai