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

#include "infra/base/dynamic_load_helper.h"

using namespace std;
using namespace hiai;

class DynamicLoadHelperUt : public testing::Test {
public:
    void SetUp()
    {
        std::cout << __func__ << std::endl;
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
        GlobalMockObject::verify();
    }
};

/*
* 测试用例名称   : model_manager_dynamic_load_helper_001
* 测试用例描述：加载动态库
* 预置条件 :  文件不存在
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(DynamicLoadHelperUt, model_manager_dynamic_load_helper_001)
{
    const string dynamicPath = "llt/third_party/mpi/lib/libmpi_2.so";
    DynamicLoadHelper loadHelper;
    auto ret = loadHelper.Init(dynamicPath);
    EXPECT_EQ(false, ret);
}


/*
* 测试用例名称   : model_manager_dynamic_load_helper_002
* 测试用例描述：加载动态库
* 预置条件 :  过长库文件名
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(DynamicLoadHelperUt, model_manager_dynamic_load_helper_002)
{
    const string dynamicPath(500, 'a');
    DynamicLoadHelper loadHelper;
    auto ret = loadHelper.Init(dynamicPath);
    EXPECT_EQ(false, ret);
}

/*
* 测试用例名称   : model_manager_dynamic_load_helper_003
* 测试用例描述：加载动态库
* 预置条件 :  空路径
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(DynamicLoadHelperUt, model_manager_dynamic_load_helper_003)
{
    const string dynamicPath = "";
    DynamicLoadHelper loadHelper;
    auto ret = loadHelper.Init(dynamicPath);
    EXPECT_EQ(false, ret);
}

/*
* 测试用例名称   : model_manager_dynamic_load_helper_004
* 测试用例描述：获取符号表
* 预置条件 :  库未加载
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(DynamicLoadHelperUt, model_manager_dynamic_load_helper_004)
{
    const string symbolName = "test";
    DynamicLoadHelper loadHelper;
    auto ret = loadHelper.GetSymbol(symbolName);
    EXPECT_EQ(nullptr, ret);
}

