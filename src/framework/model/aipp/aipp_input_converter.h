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
#ifndef FRAMEWORK_MODEL_MANAGER_AIPP_COMPATIBLE_H
#define FRAMEWORK_MODEL_MANAGER_AIPP_COMPATIBLE_H

#include "model/built_model_aipp.h"

namespace hiai {
enum AIPP_FUNC_INDEX {
    AIPP_FUNC_IMAGE_CROP_V2,
    AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2,
    AIPP_FUNC_IMAGE_COLOR_SPACE_CONVERTION_V2,
    AIPP_FUNC_IMAGE_RESIZE_V2,
    AIPP_FUNC_IMAGE_DATA_TYPE_CONVERSION_V2,
    AIPP_FUNC_IAMGE_ROTATION_V2,
    AIPP_FUNC_IAMGE_PADDING_V2,
};

class AippInputConverter {
public:
    AippInputConverter() = default;
    ~AippInputConverter() = default;

    static Status ConvertInputs(const std::vector<std::shared_ptr<INDTensorBuffer>>& inputs,
        const CustomModelData& customModelData, std::vector<std::shared_ptr<INDTensorBuffer>>& dataInputs,
        std::vector<std::shared_ptr<IAIPPPara>>& paraInputs);

    static Status ConvertInputTensorDesc(
        const CustomModelData& customModelData, std::vector<NDTensorDesc>& inputTensorDescVec);
};
} // namespace hiai

#endif
