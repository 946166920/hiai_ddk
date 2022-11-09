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
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <thread>

#include "model_manager/model_manager_aipp.h"
#include "model/built_model_aipp.h"
#include "tensor/image_config_tensor_util.h"
#include "tensor/image_tensor_buffer.h"

#include "control_c.h"
#include "stub_model_manager_listener.h"

using namespace hiai;

const uint32_t RUN_AIPP_MODEL_TIMEOUT = 10000;

class ModelManagerAippUt : public testing::Test {
public:
    void SetUp()
    {
        builtModel_ = CreateBuiltModel();
        const char* file = "../bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
        builtModel_->RestoreFromFile(file);

        modelManager_ = CreateModelManager();
    }

    void TearDown()
    {
        GlobalMockObject::verify();
    }

public:
    std::shared_ptr<IBuiltModel> builtModel_ {nullptr};
    std::shared_ptr<IModelManager> modelManager_ {nullptr};
};


/*
 * 测试用例名称: Test_RunAippModel_V2_001
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init，不提供listener（同步）
 *          2. RunAippModel推理，其中inputs参数为空
 *          3. DeInit
 * 预期结果: 失败
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_001)
{
    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs {nullptr};
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(INVALID_PARAM, managerAipp->RunAippModel(context, inputs, aippParas, outputs, 0));

    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_002
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init，不提供listener（同步）
 *          2. RunAippModel推理，其中outputs参数为空
 *          3. DeInit
 * 预期结果: 失败
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_002)
{
    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs {nullptr};
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(INVALID_PARAM, managerAipp->RunAippModel(context, inputs, aippParas, outputs, 0));

    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_003
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init，不提供listener（同步）
 *          2. RunAippModel推理，其中aippParas参数为空
 *          3. DeInit
 * 预期结果: 成功
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_003)
{
    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas {nullptr};

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(SUCCESS, managerAipp->RunAippModel(context, inputs, aippParas, outputs, 0));

    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_004
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init不执行，未加载模型
 *          2. RunAippModel推理
 *          3. DeInit
 * 预期结果: 成功，IModelManagerAipp不会做判断
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_004)
{
    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(SUCCESS, managerAipp->RunAippModel(context, inputs, aippParas, outputs, 0));

    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_005
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init，不提供listener（同步）
 *          2. RunAippModel推理
 *          3. DeInit
 * 预期结果: 成功
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_005)
{
    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    ChannelSwapPara channelswp;
    channelswp.rbuvSwapSwitch = true;
    channelswp.axSwapSwitch = false;
    std::shared_ptr<INDTensorBuffer> channelswpParaTesnor = CreateImageConfigTensor(channelswp);
    inputs.push_back(channelswpParaTesnor);

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(SUCCESS, managerAipp->RunAippModel(context, inputs, aippParas, outputs, 0));

    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_006
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. Init，提供listener参数（异步）
 *          2. RunAippModel推理
 *          3. 判断异步回到函数是否被调用
 *          4. DeInit
 * 预期结果: 成功
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_006)
{
    ModelInitOptions options;
    std::shared_ptr<StubModelManagerListener> listener = std::make_shared<StubModelManagerListener>();
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, listener));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas {nullptr};

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(SUCCESS, managerAipp->RunAippModel(context, inputs, aippParas, outputs, RUN_AIPP_MODEL_TIMEOUT));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(true, listener->IsListenerInvoked());
    modelManager_->DeInit();
}

/*
 * 测试用例名称: Test_RunAippModel_V2_007
 * 测试用例描述: 通过RunAippModel方法下发推理
 * 预置条件: ModelManager创建成功
 * 操作步骤: 1. 打桩底层返回失败
 *          2. Init，提供listener参数（异步）
 *          3. RunAippModel推理
 *          4. DeInit
 * 预期结果: 失败
 */
TEST_F(ModelManagerAippUt, Test_RunAippModel_V2_007)
{
    ControlC::GetInstance().SetExpectStatus(CKey::C_RUN_AIPP_MODEL_V2_FAILED, FAILURE);

    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    Context context;
    context.SetValue("model_name", "test_aipp_model");
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    const std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    std::shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(modelManager_);
    EXPECT_EQ(FAILURE, managerAipp->RunAippModel(context, inputs, aippParas, outputs, RUN_AIPP_MODEL_TIMEOUT));

    modelManager_->DeInit();
}
