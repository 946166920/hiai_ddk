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
#ifndef FRAMEWORK_GRAPH_CORE_OP_CONSTHOLDER_OP_DESC_H
#define FRAMEWORK_GRAPH_CORE_OP_CONSTHOLDER_OP_DESC_H

#include <string>

#include "graph/graph_api_export.h"
#include "graph/attributes_holder.h"

#include "framework/graph/core/op/op_desc.h"
#include "framework/graph/utils/range_vistor.h"

namespace ge {
class ConstHolderOpDesc;
using ConstHolderOpDescPtr = std::shared_ptr<ConstHolderOpDesc>;

class GRAPH_API_EXPORT ConstHolderOpDesc : public OpDesc {
public:
    ConstHolderOpDesc(const std::string& name, const std::string& type);
    ConstHolderOpDesc(hiai::IOpDef* opDef, bool isOwner);
    ~ConstHolderOpDesc() override;
    static bool IsConstHolderOp(const ge::OpDesc& opDesc);
    const TensorDesc& GetOutputDesc(uint32_t index) const override;
    ConstTensorDescPtr GetOutputDescPtr(uint32_t index) const override;
    TensorDescPtr MutableOutputDesc(uint32_t index) const override;
    OpDescPtr Clone() override;
    OpDesc::Vistor<TensorDesc> GetAllOutputsDesc() const override;
    const vector<TensorDescPtr>& GetOutputsDesc() const override;
    OpDesc::Vistor<TensorDescPtr> GetAllOutputsDescPtr() const override;

    size_t GetOutputsSize() const override;

    void SetHoldedOpDesc(OpDesc* opDesc);
    const OpDesc* GetHoldedOpDesc() const;

private:
    ge::OpDesc* opDesc_;
};
}
#endif // FRAMEWORK_GRAPH_CORE_OP_CONSTHOLDER_OP_DESC_H