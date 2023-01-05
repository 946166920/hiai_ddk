/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 定义压缩的数学核心工具类方法
 */

#include "omg/quantize_optimizer/quantize_math_util.h"

#include <map>
#include "infra/base/assertion.h"
#include "framework/infra/log/log.h"
#include "framework/graph/core/node/node_spec.h"
#include "graph/op/nn_defs.h"
#include "common/math/math_util.h"

using namespace std;
using namespace ge;

namespace hiai {
namespace {
const float DIV_EPS_QUANT = 1e-8;
Status CalcDeconvKernelInfo(
    const ge::Shape& filterShape, uint32_t& kernelSize, uint32_t& kernelNum, uint32_t& kernelDataCount)
{
    // Deconv算子kernel num值为c维度
    uint32_t deconvCoutDim = 1;
    kernelNum = static_cast<uint32_t>(filterShape.GetDim(deconvCoutDim));
    // 获取卷积核kernel C*H*W大小
    kernelSize = 1;
    for (size_t i = 0; i < filterShape.GetDimNum(); ++i) {
        if (i == deconvCoutDim) {
            continue;
        }
        if (CheckUint32MulOverflow(kernelSize, filterShape.GetDim(i)) != hiai::SUCCESS) {
            FMK_LOGE("Calculating kernel size occur overflow.");
            return hiai::FAILED;
        }
        kernelSize *= static_cast<uint32_t>(filterShape.GetDim(i));
    }
    if (CheckUint32MulOverflow(kernelNum, kernelSize) != hiai::SUCCESS) {
        FMK_LOGE("Calculating weight data size occur overflow.");
        return hiai::FAILED;
    }
    kernelDataCount = kernelNum * kernelSize;
    return hiai::SUCCESS;
}

Status CalcKernelInfo(
    const ge::Shape& filterShape, uint32_t& kernelSize, uint32_t& kernelNum, uint32_t& kernelDataCount)
{
    // 获取卷积核kernel C*H*W大小
    kernelSize = 1;
    for (size_t i = 1; i < filterShape.GetDimNum(); ++i) {
        if (CheckUint32MulOverflow(kernelSize, filterShape.GetDim(i)) != hiai::SUCCESS) {
            FMK_LOGE("Calculating kernel size occur overflow.");
            return hiai::FAILED;
        }
        kernelSize *= static_cast<uint64_t>(filterShape.GetDim(i));
    }

    // 将weight值转成int8
    kernelNum = static_cast<uint32_t>(filterShape.GetDim(0));
    if (CheckUint32MulOverflow(kernelNum, kernelSize) != hiai::SUCCESS) {
        FMK_LOGE("Calculating kernel size occur overflow.");
        return hiai::FAILED;
    }
    kernelDataCount = kernelNum * kernelSize;
    return hiai::SUCCESS;
}

unsigned char TransWeights2Bits(char filterValue)
{
    if (filterValue == 0) {
        return 0u;
    } else if (filterValue == 1) {
        return 1u;
    } else {
        // 2Bit -1用2表示
        return 2u;
    }
}

int8_t TransWeightsInt4(int8_t filterValue)
{
    int8_t filter4Bit = filterValue;
    if (filterValue < 0) {
        //  UINT4表示的最大值16
        filter4Bit = filterValue + 16;
    }
    filter4Bit = filter4Bit & 0x0f;
    return filter4Bit;
}

uint32_t CompressInt4ToInt8(int8_t* oriInput, uint32_t intputSize)
{
    const int32_t int4MaxValue = 7;
    const int32_t int4MinValue = -8;

    // size为inputSize对2向上取整
    uint32_t size = (intputSize + 1) / 2;
    for (uint32_t i = 0; i < size; i++) {
        // 一个INT8可以存储2个INT4
        uint32_t index = 2 * i;
        int32_t intVal = oriInput[index];
        if (intVal > int4MaxValue || intVal < int4MinValue) {
            FMK_LOGE("Value %d is out of range [%d, %d].", intVal, int4MinValue, int4MaxValue);
            return 0;
        }
        uint8_t filterValue1 = static_cast<uint8_t>(TransWeightsInt4(oriInput[index]));
        uint8_t filterValue2 = 0;
        if (index + 1 < intputSize) {
            intVal = oriInput[index + 1];
            if (intVal > int4MaxValue || intVal < int4MinValue) {
                FMK_LOGE("Value %d is out of range [%d, %d].", intVal, int4MinValue, int4MaxValue);
                return 0;
            }
            // index+1表示取index下一个，向左移4位
            filterValue2 = static_cast<uint8_t>(TransWeightsInt4(oriInput[index + 1])) << 4;
        }
        oriInput[i] = (filterValue1 & 0x0f) | (filterValue2 & 0xf0);
    }
    return size;
}

uint32_t CompressInt2ToInt8(int8_t* oriInput, uint32_t intputSize)
{
    //  size为inputSize对4向上取整
    uint32_t size = (intputSize + 3) / 4;
    for (uint32_t i = 0; i < size; i++) {
        //  一个INT8可以存储4个INT2
        uint32_t index = 4 * i;
        uint8_t filterValue1 = static_cast<uint8_t>(TransWeights2Bits(oriInput[index]));
        uint8_t filterValue2 = 0;
        if (index + 1 < intputSize) {
            // index+1表示取index下一个，向左移2位
            filterValue2 = TransWeights2Bits(oriInput[index + 1]) << 2;
        }
        uint8_t filterValue3 = 0;
        // index+2表示取index下二个
        if (index + 2 < intputSize) {
            // index+2表示取index下二个，向左移4位
            filterValue3 = TransWeights2Bits(oriInput[index + 2]) << 4;
        }
        uint8_t filterValue4 = 0;
        // index+3表示取index下三个
        if (index + 3 < intputSize) {
            // index+3表示取index下三个，向左移6位
            filterValue4 = TransWeights2Bits(oriInput[index + 3]) << 6;
        }
        oriInput[i] = (filterValue1 & 0x03) | (filterValue2 & 0x0c) | (filterValue3 & 0x30) | (filterValue4 & 0xc0);
    }
    return size;
}

Status DeCompressData(
    float scaleWeight, const int8_t* weightData, float* weightDataNew, int64_t windowIndex, const Shape& shape)
{
    const int64_t dimH = 2;
    const int64_t dimW = 3;
    for (int64_t hIndex = 0; hIndex < shape.GetDim(dimH); hIndex++) {
        for (int64_t wIndex = 0; wIndex < shape.GetDim(dimW); wIndex++) {
            // index = ((n*C+c)*H + h)*W + w
            int64_t index = (windowIndex * shape.GetDim(dimH) + hIndex) * shape.GetDim(dimW) + wIndex;
            weightDataNew[index] = static_cast<float>(weightData[index]) * scaleWeight;
        }
    }
    return hiai::SUCCESS;
}

Status CalcDeconvFP32Data(
    const float* weightScale, const int8_t* weightData, float* weightDataNew, const TensorDesc& weightTensor)
{
    const int64_t DECONV_DIM_C_IN = 0;
    const int64_t DECONV_DIM_C_OUT = 1;
    const Shape& shape = weightTensor.GetShape();
    for (int64_t nIndex = 0; nIndex < shape.GetDim(DECONV_DIM_C_IN); nIndex++) {
        for (int64_t cIndex = 0; cIndex < shape.GetDim(DECONV_DIM_C_OUT); cIndex++) {
            // 如果融合的scaleWeight是负数，需要改成正的，这样保存的量化weight和bias值也相应会乘以-1
            // 即将符号提取到weight和bias中，量化参数就不会为负数了
            float scaleWeight = weightScale[cIndex];
            int64_t windowIndex = nIndex * shape.GetDim(DECONV_DIM_C_OUT) + cIndex;
            if (DeCompressData(scaleWeight, weightData, weightDataNew, windowIndex, shape) != hiai::SUCCESS) {
                return hiai::FAILED;
            }
        }
    }
    return hiai::SUCCESS;
}

// 反量化INT8数据为FP32数据
void CalcFP32Data(const float* weightScale, uint32_t scaleSize, const int8_t* weightData, float* weightDataNew,
    const TensorDesc& weightTensor)
{
    float scaleWeight;
    int64_t kernelDataCount = weightTensor.GetShape().GetTotalDimNum();
    int64_t kernelNum = weightTensor.GetShape().GetDim(0);
    int64_t kernelSize = kernelDataCount / kernelNum;
    for (uint32_t i = 0; i < kernelNum; i++) {
        // 如果融合的scaleWeight是负数，需要改成正的，这样保存的量化weight和bias值也相应会乘以-1
        // 即将符号提取到weight和bias中，量化参数就不会为负数了
        if (scaleSize <= 1) {
            scaleWeight = weightScale[0];
        } else {
            scaleWeight = weightScale[i];
        }
        for (uint32_t j = 0; j < kernelSize; j++) {
            uint32_t index = i * static_cast<uint32_t>(kernelSize) + j;
            weightDataNew[index] = static_cast<float>(weightData[index]) * scaleWeight;
        }
    }
}

Status CompressInt8Data(float weightData, int8_t& quantizedData, float weightScale)
{
    float weight = weightData / weightScale;
    int32_t roundData = static_cast<int32_t>(round(weight));
    roundData = (roundData > INT8_MAX) ? INT8_MAX : roundData;
    roundData = (roundData < INT8_MIN) ? INT8_MIN : roundData;

    quantizedData = static_cast<int8_t>(roundData);
    return hiai::SUCCESS;
}

Status CompressInt4Data(float weightData, int8_t& quantizedData, float weightScale)
{
    const float int4Max = 7;
    const float int4Min = -8;

    float weight = weightData / weightScale;
    int8_t roundData = static_cast<int8_t>(round(weight));
    if (roundData > int4Max || roundData < int4Min) {
        FMK_LOGE(
            "CalcInt4Weight fail, exceeds int4 ranges(origin data:%3.10f, scale:%3.10f).", weightData, weightScale);
        return hiai::FAILED;
    }
    quantizedData = static_cast<int8_t>(roundData);
    return hiai::SUCCESS;
}

Status CompressInt2Data(float weightData, int8_t& quantizedData, float weightScale)
{
    const float zeroEpsilon = 1e-7;
    float scaleWeightNeg = -1 * weightScale;
    int8_t roundData = 0;
    if (fabsf(weightData) < zeroEpsilon) {
        roundData = 0;
    } else if (fabs(weightData - weightScale) < zeroEpsilon) {
        roundData = 1;
    } else if (fabs(weightData - scaleWeightNeg) < zeroEpsilon) {
        roundData = -1;
    } else {
        FMK_LOGE("Filter to 2bit failed, scale is %2.11f, but filters has %2.11f.", weightScale, weightData);
        return hiai::FAILED;
    }
    quantizedData = static_cast<int8_t>(roundData);
    return hiai::SUCCESS;
}

Status CalcDeconvInt8Data(
    const float* weightScale, const float* weightData, int8_t* weightDataNew, const TensorDesc& weightTensor)
{
    const int64_t DECONV_DIM_C_IN = 0;
    const int64_t DECONV_DIM_C_OUT = 1;
    const int64_t DECONV_DIM_H = 2;
    const int64_t DECONV_DIM_W = 3;
    ge::DataType dataType = weightTensor.GetDataType();
    if (dataType != DT_INT8) {
        FMK_LOGE("Deconv quantize not support data type:%d.", dataType);
        return hiai::FAILED;
    }
    const Shape& shape = weightTensor.GetShape();
    uint32_t CinDim = static_cast<uint32_t>(shape.GetDim(DECONV_DIM_C_IN));
    uint32_t CoutDim = static_cast<uint32_t>(shape.GetDim(DECONV_DIM_C_OUT));
    uint32_t hDim = static_cast<uint32_t>(shape.GetDim(DECONV_DIM_H));
    uint32_t wDim = static_cast<uint32_t>(shape.GetDim(DECONV_DIM_W));
    for (uint32_t nIndex = 0; nIndex < CinDim; nIndex++) {
        for (uint32_t cIndex = 0; cIndex < CoutDim; cIndex++) {
            float scaleWeight = weightScale[cIndex];
            scaleWeight = (scaleWeight < DIV_EPS_QUANT) ? DIV_EPS_QUANT : scaleWeight;
            uint32_t windowSize = hDim * wDim;
            for (uint32_t i = 0; i < windowSize; i++) {
                uint32_t index = (nIndex * CoutDim + cIndex) * windowSize + i;
                HIAI_EXPECT_EXEC(CompressInt8Data(weightData[index], weightDataNew[index], scaleWeight));
            }
        }
    }
    return hiai::SUCCESS;
}

Status CalcInt8Data(const float* weightScale, uint32_t weightScaleSize, const float* weightData, int8_t* weightDataNew,
    const TensorDesc& weightTensor)
{
    float scaleWeight;
    int64_t kernelDataCount = weightTensor.GetShape().GetTotalDimNum();
    int64_t kernelNum = weightTensor.GetShape().GetDim(0);
    if (kernelNum == 0) {
        FMK_LOGE("kernelNum is 0");
        return hiai::FAILED;
    }
    int64_t kernelSize = kernelDataCount / kernelNum;
    ge::DataType dataType = weightTensor.GetDataType();
    for (int64_t i = 0; i < kernelNum; i++) {
        // 如果融合的scaleWeight是负数，需要改成正的，这样保存的量化weight和bias值也相应会乘以-1
        // 即将符号提取到weight和bias中，量化参数就不会为负数了
        if (weightScaleSize <= 1) {
            scaleWeight = weightScale[0];
        } else {
            scaleWeight = weightScale[i];
        }
        scaleWeight = (scaleWeight < DIV_EPS_QUANT) ? DIV_EPS_QUANT : scaleWeight;

        for (int64_t j = 0; j < kernelSize; j++) {
            int64_t index = i * kernelSize + j;
            if (dataType == DT_INT8) {
                HIAI_EXPECT_EXEC(CompressInt8Data(weightData[index], weightDataNew[index], scaleWeight));
            } else if (dataType == DT_INT4) {
                HIAI_EXPECT_EXEC(CompressInt4Data(weightData[index], weightDataNew[index], scaleWeight));
            } else if (dataType == DT_2BIT) {
                HIAI_EXPECT_EXEC(CompressInt2Data(weightData[index], weightDataNew[index], scaleWeight));
            } else {
                FMK_LOGE("data type:%d not support quantize.", dataType);
                return hiai::FAILED;
            }
        }
    }
    return hiai::SUCCESS;
}
} // namespace

Status QuantizeMathUtil::CalculateKernelInfo(
    const Node& node, const Tensor& filter, uint32_t& kernelSize, uint32_t& kernelNum, uint32_t& kernelDataCount)
{
    const ge::Shape& filterShape = filter.GetTensorDesc().GetShape();
    if (node.ROLE(NodeSpec).OpDesc().GetType() == hiai::op::ConvTranspose::TYPE) {
        return CalcDeconvKernelInfo(filterShape, kernelSize, kernelNum, kernelDataCount);
    }
    return CalcKernelInfo(filterShape, kernelSize, kernelNum, kernelDataCount);
}

// 检查权重信息和量化参数是否合法
Status QuantizeMathUtil::CheckWeightParams(
    const Tensor& filter, const vector<float>& weightScale, uint32_t kernelNum, uint32_t weightDataSize)
{
    map<DataType, uint32_t> dataTypeSizes = { { DT_FLOAT, sizeof(float) }, { DT_INT8, sizeof(int8_t) } };
    ge::DataType weightDataType = filter.GetTensorDesc().GetDataType();
    map<DataType, uint32_t>::const_iterator it = dataTypeSizes.find(weightDataType);
    if (it == dataTypeSizes.cend()) {
        FMK_LOGE("data type: %d not support.", filter.GetTensorDesc().GetDataType());
        return hiai::FAILED;
    }
    uint32_t realKernelDataSize = (filter.GetData().GetSize() / it->second);
    if (realKernelDataSize != weightDataSize) {
        FMK_LOGE("realKernelDataSize[%u] is not equal to weightDataSize[%u].", realKernelDataSize, weightDataSize);
        return hiai::FAILED;
    }
    uint32_t scaleWeightSize = weightScale.size();
    if (scaleWeightSize >= (UINT_MAX / 4) || scaleWeightSize < 1) {
        FMK_LOGE("ScaleWeightSize:%u large than uint_max/4 or less than 1.", scaleWeightSize);
        return hiai::FAILED;
    }
    if ((scaleWeightSize > 1) && (scaleWeightSize != kernelNum)) {
        FMK_LOGE("ScaleWeightSize:%u need equan with kernelNum:%u and great than 0.", scaleWeightSize, kernelNum);
        return hiai::FAILED;
    }
    return hiai::SUCCESS;
}

Status QuantizeMathUtil::CalculateFP32Data(
    const Node& node, Tensor& filter, const vector<float>& weightScale, float* weightDataNew)
{
    Status ret = hiai::SUCCESS;
    const int8_t* weightData = reinterpret_cast<const int8_t*>(filter.GetData().GetData());
    if (!weightData) {
        FMK_LOGE("weightData point is null.");
        return hiai::FAILED;
    }

    const TensorDesc& weightTensor = filter.GetTensorDesc();
    if (node.ROLE(NodeSpec).OpDesc().GetType() == hiai::op::ConvTranspose::TYPE) {
        if (weightTensor.GetShape().GetDimNum() != 4) {  // ConvTranspose 要求权值必须是4维的
            FMK_LOGE("weightData dim num is illegal,not 4 dims.");
            return hiai::FAILED;
        }
    }

    if (node.ROLE(NodeSpec).OpDesc().GetType() == hiai::op::ConvTranspose::TYPE) {
        ret = CalcDeconvFP32Data(weightScale.data(), weightData, weightDataNew, weightTensor);
    } else {
        uint32_t scaleWeightSize = weightScale.size();
        CalcFP32Data(weightScale.data(), scaleWeightSize, weightData, weightDataNew, weightTensor);
    }
    return ret;
}

Status QuantizeMathUtil::CalculateINT8Data(
    const Node& node, Tensor& filter, const vector<float>& weightScale, int8_t* weightDataNew)
{
    Status ret = hiai::SUCCESS;
    const float* weightData = reinterpret_cast<const float*>(filter.GetData().GetData());
    if (!weightData) {
        FMK_LOGE("weightData point is null.");
        return hiai::FAILED;
    }
    const float* scaleWeightValue = weightScale.data();
    if (scaleWeightValue == nullptr) {
        FMK_LOGE("scaleWeightValue point is null.");
        return hiai::FAILED;
    }
    uint32_t scaleWeightSize = weightScale.size();
    const TensorDesc& weightTensor = filter.GetTensorDesc();
    if (node.ROLE(NodeSpec).Type() == hiai::op::ConvTranspose::TYPE) {
        ret = CalcDeconvInt8Data(scaleWeightValue, weightData, weightDataNew, weightTensor);
    } else {
        ret = CalcInt8Data(scaleWeightValue, scaleWeightSize, weightData, weightDataNew, weightTensor);
    }

    return ret;
}

Status QuantizeMathUtil::CompressWeightToINT8(Tensor& weight, ge::DataType dataType)
{
    uint32_t size = 0;
    int8_t* weightData = reinterpret_cast<int8_t*>(weight.MutableData().MutableData());
    uint32_t kernelDataCount = weight.GetData().GetSize() / sizeof(int8_t);
    if (dataType == DT_2BIT) {
        size = CompressInt2ToInt8(weightData, kernelDataCount);
    } else if (dataType == DT_INT4) {
        size = CompressInt4ToInt8(weightData, kernelDataCount);
    } else {
        FMK_LOGE("data type [%d] is invalid.", dataType);
        return hiai::FAILED;
    }
    HIAI_EXPECT_TRUE(size != 0);
    vector<int8_t> weightDataVec(size);
    int8_t* weightDataInt = weightDataVec.data();
    HIAI_EXPECT_NOT_NULL(weightDataInt);
    for (uint32_t index = 0; index < size; index++) {
        weightDataInt[index] = weightData[index];
    }
    weight.SetData(reinterpret_cast<uint8_t*>(weightDataInt), size * sizeof(int8_t));
    weight.MutableTensorDesc().SetDataType(dataType);
    return hiai::SUCCESS;
}
} // namespace hiai
