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

#include "aipp_input_converter.h"
#include <map>
#include "tensor/image_tensor_buffer.h"
#include "securec.h"
#include "framework/common/fmk_error_codes.h"
#include "framework/infra/log/log.h"

namespace hiai {
template <typename T>
static T* GetAippParam(size_t size, void* tensor)
{
    if (size != sizeof(T)) {
        FMK_LOGE("para size not equal!");
        return nullptr;
    }
    return static_cast<T*>(tensor);
}

bool HasDynamicPara(hiai::AippPreprocessConfig& aippConfig, size_t type, size_t& idx)
{
    for (int32_t i = 0; i < aippConfig.configDataCnt; i++) {
        if ((static_cast<uint32_t>(aippConfig.configDataInfo[i].type) == type) &&
            (aippConfig.configDataInfo[i].idx >= 0)) {
            idx = static_cast<size_t>(aippConfig.configDataInfo[i].idx);
            return true;
        }
    }
    return false;
}

bool CheckIndexValid(size_t idx, const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    return idx >= 0 && idx < inputs.size();
}

static Status SetCropPara(
    hiai::AippPreprocessConfig& aippConfig, std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    auto para = GetAippParam<CropPara>(size, tensor);
    if (para == nullptr) {
        FMK_LOGE("crop para is nullptr");
        return hiai::FAILED;
    }
    uint32_t outputW = para->cropSizeW;
    uint32_t outputH = para->cropSizeH;
    if (outputW > aippConfig.aippParamInfo.cropPara.cropSizeW ||
        outputH > aippConfig.aippParamInfo.cropPara.cropSizeH) {
        FMK_LOGE("outputW [%d] or outputH [%d] invalid", outputW, outputH);
        return hiai::FAILED;
    }
    return aippPara->SetCropPara(0, std::move(*para));
}

static Status PrepareCropParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IMAGE_CROP_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetCropPara(aippConfig, aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        if (aippConfig.aippParamInfo.enableCrop) {
            return aippPara->SetCropPara(0, std::move(aippConfig.aippParamInfo.cropPara));
        }
    }
    return hiai::SUCCESS;
}

static Status SetChannelSwapPara(std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    auto para = GetAippParam<ChannelSwapPara>(size, tensor);
    if (para == nullptr) {
        FMK_LOGE("channel swap para is nullptr");
        return hiai::FAILED;
    }

    return aippPara->SetChannelSwapPara(std::move(*para));
}

static Status PrepareChannelSwapParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetChannelSwapPara(aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        aippPara->SetChannelSwapPara(std::move(aippConfig.aippParamInfo.channelSwapPara));
    }
    return hiai::SUCCESS;
}

static Status SetCSCPara(
    hiai::AippPreprocessConfig& aippConfig, std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    hiai::CscMatrixPara* matrixPara = GetAippParam<CscMatrixPara>(size, tensor);
    if (matrixPara != nullptr) {
        return aippPara->SetCscPara(*matrixPara);
    }

    hiai::CscPara* para = GetAippParam<CscPara>(size, tensor);
    if (para != nullptr) {
        if (para->outputFormat != aippConfig.aippParamInfo.cscMatrixPara.outputFormat) {
            FMK_LOGE("csc output format error!");
            return hiai::FAILED;
        }
        return aippPara->SetCscPara(para->outputFormat);
    }
    return hiai::FAILED;
}

static Status PrepareCscParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IMAGE_COLOR_SPACE_CONVERTION_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetCSCPara(aippConfig, aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        if (aippConfig.aippParamInfo.enableCsc) {
            return aippPara->SetCscPara(aippConfig.aippParamInfo.cscMatrixPara);
        }
    }
    return hiai::SUCCESS;
}

static Status SetResizePara(
    hiai::AippPreprocessConfig& aippConfig, std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    auto para = GetAippParam<ResizePara>(size, tensor);
    if (para == nullptr) {
        FMK_LOGE("resize para is nullptr");
        return hiai::FAILED;
    }

    if (para->resizeOutputSizeW > aippConfig.aippParamInfo.resizePara.resizeOutputSizeW ||
        para->resizeOutputSizeH > aippConfig.aippParamInfo.resizePara.resizeOutputSizeH) {
        FMK_LOGE("outputW [%d] or outputH [%d] invalid", para->resizeOutputSizeW, para->resizeOutputSizeH);
        return hiai::FAILED;
    }
    return aippPara->SetResizePara(0, std::move(*para));
}

static Status PrepareResizeParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IMAGE_RESIZE_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetResizePara(aippConfig, aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        if (aippConfig.aippParamInfo.enableResize) {
            return aippPara->SetResizePara(0, std::move(aippConfig.aippParamInfo.resizePara));
        }
    }
    return hiai::SUCCESS;
}

static Status SetDtcPara(std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    auto para = GetAippParam<DtcPara>(size, tensor);
    if (para == nullptr) {
        FMK_LOGE("dtc para is nullptr");
        return hiai::FAILED;
    }

    return aippPara->SetDtcPara(0, std::move(*para));
}

static Status PrepareDtcParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IMAGE_DATA_TYPE_CONVERSION_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetDtcPara(aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        if (aippConfig.aippParamInfo.enableDtc) {
            return aippPara->SetDtcPara(0, std::move(aippConfig.aippParamInfo.dtcPara));
        }
    }
    return hiai::SUCCESS;
}

static Status SetPaddingPara(std::shared_ptr<IAIPPPara>& aippPara, size_t size, void* tensor)
{
    auto para = GetAippParam<PadPara>(size, tensor);
    if (para == nullptr) {
        FMK_LOGE("pad para is nullptr");
        return hiai::FAILED;
    }

    return aippPara->SetPaddingPara(0, std::move(*para));
}

static Status PreparePaddingParam(std::shared_ptr<IAIPPPara>& aippPara, hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs)
{
    size_t idx;
    if (HasDynamicPara(aippConfig, AIPP_FUNC_IAMGE_PADDING_V2, idx)) {
        if (!CheckIndexValid(idx, inputs)) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        return SetPaddingPara(aippPara, inputs[idx]->GetSize(), inputs[idx]->GetData());
    } else {
        if (aippConfig.aippParamInfo.enablePadding) {
            return aippPara->SetPaddingPara(0, std::move(aippConfig.aippParamInfo.paddingPara));
        }
    }
    return hiai::SUCCESS;
}

static void SetInputParam(hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs, std::shared_ptr<IAIPPPara>& aippPara)
{
    std::shared_ptr<IImageTensorBuffer> imageTensorBuffer =
        std::dynamic_pointer_cast<IImageTensorBuffer>(inputs[aippConfig.graphDataIdx]);
    if (imageTensorBuffer != nullptr) {
        aippPara->SetInputFormat(imageTensorBuffer->Format());
        std::vector<int32_t> shape = {imageTensorBuffer->Width(), imageTensorBuffer->Height()};
        aippPara->SetInputShape(shape);
    }
    aippPara->SetInputIndex(aippConfig.tensorDataIdx);
}

static Status ConvertParams(hiai::AippPreprocessConfig& aippConfig,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs, std::shared_ptr<IAIPPPara>& aippPara)
{
    using SetAippPramFunc = std::function<hiai::Status(std::shared_ptr<IAIPPPara>&,
        hiai::AippPreprocessConfig & aippConfig, const std::vector<std::shared_ptr<INDTensorBuffer>>&)>;
    std::vector<SetAippPramFunc> convertFuncList {PrepareCropParam, PrepareChannelSwapParam, PrepareCscParam,
        PrepareResizeParam, PrepareDtcParam, PreparePaddingParam};

    for (const auto& func : convertFuncList) {
        if (func(aippPara, aippConfig, inputs) != SUCCESS) {
            return FAILED;
        }
    }

    return SUCCESS;
}

static Status ExtractAippPreprocessConfig(const CustomModelData& customModelData, size_t& dynamicInputCount,
    std::vector<hiai::AippPreprocessConfig>& aippConfigVec)
{
    size_t aippNodeCnt = customModelData.value.size() / sizeof(hiai::AippPreprocessConfig);
    for (size_t i = 0; i < aippNodeCnt; i++) {
        hiai::AippPreprocessConfig aippConfig;
        size_t size = sizeof(hiai::AippPreprocessConfig);
        int ret = memcpy_s(&aippConfig, size,
            reinterpret_cast<const void*>(
                reinterpret_cast<const char*>(customModelData.value.data()) + i * size),
            size);
        if (ret != 0) {
            FMK_LOGE("ExtractAippPreprocessConfig faliled");
            return FAILED;
        }
        if (aippConfig.configDataCnt >= 0) {
            dynamicInputCount += static_cast<size_t>(aippConfig.configDataCnt);
        }
        aippConfigVec.push_back(aippConfig);
    }
    return SUCCESS;
}

static Status ConvertInputs2DataInptusParaInputs(std::vector<hiai::AippPreprocessConfig>& aippConfigs,
    const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
    std::vector<std::shared_ptr<INDTensorBuffer>>& dataInputs, std::vector<std::shared_ptr<IAIPPPara>>& paraInputs)
{
    std::vector<bool> inputsVisited(inputs.size(), false);
    for (size_t i = 0; i < aippConfigs.size(); i++) {
        if (aippConfigs[i].graphDataIdx < 0 || static_cast<uint32_t>(aippConfigs[i].graphDataIdx) >= inputs.size()) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        if (aippConfigs[i].tensorDataIdx < 0 ||
            static_cast<uint32_t>(aippConfigs[i].tensorDataIdx) >= dataInputs.size()) {
            FMK_LOGE("inputs size error");
            return hiai::FAILED;
        }
        dataInputs[aippConfigs[i].tensorDataIdx] = inputs[aippConfigs[i].graphDataIdx];
        inputsVisited[aippConfigs[i].graphDataIdx] = true;
        std::shared_ptr<IAIPPPara> aippPara = CreateAIPPPara(1);
        SetInputParam(aippConfigs[i], inputs, aippPara);
        aippPara->SetInputAippIndex(i);
        if (ConvertParams(aippConfigs[i], inputs, aippPara) != SUCCESS) {
            return FAILED;
        }

        paraInputs.push_back(aippPara);

        for (int32_t j = 0; j < aippConfigs[i].configDataCnt; j++) {
            inputsVisited[aippConfigs[i].configDataInfo[j].idx] = true;
        }
    }

    size_t j = 0;
    for (size_t i = 0; i < dataInputs.size(); i++) {
        if (dataInputs[i] != nullptr) {
            continue;
        }

        for (; j < inputsVisited.size(); j++) {
            if (!inputsVisited[j]) {
                dataInputs[i] = inputs[j];
                inputsVisited[j] = true;
                break;
            }
        }
    }

    return SUCCESS;
}

Status AippInputConverter::ConvertInputs(const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
    const CustomModelData& customModelData, std::vector<std::shared_ptr<INDTensorBuffer>>& dataInputs,
    std::vector<std::shared_ptr<IAIPPPara>>& paraInputs)
{
    std::vector<AippPreprocessConfig> aippConfigs;
    size_t dynamicInputCount = 0;

    if (ExtractAippPreprocessConfig(customModelData, dynamicInputCount, aippConfigs) != SUCCESS) {
        return hiai::FAILED;
    }
    if (inputs.size() <= dynamicInputCount) {
        FMK_LOGE("inputs size error");
        return hiai::FAILED;
    }
    dataInputs.resize(inputs.size() - dynamicInputCount);
    ConvertInputs2DataInptusParaInputs(aippConfigs, inputs, dataInputs, paraInputs);

    return SUCCESS;
}

static NDTensorDesc MakeNDTesnorDescWithType(int type)
{
    static std::map<int, int> size {
        {AIPP_FUNC_IMAGE_CROP_V2, sizeof(CropPara)},
        {AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2, sizeof(ChannelSwapPara)},
        {AIPP_FUNC_IMAGE_COLOR_SPACE_CONVERTION_V2, sizeof(CscPara)},
        {AIPP_FUNC_IMAGE_RESIZE_V2, sizeof(ResizePara)},
        {AIPP_FUNC_IMAGE_DATA_TYPE_CONVERSION_V2, sizeof(DtcPara)},
        {AIPP_FUNC_IAMGE_PADDING_V2, sizeof(PadPara)},
    };

    NDTensorDesc ndTensorDesc;

    const auto& iter = size.find(type);
    if (iter == size.cend()) {
        return ndTensorDesc;
    }

    std::vector<int32_t> dims {1, iter->second, 1, 1};
    ndTensorDesc.format = Format::NCHW;
    ndTensorDesc.dataType = DataType::UINT8;
    ndTensorDesc.dims = dims;

    return ndTensorDesc;
}

static void ConvertInputTesnor2NewInputTesnor(std::vector<AippPreprocessConfig>& aippConfig,
    std::vector<NDTensorDesc>& dataInputTensorDesc, std::vector<NDTensorDesc>& modelInputTensor,
    std::vector<bool>& dataInputTensorVisited, std::vector<bool>& modelInputTensorVisited)
{
    for (size_t i = 0; i < aippConfig.size(); i++) {
        modelInputTensor[aippConfig[i].graphDataIdx] = dataInputTensorDesc[aippConfig[i].tensorDataIdx];
        modelInputTensorVisited[aippConfig[i].graphDataIdx] = true;
        dataInputTensorVisited[aippConfig[i].tensorDataIdx] = true;
        for (int32_t j = 0; j < aippConfig[i].configDataCnt; j++) {
            modelInputTensor[aippConfig[i].configDataInfo[j].idx] =
                MakeNDTesnorDescWithType(aippConfig[i].configDataInfo[j].type);
            modelInputTensorVisited[aippConfig[i].configDataInfo[j].idx] = true;
        }
    }
}

static void MakeInputTensor(std::vector<AippPreprocessConfig>& aippConfig,
    std::vector<NDTensorDesc>& dataInputTensorDesc, std::vector<NDTensorDesc>& modelInputTensor)
{
    std::vector<bool> dataInputTensorVisited(dataInputTensorDesc.size(), false);
    std::vector<bool> modelInputTensorVisited(dataInputTensorDesc.size(), false);

    ConvertInputTesnor2NewInputTesnor(
        aippConfig, dataInputTensorDesc, modelInputTensor, dataInputTensorVisited, modelInputTensorVisited);

    size_t j = 0;
    for (size_t i = 0; i < modelInputTensor.size(); i++) {
        if (modelInputTensorVisited[i]) {
            continue;
        }

        for (; j < dataInputTensorDesc.size(); j++) {
            if (!dataInputTensorVisited[j]) {
                modelInputTensor[i] = dataInputTensorDesc[j];
                dataInputTensorVisited[j] = true;
                break;
            }
        }
    }
}

Status AippInputConverter::ConvertInputTensorDesc(
    const CustomModelData& customModelData, std::vector<NDTensorDesc>& inputTensorDescVec)
{
    std::vector<NDTensorDesc> modelInputTensor;
    std::vector<hiai::AippPreprocessConfig> aippConfigVec;
    size_t dynamicInputCount = 0;
    if (customModelData.type.empty() || strcmp(customModelData.type.c_str(), AIPP_PREPROCESS_TYPE) != 0) {
        return SUCCESS;
    }

    if (ExtractAippPreprocessConfig(customModelData, dynamicInputCount, aippConfigVec) != SUCCESS) {
        return hiai::FAILED;
    }
    modelInputTensor.resize(dynamicInputCount + inputTensorDescVec.size());
    MakeInputTensor(aippConfigVec, inputTensorDescVec, modelInputTensor);

    inputTensorDescVec.assign(modelInputTensor.begin(), modelInputTensor.end());
    return SUCCESS;
}
} // namespace hiai