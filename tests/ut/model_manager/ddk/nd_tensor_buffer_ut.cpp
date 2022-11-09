#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include "util/hiai_foundation_dl_helper.h"
#include "tensor/base/nd_tensor_buffer_impl.h"
#include "framework/c/hiai_nd_tensor_desc.h"
#include "framework/c/compatible/hiai_tensor_buffer.h"
#include "mmpa/mmpa_api.h"
#include "control_client.h"
#include "tensor/base/hiai_nd_tensor_buffer_legacy_compatible.h"
#include "model_runtime/core/hiai_model_manager_impl.h"

using testing::Test;
using namespace std;
using namespace hiai;

typedef struct HIAI_NativeHandle {
    int fd;
    int size;
    int offset;
} HIAI_NativeHandle;

class Test_NDTensorBuffer : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        GlobalMockObject::verify();
        ControlClient::GetInstance().Clear();
    }

    NDTensorDesc CreateNDTensorDesc()
    {
        NDTensorDesc grayFloatDesc;
        grayFloatDesc.dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
        grayFloatDesc.dataType = DataType::FLOAT32;
        grayFloatDesc.format = Format::NCHW;
        return grayFloatDesc;
    }

    const int INPUT_CHANNEL = 1;
    const int RESIZE_WIDTH = 224;
    const int RESIZE_HEIGHT = 224;
};

/* ------------------------------局部桩函数定义区 START------------------------------- */

// 为了支持连续打桩，需要将原代码实现搬移过来
void* HIAI_Foundation_GetSymbol_stub_orginal(const char* symbolName)
{
    cout << "*********"<<symbolName<<"**********" << endl;
    static const char* g_aiClientLibName = "libai_client_stub_ddk.so";
    void* g_aiClientHandle = dlopen(g_aiClientLibName, RTLD_NOW);
    if (g_aiClientHandle == NULL) {
        return NULL;
    }
    return dlsym(g_aiClientHandle, symbolName);
}
// 返回空符号，回落机制中使用该桩
void* HIAI_Foundation_GetSymbol_stub_fail(const char* symbolName)
{
    return NULL;
}

static void CheckNDTensorBuffer(shared_ptr<INDTensorBuffer> ndTensorBuffer)
{
    EXPECT_NE(ndTensorBuffer, nullptr);
    EXPECT_NE(ndTensorBuffer->GetData(), nullptr);
    EXPECT_NE(ndTensorBuffer->GetSize(), 0);
}

/* ------------------------------局部桩函数定义区 END ----------------------------------- */

/* ------------------------------用例定义区 START------------------------------- */
/*
 * 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_v3_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV3接口，创建NDTensorBuffer成功
 * 预置条件 :
 * 操作步骤:
 * 预期结果 :创建NDTensorBuffer 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_v3_success)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
 * 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_v2_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV2接口，创建NDTensorBuffer成功
 * 预置条件 : 打桩V3接口，使能回落执行V2接口流程
 * 操作步骤:
 * 预期结果 :创建NDTensorBuffer 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_v2_success)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
 * 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_v2_dim3_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV2接口，创建NDTensorBuffer成功
 * 预置条件 : 打桩V3接口，使能回落执行V2接口流程
 * 操作步骤:
 * 预期结果 :创建NDTensorBuffer 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_v2_dim3_success)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc;
    grayFloatDesc.dims = {1, INPUT_CHANNEL, RESIZE_WIDTH};
    grayFloatDesc.dataType = DataType::FLOAT32;
    grayFloatDesc.format = Format::ND;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
 * 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_v2_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV2接口，创建NDTensorBuffer成功
 * 预置条件 : 打桩V3、V2接口，使能回落执行HIAI_TensorBuffer_CreateWithDataType接口流程
 * 操作步骤:
 * 预期结果 :创建NDTensorBuffer 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_dataType_success)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
 * 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_v2_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV2接口，创建NDTensorBuffer成功
 * 预置条件 : 打桩V3、V2/dataType接口，使能回落执行HIAI_TensorBuffer_create接口流程
 * 操作步骤:
 * 预期结果 :创建NDTensorBuffer 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_v1_success)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_local_success)
{
    MOCKER(HIAI_Foundation_IsNpuSupport).stubs().will(returnValue((HIAI_NPU_SUPPORT_STATE)0));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_handle_v3_success
* 测试用例描述: 通过NDTensorDesc和handle V3接口，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_handle_v3_success)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    NativeHandle handle;
    handle.fd = 1;
    handle.size = dataSize;
    handle.offset = 0;

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, handle);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_handle_v1_success
* 测试用例描述: 通过NDTensorDesc和handle V1接口，创建NDTensorBuffer成功
* 预置条件 : 打桩V3接口，使能回落执行V1接口流程
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_handle_v1_success)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)1));

    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    NativeHandle handle;
    handle.fd = 1;
    handle.size = dataSize;
    handle.offset = 0;

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, handle);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v3_success
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v3_success)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * 1.5;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    CheckNDTensorBuffer(grayFloatBuffer);
}
/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v2_success
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer成功
* 预置条件 : 打桩V3接口，使能回落执行V2接口流程
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v2_success)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)1));

    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * 1.5;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v1_fail
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer失败
* 预置条件 :依次打桩V3、V2接口，再通过V1接口和size创建，size为0创建失败
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v1_fail)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 0;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v1_success
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer成功
* 预置条件 : 打桩V3接口，先走V2，但V2根据format创建失败，再通过V1接口和size创建
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v1_success)
{
    ControlClient::GetInstance().SetExpectValue(ClientKey::CLIENT_ImageBuffer_create, 1);
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)1));

    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * 1.5;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v0_fail_01
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer失败
* 预置条件 : 打桩V3先走V2，根据format创建失败,通过V1和size创建,size为0
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v0_fail_01)
{
    ControlClient::GetInstance().SetExpectValue(ClientKey::CLIENT_ImageBuffer_create, 1);
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)1));

    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 0;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v0_success_02
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer成功
* 预置条件 : 依次打桩V3、V2、V1接口，使不支持NPU
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v0_success_02)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)0));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * 1.5;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    CheckNDTensorBuffer(grayFloatBuffer);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_v0_fail_02
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer失败
* 预置条件 : 不支持NPU,size为0
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_v0_fail_02)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)0));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 0;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_datasize_success
* 测试用例描述: 通过NDTensorDesc data 和dataSize，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_datasize_success)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    void *data = malloc(dataSize);
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, data, dataSize);
    CheckNDTensorBuffer(grayFloatBuffer);
    free(data);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_desc_fail
* 测试用例描述: 通过NDTensorDesc，创建NDTensorBuffer失败
* 预置条件 : Convert2CTensorDesc 转换失败
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_desc_fail)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_NDTensorDesc *tnsorDesc = nullptr;
    MOCKER(&HIAI_NDTensorDesc_Create).stubs().will(returnValue(tnsorDesc));
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_handle_fail
* 测试用例描述: 通过NDTensorDesc和handle，创建NDTensorBuffer失败
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_handle_fail)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    NativeHandle handle;
    handle.fd = 1;
    handle.size = dataSize;
    handle.offset = 0;
    HIAI_NDTensorDesc *tnsorDesc = nullptr;
    MOCKER(&HIAI_NDTensorDesc_Create).stubs().will(returnValue(tnsorDesc));
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, handle);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_datasize_fail
* 测试用例描述: 通过NDTensorDesc data 和dataSize，创建NDTensorBuffer失败
* 预置条件 : Convert2CTensorDesc 转换失败
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_datasize_fail)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    void *data = malloc(dataSize);
    HIAI_NDTensorDesc *tnsorDesc = nullptr;
    MOCKER(&HIAI_NDTensorDesc_Create).stubs().will(returnValue(tnsorDesc));
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, data, dataSize);
    EXPECT_EQ(grayFloatBuffer, nullptr);
    free(data);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBuffer_with_format_fail_03
* 测试用例描述: 通过NDTensorDesc 和format，创建NDTensorBuffer失败
* 预置条件 : Convert2CTensorDesc 转换失败
* 操作步骤:
* 预期结果 :创建NDTensorBuffer失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBuffer_with_format_fail_03)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    HIAI_ImageFormat format = HIAI_ImageFormat::HIAI_YUV420SP_U8;
    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * 1.5;
    HIAI_NDTensorDesc *tnsorDesc = nullptr;
    MOCKER(&HIAI_NDTensorDesc_Create).stubs().will(returnValue(tnsorDesc));
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc, dataSize, format);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromSize_success_01
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromSize,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromSize，创建HIAI_NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 成功，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromSize_success_01)
{
    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromSize_success_02
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromSize，
*              ROM通过HIAI_TensorBuffer_create_v2 创建HIAI_NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 成功，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromSize_success_02)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)1));

    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromSize_fail_01
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromSize,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromSize，创建HIAI_NDTensorBuffer
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 失败，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromSize_fail_01)
{
    HIAI_NDTensorDesc* desc = NULL;

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, 0);

    EXPECT_EQ(desc, nullptr);
    EXPECT_EQ(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromSize_fail_02
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromSize,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromSize，创建HIAI_NDTensorBuffer
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 失败，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromSize_fail_02)
{
    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, 0);

    EXPECT_EQ(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_GetFd_success_01
* 测试用例描述: 构建buffer后，调用DDK侧HIAI_NDTensorBuffer_GetFd接口，rom侧调用 HIAI_NDTensorBuffer_GetFd得到fd值成功
* 预置条件 :
* 操作步骤:
* 预期结果 : HIAI_NDTensorBuffer_GetFd 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_GetFd_success_01)
{
    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    int32_t fd = HIAI_NDTensorBuffer_GetFd(buffer);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(fd, 1);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_GetFd_success_02
* 测试用例描述: 构建buffer后，调用DDK侧HIAI_NDTensorBuffer_GetFd接口，rom侧调用HIAI_TensorBuffer_getFdAndSize得到fd值成功
* 预置条件 :
* 操作步骤:
* 预期结果 :HIAI_NDTensorBuffer_GetFd 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_GetFd_success_02)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    int32_t fd = HIAI_NDTensorBuffer_GetFd(buffer);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(fd, 1);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
 * 测试用例名称: model_manager_HIAI_NDTensorBuffer_GetOriginFd_success_01
 * 测试用例描述: 构建buffer后，调用DDK侧HIAI_NDTensorBuffer_GetOriginFd接口，rom侧调用HIAI_NDTensorBuffer_GetOriginFd得到fd值成功
 * 预置条件 :
 * 操作步骤:
 * 预期结果 : HIAI_NDTensorBuffer_GetOriginFd 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_GetOriginFd_success_01)
{
    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    int32_t fd = HIAI_NDTensorBuffer_GetOriginFd(buffer);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(fd, 1);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
 * 测试用例名称: model_manager_HIAI_NDTensorBuffer_GetOriginFd_success_02
 * 测试用例描述: 构建buffer后，调用DDK侧HIAI_NDTensorBuffer_GetFd接口，rom侧调用HIAI_TensorBuffer_getOriginFd得到fd值成功
 * 预置条件 :
 * 操作步骤:
 * 预期结果 :HIAI_TensorBuffer_getOriginFd 成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_GetOriginFd_success_02)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    int32_t fd = HIAI_NDTensorBuffer_GetOriginFd(buffer);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(fd, 1);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_GetFd_nosupport_NPU_success
* 测试用例描述: 构建buffer后，调用DDK侧HIAI_NDTensorBuffer_GetFd接口，
*              rom侧没有HIAI_NDTensorBuffer_GetFd和HIAI_TensorBuffer_getFdAndSize符号表返回fd=-1
* 预置条件 :
* 操作步骤:
* 预期结果 :HIAI_NDTensorBuffer_GetFd 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_GetFd_nosupport_NPU_success)
{
    MOCKER(HIAI_Foundation_IsNpuSupport)
        .stubs()
        .will(returnValue((HIAI_NPU_SUPPORT_STATE)0));

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    size_t dataSize = HIAI_NDTensorDesc_GetByteSize(desc);

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromSize(desc, dataSize);

    int32_t fd = HIAI_NDTensorBuffer_GetFd(buffer);
    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);
    EXPECT_EQ(fd, -1);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_success_01
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromNativeHandle,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromNativeHandle，创建HIAI_NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 成功，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_success_01)
{
    int fd = 1;
    int size = 100;
    int offset = 1;

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);

    HIAI_NativeHandle* handle = HIAI_NativeHandle_Create(fd, size, offset);
    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNativeHandle(desc, handle);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
    HIAI_NativeHandle_Destroy(&handle);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_success_02
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromNativeHandle,
*              ROM侧调用HIAI_TensorBuffer_createTensorFromNativeHandle，创建HIAI_NDTensorBuffer
* 预置条件 :
* 操作步骤:
* 预期结果 :创建HIAI_NDTensorBuffer 成功，正确destroy buffer和desc
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_success_02)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    int fd = 1;
    int size = 100;
    int offset = 1;

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);

    HIAI_NativeHandle* handle = HIAI_NativeHandle_Create(fd, size, offset);
    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNativeHandle(desc, handle);

    EXPECT_NE(desc, nullptr);
    EXPECT_NE(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
    HIAI_NativeHandle_Destroy(&handle);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_fail_01
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromNativeHandle,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromNativeHandle，创建HIAI_NDTensorBuffer
* 预置条件 :
* 操作步骤:
* 预期结果 : HIAI_NDTensorDesc_Clone桩返回NULL,创建HIAI_NDTensorBuffer 失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_fail_01)
{
    int fd = 1;
    int size = 100;
    int offset = 1;

    vector<int32_t> dims = {1, INPUT_CHANNEL, RESIZE_WIDTH, RESIZE_HEIGHT};
    size_t dimNum = 4;
    HIAI_DataType dataType = HIAI_DataType(0);
    HIAI_Format format = HIAI_Format(0);

    HIAI_NDTensorDesc* desc = HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
    HIAI_NativeHandle* handle = HIAI_NativeHandle_Create(fd, size, offset);

    MOCKER(HIAI_NDTensorDesc_Clone)
        .stubs()
        .will(returnValue((HIAI_NDTensorDesc*)0));

    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNativeHandle(desc, handle);

    EXPECT_EQ(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NDTensorDesc_Destroy(&desc);
    HIAI_NativeHandle_Destroy(&handle);
}

/*
* 测试用例名称: model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_fail_02
* 测试用例描述: 通过desc 和dataSize，使用ddk侧接口HIAI_NDTensorBuffer_CreateFromNativeHandle,
*              ROM侧调用HIAI_NDTensorBuffer_CreateFromNativeHandle，创建HIAI_NDTensorBuffer
* 预置条件 :
* 操作步骤:
* 预期结果 : desc为nullptr,创建HIAI_NDTensorBuffer 失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_HIAI_NDTensorBuffer_CreateFromNativeHandle_fail_02)
{
    int fd = 1;
    int size = 100;
    int offset = 1;

    HIAI_NDTensorDesc* desc = nullptr;
    HIAI_NativeHandle* handle = HIAI_NativeHandle_Create(fd, size, offset);
    HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNativeHandle(desc, handle);

    EXPECT_EQ(buffer, nullptr);

    HIAI_NDTensorBuffer_Destroy(&buffer);
    HIAI_NativeHandle_Destroy(&handle);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBufferNoCopy_with_datasize_success
* 测试用例描述: 通过NDTensorDesc data 和dataSize，创建NDTensorBuffer成功
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 成功
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBufferNoCopy_with_datasize_success)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    void *data = malloc(dataSize);
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBufferNoCopy(grayFloatDesc, data, dataSize);
    CheckNDTensorBuffer(grayFloatBuffer);
    free(data);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBufferNoCopy_with_datasize_failed
* 测试用例描述: 通过NDTensorDesc data 和dataSize，创建NDTensorBuffer失败
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBufferNoCopy_with_datasize_failed)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    void *data = nullptr;
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBufferNoCopy(grayFloatDesc, data, dataSize);
    EXPECT_EQ(grayFloatBuffer, nullptr);
}

/*
* 测试用例名称: model_manager_CreateNDTensorBufferNoCopy_with_sizenotequal_failed
* 测试用例描述: 通过NDTensorDesc data 和dataSize，创建NDTensorBuffer失败
* 预置条件 :
* 操作步骤:
* 预期结果 :创建NDTensorBuffer 失败
* 修改历史 :
*/
TEST_F(Test_NDTensorBuffer, model_manager_CreateNDTensorBufferNoCopy_with_sizenotequal_failed)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    size_t dataSize = 1 * INPUT_CHANNEL * RESIZE_WIDTH * RESIZE_HEIGHT * sizeof(float);
    void *data = malloc(dataSize);
    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBufferNoCopy(grayFloatDesc, data, dataSize - 1);
    EXPECT_EQ(grayFloatBuffer, nullptr);
    free(data);
}

/*
 * 测试用例名称: model_manager_ChangeNDTensorBufferHandleToTensorBuffer_success
 * 测试用例描述: 通过NDTensorDesc和CreateSharedBufferV3接口，创建NDTensorBuffer成功,再将其转为HIAI_TensorBuffer
 * 预置条件 :
 * 操作步骤:
 * 预期结果 :转换成功
 * 修改历史 :
 */
TEST_F(Test_NDTensorBuffer, model_manager_ChangeNDTensorBufferHandleToTensorBuffer_success_1)
{
    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);

    std::unique_ptr<HIAI_NDTensorBuffer* []> cBuffers { new (std::nothrow) HIAI_NDTensorBuffer*[1] };

    cBuffers[0] = GetRawBufferFromNDTensorBuffer(grayFloatBuffer);
    bool ret = HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(cBuffers.get(), 1);
    EXPECT_EQ(true, ret);
}

TEST_F(Test_NDTensorBuffer, model_manager_ChangeNDTensorBufferHandleToTensorBuffer_success_2)
{
    MOCKER(HIAI_Foundation_GetSymbol)
        .stubs()
        .will(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_fail))
        .then(invoke(HIAI_Foundation_GetSymbol_stub_orginal));

    NDTensorDesc grayFloatDesc = CreateNDTensorDesc();

    shared_ptr<INDTensorBuffer> grayFloatBuffer = CreateNDTensorBuffer(grayFloatDesc);
    CheckNDTensorBuffer(grayFloatBuffer);
    std::unique_ptr<HIAI_NDTensorBuffer* []> cBuffers { new (std::nothrow) HIAI_NDTensorBuffer*[1] };

    cBuffers[0] = GetRawBufferFromNDTensorBuffer(grayFloatBuffer);
    bool ret = HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(cBuffers.get(), 1);
    EXPECT_EQ(true, ret);
}

TEST_F(Test_NDTensorBuffer, model_manager_ChangeNDTensorBufferHandleToTensorBuffer_success_3)
{
    NDTensorDesc grayFloatDesc1 = CreateNDTensorDesc();;
    shared_ptr<INDTensorBuffer> grayFloatBuffer1 = CreateNDTensorBuffer(grayFloatDesc1);

    NDTensorDesc grayFloatDesc2;
    grayFloatDesc2.dims = {1, INPUT_CHANNEL, RESIZE_WIDTH};
    grayFloatDesc2.dataType = DataType::FLOAT32;
    grayFloatDesc2.format = Format::ND;
    shared_ptr<INDTensorBuffer> grayFloatBuffer2 = CreateNDTensorBuffer(grayFloatDesc2);

    std::unique_ptr<HIAI_NDTensorBuffer* []> cBuffers { new (std::nothrow) HIAI_NDTensorBuffer*[2] };

    cBuffers[0] = GetRawBufferFromNDTensorBuffer(grayFloatBuffer1);
    cBuffers[1] = GetRawBufferFromNDTensorBuffer(grayFloatBuffer2);
    bool ret = HIAI_ChangeNDTensorBuffersHandleToTensorBuffers(cBuffers.get(), 2);
    EXPECT_EQ(false, ret);
}

TEST_F(Test_NDTensorBuffer, model_manager_HIAI_ModelManager_PreRun)
{
    HIAI_ModelRuntime runtime;
    std::unique_ptr<HIAI_NDTensorBuffer* []> cInputs { new (std::nothrow) HIAI_NDTensorBuffer*[1] };
    std::unique_ptr<HIAI_NDTensorBuffer* []> cOutputs { new (std::nothrow) HIAI_NDTensorBuffer*[1] };
    runtime.runtimeType = PLUGIN_MODEL_RUNTIME_HCL;
    runtime.handle = dlopen("libhiai_hcl_model_runtime.so", RTLD_NOW);
    HIAI_Status ret = HIAI_ModelManager_PreRun(&runtime, cInputs.get(), 0, cOutputs.get(), 0);
    EXPECT_EQ(HIAI_SUCCESS, ret);
    if (runtime.handle != nullptr) {
        dlclose(runtime.handle);
        runtime.handle = nullptr;
    }
}