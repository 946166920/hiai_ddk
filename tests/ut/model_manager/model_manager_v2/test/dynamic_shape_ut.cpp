/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: DynamicShape Unit Test
 */
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>

#include "model_builder/model_builder.h"
#include "control_c.h"
#include "common.h"

using namespace std;
using namespace hiai;

class DynamicShapeUt : public testing::TestWithParam<ControlFuncParam> {
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
static vector<ControlFuncParam> g_TestDynamicShapeParams = {
    // 1、底层均成功，预期成功，校验传递的值是否正确
    {{}, SUCCESS},
    // 2、Cbuildoptions创建失败
    {{{C_BUILD_OPTIONS_CREATE, -1}}, FAILURE},
    // 3、Cdynamicshape创建失败
    {{{C_BUILD_OPTIONS_DYNAMIC_SHAPE_CONFIG_CREATE, -1}}, FAILURE},
};

INSTANTIATE_TEST_CASE_P(DynamicShape, DynamicShapeUt, testing::ValuesIn(g_TestDynamicShapeParams));

TEST_P(DynamicShapeUt, DynamicShape)
{
    ControlFuncParam param = GetParam();
    SetConfigures(param);

    ModelBuildOptions options;
    options.dynamicShapeConfig.enable = true;
    options.dynamicShapeConfig.maxCachedNum = 5;
    options.dynamicShapeConfig.cacheMode = CACHE_LOADED_MODEL;

    ControlC::GetInstance().SetBuildOptions(options); // 用于build中校验
    const std::string modelFile = "../bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om";
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    EXPECT_EQ(param.expectStatus, modelBuilder_->Build(options, "default", modelFile, builtModel));
}
