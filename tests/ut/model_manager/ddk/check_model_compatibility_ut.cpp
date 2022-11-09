#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <iostream>

#include "stub_load_models.h"
#include "compatible/HiAiModelManagerService.h"
#include "compatible/HiAiModelManagerType.h"
#include "model_manager/model_manager.h"
#include "compatible/AiModelBuilder.h"

using namespace std;
using namespace hiai;
using namespace testing;

class CheckModelCompatibility : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

struct ModelInfo {
    string modelName;
    string modelPath;
    int32_t frequency;
    vector<TensorDimension> inputDims;
    DynamicShapeConfig dynamicShape;
    PrecisionMode precisionMode;
};

/*
* 测试用例名称 : model_manager_check_model_compatibility_001
* 测试用例描述：测试CheckModelCompatibility接口
* 预置条件 : 准备infershape类型模型文件
* 操作步骤 :
            1. 创建AiModelBuilder对象
            2. 调用AiModelBuilder对象提供的InputMemBufferCreate创建MemBuffer
            3. 设置AiModelDescription
            4. 创建AiModelMngerClient对象
            5. 调用AiModelMngerClient对象的CheckModelCompatibility接口
* 预期结果 : 成功
*/
TEST_F(CheckModelCompatibility, model_manager_check_model_compatibility_001)
{
    AIStatus ret = AI_SUCCESS;
    bool isModelCompatibility = false;

    TensorDimension dims(1,2,3,4);
    DynamicShapeConfig dynamicShape;
    dynamicShape.enable = true;
    dynamicShape.maxCachedNum = 1;

    ModelInfo info = {
        .modelName = "tf_softmax_infershaped",
        .modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om",
        .frequency = 3,
        .inputDims = {dims},
        .dynamicShape = dynamicShape,
    };

    shared_ptr<AiModelBuilder> builder = make_shared<AiModelBuilder>();
    ASSERT_TRUE(builder != nullptr);

    MemBuffer* buffer = builder->InputMemBufferCreate(info.modelPath);
    ASSERT_TRUE(buffer != nullptr);

    AiModelDescription desc(info.modelName, info.frequency, 0, 0, 0);
    ret = ret || desc.SetModelBuffer(buffer->GetMemBufferData(), buffer->GetMemBufferSize());
    ret = ret || desc.SetInputDims(info.inputDims);
    ret = ret || desc.SetDynamicShapeConfig(info.dynamicShape);
    ret = ret || desc.SetPrecisionMode(info.precisionMode);

    shared_ptr<AiModelMngerClient> client = make_shared<AiModelMngerClient>();
    client->Init(nullptr);

    ret = client->CheckModelCompatibility(desc, isModelCompatibility);
    EXPECT_EQ(AI_SUCCESS, ret);
    builder->MemBufferDestroy(buffer);
}
