/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: operator_impl.cpp
 */
#include "operator_impl.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"
#include "framework/graph/debug/ge_log.h"
#include "graph/tensor.h"

namespace ge {
const std::map<std::string, std::string> IR_OPTYPE_TO_FMK_OPTYPE = {{"QuantizedConvolution", "Convolution"},
    {"QuantizedConvolutionDepthwise", "ConvolutionDepthwise"}, {"QuantizedFullConnection", "FullConnection"},
    {"QuantizedMatMul", "MatMul"}, {"Crop", "Slice"}, {"ResizeBilinearV2", "ResizeBilinear"},
    {"QuantizedFullyConnection", "FullyConnection"}, {"BatchNormExt2", "BatchNorm"}};

OperatorImpl::OperatorImpl(const std::string& name, const std::string& type)
{
    auto iter = IR_OPTYPE_TO_FMK_OPTYPE.find(type);
    opDesc_ = hiai::make_shared_nothrow<OpDesc>(name, iter == IR_OPTYPE_TO_FMK_OPTYPE.end() ? type : iter->second);
}

OperatorImpl::OperatorImpl(OpDescPtr opDesc) : opDesc_(opDesc)
{
}

const OpDescPtr OperatorImpl::GetOpDesc() const
{
    return opDesc_;
}

void OperatorImpl::SetInput(const std::string& name, const OperatorImplPtr& outOp)
{
    if (opDesc_ == nullptr || outOp == nullptr || outOp->opDesc_ == nullptr) {
        FMK_LOGE("opdesc_ is invalid.");
        return;
    }

    int32_t index = opDesc_->GetInputIndexByName(name);
    if (index < 0) {
        FMK_LOGE("can't find %s in op[%s]", name.c_str(), opDesc_->GetName().c_str());
        return;
    }

    if (inputLink_.find(name) != inputLink_.end()) {
        FMK_LOGE("input[%s:%s] already set.", opDesc_->GetName().c_str(), name.c_str());
        return;
    }
    OpDescPtr& outOpDesc = outOp->opDesc_;
    if (outOpDesc->GetOutputsSize() != 1) {
        FMK_LOGE("the source op[%s] must has only one output", outOpDesc->GetName().c_str());
        return;
    }

    uint32_t outIndex = 0;
    inputLink_.insert(std::make_pair(name, OpAnchor(outOp, outIndex)));
    opDesc_->UpdateInputDesc(index, outOpDesc->GetOutputDesc(outIndex));

    std::string outName = outOpDesc->GetOutputNameByIndex(outIndex);
    OpAnchor inAnchor {shared_from_this(), index};
    outOp->LinkToOutputOp(outName, inAnchor);
}

void OperatorImpl::SetInput(const std::string& name, const OperatorImplPtr& outOp, const std::string& outName)
{
    if (opDesc_ == nullptr || outOp == nullptr || outOp->opDesc_ == nullptr) {
        FMK_LOGE("opdesc_ is invalid.");
        return;
    }

    int32_t index = opDesc_->GetInputIndexByName(name);
    if (index < 0) {
        FMK_LOGE("can't find %s in op[%s]", name.c_str(), opDesc_->GetName().c_str());
        return;
    }

    if (inputLink_.find(name) != inputLink_.end()) {
        FMK_LOGE("input[%s:%s] already set.", opDesc_->GetName().c_str(), name.c_str());
        return;
    }
    OpDescPtr& outOpDesc = outOp->opDesc_;
    int32_t outIndex = outOpDesc->GetOutputIndexByName(outName);
    if (outIndex < 0) {
        FMK_LOGE("can't find %s in op[%s]", outName.c_str(), outOp->opDesc_->GetName().c_str());
        return;
    }

    inputLink_.insert(std::make_pair(name, OpAnchor(outOp, outIndex)));
    opDesc_->UpdateInputDesc(index, outOpDesc->GetOutputDesc(outIndex));

    OpAnchor inAnchor {shared_from_this(), index};
    outOp->LinkToOutputOp(outName, inAnchor);
}

void OperatorImpl::SetInput(int32_t index, const OperatorImplPtr& outOp, int32_t outIndex)
{
    HIAI_EXPECT_NOT_NULL_VOID(opDesc_);
    HIAI_EXPECT_NOT_NULL_VOID(outOp);
    HIAI_EXPECT_NOT_NULL_VOID(outOp->opDesc_);

    const string ctrlName = "-1";

    string outName;
    if (outIndex >= 0) {
        outName = outOp->opDesc_->GetOutputNameByIndex(static_cast<uint32_t>(outIndex));
    } else if (outIndex == -1) {
        outName = ctrlName;
    }
    HIAI_EXPECT_TRUE_VOID(!outName.empty());

    string inputName;
    if (index >= 0) {
        inputName = opDesc_->GetInputNameByIndex(static_cast<uint32_t>(index));
    } else if (index == -1) {
        inputName = ctrlName;
    }
    HIAI_EXPECT_TRUE_VOID(!inputName.empty());

    HIAI_EXPECT_TRUE_VOID(inputLink_.find(inputName) == inputLink_.end());
    inputLink_.insert(std::make_pair(inputName, OpAnchor(outOp, outIndex)));
    OpAnchor inAnchor {shared_from_this(), index};
    outOp->LinkToOutputOp(outName, inAnchor);

    if (index != -1 && outIndex >= 0) {
        opDesc_->UpdateInputDesc(index, outOp->opDesc_->GetOutputDesc(static_cast<uint32_t>(outIndex)));
    }
}

GraphErrCodeStatus OperatorImpl::SetAttr(const std::string& name, AttrValue&& attrValue)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return GRAPH_FAILED;
    }
    return opDesc_->SetAttr(name, std::move(attrValue));
}

GraphErrCodeStatus OperatorImpl::GetAttr(const string& name, AttrValue& attrValue) const
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return GRAPH_FAILED;
    }
    return opDesc_->GetAttr(name, attrValue);
}

std::string OperatorImpl::GetName() const
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return "";
    }
    return opDesc_->GetName();
}

GraphErrCodeStatus OperatorImpl::SetGraphBuilder(const std::string& name, const GraphBuilderFn& v)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return GRAPH_FAILED;
    }
    if (subGraphBuilderFn_.find(name) != subGraphBuilderFn_.end()) {
        FMK_LOGE("sub graph %s is setted", name.c_str());
        return GRAPH_FAILED;
    }
    FMK_LOGE("subGraphBuilderFn_ add name =%s", name.c_str());
    subGraphBuilderFn_.emplace(name, v);
    return GRAPH_SUCCESS;
}

GraphBuilderFn OperatorImpl::GetGraphBuilder(const std::string& name) const
{
    auto it = subGraphBuilderFn_.find(name);
    return it != subGraphBuilderFn_.end() ? it->second : nullptr;
}

void OperatorImpl::LinkToOutputOp(const std::string& outName, OpAnchor& peerOp)
{
    auto itFind = outputLinks_.find(outName);
    if (itFind == outputLinks_.end()) {
        std::vector<OpAnchor> peerOps {peerOp};
        outputLinks_.insert(std::make_pair(outName, peerOps));
    } else {
        itFind->second.push_back(peerOp);
    }
}

const std::map<std::string, OpAnchor>& OperatorImpl::GetAllInputLinks() const
{
    return inputLink_;
}

const std::map<std::string, std::vector<OpAnchor>>& OperatorImpl::GetAllOutputLinks() const
{
    return outputLinks_;
}

void OperatorImpl::InputRegister(const std::string& name)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    (void)opDesc_->AddInputDesc(name, TensorDesc(Shape(), FORMAT_RESERVED, DT_UNDEFINED));
}

void OperatorImpl::OptionalInputRegister(const std::string& name)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }

    opDesc_->AddOptionalInputDesc(name, TensorDesc(Shape(), FORMAT_RESERVED, DT_UNDEFINED));
}

void OperatorImpl::DynamicInputRegister(const std::string& name, uint32_t num)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    opDesc_->AddDynamicInputDesc(name, num);
}

void OperatorImpl::OutputRegister(const std::string& name)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    (void)opDesc_->AddOutputDesc(name, TensorDesc());
}

void OperatorImpl::DynamicOutputRegister(const std::string& name, uint32_t num)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    opDesc_->AddDynamicOutputDesc(name, num);
}

void OperatorImpl::AttrRegister(const std::string& name, AttrValue::ValueType type)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    opDesc_->AddRequiredAttr(name, type);
}

void OperatorImpl::OptionalAttrRegister(const std::string& name, AttrValue&& attrValue)
{
    if (opDesc_ == nullptr) {
        FMK_LOGE("opDesc_ is nullptr");
        return;
    }
    opDesc_->SetAttr(name, attrValue);
}

GraphErrCodeStatus OperatorImpl::UpdateInputDesc(const string& name, const TensorDesc& tensorDesc)
{
    GE_CHK_BOOL_EXEC(opDesc_ != nullptr, return GRAPH_FAILED, "opDesc_ is nullptr.");
    return opDesc_->UpdateInputDesc(name, tensorDesc);
}
} // namespace ge