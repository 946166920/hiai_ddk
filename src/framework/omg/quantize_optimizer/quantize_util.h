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
#ifndef FRAMEWORK_OMG_COMPRESS_COMPRESS_UTIL_H
#define FRAMEWORK_OMG_COMPRESS_COMPRESS_UTIL_H

#include <cmath>

#include "securec.h"

#include "infra/base/assertion.h"

#include "framework/infra/log/log.h"
#include "framework/common/hcs_types.h"
#include "framework/graph/core/def_types.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/utils/op_desc_utils.h"
#include "framework/common/fmk_error_codes.h"
#include "quantize_types.h"
#include "omg/quantize_optimizer/quantize_types.h"

namespace hiai {
class QuantizeUtil {
public:
    QuantizeUtil() = default;
    ~QuantizeUtil() = default;
    /**
     *  @brief 检查指定的算子类型是否支持量化
     */
    static bool IsSupportQuantOpType(const std::string& opType);
    /**
     *  @brief 检查图是否为单边量化
     */
    static Status CheckOneSideQuantize(ge::ComputeGraph& graph, bool& isOneSideQuant);
    /**
     *  @brief graph中设置单边量化标志
     */
    static hiai::Status SetOneSideQuantize(ge::ComputeGraph& graph, bool value);
    /**
     *  @brief  设置初始值
     */
    template <typename Dtype> static hiai::Status NnSet(const int32_t n, const Dtype alpha, Dtype* output)
    {
        HIAI_EXPECT_NOT_NULL_R(output, hiai::PARAM_INVALID);

        if (fabs(alpha) < 1e-9) {
            int err = memset_s(output, sizeof(Dtype) * n, 0, sizeof(Dtype) * n);
            HIAI_EXPECT_TRUE_R(err == 0, hiai::PARAM_INVALID);
        }

        for (int32_t i = 0; i < n; ++i) {
            output[i] = alpha;
        }
        return hiai::SUCCESS;
    }

    /**
     *  @ingroup domi_omg
     *  @brief  获取node的filter权值,用户需保证node不为空且存在权值
     */
    static ge::TensorPtr GetFilterTensor(const ge::Node* node);
    /**
     *  @ingroup domi_omg
     *  @brief  获取node的filter权值的Anchor索引,用户需保证node不为空且存在权值
     */
    static uint32_t GetFilterIndex(const ge::Node* node);
    /**
     *  @ingroup domi_omg
     *  @brief  反量化计算图
     */
    static hiai::Status DequantizeComputeGraph(ge::ComputeGraph& graph);
    /**
     *  @ingroup domi_omg
     *  @brief 判断图中是否存在量化算子
     *  @param [in] graph 图
     */
    static bool CheckGraphQuantized(ge::ComputeGraph& graph);

    static hiai::Status CheckOpQuantizeInfo(const ge::Node& node, const QuantizeConfig& quantizeConfig);
    static hiai::Status GetOpQuantizeInfo(const ge::Node& node, QuantizeConfig& config);

    static hiai::Status GetQuantizeInfo(const ge::OpDesc& opDesc, ge::QuantizeInfo& quantInfo);
    static hiai::Status SetQuantizeInfo(ge::OpDesc& opDesc, const ge::QuantizeInfo& quantInfo);

    static hiai::Status SetQuantType(ge::OpDesc& opDesc, int64_t quantType);
    static hiai::Status SetQuantInfoExt(ge::OpDesc& opDesc, const std::string& quantInfoExt);

    static hiai::Status SetPowerTransScale(ge::OpDesc& opDesc, float powerTransScale);
    static bool GetPowerTransScale(const ge::OpDesc& opDesc, float& powerTransScale);
    static bool HasPowerTransScale(const ge::OpDesc& opDesc);
    static void DelPowerTransScale(ge::OpDesc& opDesc);

    static hiai::Status SetTransScale(ge::OpDesc& opDesc, const std::vector<float>& transScale);
    static bool GetTransScale(const ge::OpDesc& opDesc, std::vector<float>& transScale);
    static bool HasTransScale(const ge::OpDesc& opDesc);
    static void DelTransScale(ge::OpDesc& opDesc);
    static ge::Node* FindQuantizeWeightNode(ge::Node* node);
    static hiai::Status QuantizeWeight(ge::Node& weightNode, const QuantizeConfig& quantizeConfig);
};

template <typename T>
static hiai::Status GetValueFromString(const std::string& str, const std::string& key, T& value)
{
    auto pos = str.find(key);
    if (pos == std::string::npos) {
        FMK_LOGE("can not find %s.", key.c_str());
        return hiai::FAILURE;
    }
    pos++;
    auto posEnd = str.find(";", pos);
    std::string res;
    if (posEnd != std::string::npos) {
        res = str.substr(pos + key.length(), posEnd - (pos + key.length()));
    } else {
        res = str.substr(pos + key.length());
    }
    std::stringstream ss1;
    ss1 << res;
    ss1 >> value;
    return hiai::SUCCESS;
}
} // namespace hiai

#endif
