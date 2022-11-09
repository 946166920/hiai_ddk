#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>

#include "infra/buffer/local_buffer.h"

using testing::Test;
using namespace std;
using namespace hiai;

class Test_LocalBuffer : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

/*
* 测试用例名称:model_manager_CreateLocalBuffer_with_size_success
* 测试用例描述:通过size，创建LocalBuffer成功
* 预置条件:
* 操作步骤:1.设置size
*         2.创建buffer
* 预期结果 :创建LocalBuffer 成功
* 修改历史 :
*/
TEST_F(Test_LocalBuffer, model_manager_CreateLocalBuffer_with_size_success)
{
    size_t size = 1024;
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(size);
    EXPECT_NE(buffer, nullptr);
    EXPECT_NE(buffer->GetData(), nullptr);
    EXPECT_EQ(buffer->GetSize(), size);
}

/*
* 测试用例名称:model_manager_CreateLocalBuffer_with_data_size_success
* 测试用例描述:通过data和size，创建LocalBuffer成功
* 预置条件:
* 操作步骤:1.设置size，并申请内存
          2.创建buffer
* 预期结果 :创建LocalBuffer 成功
* 修改历史 :
*/
TEST_F(Test_LocalBuffer, model_manager_CreateLocalBuffer_with_data_size_success)
{
    size_t size = 1024;
    void* data = (void*)malloc(size);
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(data, size);
    EXPECT_NE(buffer, nullptr);
    EXPECT_NE(buffer->GetData(), nullptr);
    EXPECT_EQ(buffer->GetSize(), size);
    free(data);
    data = nullptr;
}


