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
#include "c/hiai_error_types.h"
#include "util/file_util.h"

using namespace std;
using namespace hiai;

class FileUtilUt : public testing::Test {
public:
    void SetUp()
    {
        std::cout << __func__ << std::endl;
        mkdir("../out", S_IRUSR | S_IWUSR | S_IXUSR); // 700
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
        remove("../out");
        GlobalMockObject::verify();
    }
};

/*
* 测试用例名称: model_manager_file_util_003
* 测试用例描述： 加载文件内容到内存
* 预置条件 :
* 操作步骤: 1.读取文件
* 预期结果 : 文件加载失败，返回空指针
*/

TEST_F(FileUtilUt, model_manager_file_util_003)
{
    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024;   // MB size
    const string modelPath = "out/testbuffer.om";
    uint32_t size = defaultSize * megaSize;
    void *data = (void *)malloc(size);

    FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    free(data);
    data = nullptr;

    FileUtil fileUtil;
    size_t expsize = defaultSize * megaSize + 2;
    auto ret = fileUtil.LoadToBuffer(modelPath, expsize);
    EXPECT_TRUE(ret == nullptr);
    remove(modelPath.c_str());
}

/*
* 测试用例名称: model_manager_file_util_004
* 测试用例描述： 加载文件内容到内存
* 预置条件 :
* 操作步骤: 1.读取文件
* 预期结果 : 文件加载失败，返回空指针
*/

TEST_F(FileUtilUt, model_manager_file_util_004)
{
    const string modelPath = "out/testbuffer.om";
    uint32_t size = 0;
    void *data = (void *)malloc(size);

    FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    free(data);
    data = nullptr;

    FileUtil fileUtil;
    auto ret = fileUtil.LoadToBuffer(modelPath, size);
    EXPECT_TRUE(ret == nullptr);
    remove(modelPath.c_str());
}

/*
* 测试用例名称: model_manager_file_util_005
* 测试用例描述： 加载文件内容到内存
* 预置条件 :
* 操作步骤: 1.读取文件
* 预期结果 : 文件加载失败，返回空指针
*/

TEST_F(FileUtilUt, model_manager_file_util_005)
{
    const string modelPath = "out/testbuffer.om";
    uint32_t size = 0;
    void *data = (void *)malloc(size);

    FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    free(data);
    data = nullptr;

    FileUtil fileUtil;
    auto ret = fileUtil.LoadToBuffer(modelPath);
    EXPECT_TRUE(ret == nullptr);
    remove(modelPath.c_str());
}

/*
* 测试用例名称: model_manager_file_util_007
* 测试用例描述： 加载文件到内存
* 预置条件 :
* 操作步骤:
* 预期结果 : 成功
*/

TEST_F(FileUtilUt, model_manager_file_util_007)
{
    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024;   // MB size
    const string modelPath = "../out/testbuffer.om";
    uint32_t size = defaultSize * megaSize;
    void *data = (void *)malloc(size);

    FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    free(data);
    data = nullptr;

    FileUtil fileUtil;
    auto ret = fileUtil.LoadToBuffer(modelPath);
    EXPECT_TRUE(ret != nullptr);
    remove(modelPath.c_str());
}

/*
* 测试用例名称   : file_util_WriteBufferToFile_success
* 测试用例描述：将buffer写入到文件成功
* 预置条件 :
* 操作步骤:
* 预期结果 :成功
* 修改历史 :
*/
TEST_F(FileUtilUt, file_util_WriteBufferToFile_success)
{
    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024;   // MB size
    const string modelPath = "../out/testbuffer.om";
    uint32_t size = defaultSize * megaSize;
    void *data = (void *)malloc(size);
    auto ret = FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    EXPECT_EQ(SUCCESS, ret);
    free(data);
    data = nullptr;
    remove(modelPath.c_str());
}

/*
* 测试用例名称   : file_util_WriteBufferToFile_fail
* 测试用例描述：将buffer写入到文件失败
* 预置条件 : size不匹配
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(FileUtilUt, file_util_WriteBufferToFile_fail)
{
    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024;   // MB size
    const string modelPath = "out/testbuffer.om";
    uint32_t size = defaultSize * megaSize;
    void *data = (void *)malloc(size);

    auto ret = FileUtil::WriteBufferToFile(nullptr, size, modelPath.c_str());
    EXPECT_EQ(FAILURE, ret);
    free(data);
    data = nullptr;
    remove(modelPath.c_str());
}

/*
* 测试用例名称   : file_util_WriteBufferToFile_fail_02
* 测试用例描述：将buffer写入到文件失败
* 预置条件 : 打开model路径失败
* 操作步骤:
* 预期结果 :失败
* 修改历史 :
*/
TEST_F(FileUtilUt, file_util_WriteBufferToFile_fail_02)
{
    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024;   // MB size
    const string modelPath = "";
    uint32_t size = defaultSize * megaSize;
    void *data = (void *)malloc(size);

    auto ret = FileUtil::WriteBufferToFile(data, size, modelPath.c_str());
    EXPECT_EQ(FAILURE, ret);
    free(data);
    data = nullptr;
}
