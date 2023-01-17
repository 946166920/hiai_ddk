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
#include "ir_build_transformer.h"
#include <list>
#include "framework/graph/utils/op_desc_utils.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_functor.h"
#include "graph/op/nn_defs.h"
#include "common/math/math_util.h"
#include "framework/util/tensor/trans_tensor.h"
#include "framework/util/rom_version_util.h"
#include "framework/graph/debug/ge_log.h"

namespace hiai {
const std::string BASE_UX11_VERSION = "100.333";
const std::string BASE_BALC10_VERSION = "100.500";

ge::GraphErrCodeStatus TransWeightKCHW2HWCK(ge::Tensor* tensor)
{
    if (tensor == nullptr) {
        return ge::GRAPH_FAILED;
    }
    std::vector<int64_t> oldDims = tensor->GetTensorDesc().GetShape().GetDims();
    if (oldDims.size() != 4) {
        FMK_LOGE("dim-num is not supported, dim-num:%zu", oldDims.size());
        return ge::GRAPH_FAILED;
    }

    int32_t H = oldDims.at(ge::NCHW_DIM_H);
    int32_t W = oldDims.at(ge::NCHW_DIM_W);
    int32_t C = oldDims.at(ge::NCHW_DIM_C);
    int32_t K = oldDims.at(ge::NCHW_DIM_N);

    tensor->MutableTensorDesc().SetShape(ge::Shape({H, W, C, K}));

    const char* wData = reinterpret_cast<const char*>(tensor->GetData().GetData());
    size_t count = static_cast<size_t>(H * W * C * K);
    if (CheckInt32MulOverflow(H, W) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("int32 mul overflow");
        return ge::GRAPH_FAILED;
    }
    if (CheckInt32MulOverflow(H * W, C) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("int32 mul overflow");
        return ge::GRAPH_FAILED;
    }
    if (CheckInt32MulOverflow(H * W * C, K) != ge::GRAPH_SUCCESS) {
        FMK_LOGE("int32 mul overflow");
        return ge::GRAPH_FAILED;
    }
    if (count == 0) {
        FMK_LOGE("Dim size invalid");
        return ge::GRAPH_FAILED;
    }
    if (tensor->GetTensorDesc().GetDataType() == ge::DT_FLOAT) {
        if (tensor->GetData().GetSize() / sizeof(float) != count) {
            FMK_LOGE("tensor size:%u is not equal to dim size:%u", tensor->GetData().GetSize() / sizeof(float), count);
            return ge::GRAPH_FAILED;
        }
        float* buf = new (std::nothrow) float[count]();
        if (buf == nullptr) {
            return ge::GRAPH_FAILED;
        }
        float* srcBuff = nullptr;
        float* dstBuff = nullptr;
        for (int32_t k = 0; k < K; ++k) {
            for (int32_t c = 0; c < C; ++c) {
                for (int32_t h = 0; h < H; ++h) {
                    for (int32_t w = 0; w < W; ++w) {
                        srcBuff = reinterpret_cast<float*>(const_cast<char*>(wData)) +
                            ((k * C * H * W) + (c * H * W) + (h * W) + (w));

                        dstBuff = buf + ((h * W * C * K) + (w * C * K) + (c * K) + (k));

                        *dstBuff = *srcBuff;
                    }
                }
            }
        }
        tensor->SetData(reinterpret_cast<uint8_t*>(buf), count * sizeof(float));
        delete[] buf;
        buf = nullptr;
    }
    return ge::GRAPH_SUCCESS;
}

ge::GraphErrCodeStatus ConvTransWeight(ge::Node& node)
{
    std::string dataFormat;
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    (void)ge::AttrUtils::GetStr(opDesc, "data_format", dataFormat);
    if (dataFormat != "NHWC") {
        return ge::GRAPH_SUCCESS;
    }
    vector<ge::TensorPtr> weights = ge::OpDescUtils::MutableWeights(node);
    if (weights.size() == 1) {
        if (TransWeightKCHW2HWCK(weights[0].get()) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("trans KCHW2HWCK weight failed.");
            return ge::GRAPH_FAILED;
        }
    } else if (weights.size() == 2) {
        ge::TensorDesc& weightDesc = weights[1]->MutableTensorDesc();
        ge::Shape weightShape = weightDesc.GetShape();
        vector<int64_t> shapeValue = weightShape.GetDims();
        std::list<uint32_t> newDimList;
        for (auto dimTemp : shapeValue) {
            newDimList.push_back(dimTemp);
        }
        if (shapeValue.size() == 1) {
            newDimList.push_front(1);
            newDimList.push_back(1);
            newDimList.push_back(1);
        }
        std::vector<int64_t> dimVector;
        for (auto dim : newDimList) {
            dimVector.push_back(dim);
        }
        weightDesc.SetShape(ge::Shape(dimVector));
        if (TransWeightKCHW2HWCK(weights[1].get()) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("trans KCHW2HWCK weight failed.");
            return ge::GRAPH_FAILED;
        }
    }
    return ge::GRAPH_SUCCESS;
}

bool IRBuildTransformer::TransferIRToTargetVersion(ge::ComputeGraphPtr graph, string aiRomVersion)
{
    if (graph == nullptr) {
        FMK_LOGE("graph is null.");
        return false;
    }
    bool is3rdRomVersion = Is3rdRomVersion(aiRomVersion.c_str());
    if (!aiRomVersion.empty()) {
        if (aiRomVersion.length() > BASE_UX11_VERSION.length()) {
            aiRomVersion = aiRomVersion.substr(0, BASE_UX11_VERSION.length());
        }
    } else {
        FMK_LOGE("rom version is empty");
        return false;
    }
    if ((aiRomVersion >= BASE_UX11_VERSION && aiRomVersion != BASE_BALC10_VERSION) || is3rdRomVersion) {
        return true;
    }

    hiai::Status ret = graph->ROLE(GraphListWalker).WalkAllNodes(ge::NodeFunctor::Typed({hiai::op::Convolution::TYPE,
        hiai::op::ConvolutionDepthwise::TYPE, hiai::op::ConvTranspose::TYPE},
        [](Node& node) { return ConvTransWeight(node); }));

    return ret == hiai::SUCCESS;
}
} // namespace hiai
