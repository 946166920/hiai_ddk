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

#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>
#include "compatible/HiAiModelManagerService.h"

using namespace std;
using namespace hiai;
class CreateMemBuffer : public testing::Test {
public:
    void SetUp()
    {
        mkdir("../out", S_IRUSR | S_IWUSR | S_IXUSR); // 700
    }

    void TearDown()
    {
        remove("../out");
        GlobalMockObject::verify();
    }
};

 /*
* 测试用例名称: model_manager_create_membuffer_from_file
* 测试用例描述： 通过文件创建membuffer
* 预置条件 : 文件已存在
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果 : 1.buffer不等于nullptr
*           2.GetMemBufferData不等于nullptr
*           3.GetMemBufferSize不等于0
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_file)
{
    std::string path = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(path);
    EXPECT_TRUE(buffer1 != nullptr);
    EXPECT_TRUE(buffer1->GetMemBufferData() != nullptr);
    EXPECT_TRUE(buffer1->GetMemBufferSize() != 0);

    auto buffer2 = builder->ReadBinaryProto(path);
    EXPECT_TRUE(buffer2 != nullptr);
    EXPECT_TRUE(buffer2->GetMemBufferData() != nullptr);
    EXPECT_TRUE(buffer2->GetMemBufferSize() != 0);

    EXPECT_EQ(buffer1->GetMemBufferSize(), buffer2->GetMemBufferSize());
    builder->MemBufferDestroy(buffer1);
    builder->MemBufferDestroy(buffer2);
}

/*
* 测试用例名称: model_manager_create_membuffer_from_noexist_file
* 测试用例描述: 通过文件创建membuffer,但文件不存在
* 预置条件: 文件不存在
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer为nullptr
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_noexist_file)
{
    std::string path = "no_exist_file.om";
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(path);
    EXPECT_EQ(nullptr, buffer1);
    builder->MemBufferDestroy(buffer1);

    auto buffer2 = builder->ReadBinaryProto(path);
    EXPECT_EQ(nullptr, buffer2);
    builder->MemBufferDestroy(buffer2);
}

/*
* 测试用例名称: model_manager_create_membuffer_from_long_file
* 测试用例描述: 通过文件创建membuffer,但文件路径超长
* 预置条件: 超长文件名
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer为nullptr
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_long_file)
{
    string path = "";
    for (int i = 0; i < 300; i ++) {
        path += "a";
    }
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(path);
    EXPECT_EQ(nullptr, buffer1);
    builder->MemBufferDestroy(buffer1);

    auto buffer2 = builder->ReadBinaryProto(path);
    EXPECT_EQ(nullptr, buffer2);
    builder->MemBufferDestroy(buffer2);
}

/*
* 测试用例名称: model_manager_create_membuffer_from_null_file
* 测试用例描述: 通过文件创建membuffer,但文件路径为空
* 预置条件: 文件路径为空
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer为nullptr
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_null_file)
{
    string path = "";
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(path);
    EXPECT_EQ(nullptr, buffer1);
    builder->MemBufferDestroy(buffer1);

    auto buffer2 = builder->ReadBinaryProto(path);
    EXPECT_EQ(nullptr, buffer2);
    builder->MemBufferDestroy(buffer2);
}

/*
* 测试用例名称: model_manager_create_membuffer_from_data_size
* 测试用例描述: 通过data和size构造membuffer
* 预置条件: 创建data和size，并申请内存
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer不等于nullptr
*          2.GetMemBufferData不等于nullptr
*          3.GetMemBufferSize不等于0
*          4.通过MemBufferDestroy无法释放，需用户自行释放
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_data_size)
{
    auto size = 100;
    void* data = malloc(size);
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(data, size);
    EXPECT_TRUE(buffer1 != nullptr);
    EXPECT_TRUE(buffer1->GetMemBufferData() != nullptr);
    EXPECT_EQ(size, buffer1->GetMemBufferSize());
    builder->MemBufferDestroy(buffer1);

    auto buffer2 = builder->ReadBinaryProto(data, size);
    EXPECT_TRUE(buffer2 != nullptr);
    EXPECT_TRUE(buffer2->GetMemBufferData() != nullptr);
    EXPECT_EQ(size, buffer2->GetMemBufferSize());
    builder->MemBufferDestroy(buffer2);

    EXPECT_TRUE(data != nullptr);
    free(data);
    data = nullptr;
}

/*
* 测试用例名称: model_manager_create_membuffer_from_nullptr
* 测试用例描述: 通过data和size构造membuffer, 但data为nullptr
* 预置条件: 创建data和size，不申请内存
* 操作步骤: 调用ReadBinaryProto和InputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer等于nullptr
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_membuffer_from_nullptr)
{
    auto size = 100;
    void* data = nullptr;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer1 = builder->InputMemBufferCreate(data, size);
    EXPECT_TRUE(buffer1 == nullptr);
    builder->MemBufferDestroy(buffer1);

    auto buffer2 = builder->ReadBinaryProto(data, size);
    EXPECT_TRUE(buffer2 == nullptr);
    builder->MemBufferDestroy(buffer2);
}

/*
* 测试用例名称: model_manager_create_output_membuffer
* 测试用例描述: 创建200M内存的membuffer
* 预置条件:
* 操作步骤: 调用OutputMemBufferCreate分别创建membuffer
* 预期结果: 1.buffer不等于nullptr
*          2.GetMemBufferData不等于nullptr
*          3.GetMemBufferSize等于200M
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_create_output_membuffer)
{
    int32_t framework;
    vector<MemBuffer*> pinputMemBuffer;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer = builder->OutputMemBufferCreate(framework, pinputMemBuffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->GetMemBufferData() != nullptr);
    EXPECT_EQ(200*1024*1024, buffer->GetMemBufferSize());
    builder->MemBufferDestroy(buffer);
}

/*
* 测试用例名称: model_manager_membuffer_exportfile_success
* 测试用例描述: 将buffer导出到文件
* 预置条件:
* 操作步骤: 1.创建MemBuffer
*          2.将MemBuffer到出到文件
* 预期结果: 将buffer导出到文件成功
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_membuffer_exportfile_success)
{
    int32_t framework;
    vector<MemBuffer*> pinputMemBuffer;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer = builder->OutputMemBufferCreate(framework, pinputMemBuffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->GetMemBufferData() != nullptr);
    const uint32_t bufferSize = 200*1024*1024;
    EXPECT_EQ(bufferSize, buffer->GetMemBufferSize());
    const std::string exportPath = "../out/export.om";
    auto ret = builder->MemBufferExportFile(buffer, bufferSize, exportPath);
    EXPECT_EQ(SUCCESS, ret);
    builder->MemBufferDestroy(buffer);
}

/*
* 测试用例名称: model_manager_membuffer_exportfile_fail
* 测试用例描述: 将buffer导出到文件
* 预置条件:文件路径超过最大长度
* 操作步骤: 1.创建MemBuffer
*          2.将MemBuffer到出到文件
* 预期结果: 将buffer导出到文件失败
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_membuffer_exportfile_fail)
{
    int32_t framework;
    vector<MemBuffer*> pinputMemBuffer;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer = builder->OutputMemBufferCreate(framework, pinputMemBuffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->GetMemBufferData() != nullptr);
    const uint32_t bufferSize = 200*1024*1024;
    EXPECT_EQ(bufferSize, buffer->GetMemBufferSize());
    const std::string exportPath(4096+1,'a');
    auto ret = builder->MemBufferExportFile(buffer, bufferSize, exportPath);
    EXPECT_EQ(FAILURE, ret);
    builder->MemBufferDestroy(buffer);
}

/*
* 测试用例名称: model_manager_membuffer_exportfile_fail_02
* 测试用例描述: 将buffer导出到文件
* 预置条件:MemBuffer为空
* 操作步骤: 1.创建MemBuffer
*          2.将MemBuffer到出到文件
* 预期结果: 将buffer导出到文件失败
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_membuffer_exportfile_fail_02)
{
    int32_t framework;
    vector<MemBuffer*> pinputMemBuffer;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    MemBuffer* buffer =nullptr;
    const uint32_t bufferSize = 200*1024*1024;
    const std::string exportPath = "../out/export.om";
    auto ret = builder->MemBufferExportFile(buffer, bufferSize, exportPath);
    EXPECT_EQ(FAILURE, ret);
}

/*
* 测试用例名称: model_manager_membuffer_exportfile_fail_03
* 测试用例描述: 将buffer导出到文件
* 预置条件:pbuildSize大于MemBuffer的size
* 操作步骤: 1.创建MemBuffer
*          2.将MemBuffer到出到文件
* 预期结果: 将buffer导出到文件失败
* 修改历史 :
*/
TEST_F(CreateMemBuffer, model_manager_membuffer_exportfile_fail_03)
{
    int32_t framework;
    vector<MemBuffer*> pinputMemBuffer;
    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    auto buffer = builder->OutputMemBufferCreate(framework, pinputMemBuffer);
    EXPECT_TRUE(buffer != nullptr);
    EXPECT_TRUE(buffer->GetMemBufferData() != nullptr);
    const uint32_t bufferSize = 300*1024*1024;
    const std::string exportPath = "../out/export.om";
    auto ret = builder->MemBufferExportFile(buffer, bufferSize, exportPath);
    EXPECT_EQ(FAILURE, ret);
    builder->MemBufferDestroy(buffer);
}
