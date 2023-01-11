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

#include "model_builder/model_builder.h"
#include "control_c.h"
#include "common.h"

using namespace std;
using namespace hiai;

namespace hiai {
class ModelDeviceConfigUt : public testing::TestWithParam<ControlFuncParam> {
public:
    void SetUp()
    {
        modelBuilder_ = CreateModelBuilder();
    }

    void TearDown()
    {
        ControlC::GetInstance().Clear();
    }

public:
    std::shared_ptr<IModelBuilder> modelBuilder_ {nullptr};
};

// 测试用例
static vector<ControlFuncParam> g_TestModelDeviceConfigParams = {
    // 1、底层均成功，预期成功，校验传递的值是否正确
    {{}, SUCCESS},
    // 2、Cbuildoptions创建失败
    {{{C_BUILD_OPTIONS_CREATE, -1}}, FAILURE},
};

INSTANTIATE_TEST_CASE_P(ModelDeviceConfig, ModelDeviceConfigUt, testing::ValuesIn(g_TestModelDeviceConfigParams));

TEST_P(ModelDeviceConfigUt, ModelDeviceConfigModel)
{
    ControlFuncParam param = GetParam();
    SetConfigures(param);

    ModelBuildOptions options;
    options.modelDeviceConfig.deviceConfigMode = DeviceConfigMode::MODEL_LEVEL;
    options.modelDeviceConfig.fallBackMode = FallBackMode::DISABLE;
    options.modelDeviceConfig.modelDeviceOrder.push_back(ExecuteDevice::NPU);
    options.modelDeviceConfig.modelDeviceOrder.push_back(ExecuteDevice::CPU);
    options.modelDeviceConfig.deviceMemoryReusePlan = DeviceMemoryReusePlan::LOW;

    ControlC::GetInstance().SetBuildOptions(options); // 用于build中校验
    const std::string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(param.expectStatus, modelBuilder_->Build(options, "default", modelFile, builtModel));
}

TEST_P(ModelDeviceConfigUt, ModelDeviceConfigOp)
{
    ControlFuncParam param = GetParam();
    SetConfigures(param);

    ModelBuildOptions options;
    options.modelDeviceConfig.deviceConfigMode = DeviceConfigMode::OP_LEVEL;
    options.modelDeviceConfig.fallBackMode = FallBackMode::DISABLE;
    std::vector<ExecuteDevice> device1 = {ExecuteDevice::NPU, ExecuteDevice::CPU};
    options.modelDeviceConfig.opDeviceOrder["aa"] = device1;
    std::vector<ExecuteDevice> device2 = {ExecuteDevice::NPU};
    options.modelDeviceConfig.opDeviceOrder["bb"] = device2;
    options.modelDeviceConfig.deviceMemoryReusePlan = DeviceMemoryReusePlan::HIGH;

    ControlC::GetInstance().SetBuildOptions(options); // 用于build中校验
    const std::string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(param.expectStatus, modelBuilder_->Build(options, "default", modelFile, builtModel));
}

/*
测试场景：第二次调用HIAI_OpDeviceOrder_Create时失败
*/
TEST_P(ModelDeviceConfigUt, ModelDeviceConfig_op_SetError)
{
    ModelBuildOptions options;
    options.modelDeviceConfig.deviceConfigMode = DeviceConfigMode::OP_LEVEL;
    options.modelDeviceConfig.fallBackMode = FallBackMode::DISABLE;
    std::vector<ExecuteDevice> device1 = {ExecuteDevice::NPU, ExecuteDevice::CPU};
    options.modelDeviceConfig.opDeviceOrder["aa"] = device1;
    std::vector<ExecuteDevice> device2 = {ExecuteDevice::NPU};
    options.modelDeviceConfig.opDeviceOrder["bb"] = device2;

    ControlC::GetInstance().SetExpectStatus(C_BUILD_OPTIONS_OPDEVICE_CREATE, FAILURE, 2);
    const std::string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(FAILURE, modelBuilder_->Build(options, "default", modelFile, builtModel));
}

/*
测试场景：转换modelDeviceOrder失败，释放modelDeviceConfig
*/
TEST_P(ModelDeviceConfigUt, ModelDeviceConfig_op_DeviceOrder)
{
    ModelBuildOptions options;
    options.modelDeviceConfig.deviceConfigMode = DeviceConfigMode::MODEL_LEVEL;
    options.modelDeviceConfig.fallBackMode = FallBackMode::DISABLE;

    const std::string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(FAILURE, modelBuilder_->Build(options, "default", modelFile, builtModel));
}

}
