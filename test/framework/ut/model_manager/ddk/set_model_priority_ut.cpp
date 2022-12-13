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
#include <dlfcn.h>
#include <iostream>

#include "stub_load_models.h"
#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager/model_manager.h"

using namespace std;
using namespace hiai;

class SetModelPriority : public testing::Test {
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
private:
    shared_ptr<AiModelMngerClient> client{nullptr};
    StubLoadModels stubLoadModels;
};

/*
* 测试用例名称   : model_manager_set_model_priority_without_load
* 测试用例描述： 模型未加载，设置此模型的优先级
* 预置条件 : 加载几个模型
* 操作步骤: 设置map中不存在名称的模型优先级
* 预期结果 : 模型名称不存在，返回失败
* 修改历史 :
*/
TEST_F(SetModelPriority, model_manager_set_model_priority_without_load)
{
    string name = "noload";
    AIStatus ret = client->SetModelPriority("noload", hiai::ModelPriority::PRIORITY_HIGH);
    EXPECT_EQ(AI_FAILED, ret);
}

/*
* 测试用例名称   : model_manager_set_model_priority_with_error_priority
* 测试用例描述： 设置此模型的优先级为枚举范围之外的值
* 预置条件 : 加载几个模型
* 操作步骤: 优先级为枚举范围之外的值
* 预期结果 : 返回失败
* 修改历史 :
*/
TEST_F(SetModelPriority, model_manager_set_model_priority_with_error_priority)
{
    int errorPriority = -1;
    AIStatus ret = client->SetModelPriority("tf_softmax_no_infershaped", (hiai::ModelPriority)errorPriority);
    EXPECT_EQ(AI_FAILED, ret);
}

/*
* 测试用例名称   : model_manager_set_model_priority_with_hcl
* 测试用例描述： 模型已加载，设置此模型的优先级，模型走的HCL
* 预置条件 : 模型已加载
* 操作步骤: 设置模型优先级
* 预期结果 : 返回成功
* 修改历史 :
*/
TEST_F(SetModelPriority, model_manager_set_model_priority_with_hcl)
{
    AIStatus ret = client->SetModelPriority("tf_softmax_no_infershaped", hiai::ModelPriority::PRIORITY_HIGH);
    EXPECT_EQ(AI_SUCCESS, ret);
}

/*
* 测试用例名称   : model_manager_set_model_priority_with_c
* 测试用例描述： 模型已加载，设置此模型的优先级，模型走的C
* 预置条件 : 模型已加载
* 操作步骤: 设置模型优先级
* 预期结果 : 返回成功
* 修改历史 :
*/
TEST_F(SetModelPriority, model_manager_set_model_priority_with_c)
{
    AIStatus ret = client->SetModelPriority("tf_softmax_infershaped", hiai::ModelPriority::PRIORITY_HIGH);
    EXPECT_EQ(AI_SUCCESS, ret);
}

