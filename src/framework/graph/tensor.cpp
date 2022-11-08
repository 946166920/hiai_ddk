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
#include "graph/tensor.h"

#include <string>
#include <map>

#include "infra/base/assertion.h"
#include "infra/base/securestl.h"

#include "graph/persistance/interface/tensor_def.h"
#include "graph/persistance/interface/shape_def.h"
#include "graph/persistance/interface/tensor_desc_def.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/proxy/proto_factory.h"

namespace ge {
const static string STR_EMPTY = "";

TensorDesc::TensorDesc() : TensorDesc(hiai::ProtoFactory::Instance()->CreateTensorDescDef(), true)
{
}

TensorDesc::TensorDesc(hiai::ITensorDescDef* tensorDescDef, bool isOwner) : tensorDescDef_(tensorDescDef),
    isOwner_(isOwner), shape_(tensorDescDef_ != nullptr ? tensorDescDef_->mutable_shape() : nullptr, false)
{
}

TensorDesc::~TensorDesc()
{
    if (isOwner_ && tensorDescDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyTensorDescDef(tensorDescDef_);
    }
    tensorDescDef_ = nullptr;
}

const hiai::IAttrMapDef* TensorDesc::GetAttrMapDef() const
{
    return tensorDescDef_ != nullptr ? tensorDescDef_->attr() : nullptr;
}

hiai::IAttrMapDef* TensorDesc::MutableAttrMapDef()
{
    return tensorDescDef_ != nullptr ? tensorDescDef_->mutable_attr() : nullptr;
}

TensorDesc::TensorDesc(Shape shape, Format format, DataType dt) : TensorDesc()
{
    ShapeReference() = shape;
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->set_layout(format);
        tensorDescDef_->set_dtype(dt);
    }
}

TensorDesc::TensorDesc(Shape shape, DataType dt) : TensorDesc()
{
    ShapeReference() = shape;
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->set_dtype(dt);
    }
}

GraphErrCodeStatus TensorDesc::Save(Buffer& buffer) const
{
    HIAI_EXPECT_NOT_NULL(tensorDescDef_);

    buffer.Resize(tensorDescDef_->GetTensorDescDefSize());
    HIAI_EXPECT_TRUE(tensorDescDef_->SaveTo(buffer.MutableData(), buffer.GetSize()));

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus TensorDesc::Load(const uint8_t* data, size_t len)
{
    HIAI_EXPECT_NOT_NULL(tensorDescDef_);

    HIAI_EXPECT_TRUE(tensorDescDef_->LoadFrom(data, len));

    return GRAPH_SUCCESS;
}

bool TensorDesc::SerializeTo(hiai::ITensorDescDef* to) const
{
    if (to != nullptr) {
        to->CopyFrom(tensorDescDef_);
        return true;
    }
    return false;
}

TensorDesc::TensorDesc(const TensorDesc& desc) : TensorDesc()
{
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->CopyFrom(desc.tensorDescDef_);
    }
}

TensorDesc& TensorDesc::operator=(const TensorDesc& desc)
{
    if (&desc == this) {
        return *this;
    }
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->CopyFrom(desc.tensorDescDef_);
    }
    return *this;
}

const string& TensorDesc::GetName() const
{
    return tensorDescDef_ != nullptr ? tensorDescDef_->name() : STR_EMPTY;
}

void TensorDesc::SetName(const std::string& name)
{
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->set_name(name);
    }
}

const Shape& TensorDesc::GetShape() const
{
    return ShapeReference();
}

Shape& TensorDesc::MutableShape()
{
    return ShapeReference();
}

void TensorDesc::SetShape(const Shape& shape)
{
    ShapeReference() = shape;
}

Format TensorDesc::GetFormat() const
{
    return tensorDescDef_ != nullptr ? tensorDescDef_->layout() : FORMAT_RESERVED;
}

void TensorDesc::SetFormat(Format format)
{
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->set_layout(format);
    }
}

DataType TensorDesc::GetDataType() const
{
    return tensorDescDef_ != nullptr ? tensorDescDef_->dtype() : DT_UNDEFINED;
}

void TensorDesc::SetDataType(DataType dataType)
{
    if (tensorDescDef_ != nullptr) {
        tensorDescDef_->set_dtype(dataType);
    }
}

Shape& TensorDesc::ShapeReference() const
{
    if (tensorDescDef_ != nullptr) {
        Shape refShape(tensorDescDef_->mutable_shape(), false);
        shape_.RefTo(refShape);
    }
    return shape_;
}

void TensorDesc::RefTo(const TensorDesc& desc)
{
    tensorDescDef_ = desc.tensorDescDef_;
    isOwner_ = desc.isOwner_;
}

Tensor::Tensor() : Tensor(hiai::ProtoFactory::Instance()->CreateTensorDef(), true)
{
}

Tensor::Tensor(hiai::ITensorDef* tensorDef, bool isOwner) : tensorDef_(tensorDef), isOwner_(isOwner),
    desc_(tensorDef_ != nullptr ? tensorDef_->mutable_desc() : nullptr, false),
    buffer_(tensorDef_ != nullptr ? tensorDef_->mutable_data() : nullptr, false)
{
}

Tensor::~Tensor()
{
    if (isOwner_ && tensorDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyTensorDef(tensorDef_);
    }
    tensorDef_ = nullptr;
}

TensorDesc& Tensor::DescReference() const
{
    if (tensorDef_ != nullptr) {
        TensorDesc tensorDesc(tensorDef_->mutable_desc(), false);
        desc_.RefTo(tensorDesc);
    }
    return desc_;
}

Buffer& Tensor::BufferReference() const
{
    if (tensorDef_ != nullptr) {
        Buffer buffer(tensorDef_->mutable_data(), false);
        buffer_.RefTo(buffer);
    }
    return buffer_;
}

bool Tensor::SerializeTo(hiai::ITensorDef* to) const
{
    if (to != nullptr) {
        to->CopyFrom(tensorDef_);
        return true;
    }
    return false;
}

Tensor::Tensor(const TensorDesc& tensorDesc) : Tensor()
{
    DescReference() = tensorDesc;
}

Tensor::Tensor(const TensorDesc& tensorDesc, const uint8_t* data, size_t size) : Tensor() // default
{
    DescReference() = tensorDesc;
    Buffer::CopyFrom(data, size, BufferReference());
}

Tensor::Tensor(const TensorDesc& tensorDesc, const Buffer& data) : Tensor() // default
{
    DescReference() = tensorDesc;
    BufferReference() = data;
}

const TensorDesc& Tensor::GetTensorDesc() const
{
    return DescReference();
}

TensorDesc& Tensor::MutableTensorDesc()
{
    return DescReference();
}

GraphErrCodeStatus Tensor::SetTensorDesc(const TensorDesc& tensorDesc)
{
    DescReference() = tensorDesc;
    return GRAPH_SUCCESS;
}

const Buffer& Tensor::GetData() const
{
    return BufferReference();
}

Buffer& Tensor::MutableData()
{
    return BufferReference();
}

GraphErrCodeStatus Tensor::SetData(const uint8_t* data, size_t size)
{
    Buffer::CopyFrom(data, size, BufferReference());
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus Tensor::SetData(const Buffer& data)
{
    BufferReference() = data;
    return GRAPH_SUCCESS;
}

std::shared_ptr<Tensor> Tensor::Clone() const
{
    HIAI_EXPECT_NOT_NULL_R(tensorDef_, nullptr);

    auto newDef = hiai::ProtoFactory::Instance()->CreateTensorDef();
    HIAI_EXPECT_NOT_NULL_R(newDef, nullptr);

    newDef->CopyFrom(tensorDef_);

    auto cloneOp = hiai::make_shared_nothrow<Tensor>(newDef, true);
    if (cloneOp == nullptr) {
        hiai::ProtoFactory::Instance()->DestroyTensorDef(newDef);
        return nullptr;
    }
    return cloneOp;
}
} // namespace ge
