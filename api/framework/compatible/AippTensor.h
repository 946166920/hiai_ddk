/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: AippTensor
 */
#ifndef FRAMEWORK_TENSOR_COMPATIBLE_AIPP_TENSOR_H
#define FRAMEWORK_TENSOR_COMPATIBLE_AIPP_TENSOR_H
#include "tensor/tensor_api_export.h"
#include "HiAiAippPara.h"
#include "compatible/AiTensor.h"

namespace hiai {
class HIAI_TENSOR_API_EXPORT AippTensor : public AiTensor {
public:
    AippTensor(std::shared_ptr<AiTensor> tensor, std::vector<std::shared_ptr<AippPara>> aippParas);

    ~AippTensor() override;

    void* GetBuffer() const override;

    uint32_t GetSize() const override;

    std::shared_ptr<AiTensor> GetAiTensor() const;

    std::vector<std::shared_ptr<AippPara>> GetAippParas() const;

    std::shared_ptr<AippPara> GetAippParas(uint32_t index) const;

private:
    using AiTensor::Init;

    std::shared_ptr<AiTensor> tensor;

    std::vector<std::shared_ptr<AippPara>> aippParas;
};

HIAI_TENSOR_API_EXPORT std::shared_ptr<AippTensor> HIAI_CreateAiPPTensorFromHandle(
    void* nativeHandle, const TensorDimension* dim, AiTensorImage_Format imageFormat = AiTensorImage_INVALID);
} // namespace hiai
#endif
