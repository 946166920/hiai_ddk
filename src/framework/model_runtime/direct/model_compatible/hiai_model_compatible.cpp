/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: hiai model compatible
 */
#include "hiai_model_compatible.h"
#include "securec.h"

// inc/infra
#include "infra/base/securestl.h"

// api/framework
#include "graph/op/nn_defs.h"
#include "graph/op/math_defs.h"
#include "graph/model.h"
#include "graph/op/const_defs.h"

// inc/framework
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/cgraph/graph_topo_walker.h"
#include "framework/graph/core/cgraph/graph_bypasser.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_walker.h"
#include "framework/graph/core/edge/edge.h"
#include "framework/graph/utils/attr_utils.h"
#include "framework/graph/utils/graph_utils.h"
#include "framework/graph/utils/op_desc_utils.h"
#include "framework/graph/utils/tensor_utils.h"
#include "framework/infra/log/log.h"
#include "framework/c/hiai_version.h"
#include "framework/c/hiai_error_types.h"

// src/framework
#include "omg/quantize_optimizer/quantize_util.h"
#include "framework/compatible/ir_transformer.h"
#include "model_runtime/direct/model_compatible/ir_build_transformer.h"
#include "common/helper/om_file_helper.h"
#include "common/helper/model_serialize_wrapper.h"
#include "util/hiai_foundation_dl_helper.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace {
const int32_t ROM_VERSION_BASE_LEN = 15; // Rom版本号固定长度
constexpr int32_t ROM_VERSION_PRODUCT_LEN = 11;
constexpr int32_t ROM_VERSION_CHIP_LEN = 7;
constexpr uint32_t FILE_HEAD_LENGTH = 256;
const char* const WEIGHT_MERGED = "weight_merged";
const char* const ATTR_NAME_WEIGHTS = "value";
const char* const MERGED_OFFSET = "merged_offset";
bool IS_MERGED = false;
static std::map<std::string, std::string> notSupportedDeconvVersion = {
    {"100.320.010", "100.320.010.021"},
    {"100.320.011", "100.320.011.017"},
};
const std::string baltimoreC10Version = "100.500";
const std::string supportExtremeVersion = "100.333";
const std::string buildModelBaseVersion = "100.320";
} // namespace

using namespace hiai;

namespace {
bool IsNeedBuildUnmergedIrModel(const std::string& version)
{
    return version.compare(0, ROM_VERSION_CHIP_LEN, buildModelBaseVersion, 0, ROM_VERSION_CHIP_LEN) == 0;
}

ge::ComputeGraphPtr GetComputeGraph(OmFileLoadHelper& omFileHelper)
{
    ModelPartition partitionModelDef;
    HIAI_EXPECT_EXEC_R(omFileHelper.GetModelPartition(ModelPartitionType::MODEL_DEF, partitionModelDef), nullptr);

    ge::Model model;
    HIAI_EXPECT_EXEC_R(model.Load(partitionModelDef.data, partitionModelDef.size), nullptr);

    return ge::GraphUtils::GetComputeGraph(model.GetGraph());
}

bool IsNotSupportSpecialDeconvRom(std::string& version)
{
    std::string baseVersion = version.substr(0, ROM_VERSION_PRODUCT_LEN);
    auto it = notSupportedDeconvVersion.find(baseVersion);
    if (it != notSupportedDeconvVersion.end()) {
        return strncmp(version.c_str(), it->second.c_str(), it->second.size()) <= 0;
    }
    return false;
}

bool IsSpecialDeconvModel(ge::ComputeGraphPtr graph)
{
    bool ret = false;
    auto visitor = [&ret](ge::Node& node) {
        if (node.ROLE(NodeSpec).Type() == hiai::op::ConvTranspose::TYPE) {
            vector<ge::TensorPtr> weights = ge::OpDescUtils::MutableWeights(node);
            if (weights.size() == 1) {
                ret = true;
            }
        }
        return hiai::SUCCESS;
    };
    (void)graph->ROLE(GraphListWalker).WalkAllNodes(visitor);
    return ret;
}

bool IsSupportedOriginFormatModel(std::string& version, ge::ComputeGraphPtr graph)
{
    bool useOriginFormat = false;
    (void)ge::AttrUtils::GetBool(graph, ge::GRAPH_ATTR_USE_ORIGIN_FORMAT, useOriginFormat);
    if (useOriginFormat) {
        int versionBranch = std::stoi(version.substr(4, 3));
        int versionPlatform = std::stoi(version.substr(9, 3));
        switch (versionBranch) {
            case 320: {
                FMK_LOGE("this version doesn't support origin format model.");
                return false;
            }
            case 330: {
                if (versionPlatform == 10 && version < "100.330.010.038") {
                    FMK_LOGE("this version doesn't support origin format model.");
                    return false;
                } else if (versionPlatform == 11 && version < "100.330.011.038") {
                    FMK_LOGE("this version doesn't support origin format model.");
                    return false;
                }
                break;
            }
            case 500: {
                if (version < "100.500.010.038") {
                    FMK_LOGE("this version doesn't support origin format model.");
                    return false;
                }
                break;
            }
            default:
                FMK_LOGI("this version supports origin format model.");
                break;
        }
    }
    return true;
}

bool IsNeedChangeModelType(const std::string& version)
{
    return version.compare(0, ROM_VERSION_CHIP_LEN, supportExtremeVersion, 0, ROM_VERSION_CHIP_LEN) == 0;
}

static bool IsNeedUnmergedWeight(const OmFileLoadHelper& omFileHelper, std::string& version, ge::ComputeGraphPtr graph)
{
    bool hasWeightPartition = false;
    for (const ModelPartition& part : omFileHelper.context_.partitionDatas) {
        if (part.type == hiai::ModelPartitionType::WEIGHTS_DATA) {
            hasWeightPartition = true;
            break;
        }
    }

    if (!hasWeightPartition) {
        return false;
    }

    IS_MERGED = true;
    if (IsNeedBuildUnmergedIrModel(version)) {
        return true;
    }
    if (hiai::IRTransformer::IsCompatible(graph, version)) {
        return true;
    }
    bool ret = false;
    if (IsNeedChangeModelType(version)) {
        vector<std::string> needConvert2FP32 {"GemmD", "MatMul"};
        auto visitor = [&ret, &needConvert2FP32](ge::Node& node) {
            if (find(needConvert2FP32.begin(), needConvert2FP32.end(), node.ROLE(NodeSpec).Type()) !=
                needConvert2FP32.end()) {
                ret = true;
                return hiai::SUCCESS;
            }
            return hiai::SUCCESS;
        };
        (void)GraphUtils::WalkAllSubGraphNodes(*graph, visitor);
    }
    return ret;
}

HIAI_Status RemakeUnmergedIrGraph(ge::ComputeGraphPtr graph, hiai::ModelPartition& weightData)
{
    auto visitor = [&weightData](ge::Node& node) {
        auto& temp = node.ROLE(NodeSpec).OpDesc();
        if (temp.GetType() == hiai::op::Const::TYPE) {
            ge::TensorPtr weight = nullptr;
            (void)ge::AttrUtils::MutableTensor(temp, ATTR_NAME_WEIGHTS, weight);

            if (weight == nullptr) {
                FMK_LOGE("can't get const op weight, name: %s", temp.GetName().c_str());
                return HIAI_FAILURE;
            }

            int64_t offset = 0;
            if (!ge::AttrUtils::GetInt(weight->GetTensorDesc(), MERGED_OFFSET, offset)) {
                FMK_LOGE("can't get const op offset, name: %s", temp.GetName().c_str());
                return HIAI_FAILURE;
            }

            uint32_t weightSize = ge::TensorUtils::GetWeightSize(weight->GetTensorDesc());
            if (offset < 0 || offset > UINT32_MAX || (weightSize > weightData.size) ||
                (weightData.size - weightSize < (uint32_t)offset)) {
                FMK_LOGE(
                    "RemakeUmergedIrModel: offset is invalid or offset[%jd] + weightSize[%u] > weightData.size[%u].",
                    offset, weightSize, weightData.size);
                return HIAI_FAILURE;
            }

            // copy weight
            if (weight->SetData(weightData.data + offset, weightSize) != hiai::SUCCESS) {
                FMK_LOGE("RemakeUmergedIrModel: set weight data failed.");
                return HIAI_FAILURE;
            }

            ge::TensorUtils::DeleteAttr(weight->MutableTensorDesc(), MERGED_OFFSET);
            ge::TensorUtils::SetWeightSize(weight->MutableTensorDesc(), 0);
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(GraphUtils::WalkAllSubGraphNodes(*graph, visitor));
    (void)graph->DelAttr(WEIGHT_MERGED);
    IS_MERGED = false;
    return HIAI_SUCCESS;
}

static HIAI_Status UnmergedWeightGraph(const OmFileLoadHelper& omFileHelper, ge::ComputeGraphPtr graph, bool& isChanged)
{
    hiai::ModelPartition partitionWeightData;
    HIAI_EXPECT_EXEC(omFileHelper.GetModelPartition(hiai::ModelPartitionType::WEIGHTS_DATA, partitionWeightData));
    HIAI_EXPECT_EXEC(RemakeUnmergedIrGraph(graph, partitionWeightData));

    isChanged = true;
    return HIAI_SUCCESS;
}

static bool IsNeedForwardCompatible(const std::string& version)
{
    if (version.compare(0, ROM_VERSION_CHIP_LEN, supportExtremeVersion, 0, ROM_VERSION_CHIP_LEN) <= 0 ||
        version.compare(0, ROM_VERSION_CHIP_LEN, baltimoreC10Version, 0, ROM_VERSION_CHIP_LEN) == 0) {
        return true;
    }
    return false;
}

HIAI_Status SetGraphFlag(ge::ComputeGraphPtr graph, bool& isChanged)
{
    bool graphInferedFlag = false;
    if (ge::AttrUtils::GetBool(graph, "graph_infershaped_flag", graphInferedFlag) != true) {
        // in old ROM set graph_infershaped_flag = false, Do Npu Infershape.
        if (ge::AttrUtils::SetBool(graph, "graph_infershaped_flag", false) != true) {
            FMK_LOGE("Set graph infershaped flag false failed.");
            return HIAI_FAILURE;
        }
        isChanged = true;
    }
    return HIAI_SUCCESS;
}

// Compatibel process for CompatibleStandardGraph function begin
ge::OpDescPtr CreateCastOp(ge::DataType srcType, ge::DataType dstType, int64_t inferFormat)
{
    static uint32_t castCount = 0;
    string sstmp = string("data_cast_") + std::to_string(castCount++);
    ge::OpDescPtr opDesc = hiai::make_shared_nothrow<ge::OpDesc>(sstmp, "CastT");
    HIAI_EXPECT_NOT_NULL_R(opDesc, nullptr);
    (void)ge::AttrUtils::SetInt(opDesc, hiai::op::CastT::dst_dtype, dstType);
    (void)ge::AttrUtils::SetInt(opDesc, hiai::op::CastT::src_dtype, srcType);
    (void)ge::AttrUtils::SetInt(opDesc, "version", 5);
    (void)ge::AttrUtils::SetInt(opDesc, "inferred_format", inferFormat);
    FMK_LOGI("create cast op:%s, src: %d, dst: %d", opDesc->GetName().c_str(), srcType, dstType);
    return opDesc;
}

bool IsFloat162Fp32CastOp(ge::Node& node)
{
    auto& nodeSpec = node.ROLE(NodeSpec);
    if (nodeSpec.Type() != "CastT") {
        return false;
    }

    int64_t srcType = 0;
    (void)ge::AttrUtils::GetInt(nodeSpec.OpDesc(), hiai::op::CastT::src_dtype, srcType);
    int64_t dstType = 0;
    (void)ge::AttrUtils::GetInt(nodeSpec.OpDesc(), hiai::op::CastT::dst_dtype, dstType);
    if (srcType == ge::DT_FLOAT16 && dstType == ge::DT_FLOAT) {
        return true;
    }
    return false;
}

bool IsFloat2Fp16CastOp(ge::Node& node)
{
    auto& nodeSpec = node.ROLE(NodeSpec);
    if (nodeSpec.Type() != "CastT") {
        return false;
    }
    int64_t srcType = 0;
    (void)ge::AttrUtils::GetInt(nodeSpec.OpDesc(), hiai::op::CastT::src_dtype, srcType);
    int64_t dstType = 0;
    (void)ge::AttrUtils::GetInt(nodeSpec.OpDesc(), hiai::op::CastT::dst_dtype, dstType);
    if (srcType == ge::DT_FLOAT && dstType == ge::DT_FLOAT16) {
        return true;
    }
    return false;
}

bool IsNotNetOuput(ge::Node& node)
{
    if (node.ROLE(NodeSpec).Type() != "CastT") {
        return false;
    }

    ge::Node* outNode = node.ROLE(NodeWalker).OutDataNode(0, 0);
    if (outNode == nullptr) {
        return false;
    }

    return outNode->ROLE(NodeSpec).Type() != "NetOutput";
}

Status AnalyzeDataNode(
    ge::Node& node, std::vector<pair<ge::Endpoint, ge::OpDescPtr>>& converterOps, std::vector<ge::Node*>& needRemove)
{
    auto& opDesc = node.ROLE(NodeSpec).OpDesc();
    if (opDesc.GetType() != "Data") {
        return hiai::SUCCESS;
    }

    if (node.ROLE(NodeSpec).OutDataEdgeSize() == 0) {
        // 该DATA节点后续无其他输出节点，跳过不处理
        return hiai::SUCCESS;
    }
    vector<pair<Node*, int>> outEndpoint;
    auto visitor = [&outEndpoint, &converterOps, &opDesc, &needRemove](ge::Edge& edge) {
        if (find(outEndpoint.begin(), outEndpoint.end(), std::make_pair(&edge.SrcNode(), edge.SrcIdx())) !=
            outEndpoint.end()) {
            return hiai::SUCCESS;
        }
        outEndpoint.push_back(std::make_pair(&edge.SrcNode(), edge.SrcIdx()));
        auto& outNode = edge.DstNode();

        if (IsFloat162Fp32CastOp(outNode) && IsNotNetOuput(outNode)) {
            needRemove.push_back(&outNode);
            return hiai::SUCCESS;
        }
        if (IsFloat2Fp16CastOp(outNode)) {
            return hiai::SUCCESS;
        }
        if (outNode.ROLE(NodeSpec).Type() == "LshProject") {
            return hiai::SUCCESS;
        }
        auto inputTensor = opDesc.GetInputDesc(edge.SrcIdx());
        auto outputTensor = opDesc.GetOutputDesc(edge.SrcIdx());
        int64_t inferFormat = 0;
        if (!ge::AttrUtils::GetInt(opDesc, "inferred_format", inferFormat)) {
            inferFormat = 0;
        }
        // 插入cast op
        // data input(fp32) output(fp32)
        if (inputTensor.GetDataType() == ge::DT_FLOAT && outputTensor.GetDataType() == ge::DT_FLOAT) {
            ge::OpDescPtr castOp = CreateCastOp(inputTensor.GetDataType(), ge::DT_FLOAT16, inferFormat);
            if (castOp) {
                converterOps.push_back(std::make_pair(edge.Src(), castOp));
            }
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(node.ROLE(NodeWalker).ListOutDataEdges(visitor));
    return hiai::SUCCESS;
}

Status InsertInputConverterOP(ge::ComputeGraphPtr& graph)
{
    std::vector<pair<ge::Endpoint, ge::OpDescPtr>> converterOps;
    std::vector<ge::Node*> needRemove;
    auto visitor = [&needRemove, &converterOps](ge::Node& n) {
        HIAI_EXPECT_EXEC(AnalyzeDataNode(n, converterOps, needRemove));
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(graph->ROLE(GraphListWalker).WalkAllNodes(visitor));

    auto& graphModifier = graph->ROLE(GraphModifier);
    for (auto& node : needRemove) {
        if (graph->ROLE(GraphBypasser).ByPassNode(*node) != ge::GRAPH_SUCCESS) {
            FMK_LOGE("Remove node failed:%s", node->ROLE(NodeSpec).Name().c_str());
            return FAILURE;
        }
    }
    for (auto& pair : converterOps) {
        FMK_LOGI("begin insert node: %s", pair.second->GetName().c_str());
        if (pair.second->GetInputsDescSize() == 0 && pair.second->GetOutputsSize() == 0) {
            pair.second->AddInputDesc(TensorDesc());
            pair.second->AddOutputDesc(TensorDesc());
        }
        auto insertNode = graphModifier.AddNode(pair.second);
        HIAI_EXPECT_EXEC(graphModifier.InsertAfter(pair.first.Node(), pair.first.Idx(), *insertNode));
        FMK_LOGI("end insert node");
    }
    HIAI_EXPECT_EXEC(graph->ROLE(GraphTopoWalker).DFSWalk([](Node&) { return hiai::SUCCESS; }));

    return hiai::SUCCESS;
}

Status ParserCastNode(ge::ComputeGraphPtr& graph)
{
    auto visitor = [](ge::Node& node) {
        auto& nodeSpec = node.ROLE(NodeSpec);
        if (nodeSpec.Name().find("data_cast_") != 0 && nodeSpec.Name().find("netoutput_cast_") != 0) {
            auto& opdesc = nodeSpec.OpDesc();
            int64_t type = -1;
            (void)ge::AttrUtils::GetInt(opdesc, hiai::op::CastT::dst_dtype, type);
            if (type == ge::DT_FLOAT) {
                (void)ge::AttrUtils::SetInt(opdesc, hiai::op::CastT::dst_dtype, ge::DT_FLOAT16);
            }
        }
        return hiai::SUCCESS;
    };
    HIAI_EXPECT_EXEC(graph->ROLE(GraphListWalker).WalkAllNodes(visitor));
    return hiai::SUCCESS;
}

void CastOMConverter(const std::string& version, ge::ComputeGraphPtr& graph, bool& isChange)
{
    if (!IsNeedChangeModelType(version)) {
        return;
    }
    auto visitor = [&isChange](ge::Node& node) {
        if (node.ROLE(NodeSpec).Type() != "CastT") {
            return hiai::SUCCESS;
        }
        if (node.ROLE(NodeSpec).Name().find("data_cast_") != string::npos) {
            auto& opdesc = node.ROLE(NodeSpec).OpDesc();
            int64_t srcType = -1;
            int64_t dstType = -1;
            (void)ge::AttrUtils::GetInt(opdesc, hiai::op::CastT::src_dtype, srcType);
            (void)ge::AttrUtils::GetInt(opdesc, hiai::op::CastT::dst_dtype, dstType);
            if (srcType == ge::DT_FLOAT16 && dstType == ge::DT_FLOAT) {
                (void)ge::AttrUtils::SetInt(opdesc, hiai::op::CastT::dst_dtype, ge::DT_FLOAT16);
                isChange = true;
            }
        }
        return hiai::SUCCESS;
    };
    (void)graph->ROLE(GraphListWalker).WalkAllNodes(visitor);
    return;
}

HIAI_Status ForwardCompatible(std::string& version, ge::ComputeGraphPtr graph, bool& isChanged)
{
    if (IsNeedForwardCompatible(version)) {
        HIAI_EXPECT_EXEC(SetGraphFlag(graph, isChanged));

        // compatible process for ux10(100.320)
        if (IsNeedBuildUnmergedIrModel(version)) {
            HIAI_EXPECT_EXEC(InsertInputConverterOP(graph));
            HIAI_EXPECT_EXEC(ParserCastNode(graph));
        }

        (void)CastOMConverter(version, graph, isChanged);

        // convert om to standard ir graph
        bool isGraphChanged = false;
        std::string curVersion = version.substr(0, ROM_VERSION_CHIP_LEN);
        if (!hiai::IRTransformer::TransferToTargetVersion(graph, curVersion, isGraphChanged)) {
            FMK_LOGE("convert to old ir failed.");
            return HIAI_FAILURE;
        }

        isChanged = isChanged || isGraphChanged;
    }
    return HIAI_SUCCESS;
}

string GetRomVersion()
{
    std::string funcName = "HIAI_GetVersion";
    const char* (*getVersionFunc)() = (const char* (*)())HIAI_Foundation_GetSymbol(funcName.c_str());
    if (getVersionFunc == nullptr) {
        FMK_LOGE("sym %s not found.", funcName.c_str());
        return "";
    }
    const char* version = getVersionFunc();
    if (version == nullptr) {
        return "";
    }
    return version;
}

HIAI_Status MakeModelCompatible(OmFileLoadHelper& omFileHelper, ge::Model& model, bool& isChanged)
{
    ge::ComputeGraphPtr graph = GetComputeGraph(omFileHelper);
    HIAI_EXPECT_NOT_NULL(graph);

    string romVersion = GetRomVersion();
    if (IsNotSupportSpecialDeconvRom(romVersion) && IsSpecialDeconvModel(graph)) {
        FMK_LOGE("Weights size of 1 is currently not supported for operator deconvolution!");
        return HIAI_FAILURE;
    }

    if (!IsSupportedOriginFormatModel(romVersion, graph)) {
        FMK_LOGE("this version doesn't support origin format model.");
        return HIAI_FAILURE;
    }

    if (IsNeedUnmergedWeight(omFileHelper, romVersion, graph)) {
        if (UnmergedWeightGraph(omFileHelper, graph, isChanged) != HIAI_SUCCESS) {
            FMK_LOGE("UnmergedWeightGraph failed.");
            return HIAI_FAILURE;
        }
    }
    bool isOneSideQuant = false;
    if (QuantizeUtil::CheckOneSideQuantize(*graph, isOneSideQuant) != hiai::SUCCESS) {
        FMK_LOGE("CheckOneSideQuantize failed.");
        return HIAI_FAILURE;
    }

    if (isOneSideQuant) {
        if (hiai::QuantizeUtil::DequantizeComputeGraph(*graph) != hiai::SUCCESS) {
            FMK_LOGE("dequant IR graph failed.");
            return HIAI_FAILURE;
        }
        isChanged = true;
    }

    if (ForwardCompatible(romVersion, graph, isChanged) != HIAI_SUCCESS) {
        FMK_LOGE("ForwardCompatible failed.");
        return HIAI_FAILURE;
    }

    // 设置main graph,并对main graph 进行序列化,返回buffer到 mainGraph_model_中
    model.SetGraph(ge::GraphUtils::CreateGraphFromComputeGraph(graph));

    FMK_LOGI("MakeModelCompatible success");
    return HIAI_SUCCESS;
}

HIAI_MemBuffer* CreateBuffer(size_t size)
{
    HIAI_EXPECT_TRUE_R(size != 0, nullptr);
    void* createFun = HIAI_Foundation_GetSymbol("HIAI_MemBuffer_create_from_buffer");
    HIAI_EXPECT_NOT_NULL_R(createFun, nullptr);

    void* data = malloc(size);
    HIAI_EXPECT_NOT_NULL_R(data, nullptr);

    HIAI_MemBuffer* buffer = ((HIAI_MemBuffer*(*)(void*, const unsigned int)) createFun)(data, size);
    if (buffer == nullptr) {
        free(data);
        return nullptr;
    }
    // cppcheck-suppress *
    return buffer;
}

void DestroyBuffer(HIAI_MemBuffer* membuf)
{
    void* destroyFun = HIAI_Foundation_GetSymbol("HIAI_MemBuffer_destroy");
    if (destroyFun == nullptr) {
        return;
    }
    ((void (*)(HIAI_MemBuffer*))destroyFun)(membuf);
}

bool JointModelBuffer(const HIAI_MemBuffer* input, std::vector<hiai::ModelPartition>& partitions, size_t partitionNum,
    size_t modelDefSize, HIAI_MemBuffer** output)
{
    if (partitionNum != partitions.size() + 1) {
        return false;
    }
    size_t partTableSize = sizeof(uint32_t) + sizeof(hiai::ModelPartitionMemInfo) * partitionNum;
    size_t headTotalSize = sizeof(hiai::ModelFileHeader) + partTableSize;
    size_t partitionSize = modelDefSize;
    for (auto& partition : partitions) {
        partitionSize += partition.size;
    }
    size_t outBufferSize = headTotalSize + partitionSize;

    HIAI_MemBuffer* outputBuffer = CreateBuffer(outBufferSize);
    HIAI_EXPECT_NOT_NULL_R(outputBuffer, false);

    // copy model header + parttable
    if (memcpy_s(outputBuffer->data, headTotalSize, input->data, headTotalSize) != 0) {
        FMK_LOGE("memcpy_s modelHead failed.");
        DestroyBuffer(outputBuffer);
        return false;
    }

    uintptr_t modelBasePtr = reinterpret_cast<uintptr_t>(outputBuffer->data);
    // rewrite header info
    hiai::ModelFileHeader* modelHeader = reinterpret_cast<hiai::ModelFileHeader*>(modelBasePtr);
    if (modelHeader == nullptr) {
        FMK_LOGE("modelHeader is nullptr.");
        DestroyBuffer(outputBuffer);
        return false;
    }
    modelHeader->length = partitionSize + partTableSize;

    // rewrite parttable
    hiai::ModelPartitionTable* partTable =
        reinterpret_cast<hiai::ModelPartitionTable*>(modelBasePtr + sizeof(hiai::ModelFileHeader));
    partTable->partition[0].memSize = modelDefSize;
    if (partitionNum >= 2) {
        size_t offset = partTable->partition[0].memOffset + partTable->partition[0].memSize;
        uint8_t* newModelConfig = (uint8_t*)(outputBuffer->data) + modelDefSize + headTotalSize;

        for (size_t index = 1; index < partitionNum; index++) {
            partTable->partition[index].memOffset = offset;
            partTable->partition[index].memSize = partitions[index - 1].size;
            partTable->partition[index].type = partitions[index - 1].type;

            if (memcpy_s((void*)newModelConfig, partitions[index - 1].size, partitions[index - 1].data,
                partitions[index - 1].size) != 0) {
                FMK_LOGE("memcpy_s modelconfig failed.");
                DestroyBuffer(outputBuffer);
                return false;
            }
            offset += partitions[index - 1].size;
            newModelConfig += partitions[index - 1].size;
        }
    }

    partTable->num = partitionNum;
    *output = outputBuffer;
    return true;
}

bool JointModel(
    hiai::OmFileLoadHelper& omFileHelper, const HIAI_MemBuffer* input, const void* modelDef, HIAI_MemBuffer** output)
{
    size_t partitionNum = 1;
    std::vector<hiai::ModelPartition> partitions;
    hiai::ModelPartition opDeviceCfgBuff;
    Status status = omFileHelper.GetModelPartition(hiai::MODEL_CONFIG, opDeviceCfgBuff);
    if (status == hiai::SUCCESS) {
        FMK_LOGI("current model contain MODEL_CONFIG partition");
        partitionNum++;
        partitions.push_back(opDeviceCfgBuff);
    }

    if (IS_MERGED) {
        hiai::ModelPartition weightMergedBuff;
        status = omFileHelper.GetModelPartition(hiai::WEIGHTS_DATA, weightMergedBuff);
        if (status == hiai::SUCCESS) {
            partitionNum++;
            partitions.push_back(weightMergedBuff);
        }
    }

    hiai::ModelSerializeWrapper modelSerialize;
    size_t modelDefSize = 0;
    bool ret = modelSerialize.GetModelDefBufferSize(modelDef, modelDefSize);
    if (!ret) {
        FMK_LOGE("GetModelDefBufferSize failed");
        return false;
    }

    size_t partTableSize = sizeof(uint32_t) + sizeof(hiai::ModelPartitionMemInfo) * partitionNum;
    size_t headTotalSize = sizeof(hiai::ModelFileHeader) + partTableSize;

    if (!JointModelBuffer(input, partitions, partitionNum, modelDefSize, output)) {
        FMK_LOGE("JointModelBuffer failed");
        return false;
    }

    // serialize modeldef to remaked-model buffer
    ret = modelSerialize.SerializeModelDefToBuffer(modelDef,
        reinterpret_cast<void*>(reinterpret_cast<uintptr_t>((*output)->data) + headTotalSize),
        (*output)->size - headTotalSize);
    if (!ret) {
        FMK_LOGE("SerializeModelDefToBuffer fail.");
        DestroyBuffer(*output);
        return false;
    }
    return true;
}

HIAI_Status SaveModel(
    OmFileLoadHelper& omFileHelper, const HIAI_MemBuffer* input, ge::Model& model, HIAI_MemBuffer** output)
{
    ModelSerializeWrapper modelSerialize;
    void* modelDef = nullptr;
    bool ret = modelSerialize.SaveModelToModelDef(model, modelDef);
    if (!ret || modelDef == nullptr) {
        FMK_LOGE("SaveModelToModelDef modelDef is nullptr");
        return HIAI_FAILURE;
    }

    ret = JointModel(omFileHelper, input, modelDef, output);
    modelSerialize.ReleaseModelDef(modelDef);
    modelDef = nullptr;
    if (!ret) {
        FMK_LOGE("SaveAfterProcessBuffer failed");
        return HIAI_FAILURE;
    }
    return HIAI_SUCCESS;
}

static std::map<std::string, std::string> IR_QUANT_PRODUCT_NOUSUPPORT_VERSION = {
    {"100.320.010", "100.320.010.023"}, // PhoenixC20 Rom版本
    {"100.320.011", "100.320.011.019"}, // DenverC10 Rom版本
    {"100.320.012", "100.320.012.011"}, // OrlandoC20 Rom版本
    {"100.330.010", "100.330.010.011"}, // PhoenixC30 UX11.0 Rom版本
    {"100.330.011", "100.330.011.011"}, // DenverC20 UX11.0 Rom版本
    {"100.330.012", "100.330.012.011"}, // OrlandoC30 UX11.0 Rom版本
    {"100.500.010", "100.500.010.011"}, // BaltimoreC10 UX11.0 Rom版本
};

bool IsSupportQuantize(const string& romVersion)
{
    if (romVersion.size() < ROM_VERSION_BASE_LEN) {
        FMK_LOGE("rom version:%s size not illegal.", romVersion.c_str());
        return false;
    }
    std::string romProduct = romVersion.substr(0, ROM_VERSION_PRODUCT_LEN);
    auto it = IR_QUANT_PRODUCT_NOUSUPPORT_VERSION.find(romProduct);
    if (it == IR_QUANT_PRODUCT_NOUSUPPORT_VERSION.end()) {
        // 低于100.320的历史版本不支持IR，不会走到此流程
        return true;
    }
    std::string romBaseVersion = it->second;
    if (strcmp(romVersion.c_str(), romBaseVersion.c_str()) < 0) {
        return false; // 对应产品的老版本不支持IR量化模型(反向判断，方便维护与日落)
    }
    return true;
}

HIAI_Status MakeCompatibleIRAPI(const HIAI_MemBuffer* input, HIAI_MemBuffer** output)
{
    // 处理IR_API_GRAPH_MODEL
    ge::Model irModel;
    if (irModel.Load(static_cast<uint8_t*>(input->data), input->size) != ge::GRAPH_SUCCESS) {
        FMK_LOGI("not ir api model");
        return HIAI_UNSUPPORTED;
    }

    (void)ge::AttrUtils::SetInt(&irModel, "stream_num", 1);
    ge::ComputeGraphPtr graph = ge::GraphUtils::GetComputeGraph(irModel.GetGraph());

    string romVersion = GetRomVersion();
    bool isGraphChanged = false;
    if (!hiai::IRBuildTransformer::TransferIRToTargetVersion(graph, romVersion)) {
        FMK_LOGE("ir build define convert failed!");
        return HIAI_FAILURE;
    }

    if (!hiai::IRTransformer::TransferToTargetVersion(graph, romVersion, isGraphChanged)) {
        FMK_LOGE("ir define convert failed!");
        return HIAI_FAILURE;
    }

    if (!IsSupportQuantize(romVersion) && hiai::QuantizeUtil::DequantizeComputeGraph(*graph) != hiai::SUCCESS) {
        FMK_LOGE("dequant IR graph failed.");
        return HIAI_FAILURE;
    }

    ModelSerializeWrapper modelSerialize;
    void* modelDef = nullptr;
    if (!modelSerialize.SaveModelToModelDef(irModel, modelDef) || modelDef == nullptr) {
        FMK_LOGE("SaveModelToModelDef modelDef is nullptr");
        return HIAI_FAILURE;
    }

    HIAI_Status ret = HIAI_FAILURE;
    do {
        size_t modelSize = 0;
        if (!modelSerialize.GetModelDefBufferSize(modelDef, modelSize)) {
            FMK_LOGE("GetGraphDefBufferSize failed");
            break;
        }

        HIAI_MemBuffer* outputBuffer = CreateBuffer(modelSize);
        if (outputBuffer == nullptr) {
            FMK_LOGE("CreateBuffer failed");
            break;
        }

        if (!modelSerialize.SerializeModelDefToBuffer(modelDef, outputBuffer->data, modelSize)) {
            FMK_LOGE("SerializeModelDefToBuffer failed");
            DestroyBuffer(outputBuffer);
            break;
        }

        *output = outputBuffer;
        ret = HIAI_SUCCESS;
    } while (0);
    modelSerialize.ReleaseModelDef(modelDef);
    return ret;
}
} // namespace
HIAI_Status HIAI_MakeDirectCompatibleModel(const HIAI_MemBuffer* input, HIAI_MemBuffer** output)
{
    if (input == nullptr || input->data == nullptr) {
        FMK_LOGE("input is invalid.");
        return HIAI_FAILURE;
    }

    HIAI_Foundation_Init();

    if (input->size < FILE_HEAD_LENGTH) {
        return MakeCompatibleIRAPI(input, output);
    }

    ModelFileHeader* header = reinterpret_cast<ModelFileHeader*>(input->data);
    if (header->magic != MODEL_FILE_MAGIC_NUM) {
        return MakeCompatibleIRAPI(input, output);
    }
    if (header->modeltype != hiai::STANDARD_IR_GRAPH_MODEL) { // 非标准IR模型，无需兼容性处理
        FMK_LOGI("Not standard ir model");
        return HIAI_SUCCESS;
    }

    OmFileLoadHelper omFileHelper;
    if (omFileHelper.Init((uint8_t*)input->data + FILE_HEAD_LENGTH, input->size - FILE_HEAD_LENGTH) != hiai::SUCCESS) {
        FMK_LOGE("Invalid Model Data.");
        return HIAI_FAILURE;
    }

    bool isChanged = false;
    ge::Model model;
    if (MakeModelCompatible(omFileHelper, model, isChanged) != HIAI_SUCCESS) {
        FMK_LOGE("MakeCompatible failed.");
        HIAI_Foundation_Deinit();
        return HIAI_FAILURE;
    }

    if (isChanged) {
        if (SaveModel(omFileHelper, input, model, output) != HIAI_SUCCESS) {
            FMK_LOGE("SaveModel failed.");
            HIAI_Foundation_Deinit();
            return HIAI_FAILURE;
        }
    }

    HIAI_Foundation_Deinit();

    FMK_LOGI("MakeDirectCompatibleModel success.");
    return HIAI_SUCCESS;
}
