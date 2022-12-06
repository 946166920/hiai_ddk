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
#include "compatible/AiModelBuilder.h"

#include <memory>
#include <functional>

#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"
#include "framework/c/compatible/hiai_mem_buffer.h"
#include "framework/infra/log/log_fmk_interface.h"

#include "model_manager/compatible/MembufferUtil.h"
#include "model_builder/compatible/BuildOptionUtil.h"
#include "util/hiai_foundation_dl_helper.h"

namespace hiai {

using HIAI_ModelManager = struct HIAI_ModelManager;
using HIAI_ModelManagerListener = struct HIAI_ModelManagerListener;

AiModelBuilder::AiModelBuilder(std::shared_ptr<AiModelMngerClient> client)
{
    (void)client;
    modelBuilder_ = CreateModelBuilder();
    if (modelBuilder_ == nullptr) {
        FMK_LOGE("create ModelBuiderImpl failed");
    }
}

AiModelBuilder::~AiModelBuilder()
{
}

AIStatus AiModelBuilder::BuildModel(
    const std::vector<MemBuffer*>& pinputMemBuffer, MemBuffer* poutputModelBuffer, uint32_t& poutputModelSize)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (modelBuilder_ == nullptr) {
        return AI_NOT_INIT;
    }

    BuildOptions options;
    return BuildModel(pinputMemBuffer, poutputModelBuffer, poutputModelSize, options);
}

AIStatus AiModelBuilder::BuildModel(const std::vector<MemBuffer*>& pinputMemBuffer, MemBuffer* poutputModelBuffer,
    uint32_t& poutputModelSize, const BuildOptions& options)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (pinputMemBuffer.size() != 1) {
        FMK_LOGE("can only support 1 input buffer to build.");
        return AI_INVALID_PARA;
    }

    if (pinputMemBuffer[0] == nullptr || pinputMemBuffer[0]->GetMemBufferData() == nullptr ||
        pinputMemBuffer[0]->GetMemBufferSize() == 0 || poutputModelBuffer == nullptr) {
        return AI_INVALID_PARA;
    }

    if (modelBuilder_ == nullptr) {
        return AI_NOT_INIT;
    }
    std::shared_ptr<IBuffer> inputBuffer =
        CreateLocalBuffer(pinputMemBuffer[0]->GetMemBufferData(), pinputMemBuffer[0]->GetMemBufferSize(), false);
    std::shared_ptr<IBuffer> outputBuffer =
        CreateLocalBuffer(poutputModelBuffer->GetMemBufferData(), poutputModelBuffer->GetMemBufferSize(), false);
    if (inputBuffer == nullptr || outputBuffer == nullptr) {
        FMK_LOGE("input buffer is invalid.");
        return AI_INVALID_PARA;
    }

    ModelBuildOptions modelBuildOptions = BuildOptionUtil::Convert2ModelBuildOptions(options);
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    Status ret = modelBuilder_->Build(modelBuildOptions, "Default", inputBuffer, builtModel);
    if (ret != SUCCESS) {
        return static_cast<AIStatus>(ret);
    }

    size_t realSize = 0;
    ret = builtModel->SaveToExternalBuffer(outputBuffer, realSize);
    if (ret == SUCCESS) {
        poutputModelSize = static_cast<uint32_t>(realSize);
    }
    return static_cast<AIStatus>(ret);
}

MemBuffer* AiModelBuilder::ReadBinaryProto(const std::string path)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::InputMemBufferCreate(path);
}

MemBuffer* AiModelBuilder::ReadBinaryProto(void* data, uint32_t size)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::InputMemBufferCreate(data, size);
}

MemBuffer* AiModelBuilder::InputMemBufferCreate(void* data, uint32_t size)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::InputMemBufferCreate(data, size);
}

MemBuffer* AiModelBuilder::InputMemBufferCreate(const std::string path)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::InputMemBufferCreate(path);
}

MemBuffer* AiModelBuilder::OutputMemBufferCreate(
    const int32_t framework, const std::vector<MemBuffer*>& pinputMemBuffer)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::OutputMemBufferCreate(framework, pinputMemBuffer);
}

void AiModelBuilder::MemBufferDestroy(MemBuffer* membuf)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    MembufferUtil::MemBufferDestroy(membuf);
}

AIStatus AiModelBuilder::MemBufferExportFile(MemBuffer* membuf, const uint32_t pbuildSize, const std::string pbuildPath)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return MembufferUtil::MemBufferExportFile(membuf, pbuildSize, pbuildPath);
}

} // namespace hiai