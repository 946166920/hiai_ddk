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

#include "model_manager/model_manager.h"
#include "control_c.h"

using namespace std;
using namespace hiai;

class LoadModelUt : public testing::Test {
public:
    void SetUp()
    {
        manager_ = CreateModelManager();
    }

    void TearDown()
    {
        GlobalMockObject::verify();
        ControlC::GetInstance().Clear();
    }
public:
    std::shared_ptr<IModelManager> manager_ {nullptr};
};

/*
 * 测试用例名称: TestCase_Load_Model_Options_001
 * 测试用例描述: 加载模型，设置动态shape
 * 预期结果 :成功
 */
TEST_F(LoadModelUt, Load_Model_Options_001)
{
    std::shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    builtModel->RestoreFromFile(file);
    ModelInitOptions options;
    options.buildOptions.dynamicShapeConfig.enable = true;
    options.buildOptions.dynamicShapeConfig.maxCachedNum = 10;
    options.buildOptions.dynamicShapeConfig.cacheMode = CacheMode::CACHE_LOADED_MODEL;
    ControlC::GetInstance().SetInitOptions(options);
    EXPECT_EQ(SUCCESS, manager_->Init(options, builtModel, nullptr));
}
