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

#include "model_builder/ir/aipp/compatible/hiai_ir_aipp_compatible.h"

#include <vector>
#include <algorithm>

// api/framework
#include "graph/op/image_defs.h"
#include "graph/op/array_defs.h"
#include "graph/op/math_defs.h"
#include "model/built_model_aipp.h"

// inc/common
#include "infra/base/securestl.h"

// inc/framework
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/op/internal_defs.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/node/node_visitor.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_functor.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/debug/ge_attr_define.h"
#include "framework/graph/utils/attr_utils.h"

// src/framework/inc
#include "infra/base/assertion.h"

#include "model_builder/ir/aipp/converter/aipp_param_info_converter.h"
#include "model_builder/ir/aipp/infershape/aipp_infershape_util.h"
#include "hiai_ir_aipp_compatible_adapter_dl.h"

using namespace hiai;

namespace {
class AippImagesInferShaper {
public:
    static hiai::Status InferShape(std::vector<ge::Node*>& nodes)
    {
        for (const auto& node : nodes) {
            auto inferFunc = AippImagesInferShaper::Instance().GetInfershapeFunc(node->ROLE(NodeSpec).Type());
            HIAI_EXPECT_NOT_NULL(inferFunc);

            std::vector<ge::Shape> outputShape;
            std::vector<ge::DataType> outputType;
            HIAI_EXPECT_EXEC(inferFunc(*node, outputShape, outputType));

            for (size_t i = 0; i < outputShape.size(); i++) {
                node->ROLE(NodeSpec).OpDesc().MutableOutputDesc(i)->SetShape(outputShape[i]);
                node->ROLE(NodeSpec).OpDesc().MutableOutputDesc(i)->SetDataType(outputType[i]);
            }

            (void)node->ROLE(NodeWalker).ListOutDataNodes(0, [&node](ge::Node& outNode) {
                outNode.ROLE(NodeSpec).OpDesc().MutableInputDesc(0)->SetShape(
                    node->ROLE(NodeSpec).OpDesc().MutableOutputDesc(0)->GetShape());
                return hiai::SUCCESS;
            });
        }

        return hiai::SUCCESS;
    }

private:
    AippImagesInferShaper()
        : inferShapeFuncs_ {{{op::Data::TYPE}, AippInfershapeUtil::DataShape},
              {{op::ImageCropV2::TYPE}, AippInfershapeUtil::ImageCropV2InferShape},
              {{op::ImageResizeV2::TYPE}, AippInfershapeUtil::ImageResizeV2InferShape},
              {{op::ImagePadV2::TYPE}, AippInfershapeUtil::ImagePadV2InferShape},
              {{op::ImageRotateV2::TYPE}, AippInfershapeUtil::ImageRotateV2InferShape},
              {{op::ImageColorSpaceConvertionV2::TYPE}, AippInfershapeUtil::ImageColorSpaceConvertionV2InferShape},
              {{op::ImageChannelSwapV2::TYPE}, AippInfershapeUtil::ImageChannelSwapV2InferShape},
              {{op::ImageDataTypeConvertionV2::TYPE}, AippInfershapeUtil::ImageDataTypeConvertionV2InferShape},
              {{op::CastT::TYPE}, AippInfershapeUtil::CastTInferShape}}
    {
    }
    virtual ~AippImagesInferShaper() = default;

    static AippImagesInferShaper& Instance()
    {
        static AippImagesInferShaper repo;
        return repo;
    }

    AippImageInferShapeFunc GetInfershapeFunc(const std::string& type)
    {
        for (const auto& inferShape : inferShapeFuncs_) {
            if (inferShape.first == type) {
                return inferShape.second;
            }
        }

        return nullptr;
    }

private:
    std::vector<std::pair<std::string, AippImageInferShapeFunc>> inferShapeFuncs_;
};

class AippImageTypes {
public:
    static constexpr int INVALID_IDX = -1;

public:
    static bool IsAippImageType(const std::string& type)
    {
        return std::find(imageTypes_.cbegin(), imageTypes_.cend(), type) != imageTypes_.cend();
    }

    static bool IsLessSequence(const std::string& first, const std::string& second)
    {
        const auto& firstIter = std::find(imageTypes_.cbegin(), imageTypes_.cend(), first);
        const auto& secondIter = std::find(imageTypes_.cbegin(), imageTypes_.cend(), second);
        return firstIter < secondIter;
    }

    static int GetAippImageTypesIdx(const std::string& type)
    {
        const auto& it = std::find(imageTypes_.cbegin(), imageTypes_.cend(), type);
        return it != imageTypes_.cend() ? std::distance(imageTypes_.cbegin(), it) : INVALID_IDX;
    }

    static std::string GetDynamicParamType(ge::OpDesc& desc)
    {
        std::string paramType;

        HIAI_EXPECT_TRUE_R(ge::AttrUtils::GetStr(desc, "type", paramType), "");

        return paramType;
    }

private:
    static bool IsSupportDynamicAipp(const std::string& type)
    {
        return std::find(noDynamics_.cbegin(), noDynamics_.cend(), type) == noDynamics_.cend();
    }

private:
    static const std::vector<const char*> imageTypes_;
    static const std::vector<const char*> noDynamics_;
};

const std::vector<const char*> AippImageTypes::imageTypes_ = {
    hiai::op::ImageCropV2::TYPE,
    hiai::op::ImageChannelSwapV2::TYPE,
    hiai::op::ImageColorSpaceConvertionV2::TYPE,
    hiai::op::ImageResizeV2::TYPE,
    hiai::op::ImageDataTypeConvertionV2::TYPE,
    hiai::op::ImageRotateV2::TYPE,
    hiai::op::ImagePadV2::TYPE,
};

const std::vector<const char*> AippImageTypes::noDynamics_ = {
    hiai::op::ImageRotateV2::TYPE // linked with ConfigData is dynamic aipp, which not support ImageRotateV2.
};

class CastTAfterAippOptimizer {
public:
    static hiai::Status Optimize(ge::ComputeGraph& graph)
    {
        std::vector<ge::Node*> aippNodes;
        (void)graph.ROLE(GraphListWalker)
            .WalkAllNodes(ge::NodeFunctor::Typed({hiai::op::Aipp::TYPE}, [&aippNodes](ge::Node& node) {
                aippNodes.push_back(&node);
                return hiai::SUCCESS;
            }));

        auto bypassCastT = ge::NodeFunctor::Typed(
            {
                hiai::op::CastT::TYPE,
            },
            [&graph](ge::Node& castT) { return graph.ROLE(GraphBypasser).ByPassNode(castT); });

        for (const auto& node : aippNodes) {
            HIAI_EXPECT_EXEC(node->ROLE(NodeWalker).ListOutDataNodes(bypassCastT));
        }

        return hiai::SUCCESS;
    }
};

class AippNodeInserter {
public:
    static ge::Node* InsertAfter(ge::ComputeGraph& graph, ge::Node& dataNode, ge::Node& preNode)
    {
        ge::Node* aippNode = MakeAippNodeByPreNode(graph, preNode.ROLE(NodeSpec).OpDesc());
        HIAI_EXPECT_NOT_NULL_R(aippNode, nullptr);

        HIAI_EXPECT_EXEC_R(LinkAippInAndOutEdges(graph.ROLE(GraphModifier), dataNode, preNode, *aippNode), nullptr);

        HIAI_EXPECT_EXEC_R(UpdateGraphOutputNode(graph, preNode, *aippNode), nullptr);

        return aippNode;
    }

private:
    static hiai::Status AddAippDescInTensor(ge::OpDesc& desc)
    {
        TensorDesc tensor;
        HIAI_EXPECT_EXEC(desc.AddInputDesc(tensor));
        HIAI_EXPECT_EXEC(desc.AddInputDesc(tensor));

        return hiai::SUCCESS;
    }

    static bool IsOpDescHasDimAttrs(ge::OpDesc& desc)
    {
        bool hasAttrDims = true;
        for (const auto& attr : aippDimAttrs_) {
            hasAttrDims = hasAttrDims && desc.HasAttr(attr);
        }

        return hasAttrDims;
    }

    static hiai::Status GetPreNodeDimsByAttrs(ge::OpDesc& preDesc, ge::OpDesc& aippDesc, std::vector<int64_t>& dims)
    {
        for (const auto& attr : aippDimAttrs_) {
            int dim = 0;
            HIAI_EXPECT_TRUE(AttrUtils::GetInt(preDesc, attr, dim));
            HIAI_EXPECT_TRUE(AttrUtils::SetInt(aippDesc, attr, dim));
            dims.push_back(dim);
        }

        return hiai::SUCCESS;
    }

    static hiai::Status GetPreNodeDimsByShape(ge::OpDesc& preDesc, ge::OpDesc& aippDesc, std::vector<int64_t>& dims)
    {
        const auto& preAippShape = preDesc.MutableOutputDesc(0)->GetShape();
        HIAI_EXPECT_TRUE(preAippShape.GetDimNum() == aippDimAttrs_.size());

        for (size_t i = 0; i < aippDimAttrs_.size(); i++) {
            int64_t dim = preAippShape.GetDim(i);
            HIAI_EXPECT_TRUE(AttrUtils::SetInt(preDesc, aippDimAttrs_[i], dim));
            HIAI_EXPECT_TRUE(AttrUtils::SetInt(aippDesc, aippDimAttrs_[i], dim));

            dims.push_back(dim);
        }

        return hiai::SUCCESS;
    }

    static hiai::Status AddAippDescOutTensor(ge::OpDesc& preDesc, ge::OpDesc& aippDesc)
    {
        std::vector<int64_t> dims;
        if (IsOpDescHasDimAttrs(preDesc)) {
            HIAI_EXPECT_EXEC(GetPreNodeDimsByAttrs(preDesc, aippDesc, dims));
        } else {
            HIAI_EXPECT_EXEC(GetPreNodeDimsByShape(preDesc, aippDesc, dims));
        }

        aippDesc.AddOutputDesc(TensorDesc(ge::Shape(std::move(dims))));

        return hiai::SUCCESS;
    }

    static hiai::Status AddAippDescUglyAttrs(ge::OpDesc& desc)
    {
        HIAI_EXPECT_TRUE(AttrUtils::SetBool(desc, hiai::ATTR_AIPP_COMPATIBLE, false));

        TensorDescPtr outTensor = desc.MutableOutputDesc(0);
        HIAI_EXPECT_NOT_NULL(outTensor);
        HIAI_EXPECT_TRUE(AttrUtils::SetInt(desc, hiai::AIPP_MODEL_DATA_TYPE, outTensor->GetDataType()));

        return hiai::SUCCESS;
    }

    static std::int32_t CountTotalAippNum(ge::ComputeGraph& graph)
    {
        int32_t aippNodeCnt = 0;
        (void)graph.ROLE(GraphListWalker)
            .WalkAllNodes(ge::NodeFunctor::Typed({hiai::op::Aipp::TYPE}, [&aippNodeCnt](ge::Node&) {
                aippNodeCnt++;
                return hiai::SUCCESS;
            }));

        return aippNodeCnt;
    }

    static ge::Node* MakeAippNodeByPreNode(ge::ComputeGraph& graph, ge::OpDesc& preDesc)
    {
        std::string aippNodeName(hiai::op::Aipp::TYPE + std::to_string(CountTotalAippNum(graph)));
        auto aippDesc = hiai::make_shared_nothrow<OpDesc>(aippNodeName, std::string(hiai::op::Aipp::TYPE));
        HIAI_EXPECT_NOT_NULL_R(aippDesc, nullptr);

        HIAI_EXPECT_EXEC_R(AddAippDescInTensor(*aippDesc), nullptr);
        HIAI_EXPECT_EXEC_R(AddAippDescOutTensor(preDesc, *aippDesc), nullptr);
        HIAI_EXPECT_EXEC_R(AddAippDescUglyAttrs(*aippDesc), nullptr);

        return graph.ROLE(GraphModifier).AddNode(aippDesc);
    }

    static hiai::Status LinkAippInAndOutEdges(
        ge::GraphModifier& modifier, ge::Node& dataNode, ge::Node& preNode, ge::Node& aippNode)
    {
        // link in edge
        HIAI_EXPECT_EXEC(modifier.AddEdge({dataNode, 0}, {aippNode, 0}));

        // link out edge
        auto linkOutEdge = [&modifier, &aippNode](ge::Edge& edge) {
            HIAI_EXPECT_EXEC(modifier.RemoveEdge(edge));
            HIAI_EXPECT_EXEC(modifier.AddEdge({aippNode, 0}, edge.Dst()));
            return hiai::SUCCESS;
        };
        HIAI_EXPECT_EXEC(preNode.ROLE(NodeWalker).ListOutDataEdges(std::move(linkOutEdge)));

        return hiai::SUCCESS;
    }

    static hiai::Status UpdateGraphOutputNode(ge::ComputeGraph& graph, ge::Node& preNode, ge::Node& aippNode)
    {
        std::vector<ge::Node*> graphOutputNodes;
        (void)graph.ROLE(GraphListWalker).WalkOutNodes([&graphOutputNodes](ge::Node& node) {
            graphOutputNodes.push_back(&node);
            return hiai::SUCCESS;
        });

        for (size_t i = 0; i < graphOutputNodes.size(); ++i) {
            if (graphOutputNodes[i] == &preNode) {
                graphOutputNodes[i] = &aippNode;
                return graph.ROLE(GraphModifier).SetOutputs(graphOutputNodes);
            }
        }

        return hiai::SUCCESS;
    }

private:
    static const std::vector<const char*> aippDimAttrs_;
};

const std::vector<const char*> AippNodeInserter::aippDimAttrs_ = {
    hiai::AIPP_MODEL_DATA_DIM_N, hiai::AIPP_MODEL_DATA_DIM_C, hiai::AIPP_MODEL_DATA_DIM_H, hiai::AIPP_MODEL_DATA_DIM_W};

class AippImagesChain {
public:
    explicit AippImagesChain(ge::ComputeGraph& graph, ge::Node& node) : graph_(graph), aippImages_ {&node}
    {
    }
    ~AippImagesChain() = default;

    void AddAippImageNode(ge::Node& aippImageNode)
    {
        aippImages_.push_back(&aippImageNode);
    }

    std::int32_t Size() const
    {
        return aippImages_.size();
    }

    ge::Node* operator[](std::int32_t idx)
    {
        return aippImages_[idx];
    }

    ge::Node* AippNode() const
    {
        return aipp_;
    }

    ge::Node* DataNode() const
    {
        return aippImages_[0];
    }

    hiai::Status AddAippNode()
    {
        HIAI_EXPECT_EXEC(AippImagesInferShaper::InferShape(aippImages_));

        aipp_ = AippNodeInserter::InsertAfter(graph_, *DataNode(), *aippImages_.back());
        HIAI_EXPECT_NOT_NULL(aipp_);

        return hiai::SUCCESS;
    }

    void ReverseAippImageNodes()
    {
        std::reverse(aippImages_.begin(), aippImages_.end());
    }

    hiai::Status RemoveChainNodes()
    {
        // first is Data, not remove.
        for (std::uint32_t i = 1; i < aippImages_.size(); i++) {
            ge::Node& imageNode = *aippImages_[i];
            ge::Node* paramNode = imageNode.ROLE(NodeWalker).InDataNode(1);
            if (paramNode != nullptr) {
                HIAI_EXPECT_EXEC(graph_.ROLE(GraphModifier).RemoveNode(*paramNode));
            }
            HIAI_EXPECT_EXEC(graph_.ROLE(GraphModifier).RemoveNode(imageNode));
        }

        return hiai::SUCCESS;
    }

private:
    ge::ComputeGraph& graph_;
    ge::Node* aipp_ {nullptr};
    std::vector<ge::Node*> aippImages_ {};
};

class AippImagesCfgPackager {
public:
    static hiai::Status Pack(ge::ComputeGraph& graph, AippImagesChain& chain, hiai::AippPreprocessConfig& config)
    {
        HIAI_EXPECT_EXEC(PackDataNodeInfo(graph, chain, config));
        HIAI_EXPECT_EXEC(PackAippImagesInfo(graph, chain, config));

        return hiai::SUCCESS;
    }

private:
    static hiai::Status GetIdxOfGraphInput(const ge::ComputeGraph& graph, ge::NodeVisitor v)
    {
        hiai::Status ret = graph.ROLE(GraphListWalker).WalkInNodes(std::move(v));
        return ret == hiai::COMM_EXCEPTION ? hiai::SUCCESS : hiai::FAILURE;
    }

    static ge::NodeVisitor CreateGetIdxVisitor(const ge::Node& node, std::int32_t& idx)
    {
        return [&node, &idx](ge::Node& inNode) {
            if (&inNode == &node) {
                return hiai::COMM_EXCEPTION;
            }
            idx++;
            return hiai::SUCCESS;
        };
    }

    static hiai::Status GetIdxOfInputNode(const ge::ComputeGraph& graph, ge::Node& node, std::int32_t& idx)
    {
        idx = 0;
        return GetIdxOfGraphInput(graph, CreateGetIdxVisitor(node, idx));
    }

    static hiai::Status GetIdxOfInputData(const ge::ComputeGraph& graph, ge::Node& node, std::int32_t& idx)
    {
        idx = 0;
        return GetIdxOfGraphInput(
            graph, ge::NodeFunctor::Typed({hiai::op::Data::TYPE}, CreateGetIdxVisitor(node, idx)));
    }

    static hiai::Status ConvertAippInfoToAttrs(
        const ge::Node& data, const hiai::AippParamInfo& para, bool dynamic, ge::Node& aipp)
    {
        AttrValue::NamedAttrs attrs;
        HIAI_EXPECT_EXEC(AippParamInfoConverter::ConvertAippParamInfo2Attr(attrs, para));

        HIAI_EXPECT_EXEC(AippParamInfoConverter::ConvertAippInputInfo2Attr(attrs, data, dynamic));

        ge::OpDesc& opDesc = aipp.ROLE(NodeSpec).OpDesc();
        HIAI_EXPECT_TRUE(AttrUtils::SetNamedAttrs(opDesc, hiai::ATTR_NAME_AIPP, attrs));

        return hiai::SUCCESS;
    }

    static hiai::Status PackDataNodeInfo(
        ge::ComputeGraph& graph, AippImagesChain& chain, hiai::AippPreprocessConfig& aippConfig)
    {
        ge::Node& dataNode = *chain.DataNode();

        HIAI_EXPECT_EXEC(GetIdxOfInputNode(graph, dataNode, aippConfig.graphDataIdx));
        HIAI_EXPECT_EXEC(GetIdxOfInputData(graph, dataNode, aippConfig.tensorDataIdx));

        return hiai::SUCCESS;
    }

    static hiai::Status PackDynamicAippImageInfo(
        ge::ComputeGraph& graph, ge::Node& aippNode, ge::Node& paramNode, hiai::AippPreprocessConfig& aippConfig)
    {
        std::string paramType = AippImageTypes::GetDynamicParamType(paramNode.ROLE(NodeSpec).OpDesc());
        HIAI_EXPECT_TRUE(!paramType.empty());

        HIAI_EXPECT_TRUE(AttrUtils::SetBool(aippNode.ROLE(NodeSpec).OpDesc(), paramType, true));

        int dynamicTypeIdx = AippImageTypes::GetAippImageTypesIdx(paramType);
        HIAI_EXPECT_TRUE(dynamicTypeIdx != AippImageTypes::INVALID_IDX);
        aippConfig.configDataInfo[aippConfig.configDataCnt].type = dynamicTypeIdx;

        std::int32_t graphInputIdxOfParamNode = 0;
        HIAI_EXPECT_EXEC(GetIdxOfInputNode(graph, paramNode, graphInputIdxOfParamNode));
        aippConfig.configDataInfo[aippConfig.configDataCnt].idx = graphInputIdxOfParamNode;

        aippConfig.configDataCnt++;

        return hiai::SUCCESS;
    }

    static hiai::Status PackAippImagesInfo(
        ge::ComputeGraph& graph, AippImagesChain& chain, hiai::AippPreprocessConfig& config)
    {
        config.configDataCnt = 0;

        ge::Node& aippNode = *chain.AippNode();

        for (std::int32_t i = 1; i < chain.Size(); i++) {
            ge::Node& imageNode = *chain[i];
            ge::Node* paramNode = imageNode.ROLE(NodeWalker).InDataNode(1);
            HIAI_EXPECT_NOT_NULL(paramNode);

            if (paramNode->ROLE(NodeSpec).Type() == hiai::op::ConfigData::TYPE) { // dyncmic aipp
                HIAI_EXPECT_EXEC(PackDynamicAippImageInfo(graph, aippNode, *paramNode, config));
            }

            HIAI_EXPECT_EXEC(AippParamInfoConverter::ConvertConst2AippParamInfo(
                *paramNode, imageNode.ROLE(NodeSpec).Type(), config.aippParamInfo));
        }

        ge::Node& dataNode = *chain.DataNode();
        HIAI_EXPECT_EXEC(ConvertAippInfoToAttrs(dataNode, config.aippParamInfo, config.configDataCnt > 0, aippNode));

        return hiai::SUCCESS;
    }
};

class GraphAippImagesChains {
public:
    GraphAippImagesChains(ge::ComputeGraph& graph) : graph_(graph)
    {
    }
    ~GraphAippImagesChains() = default;

    void CreateNewChain(ge::Node& node)
    {
        chains_.push_back(AippImagesChain(graph_, node));
    }

    void AddCurrChainNode(ge::Node& node)
    {
        chains_.back().AddAippImageNode(node);
    }

    void ReverseChainsNode()
    {
        for (auto& chain : chains_) {
            chain.ReverseAippImageNodes();
        }
    }

    bool HasAippImageChain() const
    {
        return !chains_.empty();
    }

    hiai::Status FusionAippImages(std::string& customData)
    {
        std::vector<hiai::AippPreprocessConfig> configs;

        for (auto& chain : chains_) {
            HIAI_EXPECT_EXEC(chain.AddAippNode());

            hiai::AippPreprocessConfig config;
            AippImagesCfgPackager::Pack(graph_, chain, config);

            if (config.configDataCnt > 0) {
                configs.push_back(config);
            }
        }
        for (auto& chain : chains_) {
            HIAI_EXPECT_EXEC(chain.RemoveChainNodes());
        }

        if (configs.size() > 0) {
            customData = {reinterpret_cast<char*>(configs.data()), configs.size() * sizeof(hiai::AippPreprocessConfig)};
        }
        return hiai::SUCCESS;
    }

private:
    ge::ComputeGraph& graph_;
    std::vector<AippImagesChain> chains_ {};
};

class AippImagesChainSearcher {
public:
    static GraphAippImagesChains Search(ge::ComputeGraph& graph)
    {
        GraphAippImagesChains chains(graph);

        auto search = ge::NodeFunctor::Typed({hiai::op::Data::TYPE}, [&chains](ge::Node& data) {
            data.ROLE(NodeWalker).ListOutDataNodes([&chains, &data](ge::Node& node) {
                if (AippImageTypes::IsAippImageType(node.ROLE(NodeSpec).Type())) {
                    if (SaveImageChainNode(node, chains)) {
                        chains.AddCurrChainNode(data);
                    }
                }
                return hiai::SUCCESS;
            });
            return hiai::SUCCESS;
        });
        (void)graph.ROLE(GraphListWalker).WalkAllNodes(std::move(search));

        // depth first serach, need reverse.
        chains.ReverseChainsNode();

        return chains;
    }

private:
    static bool IsNextNodeMatch(ge::Node& curr, ge::Node& next, GraphAippImagesChains& chains)
    {
        const std::string& nextType = next.ROLE(NodeSpec).Type();
        if (AippImageTypes::IsAippImageType(nextType)) {
            HIAI_EXPECT_TRUE_R(AippImageTypes::IsLessSequence(curr.ROLE(NodeSpec).Type(), nextType), false);
            bool ret = SaveImageChainNode(next, chains);
            if (ret) {
                chains.AddCurrChainNode(curr);
            }
            return ret;
        } else if (nextType == hiai::op::Aipp::TYPE) { // last node can not aipp.
            return false;
        } else { // next node not aipp image, chain serach finish.
            chains.CreateNewChain(curr);
            return true;
        }
    }

    static bool SaveImageChainNode(ge::Node& curr, GraphAippImagesChains& chains)
    {
        // images input0 must Data/image, input1 must Const/ConfigData.
        HIAI_EXPECT_TRUE_R(curr.ROLE(NodeSpec).InDataEdgeSize() == 2, false);

        auto outEdgeSize = curr.ROLE(NodeSpec).OutDataEdgeSize();
        if (outEdgeSize == 0) { // aipp image is last node, chain serach finish.
            chains.CreateNewChain(curr);
            return true;
        }
        return curr.ROLE(NodeWalker).ListOutDataNodes([&curr, &chains](ge::Node& next) {
            return IsNextNodeMatch(curr, next, chains);
        });
    }
};
} // namespace

namespace hiai {
Status HiAIIRAippCompatible::GenerateAippCompatibleInfo(ge::ComputeGraph& graph, std::string& customData)
{
    GraphAippImagesChains chains = AippImagesChainSearcher::Search(graph);
    if (chains.HasAippImageChain()) {
        HIAI_EXPECT_EXEC(chains.FusionAippImages(customData));
    }

    // This optimizer not only optimize aipps created above, but also aipps already exist.
    HIAI_EXPECT_EXEC(CastTAfterAippOptimizer::Optimize(graph));

    return hiai::SUCCESS;
}

GRAPH_API_EXPORT Status GenerateAippCompatibleInfoAdapter(ge::ComputeGraph& graph, std::string& customData)
{
    return HiAIIRAippCompatible::GenerateAippCompatibleInfo(graph, customData);
}
} // namespace hiai
