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

#include "framework/util/base_buffer.h"

using namespace std;
using namespace hiai;

class BaseBufferUt : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
    }

};

/*
* 测试用例名称   : model_manager_base_buffer_001
* 测试用例描述：将buffer写入到文件成功
* 预置条件 :
* 操作步骤:
* 预期结果 :成功
* 修改历史 :
*/
TEST_F(BaseBufferUt, model_manager_base_buffer_001)
{
     uint8_t *data = new (std::nothrow) uint8_t[100];
     BaseBuffer test1(data, 100, false);
     BaseBuffer test2 = test1;
     EXPECT_TRUE(test1.GetSize() == test2.GetSize());
     free(data);
}

/*
* 测试用例名称   : model_manager_base_buffer_002
* 测试用例描述：将buffer写入到文件成功
* 预置条件 :
* 操作步骤:
* 预期结果 :成功
* 修改历史 :
*/
TEST_F(BaseBufferUt, model_manager_base_buffer_002)
{
     uint8_t *data = new (std::nothrow) uint8_t[100];
     BaseBuffer test1(data, 100, true);
     BaseBuffer test2 = test1;
     EXPECT_TRUE(test1.GetSize() == test2.GetSize());
}

