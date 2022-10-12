/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: built model
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
