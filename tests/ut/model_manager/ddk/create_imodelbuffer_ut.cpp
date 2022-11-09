#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include "v2/model_manager_v2.h"

using namespace std;
using namespace hiai;
class CreateIModelBuffer : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        GlobalMockObject::verify();
    }
};

 /*
* 测试用例名称: model_manager_create_imodelbuffer_from_file
* 测试用例描述： 通过文件创建IModelBuffer
* 预置条件 : 文件已存在
* 操作步骤: 调用HIAI_CreateModelBufferFromFile创建IModelBuffer
* 预期结果 : 1.IModelBuffer创建成功
*           2.Data不等于nullptr
*           3.Size不等于0
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_file)
{
    std::string path = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    ExecuteOption option;
    option.performance = ExecutionPerformance::HIGH;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateModelBufferFromFileV2("mtcnn_lite_v100_model", path.c_str(), std::move(option), buffer);

    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->Data() != nullptr);
    EXPECT_TRUE(buffer->Size() != 0);
    EXPECT_EQ(ExecutionPerformance::HIGH, buffer->ExecuteOption().performance);
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_from_noexist_file
* 测试用例描述: 通过文件创建IModelBuffer,但文件不存在
* 预置条件: 文件不存在
* 操作步骤: 调用HIAI_CreateModelBufferFromFile创建IModelBuffer
* 预期结果: 1.buffer为nullptr,创建失败
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_noexist_file)
{
    std::string path = "no_exist_file.om";
    ExecuteOption option;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateModelBufferFromFileV2("no_exist_file", path.c_str(), std::move(option), buffer);
    EXPECT_TRUE(buffer == nullptr);
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_from_long_file
* 测试用例描述: 通过文件创建IModelBuffer,但文件路径超长
* 预置条件: 超长文件名
* 操作步骤: 调用HIAI_CreateModelBufferFromFile创建IModelBuffer
* 预期结果: 1.buffer为nullptr,创建失败
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_long_file)
{
    string path = "";
    for (int i = 0; i < 300; i ++) {
        path += "a";
    }
    ExecuteOption option;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateModelBufferFromFileV2("fail", path.c_str(), std::move(option), buffer);
    EXPECT_TRUE(buffer == nullptr);
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_from_null_file
* 测试用例描述: 通过文件创建IModelBuffer,但文件路径为空
* 预置条件: 文件路径为空
* 操作步骤: 调用HIAI_CreateModelBufferFromFile创建IModelBuffer
* 预期结果: 1.buffer为nullptr,创建失败
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_null_file)
{
    string path = "";
    ExecuteOption option;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateModelBufferFromFileV2("fail", path.c_str(), std::move(option), buffer);
    EXPECT_TRUE(buffer == nullptr);
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_from_data_size
* 测试用例描述: 通过data和size构造IModelBuffer
* 预置条件: 创建data和size，并申请内存
* 操作步骤: 调用HIAI_CreateModelBuffer创建IModelBuffer
* 预期结果: 1.buffer不等于nullptr
*          2.Data不等于nullptr
*          3.Size不等于0
*          4.需用户自行释放
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_data_size)
{
    auto size = 100;
    void* data = malloc(size);
    ExecuteOption option;
    option.precisionMode = PrecisionMode::PRECISION_MODE_FP16;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateModelBufferV2("", data, size, std::move(option), buffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->Data() != nullptr);
    EXPECT_EQ(size, buffer->Size());
    EXPECT_EQ(PrecisionMode::PRECISION_MODE_FP16, buffer->ExecuteOption().precisionMode);

    free(data);
    data = nullptr;
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_from_nullptr
* 测试用例描述: 通过data和size构造IModelBuffer, 但data为nullptr
* 预置条件: 创建data和size，不申请内存
* 操作步骤: 调用HIAI_CreateModelBuffer创建IModelBuffer
* 预期结果: 1.buffer等于nullptr
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_from_nullptr)
{
    auto size = 100;
    void* data = nullptr;
    ExecuteOption option;
    shared_ptr<IModelBufferV2> buffer = nullptr;
    HIAI_CreateModelBufferV2("", data, size, std::move(option), buffer);
    EXPECT_TRUE(buffer == nullptr);
}

/*
* 测试用例名称: model_manager_create_imodelbuffer_with_size
* 测试用例描述: 根据指定大小构造IModelBuffer
* 预置条件:
* 操作步骤: 调用HIAI_CreateModelBuffer创建IModelBuffer
* 预期结果: 1.buffer不等于nullptr
*          2.Data不等于nullptr
*          3.Size等于指定大小
* 修改历史 :
*/
TEST_F(CreateIModelBuffer, model_manager_create_imodelbuffer_with_size)
{
    auto size = 150;
    ExecuteOption option;
    option.precisionMode = PrecisionMode::PRECISION_MODE_FP16;
    shared_ptr<IModelBufferV2> buffer = nullptr;

    HIAI_CreateEmptyModelBufferV2("", size, std::move(option), buffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->Data() != nullptr);
    EXPECT_EQ(150, buffer->Size());
    EXPECT_EQ(PrecisionMode::PRECISION_MODE_FP16, buffer->ExecuteOption().precisionMode);
}
