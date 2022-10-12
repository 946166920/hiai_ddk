/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: operator
 */

#ifndef GE_OPERATOR_H
#define GE_OPERATOR_H

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "graph/attr_value.h"
#include "graph/graph_api_export.h"

namespace ge {
constexpr int CURRENT_OP_VERSION = 0;

class OperatorImpl;
using OperatorImplPtr = std::shared_ptr<OperatorImpl>;

class Graph;
using GraphBuilderFn = std::function<void(ge::Graph&)>;

using OpAnchor = std::pair<std::weak_ptr<OperatorImpl>, int32_t>;

class GRAPH_API_EXPORT Operator {
public:
    Operator() = default;
    explicit Operator(const string& type);
    Operator(const string& name, const string& type, int version = CURRENT_OP_VERSION);
    explicit Operator(OperatorImplPtr&& opImpl);
    virtual ~Operator() = default;

    const OperatorImplPtr GetImpl() const;
    OpAnchor GetOutput(const uint32_t outIndex) const;

    Operator& SetInput(const string& name, const Operator& outOp);
    Operator& SetInput(const string& name, const Operator& outOp, const string& outName);
    Operator& SetInput(int32_t index, const Operator& outOp, int32_t outIndex);
    Operator& SetInput(const string& name, const ge::OpAnchor& opAnchor);

    Operator& SetDynamicInput(const string& name, int32_t index, const Operator& outOp);
    Operator& SetDynamicInput(const string& name, int32_t index, const Operator& outOp, const std::string& outName);
    Operator& SetDynamicInput(const string& name, int32_t index, const ge::OpAnchor& opAnchor);

    Operator& SetAttr(const string& name, AttrValue&& attrValue);
    Operator& SetGraphBuilder(const string& name, const GraphBuilderFn& graphBuilderFn);

    GraphErrCodeStatus UpdateInputDesc(const string& name, const TensorDesc& tensorDesc);
    string GetName() const;

protected:
    void InputRegister(const string& name);
    void OptionalInputRegister(const string& name);
    void DynamicInputRegister(const string& name, const unsigned int num);

    void OutputRegister(const string& name);
    void DynamicOutputRegister(const string& name, const unsigned int num);

    void AttrRegister(const string& name, AttrValue::ValueType type);
    void OptionalAttrRegister(const string& name, AttrValue&& attrValue);

private:
    std::vector<bool> GetOpIsInputConst() const;
    void SetOpIsInputConst(bool inputConst, uint32_t index);

private:
    OperatorImplPtr impl_ {nullptr};
};
} // namespace ge
#endif // GE_OPERATOR_H
