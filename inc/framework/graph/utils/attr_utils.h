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

#ifndef GE_ATTR_VALUE_UTILS_H
#define GE_ATTR_VALUE_UTILS_H

#include "graph/attr_value.h"
#include "graph/attributes_holder.h"

namespace ge {
class OpDesc;

using OpDescPtr = std::shared_ptr<OpDesc>;
using ConstOpDescPtr = std::shared_ptr<const OpDesc>;

class AttrUtils {
public:
    class ConstAttrHolderAdapter;

    class AttrHolderAdapter;

    GRAPH_API_EXPORT static OpDescPtr CloneOpDesc(const OpDescPtr& orgOpDesc);

    GRAPH_API_EXPORT static bool GetInt(ConstAttrHolderAdapter&& obj, const string& name, int64_t& value);

    GRAPH_API_EXPORT static bool GetInt(ConstAttrHolderAdapter&& obj, const string& name, int32_t& value);

    GRAPH_API_EXPORT static bool GetInt(ConstAttrHolderAdapter&& obj, const string& name, uint32_t& value);

    GRAPH_API_EXPORT static bool GetListInt(ConstAttrHolderAdapter&& obj, const string& name,
        std::vector<int64_t>& value);

    GRAPH_API_EXPORT static bool GetListInt(ConstAttrHolderAdapter&& obj, const string& name,
        std::vector<int32_t>& value);

    GRAPH_API_EXPORT static bool GetListInt(ConstAttrHolderAdapter&& obj, const string& name,
        std::vector<uint32_t>& value);

    GRAPH_API_EXPORT static bool GetTensor(ConstAttrHolderAdapter&& obj, const string& name, TensorPtr& value);

    GRAPH_API_EXPORT static bool SetTensor(AttrHolderAdapter&& obj, const string& name, const TensorPtr& value);

    GRAPH_API_EXPORT static bool MutableTensor(AttrHolderAdapter&& obj, const string& name, TensorPtr& value);

    GRAPH_API_EXPORT static bool SetListInt(AttrHolderAdapter&& obj, const string& name,
        const std::vector<int64_t>& value);

    GRAPH_API_EXPORT static bool SetListInt(AttrHolderAdapter&& obj, const string& name,
        const std::vector<uint32_t>& value);

    GRAPH_API_EXPORT static bool SetListInt(AttrHolderAdapter&& obj, const string& name,
        const std::vector<int32_t>& value);

    GRAPH_API_EXPORT static bool SetGraph(AttrHolderAdapter&& obj, const string& name, const ComputeGraphPtr& value);

    GRAPH_API_EXPORT static bool GetGraph(ConstAttrHolderAdapter&& obj, const string& name, ComputeGraphPtr& value);

    static bool SetInt(AttrHolderAdapter&& obj, const string& name, const int64_t& value);

    static bool SetFloat(AttrHolderAdapter&& obj, const string& name, const float& value);

    static bool SetListFloat(AttrHolderAdapter&& obj, const string& name, const std::vector<float>& value);

    static bool SetBool(AttrHolderAdapter&& obj, const string& name, const bool& value);

    static bool SetListBool(AttrHolderAdapter&& obj, const string& name, const std::vector<bool>& value);

    static bool SetStr(AttrHolderAdapter&& obj, const string& name, const string& value);

    static bool SetListStr(AttrHolderAdapter&& obj, const string& name, const std::vector<string>& value);

    static bool SetTensorDesc(AttrHolderAdapter&& obj, const string& name, const TensorDesc& value);

    static bool SetListTensor(AttrHolderAdapter&& obj, const string& name, const std::vector<TensorPtr>& value);

    static bool SetBytes(AttrHolderAdapter&& obj, const string& name, const AttrValue::BYTES& value);

    static bool SetNamedAttrs(AttrHolderAdapter&& obj, const string& name, const AttrValue::NamedAttrs& value);

    static bool GetFloat(ConstAttrHolderAdapter&& obj, const string& name, float& value);

    static bool GetListFloat(ConstAttrHolderAdapter&& obj, const string& name, std::vector<float>& value);

    static bool GetBool(ConstAttrHolderAdapter&& obj, const string& name, bool& value);

    static bool GetListBool(ConstAttrHolderAdapter&& obj, const string& name, std::vector<bool>& value);

    static bool GetStr(ConstAttrHolderAdapter&& obj, const string& name, string& value);

    static bool GetListStr(ConstAttrHolderAdapter&& obj, const string& name, std::vector<string>& value);

    static bool GetTensorDesc(ConstAttrHolderAdapter&& obj, const string& name, TensorDesc& value);

    static bool GetListTensor(ConstAttrHolderAdapter&& obj, const string& name, std::vector<TensorPtr>& value);

    static bool MutableListTensor(AttrHolderAdapter&& obj, const string& name, std::vector<TensorPtr>& value);

    static bool GetBytes(ConstAttrHolderAdapter&& obj, const string& name, AttrValue::BYTES& value);

    static bool GetNamedAttrs(ConstAttrHolderAdapter&& obj, const string& name, AttrValue::NamedAttrs& value);

    class AttrHolderAdapter {
    public:
        AttrHolderAdapter(AttrHolder* obj) : obj_(obj)
        {
        }

        ~AttrHolderAdapter()
        {
            obj_ = nullptr;
        }

        template <class T>
        AttrHolderAdapter(std::shared_ptr<T> obj) : obj_(obj.get())
        {
        }

        AttrHolderAdapter(AttrHolder& obj) : obj_(&obj)
        {
        }

        operator bool() const
        {
            return obj_ != nullptr;
        }

        AttrHolder* operator->()
        {
            return obj_;
        }

        AttrHolder* get()
        {
            return obj_;
        }

    public:
        AttrHolder* obj_;
    };

    class ConstAttrHolderAdapter {
    public:
        ConstAttrHolderAdapter(const AttrHolder* obj) : obj_(obj)
        {
        }

        ~ConstAttrHolderAdapter()
        {
            obj_ = nullptr;
        }

        template <class T>
        ConstAttrHolderAdapter(const std::shared_ptr<T> obj) : obj_(obj.get())
        {
        }

        ConstAttrHolderAdapter(const AttrHolder& obj) : obj_(&obj)
        {
        }

        operator bool() const
        {
            return obj_ != nullptr;
        }

        const AttrHolder* operator->() const
        {
            return obj_;
        }

        const AttrHolder* get() const
        {
            return obj_;
        }

    private:
        const AttrHolder* obj_;
    };
};
} //  namespace ge

#endif // GE_ATTR_VALUE_UTILS_H
