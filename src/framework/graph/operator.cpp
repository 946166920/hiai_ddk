/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: operator.cpp
 */
#include "graph/operator.h"
#include <algorithm>
#include <set>
#include "infra/base/securestl.h"
#include "operator_impl.h"
#include "framework/graph/debug/ge_log.h"

namespace ge {
Operator::Operator(const std::string& type)
{
    static uint32_t index = 0;
    string name = type + ":" + std::to_string(index++);
    impl_ = hiai::make_shared_nothrow<OperatorImpl>(name, type);
}

Operator::Operator(const string& name, const string& type, int version)
{
    impl_ = hiai::make_shared_nothrow<OperatorImpl>(name, type);
    if (impl_ == nullptr) {
        return;
    }
    Operator::SetAttr("version", ge::AttrValue::CreateFrom(static_cast<int64_t>(version)));
}

const OperatorImplPtr Operator::GetImpl() const
{
    return impl_;
}

Operator::Operator(ge::OperatorImplPtr&& opImpl)
{
    impl_ = std::move(opImpl);
}

OpAnchor Operator::GetOutput(const uint32_t outIndex) const
{
    return OpAnchor(impl_, outIndex);
}

Operator& Operator::SetInput(const string& name, const ge::Operator& outOp)
{
    if (name.empty()) {
        FMK_LOGE("name is empty.");
        return *this;
    }
    if (outOp.impl_ == nullptr) {
        FMK_LOGE("output impl_ is nullptr.");
        return *this;
    }
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return *this;
    }

    impl_->SetInput(name, outOp.impl_);

    if (impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("OpDesc is nullptr.");
        return *this;
    }
    int dstIndex = impl_->GetOpDesc()->GetInputIndexByName(name);

    GE_CHK_BOOL_EXEC(dstIndex >= 0, return *this, "Find input index by name failed. name[%s]", name.c_str());

    if (outOp.impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("OpDesc is nullptr.");
        return *this;
    }
    if (outOp.impl_->GetOpDesc()->GetType() == "Const") {
        (*this).SetOpIsInputConst(true, dstIndex);
    } else {
        (*this).SetOpIsInputConst(false, dstIndex);
    }
    std::vector<bool> is_input_const = (*this).GetOpIsInputConst();
    auto attr = AttrValue::CreateFrom(is_input_const);
    Operator::SetAttr("is_input_const", std::move(attr));
    return *this;
}
Operator& Operator::SetInput(const string& name, const ge::OpAnchor& opAnchor)
{
    if (name.empty()) {
        FMK_LOGE("name is empty.");
        return *this;
    }
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return *this;
    }
    if (impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("OpDesc is nullptr.");
        return *this;
    }
    int index = impl_->GetOpDesc()->GetInputIndexByName(name);
    GE_CHK_BOOL_EXEC(index >= 0, return *this, "Find input index by name failed. name[%s]", name.c_str());
    OperatorImplPtr outImpl = opAnchor.first.lock();
    if (outImpl == nullptr) {
        FMK_LOGE("output impl is nullptr.");
        return *this;
    }
    impl_->SetInput(index, outImpl, opAnchor.second);

    if (outImpl->GetOpDesc() == nullptr) {
        FMK_LOGE("OpDesc is nullptr.");
        return *this;
    }
    if (outImpl->GetOpDesc()->GetType() == "Const") {
        (*this).SetOpIsInputConst(true, index);
    } else {
        (*this).SetOpIsInputConst(false, index);
    }
    std::vector<bool> is_input_const = (*this).GetOpIsInputConst();
    auto attr = AttrValue::CreateFrom(is_input_const);
    Operator::SetAttr("is_input_const", std::move(attr));
    return *this;
}
Operator& Operator::SetInput(const std::string& name, const ge::Operator& outOp, const std::string& outName)
{
    if (name.empty() || outName.empty()) {
        FMK_LOGE("name is empty.");
        return *this;
    }
    if (outOp.impl_ == nullptr) {
        FMK_LOGE("output impl_ is nullptr.");
        return *this;
    }
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return *this;
    }
    impl_->SetInput(name, outOp.impl_, outName);

    if (impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("impl_'s opDesc is nullptr.");
        return *this;
    }
    int dstIndex = impl_->GetOpDesc()->GetInputIndexByName(name);

    GE_CHK_BOOL_EXEC(dstIndex >= 0, return *this, "Find input index by name failed. name[%s]", name.c_str());

    if (outOp.impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("output impl_'s opDesc is nullptr.");
        return *this;
    }
    if (outOp.impl_->GetOpDesc()->GetType() == "Const") {
        (*this).SetOpIsInputConst(true, dstIndex);
    } else {
        (*this).SetOpIsInputConst(false, dstIndex);
    }
    std::vector<bool> is_input_const = (*this).GetOpIsInputConst();
    auto attr = AttrValue::CreateFrom(is_input_const);
    Operator::SetAttr("is_input_const", std::move(attr));
    return *this;
}

GRAPH_API_EXPORT Operator& Operator::SetInput(int32_t index, const ge::Operator& outOp, int32_t outIndex)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return *this;
    }
    impl_->SetInput(index, outOp.impl_, outIndex);

    if (outOp.impl_ == nullptr || outOp.impl_->GetOpDesc() == nullptr) {
        FMK_LOGE("impl_ or opDesc is nullptr.");
        return *this;
    }
    if (index >= 0) {
        if (outOp.impl_->GetOpDesc()->GetType() == "Const") {
            (*this).SetOpIsInputConst(true, index);
        } else {
            (*this).SetOpIsInputConst(false, index);
        }
        std::vector<bool> is_input_const = (*this).GetOpIsInputConst();
        auto attr = AttrValue::CreateFrom(is_input_const);
        Operator::SetAttr("is_input_const", std::move(attr));
    }

    return *this;
}

Operator& Operator::SetDynamicInput(const std::string& name, int32_t index, const Operator& outOp)
{
    return SetInput(name + std::to_string(index), outOp);
}

Operator& Operator::SetDynamicInput(
    const std::string& name, int32_t index, const Operator& outOp, const std::string& outName)
{
    return SetInput(name + std::to_string(index), outOp, outName);
}
Operator& Operator::SetDynamicInput(const string& name, int32_t index, const ge::OpAnchor& opAnchor)
{
    return SetInput(name + std::to_string(index), opAnchor);
}

Operator& Operator::SetGraphBuilder(const string& name, const GraphBuilderFn& graphBuilderFn)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return *this;
    }
    std::string graphName = impl_->GetName() + "_" + name;
    std::replace(graphName.begin(), graphName.end(), '/', '_');
    auto attr = AttrValue::CreateFrom(graphName);
    Operator::SetAttr(name, std::move(attr));
    impl_->SetGraphBuilder(graphName, graphBuilderFn);
    return *this;
}

GraphErrCodeStatus Operator::UpdateInputDesc(const std::string& name, const ge::TensorDesc& tensorDesc)
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return GRAPH_FAILED, "operator impl_ is nullptr.");
    return impl_->UpdateInputDesc(name, tensorDesc);
}

Operator& Operator::SetAttr(const string& name, ge::AttrValue&& attrValue)
{
    GE_CHK_BOOL_EXEC(!!impl_, return *this, "operator impl_ is nullptr.");
    impl_->SetAttr(name, std::move(attrValue));
    return *this;
}

void Operator::InputRegister(const string& name)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->InputRegister(name);
}

void Operator::OptionalInputRegister(const string& name)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->OptionalInputRegister(name);
}

void Operator::DynamicInputRegister(const string& name, const unsigned int num)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->DynamicInputRegister(name, num);
}

void Operator::OutputRegister(const string& name)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->OutputRegister(name);
}

void Operator::DynamicOutputRegister(const string& name, const unsigned int num)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->DynamicOutputRegister(name, num);
}

void Operator::AttrRegister(const string& name, AttrValue::ValueType type)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->AttrRegister(name, type);
}

void Operator::OptionalAttrRegister(const string& name, AttrValue&& attrValue)
{
    if (impl_ == nullptr) {
        FMK_LOGE("impl_ is nullptr.");
        return;
    }
    impl_->OptionalAttrRegister(name, std::move(attrValue));
}

string Operator::GetName() const
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return "", "operator impl_ is nullptr.");
    return impl_->GetName();
}

std::vector<bool> Operator::GetOpIsInputConst() const
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return vector<bool>(), "operator impl_ is nullptr.");
    GE_CHK_BOOL_EXEC(impl_->GetOpDesc() != nullptr, return vector<bool>(), "opDesc is nullptr..");
    return impl_->GetOpDesc()->GetIsInputConst();
}

void Operator::SetOpIsInputConst(bool inputConst, uint32_t index)
{
    GE_CHK_BOOL_EXEC(impl_ != nullptr, return, "operator impl_ is nullptr.");
    GE_CHK_BOOL_EXEC(impl_->GetOpDesc() != nullptr, return, "opDesc is nullptr.");
    vector<bool> isInputConst = impl_->GetOpDesc()->GetIsInputConst();
    if (index >= isInputConst.size()) {
        isInputConst.resize(index + 1);
        isInputConst[index] = inputConst;
    } else {
        isInputConst[index] = inputConst;
    }

    impl_->GetOpDesc()->SetIsInputConst(isInputConst);
}
} // namespace ge
