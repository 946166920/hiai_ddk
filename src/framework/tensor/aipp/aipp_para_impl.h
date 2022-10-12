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
#ifndef FRAMEWORK_TENSOR_AIPP_PARA_IMPL_H
#define FRAMEWORK_TENSOR_AIPP_PARA_IMPL_H
#include "tensor/aipp_para.h"
#include "framework/c/hiai_tensor_aipp_para.h"

namespace hiai {

class AIPPParaBufferImpl {
public:
    HIAI_TensorAippPara* Init(uint32_t batchCount);
    Status Release(HIAI_TensorAippPara* paraBuff);
    void* GetRawBuffer(HIAI_TensorAippPara* paraBuff);
    uint32_t GetRawBufferSize(HIAI_TensorAippPara* paraBuff);
    int GetInputIndex(HIAI_TensorAippPara* paraBuff);
    void SetInputIndex(HIAI_TensorAippPara* paraBuff, uint32_t inputIndex);
    int GetInputAippIndex(HIAI_TensorAippPara* paraBuff);
    void SetInputAippIndex(HIAI_TensorAippPara* paraBuff, uint32_t inputAippIndex);
};

class HIAI_TENSOR_API_EXPORT AIPPParaImpl : public IAIPPPara {
public:
    AIPPParaImpl() = default;
    ~AIPPParaImpl() override;
    AIPPParaImpl(const AIPPParaImpl&) = delete;
    AIPPParaImpl& operator=(const AIPPParaImpl&) = delete;

    Status Init(uint32_t batchCount);
    Status Init(void* paraBuff);

    uint32_t GetBatchCount() override;
    Status SetInputIndex(uint32_t inputIndex) override;

    int32_t GetInputIndex() override;
    Status SetInputAippIndex(uint32_t inputAippIndex) override;
    int32_t GetInputAippIndex() override;

    Status SetCscPara(ImageFormat targetFormat, ImageColorSpace colorSpace) override;
    Status SetCscPara(CscMatrixPara cscPara) override;
    CscMatrixPara GetCscPara() override;

    Status SetChannelSwapPara(ChannelSwapPara&& channelSwapPara) override;
    ChannelSwapPara GetChannelSwapPara() override;
    // end common interface

    // set/get interface for batch, if batchIndex == -1 means for all batch
    Status SetCropPara(int32_t batchIndex, CropPara&& cropPara) override;
    CropPara GetCropPara(int32_t batchIndex) override;

    Status SetResizePara(int32_t batchIndex, ResizePara&& resizePara) override;
    ResizePara GetResizePara(int32_t batchIndex) override;

    Status SetPaddingPara(int32_t batchIndex, PadPara&& paddingPara) override;
    PadPara GetPaddingPara(int32_t batchIndex) override;

    Status SetDtcPara(int32_t batchIndex, DtcPara&& dtcPara) override;
    DtcPara GetDtcPara(int32_t batchIndex) override;

    // internel
    Status SetInputFormat(ImageFormat inputFormat) override;
    ImageFormat GetInputFormat() override;
    std::vector<int32_t> GetInputShape() override;
    Status SetInputShape(std::vector<int32_t>& shape) override;

    void* GetData() override;

    size_t GetSize() const override;

    HIAI_TENSOR_API_EXPORT HIAI_TensorAippPara* GetParaBuffer();

public:
    void SetEnableCrop(bool enable);
    bool GetEnableCrop();

    void SetEnableResize(bool enable);
    bool GetEnableResize();

    void SetEnableCsc(bool enable);
    bool GetEnableCsc();

    void SetEnablePadding(bool enable);
    bool GetEnablePadding();

protected:
    Status GetAippParaBufferImpl(std::shared_ptr<AIPPParaBufferImpl>& aippParaImpl);

private:
    ImageFormat GetInputFormat(void* buffer);
    Status InitAippPara(uint32_t batchCount);

private:
    std::shared_ptr<AIPPParaBufferImpl> aippParaImpl_{nullptr};
    HIAI_TensorAippPara* paraBuff_{nullptr};

    bool enableCrop = false;
    bool enableResize = false;
    bool enableCsc = false;
    bool enablePadding = false;
};
HIAI_TENSOR_API_EXPORT HIAI_TensorAippPara* GetTensorAippParaFromAippPara(const std::shared_ptr<IAIPPPara>& aippPara);
} // namespace hiai
#endif