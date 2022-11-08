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
#ifndef HIAI_API_BUILT_MODEL_AIPP_H
#define HIAI_API_BUILT_MODEL_AIPP_H

#include "built_model.h"
#include "tensor/aipp_para.h"

namespace hiai {
static const char* AIPP_PREPROCESS_TYPE = "AippPreprocessConfig";
constexpr int32_t MAX_AIPP_FUNC_SIZE = 7;

struct AippConfigDataInfo {
    int32_t idx;
    int32_t type;
};

struct AippParamInfo {
    ImageFormat inputFormat = ImageFormat::INVALID;
    bool enableCrop = false;
    CropPara cropPara;
    ChannelSwapPara channelSwapPara;
    bool enableCsc = false;
    CscPara cscPara;
    CscMatrixPara cscMatrixPara;
    bool enableResize = false;
    ResizePara resizePara;
    bool enableDtc = false;
    DtcPara dtcPara;
    bool enablePadding = false;
    PadPara paddingPara;
    bool enableRotate = false;
    RotatePara rotatePara;
};

struct AippPreprocessConfig {
    int32_t graphDataIdx;
    int32_t tensorDataIdx;

    int32_t configDataCnt; // 每一个aipp_node对应的动态aipp param输入的个数
    AippConfigDataInfo configDataInfo[MAX_AIPP_FUNC_SIZE]; // 每一个aipp_node对应的动态aipp param输入的idx和类型

    AippParamInfo aippParamInfo; // 每一个aipp_node对应的静态输入的aipp param
};

class IBuiltModelAipp : public IBuiltModel {
public:
    virtual ~IBuiltModelAipp() = default;

    virtual Status GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount) = 0;

    virtual Status GetTensorAippPara(int32_t index, std::vector<void*>& aippParas) const = 0;
};
} // namespace hiai
#endif // HIAI_API_BUILT_MODEL_AIPP_H
