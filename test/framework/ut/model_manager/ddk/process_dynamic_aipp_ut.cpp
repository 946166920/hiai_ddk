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

#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include <thread>
#include <chrono>

#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager/model_manager.h"
#include "compatible/AippTensor.h"
#include "infra/base/securestl.h"
#include "model_manager/model_manager_aipp.h"

using namespace std;
using namespace hiai;

struct ModelInfo {
    string modelName;
    string modelPath;
    int32_t frequency;
    vector<TensorDimension> inputDims;
    DynamicShapeConfig dynamicShape;
    hiai::PrecisionMode precisionMode;
};

class ProcessDynamicShapeModel : public testing::Test {
public:
    void SetUp()
    {
        std::cout << __func__ << std::endl;
        // 创建模型管家
        client = make_shared<AiModelMngerClient>();
        client->Init(nullptr);
        builder = make_shared<AiModelBuilder>();
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
        // 模型卸载
        client->UnLoadModel();
    }

protected:
    shared_ptr<AiModelMngerClient> client = nullptr;
    std::shared_ptr<AiModelManagerClientListener> listener = nullptr;
    shared_ptr<AiModelBuilder> builder = nullptr;
    vector<MemBuffer*> buffers;
};

void CreateNNTensor(vector<TensorDimension>& dimensions, vector<shared_ptr<AiTensor>>& nnTensors)
{
    if (dimensions.size() == 0) {
        TensorDimension dims(2, 3, 100, 100);
        dimensions.push_back(dims);
    }

    for (auto& dim : dimensions) {
        shared_ptr<AiTensor> tensor = make_shared<AiTensor>();
        tensor->Init(&dim);
        nnTensors.push_back(tensor);
    }
}

TEST_F(ProcessDynamicShapeModel, DynamicShapeModel_001)
{
    AIStatus ret = AI_SUCCESS;
    TensorDimension dims(1, 3, 224, 224);

    DynamicShapeConfig dynamicShape;
    dynamicShape.enable = true;
    dynamicShape.maxCachedNum = 3;

    ModelInfo info = {
        .modelName = "tf_conv_opversion_5",
        .modelPath = "bin/llt/framework/domi/modelmanager/tf_conv_opversion_5.om",
        .frequency = 3,
        .inputDims = {dims},
        .dynamicShape = dynamicShape,
    };

    vector<ModelInfo> infos = {info};

    // 设置AiModelDescription
    vector<shared_ptr<AiModelDescription>> modelDescs;
    vector<MemBuffer*> buffers;
    for (auto& info : infos) {
        MemBuffer* buffer = builder->InputMemBufferCreate(info.modelPath);
        ASSERT_TRUE(buffer != nullptr);

        buffers.push_back(buffer);
        shared_ptr<AiModelDescription> desc = make_shared<AiModelDescription>(info.modelName, info.frequency, 0, 0, 0);
        ASSERT_TRUE(desc != nullptr);

        ret = ret || desc->SetModelBuffer(buffer->GetMemBufferData(), buffer->GetMemBufferSize());
        modelDescs.push_back(desc);
        ASSERT_TRUE(ret == AI_SUCCESS);
    }

    // 模型加载
    ret = client->Load(modelDescs);
    EXPECT_EQ(AI_SUCCESS, ret);

    vector<TensorDimension> inputDimensions;
    vector<TensorDimension> outputDimensions;
    client->GetModelIOTensorDim(info.modelName, inputDimensions, outputDimensions);
    std::cout << "inputDimensions size = " << inputDimensions.size() << std::endl;

    vector<shared_ptr<AiTensor>> inputTensor;
    vector<shared_ptr<AiTensor>> outputTensor;
    CreateNNTensor(inputDimensions, inputTensor);
    CreateNNTensor(outputDimensions, outputTensor);

    AiContext context;
    context.SetPara("model_name", info.modelName);
    int taskId = 0;
    uint32_t timeout = 100;

    // 模型推理
    ret = client->Process(context, inputTensor, outputTensor, timeout, taskId);
    EXPECT_EQ(AI_SUCCESS, ret);

    // 再次推理验证BuiltModelExecutorCache::GetExecutor流程
    std::cout << "proecc again......" << std::endl;
    ret = client->Process(context, inputTensor, outputTensor, timeout, taskId);
    EXPECT_EQ(AI_SUCCESS, ret);

    for (auto& buffer : buffers) {
        builder->MemBufferDestroy(buffer);
    }
}
