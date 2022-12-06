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

#ifndef GE_ATTR_VALUE_H
#define GE_ATTR_VALUE_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "graph/buffer.h"
#include "graph/attributes_holder.h"
#include "graph/graph_api_export.h"
#include "graph/tensor.h"

namespace hiai {
class IAttrDef;
class INamedAttrDef;
}

namespace ge {
class Tensor;

using TensorPtr = std::shared_ptr<Tensor>;
using ConstTensorPtr = std::shared_ptr<const Tensor>;

class ComputeGraph;

using ComputeGraphPtr = std::shared_ptr<ComputeGraph>;
using ConstComputeGraphPtr = std::shared_ptr<const ComputeGraph>;

class TensorDesc;

class GRAPH_API_EXPORT AttrValue {
public:
    class NamedAttrs : public AttrHolder {
    public:
        NamedAttrs();
        NamedAttrs(hiai::INamedAttrDef* namedDef, bool isOwner);

        ~NamedAttrs() override;

        NamedAttrs(const NamedAttrs& other);
        NamedAttrs& operator=(const NamedAttrs& other);

        bool SerializeTo(hiai::INamedAttrDef* to) const;

        void SetName(const std::string& name);
        const std::string& GetName() const;

        const AttrValue& GetItem(const string& key) const;

    protected:
        const hiai::IAttrMapDef* GetAttrMapDef() const override;
        hiai::IAttrMapDef* MutableAttrMapDef() override;

    private:
        hiai::INamedAttrDef* namedDef_;
        bool isOwner_;
        mutable std::map<std::string, AttrValue> attrMap_;
    };

public:
    using INT = int64_t;
    using FLOAT = float;
    using BOOL = bool;
    using STR = std::string;
    using TENSOR = TensorPtr;
    using TENSOR_DESC = TensorDesc;
    using GRAPH = ComputeGraphPtr;
    using BYTES = Buffer;
    using NAMED_ATTRS = NamedAttrs;
    using DATA_TYPE = int64_t;
    using LIST_INT = std::vector<INT>;
    using LIST_FLOAT = std::vector<FLOAT>;
    using LIST_BOOL = std::vector<BOOL>;
    using LIST_STR = std::vector<STR>;
    using LIST_TENSOR = std::vector<TENSOR>;
    using LIST_TENSOR_DESC = std::vector<TENSOR_DESC>;
    using LIST_GRAPH = std::vector<GRAPH>;
    using LIST_BYTES = std::vector<BYTES>;
    using LIST_NAMED_ATTRS = std::vector<NAMED_ATTRS>;

    enum ValueType {
        VT_NONE = 0,
        VT_STRING,
        VT_FLOAT,
        VT_BOOL,
        VT_INT,
        VT_TENSOR_DESC,
        VT_TENSOR,
        VT_BYTES,
        VT_GRAPH,
        VT_NAMED_ATTRS,

        VT_LIST_BASE = 1000,
        VT_LIST_STRING = VT_LIST_BASE + VT_STRING,
        VT_LIST_FLOAT = VT_LIST_BASE + VT_FLOAT,
        VT_LIST_BOOL = VT_LIST_BASE + VT_BOOL,
        VT_LIST_INT = VT_LIST_BASE + VT_INT,
        VT_LIST_TENSOR_DESC = VT_LIST_BASE + VT_TENSOR_DESC,
        VT_LIST_TENSOR = VT_LIST_BASE + VT_TENSOR,
        VT_LIST_BYTES = VT_LIST_BASE + VT_BYTES,
        VT_LIST_GRAPH = VT_LIST_BASE + VT_GRAPH,
        VT_LIST_NAMED_ATTRS = VT_LIST_BASE + VT_NAMED_ATTRS,
    };

public:
    AttrValue();
    AttrValue(hiai::IAttrDef* attrDef, bool isOwner);

    ~AttrValue();

    AttrValue(const AttrValue& other);
    AttrValue& operator=(const AttrValue& other);

    bool SerializeTo(hiai::IAttrDef* to) const;

    bool SetInt(int64_t val);
    int64_t GetInt() const;
    static AttrValue CreateFrom(int64_t val);

    bool SetFloat(float val);
    float GetFloat() const;
    static AttrValue CreateFrom(float val);

    bool SetBool(bool val);
    bool GetBool() const;
    static AttrValue CreateFrom(bool val);

    bool SetString(const std::string& val);
    const std::string& GetString() const;
    static AttrValue CreateFrom(const std::string& val);

    bool SetTensor(const TensorPtr& val);
    const TensorPtr GetTensor() const;
    static AttrValue CreateFrom(const TensorPtr& val);

    bool SetNamedAttrs(const NamedAttrs& val);
    NamedAttrs GetNamedAttrs() const;
    static AttrValue CreateFrom(const NamedAttrs& val);

    bool SetGraph(const ComputeGraphPtr& val);
    const ComputeGraphPtr GetGraph() const;

    bool SetBuffer(const Buffer& val);
    Buffer GetBuffer() const;
    static AttrValue CreateFrom(const Buffer& val);

    bool SetTensorDesc(const TensorDesc& val);
    TensorDesc GetTensorDesc() const;
    static AttrValue CreateFrom(const TensorDesc& val);

    bool SetIntList(const std::vector<int64_t>& val);
    const std::vector<int64_t> GetIntList() const;
    static AttrValue CreateFrom(const std::vector<int64_t>& val);

    bool SetFloatList(const std::vector<float>& val);
    const std::vector<float> GetFloatList() const;
    static AttrValue CreateFrom(const std::vector<float>& val);

    bool SetBoolList(const std::vector<bool>& val);
    const std::vector<bool> GetBoolList() const;
    static AttrValue CreateFrom(const std::vector<bool>& val);

    bool SetStringList(const std::vector<std::string>& val);
    const std::vector<std::string> GetStringList() const;
    static AttrValue CreateFrom(const std::vector<std::string>& val);

    bool SetTensorList(const std::vector<TensorPtr>& val);
    const std::vector<TensorPtr> GetTensorList() const;
    static AttrValue CreateFrom(const std::vector<TensorPtr>& val);

    bool SetTensorDescList(const std::vector<TensorDesc>& val);
    const std::vector<TensorDesc> GetTensorDescList() const;
    static AttrValue CreateFrom(const std::vector<TensorDesc>& val);

    ValueType GetValueType() const;
    bool IsEmpty() const;

private:
    hiai::IAttrDef* attrDef_;
    bool isOwner_;

    ValueType type_ {VT_NONE};
};

} // namespace ge

#endif // GE_ATTR_VALUE_H
