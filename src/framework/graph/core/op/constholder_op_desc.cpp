/**
 * Copyright 2022-2022 Huawei Technologies Co., Ltd
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

#include "graph/core/op/constholder_op_desc.h"

// npu/api/framework
#include "graph/op/const_defs.h"
#include "graph/tensor.h"

// npu/inc/infra
#include "infra/base/assertion.h"
#include "infra/base/securestl.h"

// npu/inc/framework
#include "framework/infra/log/log.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/utils/attr_utils.h"

// npu/src/framework
#include "graph/core/op/op_desc_factory.h"
#include "graph/persistance/interface/op_def.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/interface/attr_def.h"
#include "graph/persistance/proxy/proto_factory.h"

namespace ge {
namespace {
static constexpr const char* ATTR_NAME_IS_CONST_HOLDER = "is_const_holder";
}
ConstHolderOpDesc::ConstHolderOpDesc(const std::string& name, const std::string& type)
    : OpDesc(name, type), opDesc_(nullptr)
{
    SetAttr(ATTR_NAME_IS_CONST_HOLDER, AttrValue::CreateFrom(true)); // 添加 is_const_holder
}
ConstHolderOpDesc::ConstHolderOpDesc(hiai::IOpDef* opDef, bool isOwner) : OpDesc(opDef, isOwner), opDesc_(nullptr)
{
}

ConstHolderOpDesc::~ConstHolderOpDesc()
{
    opDesc_ = nullptr;
}

bool ConstHolderOpDesc::IsConstHolderOp(const ge::OpDesc& opDesc)
{
    return opDesc.HasAttr(ATTR_NAME_IS_CONST_HOLDER);
}

const TensorDesc& ConstHolderOpDesc::GetOutputDesc(uint32_t index) const
{
    static TensorDesc tensorDesc;
    if (opDesc_ == nullptr) {
        HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), tensorDesc);
        return *outputsDesc_[index];
    }
    return opDesc_->GetOutputDesc(index);
}

ConstTensorDescPtr ConstHolderOpDesc::GetOutputDescPtr(uint32_t index) const
{
    if (opDesc_ == nullptr) {
        HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), nullptr);
        return outputsDesc_[index];
    }
    return opDesc_->GetOutputDescPtr(index);
}

TensorDescPtr ConstHolderOpDesc::MutableOutputDesc(uint32_t index) const
{
    if (opDesc_ == nullptr) {
        HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), nullptr);
        return outputsDesc_[index];
    }
    return opDesc_->MutableOutputDesc(index);
}

OpDesc::Vistor<TensorDesc> ConstHolderOpDesc::GetAllOutputsDesc() const
{
    if (opDesc_ == nullptr) {
        vector<TensorDesc> temp;
        for (const auto& it : outputsDesc_) {
            temp.push_back(*it);
        }
        return OpDesc::Vistor<TensorDesc>(shared_from_this(), temp);
    }
    return opDesc_->GetAllOutputsDesc();
}

OpDesc::Vistor<TensorDescPtr> ConstHolderOpDesc::GetAllOutputsDescPtr() const
{
    if (opDesc_ == nullptr) {
        return OpDesc::Vistor<TensorDescPtr>(shared_from_this(), outputsDesc_);
    }
    return opDesc_->GetAllOutputsDescPtr();
}

size_t ConstHolderOpDesc::GetOutputsSize() const
{
    if (opDesc_ == nullptr) {
        return outputsDesc_.size();
    }
    return opDesc_->GetOutputsSize();
}

const vector<TensorDescPtr>& ConstHolderOpDesc::GetOutputsDesc() const
{
    if (opDesc_ == nullptr) {
        return outputsDesc_;
    }
    return opDesc_->GetOutputsDesc();
}

void ConstHolderOpDesc::SetHoldedOpDesc(OpDesc* opDesc)
{
    opDesc_ = opDesc;
}

const OpDesc* ConstHolderOpDesc::GetHoldedOpDesc() const
{
    return opDesc_;
}

OpDescPtr ConstHolderOpDesc::Clone()
{
    auto newOpDef = hiai::ProtoFactory::Instance()->CreateOpDef();
    HIAI_EXPECT_NOT_NULL_R(newOpDef, nullptr);

    if (SerializeTo(newOpDef) != GRAPH_SUCCESS) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(newOpDef);
        return nullptr;
    }

    auto cloneOp = hiai::make_shared_nothrow<ConstHolderOpDesc>(newOpDef, true);
    if (cloneOp == nullptr) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(newOpDef);
        return nullptr;
    }

    if (cloneOp->UnSerialize() != GRAPH_SUCCESS) {
        cloneOp.reset();
    }

    if (opDesc_ != nullptr) {
        cloneOp->SetHoldedOpDesc(opDesc_);
    }

    return cloneOp;
}

OpDescPtr CreateConstHolderOpDesc(hiai::IOpDef* opDef, bool isOwner)
{
    if (opDef->attr() == nullptr || !opDef->attr()->has_attr(ATTR_NAME_IS_CONST_HOLDER)) {
        return nullptr;
    }
    const hiai::IAttrDef* valueDef = opDef->attr()->attr(ATTR_NAME_IS_CONST_HOLDER);
    if (!valueDef->b()) {
        return nullptr;
    }

    OpDescPtr ptr = hiai::make_shared_nothrow<ConstHolderOpDesc>(opDef, isOwner);
    return ptr;
}

REGISTER_OP_DESC_CREATOR(CreateConstHolderOpDesc);
} // namespace ge