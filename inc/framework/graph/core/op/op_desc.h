/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: op_desc
 */
#ifndef GE_OPERATOR_DESC_H
#define GE_OPERATOR_DESC_H

#include <map>
#include <unordered_set>
#include <memory>
#include <functional>
#include "framework/graph/utils/range_vistor.h"
#include "graph/attr_value.h"
#include "graph/detail/attributes_holder.h"
#include "graph/debug/ge_error_codes.h"
#include "framework/graph/debug/ge_util.h"
/*lint -e553*/

#define DYNAMIN_INPUT_NAME(name, index) (((name)) + std::to_string((index)))
#define DYNAMIN_OUTPUT_NAME(name, index) (((name)) + std::to_string((index)))

namespace hiai {
class IOpDef;
}

namespace ge {
using std::map;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

class Operator;

class TensorDesc;
class Buffer;

using TensorDescPtr = shared_ptr<TensorDesc>;
using ConstTensorDescPtr = shared_ptr<const TensorDesc>;

class OpDesc;

using OpDescPtr = shared_ptr<OpDesc>;
using ConstOpDescPtr = shared_ptr<const OpDesc>;

class AttrValue;

using ConstOpDesc = const OpDesc;

class GRAPH_API_EXPORT OpDesc : public std::enable_shared_from_this<OpDesc>, public AttrHolder {
public:
    template<class T>
    using Vistor = RangeVistor<T, shared_ptr<ConstOpDesc>>;

    friend class OpDescUtils;

public:
    OpDesc();

    OpDesc(const string& name, const string& type);

    OpDesc(hiai::IOpDef* opDef, bool isOwner);

    ~OpDesc() override;

    OpDesc(const OpDesc& other) = delete;

    OpDesc& operator=(const OpDesc& other) = delete;

    OpDescPtr Clone();

    GraphErrCodeStatus Save(Buffer& buffer) const;

    GraphErrCodeStatus Load(const uint8_t* data, size_t len);

    GraphErrCodeStatus SerializeTo(hiai::IOpDef* dstDef) const;

    GraphErrCodeStatus UnSerialize();

    const string& GetName() const;

    void SetName(const string& name);

    const string& GetType() const;

    void SetType(const string& type);

    bool ClearInputDesc(uint32_t index);

    GraphErrCodeStatus AddInputDesc(const TensorDesc& inputDesc);

    GraphErrCodeStatus AddInputDesc(const string& name, const TensorDesc& inputDesc);

    GraphErrCodeStatus AddOptionalInputDesc(const string& name, const TensorDesc& inputDesc);

    GraphErrCodeStatus UpdateInputDesc(uint32_t index, const TensorDesc& tensorDesc);

    GraphErrCodeStatus UpdateInputDesc(const string& name, const TensorDesc& tensorDesc);

    bool InputIsSet(uint32_t index) const;

    bool InputIsSet(const string& name) const;

    void GetUnSetInputIndexs(vector<uint32_t>& indexList) const;

    const TensorDesc& GetInputDesc(uint32_t index) const;

    const TensorDesc& GetInputDesc(const string& name) const;

    Vistor<string> GetAllInputNames() const;

    TensorDescPtr MutableInputDesc(uint32_t index) const;

    Vistor<TensorDesc> GetAllInputsDesc() const;

    int32_t GetInputsDescSize() const;

    const vector<TensorDescPtr>& GetInputsDesc() const;

    GraphErrCodeStatus AddInputDesc(const vector<TensorDescPtr>& descVec);

    Vistor<TensorDescPtr> GetAllInputsDescPtr() const;

    size_t GetInputsSize() const;

    GraphErrCodeStatus AddOutputDesc(const TensorDesc& outputDesc);

    GraphErrCodeStatus AddOutputDesc(const string& name, const TensorDesc& outputDesc);

    GraphErrCodeStatus UpdateOutputDesc(uint32_t index, const TensorDesc& tensorDesc);

    GraphErrCodeStatus UpdateOutputDesc(const string& name, const TensorDesc& tensorDesc);

    const TensorDesc& GetOutputDesc(uint32_t index) const;

    const TensorDesc& GetOutputDesc(const string& name) const;

    TensorDescPtr MutableOutputDesc(uint32_t index) const;

    Vistor<TensorDesc> GetAllOutputsDesc() const;

    Vistor<TensorDescPtr> GetAllOutputsDescPtr() const;

    size_t GetOutputsSize() const;

    const vector<TensorDescPtr>& GetOutputsDesc() const;

    GraphErrCodeStatus AddOutputDesc(const vector<TensorDescPtr>& descVec);

    ConstTensorDescPtr GetOutputDescPtr(uint32_t index) const;

    ConstTensorDescPtr GetInputDescPtr(uint32_t index) const;

    GraphErrCodeStatus AddDynamicInputDesc(const string& name, const unsigned int num);

    GraphErrCodeStatus AddDynamicOutputDesc(const string& name, const unsigned int num);

    bool IsOptionalInput(const string& name) const;

    bool IsOptionalInput(uint32_t index) const;

    using AttrHolder::GetAllAttrs;

    void SetId(int64_t id);

    int64_t GetId() const;

    void SetStreamId(int64_t streamId);

    int64_t GetStreamId() const;

    void SetInputName(const vector<string>& inputName);

    vector<string> GetInputName() const;

    void SetSrcName(const vector<string>& srcName);

    vector<string> GetSrcName() const;

    void SetSrcIndex(const vector<int64_t>& srcIndex);

    vector<int64_t> GetSrcIndex() const;

    void SetInputOffset(const vector<int64_t>& input);

    size_t GetInputOffsetSize() const;

    void ClearInputOffset();

    void AddInputOffset(int64_t value);

    int64_t GetInputOffset(int32_t index) const;

    void SetInputOffset(int32_t index, int64_t value);

    vector<int64_t> GetInputOffset() const;

    void SetOutputOffset(const vector<int64_t>& input);

    size_t GetOutputOffsetSize() const;

    void ClearOutputOffset();

    void AddOutputOffset(int64_t value);

    int64_t GetOutputOffset(int32_t index) const;

    void SetOutputOffset(int32_t index, int64_t value);

    vector<int64_t> GetOutputOffset() const;

    void SetDstName(const vector<string>& dstName);

    vector<string> GetDstName() const;

    void SetDstIndex(const vector<int64_t>& dstIndex);

    vector<int64_t> GetDstIndex() const;

    void SetWorkspace(const vector<int64_t>& workspace);

    size_t GetWorkspaceSize() const;

    void ClearWorkspace();

    void AddWorkspace(int64_t value);

    int64_t GetWorkspace(int32_t index) const;

    void SetWorkspace(int32_t index, int64_t value);

    vector<int64_t> GetWorkspace() const;

    void SetWorkspaceBytes(const vector<uint32_t>& workspaceBytes);

    vector<uint32_t> GetWorkspaceBytes() const;

    size_t GetWorkspaceBytesSize() const;

    void ClearWorkspaceBytes();

    void AddWorkspaceBytes(uint32_t value);

    void SetWorkspaceBytes(int32_t index, uint32_t value);

    uint32_t GetWorkspaceBytes(int32_t index) const;

    void SetIsInputConst(const vector<bool>& isInputConst);

    size_t GetIsInputConstSize() const;

    bool IsInputConst(int32_t index) const;

    void AddIsInputConst(bool value);

    vector<bool> GetIsInputConst() const;

    const std::string& GetInputNameByIndex(uint32_t index) const;

    const std::string& GetOutputNameByIndex(uint32_t index) const;

    int GetInputIndexByName(const string& name) const;

    int GetOutputIndexByName(const string& name) const;

    uint64_t opDesc_workspace_data_addr_ = 0;

    uint64_t opDesc_workspace_size_ = 0;

    void SetRunCL(string clname);

    string GetRunCL() const;

    void ClearAllInputsDesc();

    void ClearAllOutputsDesc();

    GraphErrCodeStatus AddRequiredAttr(const std::string& name, ge::AttrValue::ValueType type);

    bool IsDimValid() const;

    std::vector<uint32_t> GetOptionalInputsIdx() const;

protected:
    const hiai::IAttrMapDef* GetAttrMapDef() const override;

    hiai::IAttrMapDef* MutableAttrMapDef() override;

private:
    hiai::IOpDef* opDef_;

    bool isOwner_;

    vector<TensorDescPtr> inputsDesc_;

    vector<TensorDescPtr> outputsDesc_;

private:
    map<string, uint32_t> inputNameIdx_ {};

    std::unordered_set<string> optionalInputNames_ {};

    map<string, uint32_t> outputNameIdx_ {};

    string mBestRunCl_;

    std::map<std::string, ge::AttrValue::ValueType> requiredAttrs_;
};
} // namespace ge
#endif // GE_OPERATOR_DESC_H
