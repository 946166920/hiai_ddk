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
#ifndef HIAI_OPERATOR_IMPL_H
#define HIAI_OPERATOR_IMPL_H
#include <string>
#include <map>
#include <memory>
#include <functional>

#include "framework/graph/core/op/op_desc.h"
#include "graph/attr_value.h"

namespace ge {
class OperatorImpl;
using OperatorImplPtr = std::shared_ptr<OperatorImpl>;
using OpAnchor = std::pair<std::weak_ptr<OperatorImpl>, int32_t>;

class Graph;
using GraphBuilderFn = std::function<void(Graph&)>;

class OperatorImpl : public std::enable_shared_from_this<OperatorImpl> {
public:
    OperatorImpl(const std::string& name, const std::string& type);

    explicit OperatorImpl(OpDescPtr opDesc);

    ~OperatorImpl() = default;

    const OpDescPtr GetOpDesc() const;

    void SetInput(const std::string& name, const OperatorImplPtr& outOp);

    void SetInput(const std::string& name, const OperatorImplPtr& outOp, const std::string& outName);

    void SetInput(int32_t index, const OperatorImplPtr& outOp, int32_t outIndex);

    GraphErrCodeStatus UpdateInputDesc(const string& name, const TensorDesc& tensorDesc);

    GraphErrCodeStatus SetAttr(const std::string& name, AttrValue&& attrValue);

    GraphErrCodeStatus GetAttr(const string& name, AttrValue& attrValue) const;

    std::string GetName() const;

    GraphErrCodeStatus SetGraphBuilder(const std::string& name, const GraphBuilderFn& v);
    GraphBuilderFn GetGraphBuilder(const std::string& name) const;

    const std::map<std::string, OpAnchor>& GetAllInputLinks() const;
    const std::map<std::string, std::vector<OpAnchor>>& GetAllOutputLinks() const;

    void InputRegister(const std::string& name);
    void OptionalInputRegister(const std::string& name);
    void DynamicInputRegister(const std::string& name, uint32_t num);

    void OutputRegister(const std::string& name);
    void DynamicOutputRegister(const std::string& name, uint32_t num);

    void AttrRegister(const std::string& name, AttrValue::ValueType type);
    void OptionalAttrRegister(const std::string& name, AttrValue&& attrValue);

private:
    void LinkToOutputOp(const std::string& outName, OpAnchor& peerOp);

private:
    OpDescPtr opDesc_ {nullptr};
    std::map<std::string, std::vector<OpAnchor>> outputLinks_ {};
    std::map<std::string, OpAnchor> inputLink_ {};
    std::map<std::string, GraphBuilderFn> subGraphBuilderFn_ {};
};
} // namespace ge

#endif