/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: AippTensor Unit Test
 */
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <dlfcn.h>
#include "compatible/HiAiAippPara.h"
#include "compatible/AiTensor.h"
#include "compatible/AippTensor.h"
#include "util/hiai_foundation_dl_helper.h"
#include "util/native_handle_creator.h"

using namespace std;
using namespace hiai;

class AippTensorUTest : public testing::Test {
public:
    void SetUp()
    {
        std::cout << __func__ << std::endl;
        InitAippTensor();
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
        GlobalMockObject::verify();
    }

    void InitAippTensor(uint32_t batchCount = 1)
    {
        aippPara_ = std::make_shared<AippPara>();
        ASSERT_NE(nullptr, aippPara_);
        AIStatus ret = aippPara_->Init();
        ASSERT_EQ(AI_SUCCESS, ret);
        aippParaList_.push_back(aippPara_);

        aiTensor_ = std::make_shared<AiTensor>();
        ASSERT_NE(nullptr, aiTensor_);
        uint32_t number = 2;
        uint32_t height = 100;
        uint32_t width = 200;
        ret = aiTensor_->Init(number, height, width, AiTensorImage_YUV420SP_U8);
        ASSERT_EQ(AI_SUCCESS, ret);

        aippTensor_ = std::make_shared<AippTensor>(aiTensor_, aippParaList_);
        ASSERT_NE(nullptr, aippTensor_);
    }

    std::shared_ptr<AippPara> aippPara_ = nullptr;
    std::vector<std::shared_ptr<AippPara>> aippParaList_;
    std::shared_ptr<AiTensor> aiTensor_ = nullptr;
    std::shared_ptr<AippTensor> aippTensor_ = nullptr;
};

/*
* 测试用例标题：GetBuffer_001
* 测试用例描述：测试GetBuffer接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 调用GetBuffer接口
*           2. 检查结果
* 预期结果：
*          1.GetBuffer接口返回非空地址
*/
TEST_F(AippTensorUTest, GetBuffer_001)
{
    void* buffer = aippTensor_->GetBuffer();
    EXPECT_NE(nullptr, buffer);
}

/*
* 测试用例标题：GetSize_001
* 测试用例描述：测试GetBuffer接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 调用GetSize接口
*           2. 检查结果
* 预期结果：
*          1.GetSize接口返回非0
*/
TEST_F(AippTensorUTest, GetSize_001)
{
    uint32_t size = aippTensor_->GetSize();
    EXPECT_NE(0, size);
}

/*
* 测试用例标题：GetAiTensor_001
* 测试用例描述：测试GetAiTensor接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 调用GetAiTensor接口
*           2. 检查结果
* 预期结果：
*          1.GetAiTensor接口返回非空AiTensor对象
*/
TEST_F(AippTensorUTest, GetAiTensor_001)
{
    std::shared_ptr<AiTensor> tensor = aippTensor_->GetAiTensor();
    EXPECT_NE(nullptr, tensor);
}

/*
* 测试用例标题：GetAippParas_001
* 测试用例描述：测试GetAippParas接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 调用GetAippParas接口，使用默认参数
*           2. 检查结果
* 预期结果：
*          1.GetAippParas接口返回非空向量
*/
TEST_F(AippTensorUTest, GetAippParas_001)
{
    std::vector<std::shared_ptr<AippPara>> aippParas = aippTensor_->GetAippParas();
    EXPECT_NE(0, aippParas.size());
}

/*
* 测试用例标题：GetAippParas_002
* 测试用例描述：测试GetAippParas接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 指定入参，调用GetAippParas接口
*           2. 检查结果
* 预期结果：
*          1.GetAippParas接口返回非空AippPara对象
*/
TEST_F(AippTensorUTest, GetAippParas_002)
{
    std::shared_ptr<AippPara> aippParas = aippTensor_->GetAippParas(0);
    EXPECT_NE(nullptr, aippParas.get());
}

/*
* 测试用例标题：GetAippParas_003
* 测试用例描述：测试GetAippParas接口
* 预置条件：
*           1.创建并初始化AippPara对象
*           2.创建并初始化AiTensor对象
*           3.利用AippPara、AiTensor对象创建AippTensor对象
* 操作步骤：
*           1. 指定入参，调用GetAippParas接口
*           2. 检查结果
* 预期结果：
*          1.GetAippParas接口返回空AippPara对象
*/
TEST_F(AippTensorUTest, GetAippParas_003)
{
    std::shared_ptr<AippPara> aippParas = aippTensor_->GetAippParas(1000);
    EXPECT_EQ(nullptr, aippParas.get());
}

/*
* 测试用例标题：HIAI_CreateAiPPTensorFromHandle_OK
* 测试用例描述：测试HIAI_CreateAiPPTensorFromHandle接口
* 预置条件：
* 操作步骤：
*           1. 设置TensorDimension
*           2. 调用_CreateAiPPTensorFromHandle接口,使用默认AiTensorImage_Format
*           3. 检查结果
* 预期结果：
*          1._CreateAiPPTensorFromHandle接口返回非空AippTensor对象
*/
TEST_F(AippTensorUTest, HIAI_CreateAiPPTensorFromHandle_001)
{
    TensorDimension dim;
    dim.SetNumber(1);
    dim.SetChannel(3);
    dim.SetHeight(100);
    dim.SetWidth(200);

    buffer_handle_t bufHandle;
    shared_ptr<AippTensor> aippTensor = HIAI_CreateAiPPTensorFromHandle(bufHandle, &dim);
    EXPECT_NE(nullptr, aippTensor.get());
}

/*
* 测试用例标题：_CreateAiPPTensorFromHandle_002
* 测试用例描述：测试_CreateAiPPTensorFromHandle接口,使用指定AiTensorImage_Format
* 预置条件：
* 操作步骤：
*           1. 设置TensorDimension
*           2. 调用_CreateAiPPTensorFromHandle接口
*           3. 检查结果
* 预期结果：
*          1._CreateAiPPTensorFromHandle接口返回非空AippTensor对象
*/
TEST_F(AippTensorUTest, HIAI_CreateAiPPTensorFromHandle_002)
{
    const TensorDimension dim;
    dim.SetNumber(1);
    dim.SetChannel(3);
    dim.SetHeight(100);
    dim.SetWidth(200);

    buffer_handle_t handle;
    void* bufHandle = const_cast<native_handle*>(handle);
    shared_ptr<AippTensor> aippTensor = HIAI_CreateAiPPTensorFromHandle(bufHandle, &dim, AiTensorImage_YUV422SP_U8);
    EXPECT_NE(nullptr, aippTensor.get());
}

/*
* 测试用例名称: CreateNDTensorBufferFromHwHandle_success
* 测试用例描述：通过buffer_handle_t，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(AippTensorUTest, CreateNDTensorBufferFromHwHandle_success)
{
    buffer_handle_t handle;
    hiai::NativeHandle nativeHandle;
    Status ret = CreateNativeHandle(nativeHandle, handle);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_NE(nativeHandle.size, -1);
    EXPECT_NE(nativeHandle.size, -1);
}

// 返回空符号，获取符号失败使用该桩
void *HIAI_Foundation_GetSymbol_HwHandlestub_fail(const char *symbolName)
{
    return nullptr;
}
/*
* 测试用例名称: CreateNDTensorBufferFromHwHandle_failed
* 测试用例描述：通过buffer_handle_t，创建NDTensorBuffer失败
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 失败
* 修改历史 :
*/
TEST_F(AippTensorUTest, CreateNDTensorBufferFromHwHandle_failed)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_HwHandlestub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_HwHandlestub_fail));

    buffer_handle_t bufHandle;
    hiai::NativeHandle nativeHandle;
    Status ret = CreateNativeHandle(nativeHandle, bufHandle);
    EXPECT_EQ(ret, FAILURE);
}
/*
* 测试用例名称: CreateNDTensorBufferFromHwHandle_realSize_success
* 测试用例描述：通过buffer_handle_t，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(AippTensorUTest, CreateNDTensorBufferFromHwHandle_realSize_success)
{
    buffer_handle_t handle;
    hiai::NativeHandle nativeHandle;
    uint size = 1000;
    uint offset = -1;
    Status ret = CreateNativeHandle(nativeHandle, handle, offset, size);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_EQ(nativeHandle.size, size);
}
/*
* 测试用例名称: CreateNDTensorBufferFromHwHandle_Offset_success
* 测试用例描述：通过buffer_handle_t，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(AippTensorUTest, CreateNDTensorBufferFromHwHandle_Offset_success)
{
    buffer_handle_t handle;
    hiai::NativeHandle nativeHandle;
    uint size = -1;
    uint offset = 10001;
    Status ret = CreateNativeHandle(nativeHandle, handle, offset, size);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_EQ(nativeHandle.offset, offset);
}
/*
* 测试用例名称: CreateNDTensorBufferFromHwHandle_OffsetSize_success
* 测试用例描述：通过buffer_handle_t，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(AippTensorUTest, CreateNDTensorBufferFromHwHandle_OffsetSize_success)
{
    buffer_handle_t handle;
    hiai::NativeHandle nativeHandle;
    uint size = 1000;
    uint offset = 10001;
    Status ret = CreateNativeHandle(nativeHandle, handle, offset, size);
    EXPECT_EQ(ret, SUCCESS);
    EXPECT_EQ(nativeHandle.size, size);
    EXPECT_EQ(nativeHandle.offset, offset);
}