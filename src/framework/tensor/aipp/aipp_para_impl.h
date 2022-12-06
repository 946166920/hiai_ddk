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
using TensorAippCommPara = std::pair<HIAI_TensorAippCommPara*, uint8_t>;

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

    Status SetCropPara(CropPara&& cropPara) override;
    Status SetCropPara(uint32_t batchIndex, CropPara&& cropPara) override;
    CropPara GetCropPara(uint32_t batchIndex) override;

    Status SetResizePara(ResizePara&& resizePara) override;
    Status SetResizePara(uint32_t batchIndex, ResizePara&& resizePara) override;
    ResizePara GetResizePara(uint32_t batchIndex) override;

    Status SetPaddingPara(PadPara&& paddingPara) override;
    Status SetPaddingPara(uint32_t batchIndex, PadPara&& paddingPara) override;
    PadPara GetPaddingPara(uint32_t batchIndex) override;

    Status SetDtcPara(DtcPara&& dtcPara) override;
    Status SetDtcPara(uint32_t batchIndex, DtcPara&& dtcPara) override;
    DtcPara GetDtcPara(uint32_t batchIndex) override;

    // internel
    Status SetInputFormat(ImageFormat inputFormat) override;
    ImageFormat GetInputFormat() override;
    std::vector<int32_t> GetInputShape() override;
    Status SetInputShape(std::vector<int32_t>& shape) override;

    void* GetData() override;

    size_t GetSize() const override;

    HIAI_TENSOR_API_EXPORT HIAI_TensorAippPara* GetParaBuffer();

public:
    bool GetEnableCrop(uint32_t batchIndex);

    bool GetEnableResize(uint32_t batchIndex);

    bool GetEnableCsc();

    bool GetEnablePadding(uint32_t batchIndex);

protected:
    Status GetAippParaBufferImpl(std::shared_ptr<AIPPParaBufferImpl>& aippParaImpl);

private:
    ImageFormat GetInputFormat(HIAI_TensorAippCommPara* commPara);
    Status InitAippPara(uint32_t batchCount);

    TensorAippCommPara GetTensorAippCommPara();

    template <typename T, typename UpdateParaFunc>
    Status SetAippFuncPara(T&& funcPara, UpdateParaFunc updateParaFunc);

    template <typename T, typename UpdateParaFunc>
    Status SetAippFuncPara(uint32_t batchIndex, T&& funcPara, UpdateParaFunc updateParaFunc);

private:
    std::shared_ptr<AIPPParaBufferImpl> aippParaImpl_ {nullptr};
    HIAI_TensorAippPara* paraBuff_ {nullptr};
    void* rawBuffer_ {nullptr};
};
HIAI_TENSOR_API_EXPORT HIAI_TensorAippPara* GetTensorAippParaFromAippPara(const std::shared_ptr<IAIPPPara>& aippPara);
} // namespace hiai
#endif