#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <gtest/gtest.h>
#include <dlfcn.h>
#include <iostream>

#include "compatible/HiAiModelManagerService.h"
#include "util/version_util.h"
#include "cloud_service/cloud_service.h"

using namespace std;
using namespace hiai;
class GetVersion : public testing::Test {
public:
    void SetUp()
    {
        std::cout << __func__ << std::endl;
//        HIAI_Init();
    }

    void TearDown()
    {
        std::cout << __func__ << std::endl;
 //       HIAI_Deinit();
        GlobalMockObject::verify();
    }
};

/*
* 测试用例名称   : model_manager_get_version
* 测试用例描述： rom上有ai_client,获取版本号,再次设置环境变量不再生效
* 预置条件 : 打开ai_client成功
* 操作步骤: 获取版本号
* 预期结果 : 返回的版本号与桩内的相同，桩内的版本号为100.500.000.000
* 修改历史 :
*/
TEST_F(GetVersion, model_manager_get_version_with_aiclient)
{
    shared_ptr<AiModelMngerClient> client = make_shared<AiModelMngerClient>();
    string v1_version = (string)client->GetVersion();
    EXPECT_EQ("100.500.000.000", v1_version);

    const char* version = "100.330.100.120";
    setenv("ROM_HIAI_VERSION", version, 1);
    v1_version = (string)client->GetVersion();
    EXPECT_EQ("100.500.000.000", v1_version);
}

TEST_F(GetVersion, GetPluginVersionName)
{
    string version = (string)CloudService().GetComputeCapabilityVersion();
    EXPECT_EQ("", version);
}