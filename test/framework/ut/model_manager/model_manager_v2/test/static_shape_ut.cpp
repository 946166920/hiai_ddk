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

class StaticShapeUt : public testing::TestWithParam<ControlFuncParam> {
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
static vector<ControlFuncParam> g_TestStaticShapeParams = {
    // 1、底层均成功，预期成功，校验传递的值是否正确
    {{}, SUCCESS},
    // 2、Cbuildoptions创建失败
    {{{C_BUILD_OPTIONS_CREATE, -1}}, FAILURE},
    // 3、ndtensordesc创建第一次失败
    {{{C_ND_TENSOR_DESC_CREATE, -1}}, FAILURE},
    // 4、ndtensordesc创建第一次成功，第二次失败
    {{{C_ND_TENSOR_DESC_CREATE, 2}}, FAILURE},

};

INSTANTIATE_TEST_CASE_P(StaticShape, StaticShapeUt, testing::ValuesIn(g_TestStaticShapeParams));

TEST_P(StaticShapeUt, StaticShape)
{
    ControlFuncParam param = GetParam();
    SetConfigures(param);

    NDTensorDesc desc1;
    desc1.dims = {1, 2, 3, 4};

    NDTensorDesc desc2;
    desc2.dims = {5, 6, 7};
    desc2.dataType = DataType::FLOAT16;
    desc2.format = Format::ND;

    ModelBuildOptions options;
    options.inputTensorDescs.push_back(desc1);
    options.inputTensorDescs.push_back(desc2);

    ControlC::GetInstance().SetBuildOptions(options); // 用于build中校验
    const std::string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(param.expectStatus, modelBuilder_->Build(options, "default", modelFile, builtModel));
}
