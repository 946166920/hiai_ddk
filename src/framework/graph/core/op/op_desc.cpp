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

#include "framework/graph/core/op/op_desc.h"

#include <climits>

// inc/common
#include "infra/base/securestl.h"

// api/framework
#include "graph/tensor.h"
#include "graph/attr_value.h"
#include "graph/operator.h"
#include "graph/buffer.h"

// framework/inc
#include "infra/base/assertion.h"

#include "graph/persistance/interface/op_def.h"
#include "graph/persistance/interface/tensor_desc_def.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/proxy/proto_factory.h"

using namespace std;
namespace ge {
const static string STR_EMPTY = "";

OpDesc::OpDesc() : OpDesc(hiai::ProtoFactory::Instance()->CreateOpDef(), true)
{
}

OpDesc::OpDesc(hiai::IOpDef* opDef, bool isOwner) : opDef_(opDef), isOwner_(isOwner)
{
}

OpDesc::OpDesc(const string& name, const string& type) : OpDesc()
{
    if (opDef_ != nullptr) {
        opDef_->set_name(name);
        opDef_->set_type(type);
    }
}

OpDesc::~OpDesc()
{
    if (isOwner_ && opDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(opDef_);
    }
    opDef_ = nullptr;
}

const hiai::IAttrMapDef* OpDesc::GetAttrMapDef() const
{
    return opDef_ != nullptr ? opDef_->attr() : nullptr;
}

hiai::IAttrMapDef* OpDesc::MutableAttrMapDef()
{
    return opDef_ != nullptr ? opDef_->mutable_attr() : nullptr;
}

GraphErrCodeStatus OpDesc::UnSerialize()
{
    HIAI_EXPECT_NOT_NULL(opDef_);

    inputsDesc_.clear();
    for (size_t i = 0; i < opDef_->input_desc_size(); i++) {
        auto desc = hiai::make_shared_nothrow<TensorDesc>(opDef_->mutable_input_desc(i), false);
        HIAI_EXPECT_NOT_NULL(desc);
        inputsDesc_.push_back(desc);
    }

    outputsDesc_.clear();
    for (size_t i = 0; i < opDef_->output_desc_size(); i++) {
        auto desc = hiai::make_shared_nothrow<TensorDesc>(opDef_->mutable_output_desc(i), false);
        HIAI_EXPECT_NOT_NULL(desc);
        outputsDesc_.push_back(desc);
    }
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::Save(Buffer& buffer) const
{
    auto opDef = hiai::ProtoFactory::Instance()->CreateOpDef();
    HIAI_EXPECT_NOT_NULL(opDef);

    if (SerializeTo(opDef) != GRAPH_SUCCESS) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(opDef);
        return GRAPH_FAILED;
    }

    buffer.Resize(opDef->GetOpDefSize());

    if (!opDef->SaveTo(buffer.MutableData(), buffer.GetSize())) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(opDef);
        return GRAPH_FAILED;
    }

    hiai::ProtoFactory::Instance()->DestroyOpDef(opDef);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::Load(const uint8_t* data, size_t len)
{
    HIAI_EXPECT_NOT_NULL(opDef_);

    HIAI_EXPECT_TRUE(opDef_->LoadFrom(data, len));

    HIAI_EXPECT_EXEC(UnSerialize());

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::SerializeTo(hiai::IOpDef* dstDef) const
{
    HIAI_EXPECT_NOT_NULL(opDef_);

    dstDef->CopyFrom(opDef_);

    dstDef->clear_input_desc();
    for (auto desc : inputsDesc_) {
        if (desc == nullptr || desc->GetFormat() == FORMAT_RESERVED) {
            continue;
        }
        HIAI_EXPECT_TRUE(desc->SerializeTo(dstDef->add_input_desc()));
    }
    dstDef->clear_output_desc();
    for (auto desc : outputsDesc_) {
        if (desc == nullptr || desc->GetFormat() == FORMAT_RESERVED) {
            continue;
        }
        HIAI_EXPECT_TRUE(desc->SerializeTo(dstDef->add_output_desc()));
    }
    return GRAPH_SUCCESS;
}

OpDescPtr OpDesc::Clone()
{
    auto newOpDef = hiai::ProtoFactory::Instance()->CreateOpDef();
    HIAI_EXPECT_NOT_NULL_R(newOpDef, nullptr);

    if (SerializeTo(newOpDef) != GRAPH_SUCCESS) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(newOpDef);
        return nullptr;
    }

    auto cloneOp = hiai::make_shared_nothrow<OpDesc>(newOpDef, true);
    if (cloneOp == nullptr) {
        hiai::ProtoFactory::Instance()->DestroyOpDef(newOpDef);
        return nullptr;
    }

    if (cloneOp->UnSerialize() != GRAPH_SUCCESS) {
        cloneOp.reset();
    }

    return cloneOp;
}

const string& OpDesc::GetName() const
{
    return opDef_ != nullptr ? opDef_->name() : STR_EMPTY;
}

void OpDesc::SetName(const std::string& name)
{
    if (opDef_ != nullptr) {
        opDef_->set_name(name);
    }
}

const string& OpDesc::GetType() const
{
    return opDef_ != nullptr ? opDef_->type() : STR_EMPTY;
}

void OpDesc::SetType(const string& type)
{
    if (opDef_ != nullptr) {
        opDef_->set_type(type);
    }
}

bool OpDesc::ClearInputDesc(uint32_t index)
{
    HIAI_EXPECT_TRUE_R(index < inputsDesc_.size(), false);

    inputsDesc_.erase(inputsDesc_.cbegin() + index);

    return true;
}

GraphErrCodeStatus OpDesc::AddInputDesc(const ge::TensorDesc& inputDesc)
{
    auto addDesc = hiai::make_shared_nothrow<TensorDesc>(inputDesc);
    HIAI_EXPECT_NOT_NULL(addDesc);

    inputsDesc_.emplace_back(addDesc);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::AddInputDesc(const string& name, const ge::TensorDesc& inputDesc)
{
    HIAI_EXPECT_TRUE(inputNameIdx_.find(name) == inputNameIdx_.end());

    uint32_t index = static_cast<uint32_t>(GetInputsDescSize());

    HIAI_EXPECT_EXEC(AddInputDesc(inputDesc));

    inputNameIdx_.insert(make_pair(name, index));
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::AddOptionalInputDesc(const string& name, const ge::TensorDesc& inputDesc)
{
    HIAI_EXPECT_EXEC(AddInputDesc(name, inputDesc));

    optionalInputNames_.insert(name);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::UpdateInputDesc(uint32_t index, const ge::TensorDesc& tensorDesc)
{
    HIAI_EXPECT_TRUE(index < inputsDesc_.size());

    *inputsDesc_[index] = tensorDesc;
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::UpdateInputDesc(const string& name, const ge::TensorDesc& tensorDesc)
{
    map<string, uint32_t>::const_iterator it = inputNameIdx_.find(name);
    HIAI_EXPECT_TRUE(it != inputNameIdx_.end());

    return UpdateInputDesc(it->second, tensorDesc);
}

bool OpDesc::InputIsSet(uint32_t index) const
{
    if (index >= inputsDesc_.size()) {
        return false;
    }

    auto format = inputsDesc_[index]->GetFormat();
    return format != FORMAT_RESERVED;
}

bool OpDesc::InputIsSet(const string& name) const
{
    auto it = inputNameIdx_.find(name);
    if (it == inputNameIdx_.end()) {
        return false;
    }
    return InputIsSet(it->second);
}

void OpDesc::GetUnSetInputIndexs(vector<uint32_t>& indexList) const
{
    indexList.clear();
    for (uint32_t i = 0; i < inputsDesc_.size(); i++) {
        auto format = inputsDesc_[i]->GetFormat();
        if (format == FORMAT_RESERVED) {
            indexList.push_back(i);
        }
    }
}

const TensorDesc& OpDesc::GetInputDesc(uint32_t index) const
{
    static TensorDesc tensorDesc;
    HIAI_EXPECT_TRUE_R(index < inputsDesc_.size(), tensorDesc);

    return *inputsDesc_[index];
}

const TensorDesc& OpDesc::GetInputDesc(const string& name) const
{
    static TensorDesc tensorDesc;
    auto it = inputNameIdx_.find(name);
    HIAI_EXPECT_TRUE_R(it != inputNameIdx_.end(), tensorDesc);

    return GetInputDesc(it->second);
}

OpDesc::Vistor<string> OpDesc::GetAllInputNames() const
{
    vector<string> names;
    if (inputNameIdx_.empty()) {
        return OpDesc::Vistor<string>(shared_from_this(), names);
    }
    for (std::pair<string, uint32_t> input : inputNameIdx_) {
        names.push_back(input.first);
    }
    return OpDesc::Vistor<string>(shared_from_this(), names);
}

TensorDescPtr OpDesc::MutableInputDesc(uint32_t index) const
{
    HIAI_EXPECT_TRUE_R(index < inputsDesc_.size(), nullptr);
    return inputsDesc_[index];
}

OpDesc::Vistor<TensorDesc> OpDesc::GetAllInputsDesc() const
{
    vector<TensorDesc> temp;
    for (const auto& it : inputsDesc_) {
        temp.push_back(*it);
    }
    return OpDesc::Vistor<TensorDesc>(shared_from_this(), temp);
}

int32_t OpDesc::GetInputsDescSize() const
{
    return inputsDesc_.size();
}

const vector<TensorDescPtr>& OpDesc::GetInputsDesc() const
{
    return inputsDesc_;
}

GraphErrCodeStatus OpDesc::AddInputDesc(const vector<TensorDescPtr>& descVec)
{
    vector<TensorDescPtr> tmpDescVec;
    for (uint32_t i = 0; i < descVec.size(); i++) {
        if (descVec[i] == nullptr) {
            return GRAPH_FAILED;
        }
        tmpDescVec.push_back(descVec[i]);
    }
    inputsDesc_.swap(tmpDescVec);
    return GRAPH_SUCCESS;
}

OpDesc::Vistor<TensorDescPtr> OpDesc::GetAllInputsDescPtr() const
{
    return OpDesc::Vistor<TensorDescPtr>(shared_from_this(), inputsDesc_);
}

size_t OpDesc::GetInputsSize() const
{
    size_t size = 0;
    for (uint32_t i = 0; i < inputsDesc_.size(); i++) {
        if (InputIsSet(i)) {
            size++;
        }
    }
    return size;
}

GraphErrCodeStatus OpDesc::AddOutputDesc(const ge::TensorDesc& outputDesc)
{
    auto addDesc = hiai::make_shared_nothrow<TensorDesc>(outputDesc);
    HIAI_EXPECT_NOT_NULL(addDesc);

    outputsDesc_.emplace_back(addDesc);
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::AddOutputDesc(const string& name, const ge::TensorDesc& outputDesc)
{
    HIAI_EXPECT_TRUE(outputNameIdx_.find(name) == outputNameIdx_.end());

    uint32_t index = GetOutputsSize();

    HIAI_EXPECT_EXEC(AddOutputDesc(outputDesc));

    outputNameIdx_.insert(make_pair(name, index));

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::UpdateOutputDesc(uint32_t index, const ge::TensorDesc& tensorDesc)
{
    HIAI_EXPECT_TRUE(index < outputsDesc_.size());

    *outputsDesc_[index] = tensorDesc;

    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::UpdateOutputDesc(const string& name, const ge::TensorDesc& tensorDesc)
{
    map<string, uint32_t>::const_iterator it = outputNameIdx_.find(name);
    HIAI_EXPECT_TRUE(it != outputNameIdx_.cend());

    return UpdateOutputDesc(it->second, tensorDesc);
}

const TensorDesc& OpDesc::GetOutputDesc(uint32_t index) const
{
    static TensorDesc tensorDesc;
    HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), tensorDesc);

    return *outputsDesc_[index];
}

const TensorDesc& OpDesc::GetOutputDesc(const string& name) const
{
    static TensorDesc tensorDesc;
    auto it = outputNameIdx_.find(name);
    HIAI_EXPECT_TRUE_R(it != outputNameIdx_.end(), tensorDesc);

    return GetOutputDesc(it->second);
}

const vector<TensorDescPtr>& OpDesc::GetOutputsDesc() const
{
    return outputsDesc_;
}

GraphErrCodeStatus OpDesc::AddOutputDesc(const vector<TensorDescPtr>& descVec)
{
    vector<TensorDescPtr> tmpDescVec;
    for (uint32_t i = 0; i < descVec.size(); i++) {
        if (descVec[i] == nullptr) {
            return GRAPH_FAILED;
        }
        tmpDescVec.push_back(descVec[i]);
    }
    outputsDesc_.swap(tmpDescVec);
    return GRAPH_SUCCESS;
}

TensorDescPtr OpDesc::MutableOutputDesc(uint32_t index) const
{
    HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), nullptr);

    return outputsDesc_[index];
}

OpDesc::Vistor<TensorDesc> OpDesc::GetAllOutputsDesc() const
{
    vector<TensorDesc> temp;
    for (const auto& it : outputsDesc_) {
        temp.push_back(*it);
    }
    return OpDesc::Vistor<TensorDesc>(shared_from_this(), temp);
}

OpDesc::Vistor<TensorDescPtr> OpDesc::GetAllOutputsDescPtr() const
{
    return OpDesc::Vistor<TensorDescPtr>(shared_from_this(), outputsDesc_);
}

size_t OpDesc::GetOutputsSize() const
{
    return outputsDesc_.size();
}

ConstTensorDescPtr OpDesc::GetOutputDescPtr(uint32_t index) const
{
    HIAI_EXPECT_TRUE_R(index < outputsDesc_.size(), nullptr);
    return outputsDesc_[index];
}

ConstTensorDescPtr OpDesc::GetInputDescPtr(uint32_t index) const
{
    HIAI_EXPECT_TRUE_R(index < inputsDesc_.size(), nullptr);
    return inputsDesc_[index];
}

GraphErrCodeStatus OpDesc::AddDynamicInputDesc(const string& name, const unsigned int num)
{
    for (unsigned int i = 1; i <= num; i++) {
        if (AddInputDesc(name + std::to_string(i), TensorDesc()) != GRAPH_SUCCESS) {
            return GRAPH_FAILED;
        }
    }
    return GRAPH_SUCCESS;
}

GraphErrCodeStatus OpDesc::AddDynamicOutputDesc(const string& name, const unsigned int num)
{
    for (unsigned int i = 1; i <= num; i++) {
        if (AddOutputDesc(name + std::to_string(i), TensorDesc()) != GRAPH_SUCCESS) {
            return GRAPH_FAILED;
        }
    }
    return GRAPH_SUCCESS;
}

void OpDesc::ClearAllInputsDesc()
{
    inputsDesc_.clear();
}

void OpDesc::ClearAllOutputsDesc()
{
    outputsDesc_.clear();
}

bool OpDesc::IsOptionalInput(const string& name) const
{
    return optionalInputNames_.find(name) != optionalInputNames_.end();
}

bool OpDesc::IsOptionalInput(uint32_t index) const
{
    if (index >= inputsDesc_.size()) {
        return false;
    }
    auto format = inputsDesc_[index]->GetFormat();
    if (format == FORMAT_RESERVED) {
        return true;
    }
    return IsOptionalInput(GetInputNameByIndex(index));
}

const std::string& OpDesc::GetInputNameByIndex(uint32_t index) const
{
    for (const auto& info : inputNameIdx_) {
        if (info.second == index) {
            return info.first;
        }
    }
    return STR_EMPTY;
}

int OpDesc::GetInputIndexByName(const string& name) const
{
    auto itFind = inputNameIdx_.find(name);
    HIAI_EXPECT_TRUE_R(itFind != inputNameIdx_.end(), -1);

    return static_cast<int>(itFind->second);
}

const std::string& OpDesc::GetOutputNameByIndex(uint32_t index) const
{
    for (const auto& info : outputNameIdx_) {
        if (info.second == index) {
            return info.first;
        }
    }
    return STR_EMPTY;
}

int OpDesc::GetOutputIndexByName(const string& name) const
{
    auto itFind = outputNameIdx_.find(name);
    HIAI_EXPECT_TRUE_R(itFind != outputNameIdx_.end(), -1);

    return static_cast<int>(itFind->second);
}

void OpDesc::SetId(int64_t id)
{
    if (opDef_ != nullptr) {
        opDef_->set_id(id);
    }
}

int64_t OpDesc::GetId() const
{
    return opDef_ != nullptr ? opDef_->id() : 0;
}

void OpDesc::SetStreamId(int64_t streamId)
{
    if (opDef_ != nullptr) {
        opDef_->set_stream_id(streamId);
    }
}

int64_t OpDesc::GetStreamId() const
{
    return opDef_ != nullptr ? opDef_->stream_id() : 0;
}

void OpDesc::SetInputName(const vector<string>& inputName)
{
    if (opDef_ != nullptr) {
        opDef_->clear_input_name();
        for (size_t i = 0; i < inputName.size(); i++) {
            opDef_->add_input_name(inputName[i]);
        }
    }
}

vector<string> OpDesc::GetInputName() const
{
    vector<string> inputNames;
    HIAI_EXPECT_NOT_NULL_R(opDef_, inputNames);

    size_t size = opDef_->input_name_size();
    for (size_t i = 0; i < size; i++) {
        inputNames.push_back(*opDef_->mutable_input_name(i));
    }
    return inputNames;
}

void OpDesc::SetSrcName(const vector<string>& srcName)
{
    if (opDef_ != nullptr) {
        opDef_->clear_src_name();
        for (size_t i = 0; i < srcName.size(); i++) {
            opDef_->add_src_name(srcName[i]);
        }
    }
}

vector<string> OpDesc::GetSrcName() const
{
    vector<string> srcNames;
    HIAI_EXPECT_NOT_NULL_R(opDef_, srcNames);

    size_t size = opDef_->src_name_size();
    for (size_t i = 0; i < size; i++) {
        srcNames.push_back(*opDef_->mutable_src_name(i));
    }
    return srcNames;
}

void OpDesc::SetSrcIndex(const vector<int64_t>& srcIndex)
{
    if (opDef_ != nullptr) {
        opDef_->clear_src_index();
        for (size_t i = 0; i < srcIndex.size(); i++) {
            opDef_->add_src_index(srcIndex[i]);
        }
    }
}

vector<int64_t> OpDesc::GetSrcIndex() const
{
    vector<int64_t> srcIndexs;
    HIAI_EXPECT_NOT_NULL_R(opDef_, srcIndexs);

    size_t size = opDef_->src_index_size();
    for (size_t i = 0; i < size; i++) {
        srcIndexs.push_back(opDef_->src_index(i));
    }
    return srcIndexs;
}

void OpDesc::SetInputOffset(const vector<int64_t>& input)
{
    if (opDef_ != nullptr) {
        opDef_->clear_input_i();
        for (size_t i = 0; i < input.size(); i++) {
            opDef_->add_input_i(input[i]);
        }
    }
}

size_t OpDesc::GetInputOffsetSize() const
{
    return opDef_ != nullptr ? opDef_->input_i_size() : 0;
}

void OpDesc::ClearInputOffset()
{
    if (opDef_ != nullptr) {
        opDef_->clear_input_i();
    }
}

void OpDesc::AddInputOffset(int64_t value)
{
    if (opDef_ != nullptr) {
        opDef_->add_input_i(value);
    }
}

int64_t OpDesc::GetInputOffset(int32_t index) const
{
    HIAI_EXPECT_NOT_NULL_R(opDef_, -1);

    size_t size = opDef_->input_i_size();
    if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
        return opDef_->input_i(index);
    }
    return -1;
}

void OpDesc::SetInputOffset(int32_t index, int64_t value)
{
    if (opDef_ != nullptr) {
        size_t size = opDef_->input_i_size();
        if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
            opDef_->set_input_i(index, value);
        }
    }
}

vector<int64_t> OpDesc::GetInputOffset() const
{
    vector<int64_t> inputOffsets;
    HIAI_EXPECT_NOT_NULL_R(opDef_, inputOffsets);

    size_t size = opDef_->input_i_size();
    for (size_t i = 0; i < size; i++) {
        inputOffsets.push_back(opDef_->input_i(i));
    }
    return inputOffsets;
}

void OpDesc::SetOutputOffset(const vector<int64_t>& output)
{
    if (opDef_ != nullptr) {
        opDef_->clear_output_i();
        for (size_t i = 0; i < output.size(); i++) {
            opDef_->add_output_i(output[i]);
        }
    }
}

size_t OpDesc::GetOutputOffsetSize() const
{
    return opDef_ != nullptr ? opDef_->output_i_size() : 0;
}

void OpDesc::ClearOutputOffset()
{
    if (opDef_ != nullptr) {
        opDef_->clear_output_i();
    }
}

void OpDesc::AddOutputOffset(int64_t value)
{
    if (opDef_ != nullptr) {
        opDef_->add_output_i(value);
    }
}

int64_t OpDesc::GetOutputOffset(int32_t index) const
{
    HIAI_EXPECT_NOT_NULL_R(opDef_, -1);
    size_t size = opDef_->output_i_size();
    if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
        return opDef_->output_i(index);
    }
    return -1;
}

void OpDesc::SetOutputOffset(int32_t index, int64_t value)
{
    if (opDef_ != nullptr) {
        size_t size = opDef_->output_i_size();
        if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
            opDef_->set_output_i(index, value);
        }
    }
}

vector<int64_t> OpDesc::GetOutputOffset() const
{
    vector<int64_t> outputOffsets;
    HIAI_EXPECT_NOT_NULL_R(opDef_, outputOffsets);

    size_t size = opDef_->output_i_size();
    for (size_t i = 0; i < size; i++) {
        outputOffsets.push_back(opDef_->output_i(i));
    }
    return outputOffsets;
}

void OpDesc::SetDstName(const vector<string>& dstName)
{
    if (opDef_ != nullptr) {
        opDef_->clear_dst_name();
        for (size_t i = 0; i < dstName.size(); i++) {
            opDef_->add_dst_name(dstName[i]);
        }
    }
}

vector<string> OpDesc::GetDstName() const
{
    std::vector<std::string> dstNames;
    HIAI_EXPECT_NOT_NULL_R(opDef_, dstNames);

    size_t size = opDef_->dst_name_size();
    for (size_t i = 0; i < size; i++) {
        dstNames.push_back(*opDef_->mutable_dst_name(i));
    }
    return dstNames;
}

void OpDesc::SetDstIndex(const vector<int64_t>& dstIndex)
{
    if (opDef_ != nullptr) {
        opDef_->clear_dst_index();
        for (size_t i = 0; i < dstIndex.size(); i++) {
            opDef_->add_dst_index(dstIndex[i]);
        }
    }
}

vector<int64_t> OpDesc::GetDstIndex() const
{
    vector<int64_t> dstIndexs;
    HIAI_EXPECT_NOT_NULL_R(opDef_, dstIndexs);

    size_t size = opDef_->dst_index_size();
    for (size_t i = 0; i < size; i++) {
        dstIndexs.push_back(opDef_->dst_index(i));
    }
    return dstIndexs;
}

void OpDesc::SetWorkspace(const vector<int64_t>& workspace)
{
    if (opDef_ != nullptr) {
        opDef_->clear_workspace();
        for (size_t i = 0; i < workspace.size(); i++) {
            opDef_->add_workspace(workspace[i]);
        }
    }
}

size_t OpDesc::GetWorkspaceSize() const
{
    return opDef_ != nullptr ? opDef_->workspace_size() : 0;
}

void OpDesc::ClearWorkspace()
{
    if (opDef_ != nullptr) {
        opDef_->clear_workspace();
    }
}

void OpDesc::AddWorkspace(int64_t value)
{
    if (opDef_ != nullptr) {
        opDef_->add_workspace(value);
    }
}

int64_t OpDesc::GetWorkspace(int32_t index) const
{
    HIAI_EXPECT_NOT_NULL_R(opDef_, -1);

    size_t size = opDef_->workspace_size();
    if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
        return opDef_->workspace(index);
    }
    return -1;
}

void OpDesc::SetWorkspace(int32_t index, int64_t value)
{
    if (opDef_ != nullptr) {
        size_t size = opDef_->workspace_size();
        if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
            opDef_->set_workspace(index, value);
        }
    }
}

vector<int64_t> OpDesc::GetWorkspace() const
{
    vector<int64_t> workspaces;
    HIAI_EXPECT_NOT_NULL_R(opDef_, workspaces);

    size_t size = opDef_->workspace_size();
    for (size_t i = 0; i < size; i++) {
        workspaces.push_back(opDef_->workspace(i));
    }
    return workspaces;
}

void OpDesc::SetWorkspaceBytes(const vector<uint32_t>& workspaceBytes)
{
    if (opDef_ != nullptr) {
        opDef_->clear_workspace_bytes();
        for (size_t i = 0; i < workspaceBytes.size(); i++) {
            opDef_->add_workspace_bytes(static_cast<int64_t>(workspaceBytes[i]));
        }
    }
}

vector<uint32_t> OpDesc::GetWorkspaceBytes() const
{
    std::vector<uint32_t> workspaceBytes;
    HIAI_EXPECT_NOT_NULL_R(opDef_, workspaceBytes);

    size_t size = opDef_->workspace_bytes_size();
    for (size_t i = 0; i < size; i++) {
        workspaceBytes.push_back(static_cast<uint32_t>(opDef_->workspace_bytes(i)));
    }
    return workspaceBytes;
}

size_t OpDesc::GetWorkspaceBytesSize() const
{
    return opDef_ != nullptr ? opDef_->workspace_bytes_size() : 0;
}

void OpDesc::ClearWorkspaceBytes()
{
    if (opDef_ != nullptr) {
        opDef_->clear_workspace_bytes();
    }
}

void OpDesc::AddWorkspaceBytes(uint32_t value)
{
    if (opDef_ != nullptr) {
        opDef_->add_workspace_bytes(value);
    }
}

void OpDesc::SetWorkspaceBytes(int32_t index, uint32_t value)
{
    if (opDef_ != nullptr) {
        size_t size = opDef_->workspace_bytes_size();
        if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
            opDef_->set_workspace_bytes(index, value);
        }
    }
}

uint32_t OpDesc::GetWorkspaceBytes(int32_t index) const
{
    HIAI_EXPECT_NOT_NULL_R(opDef_, UINT_MAX);

    size_t size = opDef_->workspace_bytes_size();
    if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
        return static_cast<uint32_t>(opDef_->workspace_bytes(index));
    }
    return UINT_MAX;
}

void OpDesc::SetIsInputConst(const vector<bool>& isInputConst)
{
    if (opDef_ != nullptr) {
        opDef_->clear_is_input_const();
        for (size_t i = 0; i < isInputConst.size(); i++) {
            opDef_->add_is_input_const(isInputConst[i]);
        }
    }
}

size_t OpDesc::GetIsInputConstSize() const
{
    return opDef_ != nullptr ? opDef_->is_input_const_size() : 0;
}

bool OpDesc::IsInputConst(int32_t index) const
{
    HIAI_EXPECT_NOT_NULL_R(opDef_, false);

    size_t size = opDef_->is_input_const_size();
    if (index >= 0 && static_cast<size_t>(static_cast<uint32_t>(index)) < size) {
        return opDef_->is_input_const(index);
    }
    return false;
}

void OpDesc::AddIsInputConst(bool value)
{
    if (opDef_ != nullptr) {
        opDef_->add_is_input_const(value);
    }
}

vector<bool> OpDesc::GetIsInputConst() const
{
    vector<bool> inputConsts;
    HIAI_EXPECT_NOT_NULL_R(opDef_, inputConsts);

    size_t size = opDef_->is_input_const_size();
    for (size_t i = 0; i < size; i++) {
        inputConsts.push_back(opDef_->is_input_const(i));
    }
    return inputConsts;
}

void OpDesc::SetRunCL(string clname)
{
    mBestRunCl_ = clname;
}

string OpDesc::GetRunCL() const
{
    return mBestRunCl_;
}

GraphErrCodeStatus OpDesc::AddRequiredAttr(const std::string& name, ge::AttrValue::ValueType type)
{
    if (requiredAttrs_.find(name) != requiredAttrs_.end()) {
        return GRAPH_FAILED;
    }
    requiredAttrs_[name] = type;
    return GRAPH_SUCCESS;
}

bool OpDesc::IsDimValid() const
{
    for (const auto& desc : inputsDesc_) {
        for (const auto& dim : desc->GetShape().GetDims()) {
            HIAI_EXPECT_TRUE_R(dim > 0, false);
        }
    }

    return true;
}

std::vector<uint32_t> OpDesc::GetOptionalInputsIdx() const
{
    std::vector<uint32_t> vIdx;

    for (const auto& optionalName : optionalInputNames_) {
        const auto& it = inputNameIdx_.find(optionalName);
        if (it != inputNameIdx_.cend()) {
            vIdx.push_back(it->second);
        }
    }

    return vIdx;
}
} // namespace ge
