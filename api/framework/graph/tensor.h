/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: tensor
 */

#ifndef GE_TENSOR_H
#define GE_TENSOR_H

#include "graph/buffer.h"
#include "graph/debug/ge_error_codes.h"
#include "graph/attributes_holder.h"
#include "graph/shape.h"
#include "graph/types.h"
#include <atomic>
#include <memory>
#include <vector>

namespace hiai {
class ITensorDescDef;
class ITensorDef;
}

namespace ge {
class Buffer;

class GRAPH_API_EXPORT TensorDesc : public AttrHolder {
    friend class TensorUtils;

public:
    TensorDesc();
    TensorDesc(hiai::ITensorDescDef* tensorDescDef, bool isOwner);
    ~TensorDesc() override;

    explicit TensorDesc(Shape shape, Format format = FORMAT_NCHW, DataType dt = DT_FLOAT);
    TensorDesc(Shape shape, DataType dt);

    TensorDesc(const TensorDesc& desc);
    TensorDesc& operator=(const TensorDesc& desc);

    GraphErrCodeStatus Save(Buffer& buffer) const;
    GraphErrCodeStatus Load(const uint8_t* data, size_t len);

    const string& GetName() const;
    void SetName(const string& name);

    const Shape& GetShape() const;
    Shape& MutableShape();
    void SetShape(const Shape& shape);

    Format GetFormat() const;
    void SetFormat(Format format);

    DataType GetDataType() const;
    void SetDataType(DataType dataType);

    bool SerializeTo(hiai::ITensorDescDef* to) const;

protected:
    const hiai::IAttrMapDef* GetAttrMapDef() const override;
    hiai::IAttrMapDef* MutableAttrMapDef() override;

private:
    hiai::ITensorDescDef* tensorDescDef_;
    bool isOwner_;

    Shape& ShapeReference() const;
    mutable Shape shape_;

    friend class Tensor;
    void RefTo(const TensorDesc& desc);
};

class GRAPH_API_EXPORT Tensor {
public:
    Tensor();
    Tensor(hiai::ITensorDef* tensorDef, bool isOwner);
    explicit Tensor(const TensorDesc& tensorDesc);
    Tensor(const TensorDesc& tensorDesc, const Buffer& data);
    Tensor(const TensorDesc& tensorDesc, const uint8_t* data, size_t size);

    ~Tensor();

    Tensor(const Tensor& other) = delete;
    Tensor& operator=(const Tensor& other) = delete;

    std::shared_ptr<Tensor> Clone() const;

    const TensorDesc& GetTensorDesc() const;
    TensorDesc& MutableTensorDesc();
    GraphErrCodeStatus SetTensorDesc(const TensorDesc& tensorDesc);

    const Buffer& GetData() const;
    Buffer& MutableData();
    GraphErrCodeStatus SetData(const Buffer& data);
    GraphErrCodeStatus SetData(const uint8_t* data, size_t size);

    bool SerializeTo(hiai::ITensorDef* to) const;

private:
    hiai::ITensorDef* tensorDef_;
    bool isOwner_;

    TensorDesc& DescReference() const;
    mutable TensorDesc desc_;

    Buffer& BufferReference() const;
    mutable Buffer buffer_;
};
} // namespace ge

#endif // GE_TENSOR_H
