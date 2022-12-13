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

#include "stub_load_models.h"
#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager/model_manager.h"
#include "compatible/AippTensor.h"
#include "infra/base/securestl.h"
#include "framework/model_manager/model_manager_aipp.h"

using namespace std;
using namespace hiai;

struct ProcessInfo {
    string modelName;
    uint32_t timeout;
};

class V1Listener : public AiModelManagerClientListener {
public:
    ~V1Listener() {}
    void OnProcessDone(const AiContext& context, int32_t result,
        const std::vector<std::shared_ptr<AiTensor>>& poutTensor, int32_t piStamp)
    {
        std::cout << __func__ << std::endl;
    }

    void OnServiceDied()
    {
    }
};

class ProcessModel : public testing::Test {
public:
    void SetUp()
    {
        stubLoadModels.LoadModelsByV1();
        client = stubLoadModels.GetClient();
    }

    void TearDown()
    {
        stubLoadModels.UnloadModels();
    }

    void CreateNNTensorV1(vector<TensorDimension>& dimensions, vector<shared_ptr<AiTensor>>& nnTensors)
    {
        for (auto& dim : dimensions) {
            shared_ptr<AiTensor> tensor = make_shared<AiTensor>();
            tensor->Init(&dim);
            nnTensors.push_back(tensor);
        }
    }

    void CreateAippTensorV1(vector<TensorDimension>& dimensions, vector<shared_ptr<AiTensor>>& aippTensors)
    {
        for (auto& dim : dimensions) {
            shared_ptr<AippPara> aippPara = make_shared<AippPara>();
            vector<shared_ptr<AippPara>> aippParas;
            AiTensorImage_Format imageFormat =
            static_cast<AiTensorImage_Format>(AiTensorImage_Format::AiTensorImage_XRGB8888_U8);
            AippInputShape aippInputShape;
            aippInputShape.srcImageSizeW = 100;
            aippInputShape.srcImageSizeH = 200;
            AippCropPara aippCropPara;
            aippCropPara.cropSizeW = 100;
            aippCropPara.cropSizeH = 200;
            aippCropPara.cropStartPosW = 100;
            aippCropPara.cropStartPosH = 200;
            aippCropPara.switch_ = true;
            aippPara->Init(1);
            aippPara->SetInputFormat(imageFormat);
            aippPara->SetInputShape(aippInputShape);
            aippPara->SetCropPara(aippCropPara);
            aippParas.push_back(aippPara);
            shared_ptr<AiTensor> aiTensor = std::make_shared<AiTensor>();

            hiai::HIAI_DataType dataType = hiai::HIAI_DATATYPE_FLOAT32;
            int32_t ret = aiTensor->Init(1, dim.GetHeight(), dim.GetWidth(), imageFormat);
            ASSERT_TRUE(ret == 0);
            //create AippTensor
            shared_ptr<AippTensor> aippTensor = hiai::make_shared_nothrow<AippTensor>(aiTensor, aippParas);
            ASSERT_TRUE(aippTensor != nullptr);
            aippTensors.push_back(aippTensor);
        }
    }

    AIStatus ProcessV1WithNNTensor(ProcessInfo info)
    {
        vector<TensorDimension> inputDimensions;
        vector<TensorDimension> outputDimensions;
        EXPECT_TRUE(client != nullptr);
        client->GetModelIOTensorDim(info.modelName, inputDimensions, outputDimensions);

        vector<shared_ptr<AiTensor>> inputTensor;
        vector<shared_ptr<AiTensor>> outputTensor;
        CreateNNTensorV1(inputDimensions, inputTensor);
        CreateNNTensorV1(outputDimensions, outputTensor);

        AiContext context;
        context.SetPara("model_name", info.modelName);
        int taskId = 0;

        return client->Process(context, inputTensor, outputTensor, info.timeout, taskId);
    }

    shared_ptr<AiModelMngerClient> client{nullptr};
    StubLoadModels stubLoadModels;
};


/*
* 测试用例名称   : model_manager_process_001
* 测试用例描述： 模型走异构接口，进行推理
* 预置条件 : 加载几个模型
* 操作步骤: 1、获取模型的输入输出维度
           2、创建输入输出tensor
           3、调用推理接口
* 预期结果 : 推理成功
* 修改历史 :
*/
TEST_F(ProcessModel, model_manager_process_001)
{
    ProcessInfo info = {
        .modelName = "tf_softmax_no_infershaped",
        .timeout = 0,
    };

    AIStatus ret = ProcessV1WithNNTensor(info);
    EXPECT_EQ(AI_SUCCESS, ret);
}

/*
* 测试用例名称   : model_manager_process_002
* 测试用例描述： 模型走C接口，进行推理
* 预置条件 : 加载几个模型
* 操作步骤: 1、获取模型的输入输出维度
           2、创建输入输出tensor
           3、调用推理接口
* 预期结果 : 推理成功
* 修改历史 :
*/
TEST_F(ProcessModel, model_manager_process_002)
{
    ProcessInfo info = {
        .modelName = "tf_softmax_infershaped",
        .timeout = 0,
    };
    AIStatus ret = ProcessV1WithNNTensor(info);
    EXPECT_EQ(AI_SUCCESS, ret);
}

/*
* 测试用例名称   : model_manager_process_003
* 测试用例描述： 模型走HCL接口，异步推理
* 预置条件 :
* 操作步骤: 1、加载模型
           2、获取模型的输入输出维度
           3、创建输入输出tensor
           4、调用推理接口
* 预期结果 : 推理成功
* 修改历史 :
*/
TEST_F(ProcessModel, model_manager_process_003)
{
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    shared_ptr<AiModelMngerClient> client_hcl = make_shared<AiModelMngerClient>();
    shared_ptr<V1Listener> listener = make_shared<V1Listener>();
    client_hcl->Init(listener);

    string modelName = "tf_softmax_hcl";
    string modelPath = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    MemBuffer* buffer = builder->InputMemBufferCreate(modelPath);
    shared_ptr<AiModelDescription> desc = make_shared<AiModelDescription>(modelName, 3, 0, 0, 0);
    TensorDimension dims(1,2,3,4);
    std::vector<TensorDimension> inputDims = {dims};
    desc->SetInputDims(inputDims);
    desc->SetModelBuffer(buffer->GetMemBufferData(), buffer->GetMemBufferSize());

    vector<shared_ptr<AiModelDescription>> modelDescs;
    modelDescs.push_back(desc);

    client_hcl->Load(modelDescs);
    builder->MemBufferDestroy(buffer);

    vector<TensorDimension> inputDimensions;
    vector<TensorDimension> outputDimensions;
    client_hcl->GetModelIOTensorDim(modelName, inputDimensions, outputDimensions);

    vector<shared_ptr<AiTensor>> inputTensor;
    vector<shared_ptr<AiTensor>> outputTensor;
    CreateNNTensorV1(inputDimensions, inputTensor);
    CreateNNTensorV1(outputDimensions, outputTensor);

    AiContext context;
    context.SetPara("model_name", modelName);
    int taskId = 0;

    AIStatus ret = client_hcl->Process(context, inputTensor, outputTensor, 100, taskId);
    EXPECT_EQ(AI_SUCCESS, ret);
    // 等待回调线程结束
    this_thread::sleep_for(chrono::seconds(1));
}

/*
* 测试用例名称   : model_manager_process_004
* 测试用例描述： 模型走HCL接口，异步推理过程取消
* 预置条件 :
* 操作步骤:1、加载模型
           2、获取模型的输入输出维度
           3、创建输入输出tensor
           4、调用推理接口
           5、取消推理
* 预期结果 : 取消成功
* 修改历史 :
*/
TEST_F(ProcessModel, model_manager_process_004)
{
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    shared_ptr<AiModelMngerClient> client_hcl = make_shared<AiModelMngerClient>();
    shared_ptr<V1Listener> listener = make_shared<V1Listener>();
    client_hcl->Init(listener);

    string modelName = "tf_softmax_hcl";
    string modelPath = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    MemBuffer* buffer = builder->InputMemBufferCreate(modelPath);
    shared_ptr<AiModelDescription> desc = make_shared<AiModelDescription>(modelName, 3, 0, 0, 0);
    TensorDimension dims(1,2,3,4);
    std::vector<TensorDimension> inputDims = {dims};
    desc->SetInputDims(inputDims);
    desc->SetModelBuffer(buffer->GetMemBufferData(), buffer->GetMemBufferSize());

    vector<shared_ptr<AiModelDescription>> modelDescs;
    modelDescs.push_back(desc);

    client_hcl->Load(modelDescs);
    builder->MemBufferDestroy(buffer);

    vector<TensorDimension> inputDimensions;
    vector<TensorDimension> outputDimensions;
    client_hcl->GetModelIOTensorDim(modelName, inputDimensions, outputDimensions);

    vector<shared_ptr<AiTensor>> inputTensor;
    vector<shared_ptr<AiTensor>> outputTensor;
    CreateNNTensorV1(inputDimensions, inputTensor);
    CreateNNTensorV1(outputDimensions, outputTensor);

    AiContext context;
    context.SetPara("model_name", modelName);
    int taskId = 0;

    AIStatus ret = client_hcl->Process(context, inputTensor, outputTensor, 100, taskId);
    EXPECT_EQ(AI_SUCCESS, ret);
    client_hcl->Cancel(modelName);
    // 等待回调线程结束
    this_thread::sleep_for(chrono::seconds(1));
}
