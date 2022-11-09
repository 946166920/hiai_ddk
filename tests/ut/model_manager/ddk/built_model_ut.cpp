#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <memory>

#include "model_builder/om/model_builder_impl.h"
#include "model/built_model/built_model_impl.h"
#include "model_manager/core/model_manager_impl.h"
#include "c/compatible/HIAIModelManager.h"
#include "model_builder/model_builder_types.h"
#include "model_manager/model_manager_types.h"
#include "framework/c/hiai_tensor_aipp_para.h"

#include "control_generate_compute.h"

using testing::Test;
using namespace std;
using namespace hiai;

class Test_BuiltModel : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        GlobalMockObject::verify();
        ControlGenerateCompute::GetInstance().Clear();
    }
};

/*
 * 测试用例名称:model_manager_built_model_GetName_success
 * 测试用例描述:创建builtModel,获取name成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.获取name
 * 预期结果 :获取name成功
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetName_success)
{
    const string modelName = "get_brow_baltimoreB370";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);

    string getModelName = builtModel->GetName();
    EXPECT_EQ(true, getModelName == modelName);
}

/*
 * 测试用例名称:model_manager_built_model_GetName_fail
 * 测试用例描述:创建builtModel,获取name失败
 * 预置条件:模型未加载
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.获取name
 * 预期结果 :获取name失败
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetName_fail)
{
    const string modelName = "";
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    EXPECT_NE(builtModel, nullptr);

    string getModelName = builtModel->GetName();
    EXPECT_EQ(true, getModelName == modelName);
}

/*
* 测试用例名称:model_manager_built_model_GetInputTensorDescs_success
* 测试用例描述:创建builtModel,获取InputTensorDescs 成功
* 预置条件:创建builtModel
* 操作步骤:1.创建builder
          2.通过build申请builtModel
          3.获取InputTensorDescs
* 预期结果:获取InputTensorDescs 成功
* 修改历史:
*/
TEST_F(Test_BuiltModel, model_manager_built_model_GetInputTensorDescs_success)
{
    ControlGenerateCompute::GetInstance().SetStatus(RECOMPILE, ge::FAIL);
    const string modelName = "get_brow_baltimoreB370";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";

    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    vector<NDTensorDesc> inputDesc = builtModel->GetInputTensorDescs();
    EXPECT_EQ(1, inputDesc.size());
}

/*
* 测试用例名称:model_manager_built_model_GetOutputTensorDescs_success
* 测试用例描述:创建builtModel,获取OutputTensorDescs 成功
* 预置条件:创建builtModel
* 操作步骤:1.创建builder
          2.通过build申请builtModel
          3.获取OutputTensorDescs
* 预期结果:获取OutputTensorDescs 成功
* 修改历史:
*/
TEST_F(Test_BuiltModel, model_manager_built_model_GetOutputTensorDescs_success)
{
    const string modelName = "get_brow_baltimoreB370";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";

    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    vector<NDTensorDesc> outputDesc = builtModel->GetOutputTensorDescs();
    EXPECT_EQ(1, outputDesc.size());
}

/*
 * 测试用例名称:model_manager_built_model_SaveToExternalBuffer_success
 * 测试用例描述:创建builtModel,保存到ExternalBuffer成功
 * 预置条件:创建builtModel，申请IBuffer
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.申请内存，创建IBuffer
 *         4.保存到ExternalBuffer
 * 预期结果:保存到ExternalBuffer成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToExternalBuffer_success)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);

    const uint32_t defaultSize = 200; // default allocated buffer size, 200 MB
    const uint64_t megaSize = 1024; // MB size
    uint32_t size = defaultSize * megaSize;
    size_t realSize = 0;
    void* data = (void*)malloc(size);
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(data, size, false);
    ret = builtModel->SaveToExternalBuffer(buffer, realSize);
    EXPECT_EQ(SUCCESS, ret);
    free(data);
    data = nullptr;
}

/*
 * 测试用例名称:model_manager_built_model_SaveToExternalBuffer_fail_01
 * 测试用例描述:创建builtModel,保存到ExternalBuffer失败
 * 预置条件:size等于0
 * 操作步骤:1.创建builtModel
 *         2.申请内存，创建IBuffer
 *         3.保存到ExternalBuffer
 * 预期结果:保存到ExternalBuffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToExternalBuffer_fail_01)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    size_t realSize = 0;
    shared_ptr<IBuffer> buffer = nullptr;
    auto ret = builtModel->SaveToExternalBuffer(buffer, realSize);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToExternalBuffer_fail_02
 * 测试用例描述:创建builtModel,保存到ExternalBuffer失败
 * 预置条件:走hclruntime失败
 * 操作步骤:1.创建builtModel
 *         2.申请内存，创建IBuffer
 *         3.保存到ExternalBuffer
 * 预期结果:保存到ExternalBuffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToExternalBuffer_fail_02)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    auto ret = builtModel->RestoreFromFile(modelPath.c_str());
    EXPECT_EQ(SUCCESS, ret);

    size_t size = 1024;
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(size);
    size_t realSize = 0;
    MOCKER(&HIAI_BuiltModel_SaveToExternalBuffer).stubs().will(returnValue(FAILURE));
    ret = builtModel->SaveToExternalBuffer(buffer, realSize);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToBuffer_success
 * 测试用例描述:创建builtModel,保存到Buffer成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.保存到Buffer
 * 预期结果:保存到Buffer成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToBuffer_success)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_hcs_npucl.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    shared_ptr<IBuffer> buffer = nullptr;
    ret = builtModel->SaveToBuffer(buffer);
    EXPECT_EQ(SUCCESS, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToBuffer_fail_01
 * 测试用例描述:创建builtModel,保存到Buffer失败
 * 预置条件:模型未加载
 * 操作步骤:1.创建builtModel
 *         2.保存到Buffer
 * 预期结果:保存到Buffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToBuffer_fail_01)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    EXPECT_NE(builtModel, nullptr);
    shared_ptr<IBuffer> buffer = nullptr;
    auto ret = builtModel->SaveToBuffer(buffer);
    EXPECT_TRUE(ret != SUCCESS);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToBuffer_fail_02
 * 测试用例描述:创建builtModel,保存到Buffer失败
 * 预置条件:走hclruntime失败
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.保存到Buffer
 * 预期结果:保存到Buffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToBuffer_fail_02)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_hcs_npucl.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    shared_ptr<IBuffer> buffer = nullptr;
    MOCKER(&HIAI_BuiltModel_Save).stubs().will(returnValue(FAILURE));
    ret = builtModel->SaveToBuffer(buffer);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_success
 * 测试用例描述:创建builtModel,保存到文件成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.保存到文件
 * 预期结果:保存到文件成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_success)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);

    const string filename = "out/TestModel.om";
    ret = builtModel->SaveToFile(filename.c_str());
    EXPECT_EQ(SUCCESS, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_fail
 * 测试用例描述:创建builtModel,保存到文件失败
 * 预置条件:模型未加载
 * 操作步骤:1.创建builtModel
 *         2.保存到文件
 * 预期结果:保存到文件失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    EXPECT_NE(builtModel, nullptr);

    const string filename = "out/TestModel.om";
    auto ret = builtModel->SaveToFile(filename.c_str());
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_fail_02
 * 测试用例描述:创建builtModel,保存到文件失败
 * 预置条件:走hclruntime失败
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.保存到文件
 * 预期结果:保存到文件失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_fail_02)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_fail_03
 * 测试用例描述:创建builtModel,保存文件失败
 * 预置条件:未执行build
 * 操作步骤:1.创建builtModel
 *         2.保存到文件
 * 预期结果:保存到文件失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_fail_03)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto ret = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->SaveToFile(nullptr);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_fail_04
 * 测试用例描述:创建builtModel,保存文件失败
 * 预置条件:文件指针为空
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.调用HIAI_BuiltModel_SaveToFile保存到文件
 * 预期结果:保存到文件失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_fail_04)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
}

/*
 * 测试用例名称:model_manager_built_model_SaveToFile_fail_05
 * 测试用例描述:创建builtModel,保存文件失败
 * 预置条件:文件指针为空
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.保存到文件
 * 预期结果:保存到文件失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_SaveToFile_fail_05)
{
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);

    ret = builtModel->SaveToFile(nullptr);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromBuffer_fail
 * 测试用例描述:创建builtModel,RestoreBuffer失败
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.Restore Buffer
 * 预期结果:RestoreBuffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromBuffer_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    shared_ptr<IBuffer> buffer = nullptr;
    auto ret = builtModel->RestoreFromBuffer(buffer);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromBuffer_fail_02
 * 测试用例描述:创建builtModel,RestoreBuffer失败
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builtModel
 *         2.创建IBuffer
 *         3.Restore Buffer
 * 预期结果:RestoreBuffer失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromBuffer_fail_02)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    size_t size = 1024;
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(size);
    EXPECT_NE(buffer, nullptr);
    EXPECT_NE(buffer->GetData(), nullptr);
    EXPECT_EQ(buffer->GetSize(), size);
    auto ret = builtModel->RestoreFromBuffer(buffer);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromFile_hcs_success
 * 测试用例描述:创建builtModel,Restore File成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.Restore File
 * 预期结果:Restore File成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromFile_hcs_success)
{
    ControlGenerateCompute::GetInstance().SetStatus(RECOMPILE, ge::FAIL);
    const string modelName = "tf_softmax_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto ret = builtModel->RestoreFromFile(modelPath.c_str());
    EXPECT_EQ(SUCCESS, ret);
    std::vector<NDTensorDesc> inputDesc = builtModel->GetInputTensorDescs();
    EXPECT_EQ(1, inputDesc.size());
    std::vector<NDTensorDesc> outputDesc = builtModel->GetOutputTensorDescs();
    EXPECT_EQ(1, outputDesc.size());
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromFile_hcl_success
 * 测试用例描述:创建builtModel,Restore File成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.Restore File
 * 预期结果:Restore File成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromFile_hcl_success)
{
    const string modelName = "tf_softmax_no_infershaped.om";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto ret = builtModel->RestoreFromFile(modelPath.c_str());
    EXPECT_EQ(SUCCESS, ret);
    std::vector<NDTensorDesc> inputDesc = builtModel->GetInputTensorDescs();
    EXPECT_EQ(2, inputDesc.size());
    std::vector<NDTensorDesc> outputDesc = builtModel->GetOutputTensorDescs();
    EXPECT_EQ(1, outputDesc.size());
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromFile_fail
 * 测试用例描述:创建builtModel,Restore File失败
 * 预置条件:restor file路径为nullptr
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.Restore File
 * 预期结果:Restore File失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromFile_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto ret = builtModel->RestoreFromFile(nullptr);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_RestoreFromFile_fail_02
 * 测试用例描述:创建builtModel,Restore File失败
 * 预置条件:模型未加载
 * 操作步骤:1.创建builtModel
 *         2.Restore File
 * 预期结果:Restore File失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_RestoreFromFile_fail_02)
{
    const string invaildModelPath = "../bin/llt/framework/domi/modelmanager/om/xxx.om";
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    EXPECT_NE(builtModel, nullptr);
    auto ret = builtModel->RestoreFromFile(invaildModelPath.c_str());
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_CheckCompatibility_success
 * 测试用例描述:创建builtModel,检查兼容性成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.检查兼容性
 * 预期结果 :检查兼容性成功
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_CheckCompatibility_success)
{
    const string modelName = "NPUCL";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    bool isModelCompatibility = false;
    ret = builtModel->CheckCompatibility(isModelCompatibility);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(true, isModelCompatibility);
}

/*
 * 测试用例名称:model_manager_built_model_CheckCompatibility_fail
 * 测试用例描述:创建builtModel,检查兼容性失败
 * 预置条件:模型未加载
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.检查兼容性
 * 预期结果 :检查兼容性失败
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_CheckCompatibility_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    EXPECT_NE(builtModel, nullptr);
    bool isModelCompatibility = false;
    auto ret = builtModel->CheckCompatibility(isModelCompatibility);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_CheckCompatibility_fail_02
 * 测试用例描述:创建builtModel,检查兼容性失败
 * 预置条件:走hclruntime失败
 * 操作步骤:1.创建builtModel
 *         2.检查兼容性
 * 预期结果 :检查兼容性失败
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_CheckCompatibility_fail_02)
{
    const string modelName = "NPUCL";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    MOCKER(&HIAI_BuiltModel_CheckCompatibility).stubs().will(returnValue(FAILURE));
    bool isModelCompatibility = false;
    ret = builtModel->CheckCompatibility(isModelCompatibility);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_GetTensorAippInfo_success
 * 测试用例描述:创建builtModel,获取AippInfo成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.获取AippInfo
 * 预期结果 :获取AippInfo成功
 * 修改历史 :
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetTensorAippInfo_success)
{
    ControlGenerateCompute::GetInstance().SetStatus(RECOMPILE, ge::FAIL);

    const string modelName = "NPUCL";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    shared_ptr<IBuffer> buffer = nullptr;

    int32_t inputIndex = 0;
    uint32_t batchNum = 0;
    uint32_t aippNodesCount = 0;
    ret =
        dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetTensorAippInfo(inputIndex, &batchNum, &aippNodesCount);
    EXPECT_EQ(SUCCESS, ret);
}


/*
 * 测试用例名称:model_manager_built_model_GetTensorAippInfo_fail
 * 测试用例描述:创建builtModel,获取AippInfo失败
 * 预置条件:未执行build
 * 操作步骤:1.创建builtModel
 *         2.获取AippInfo
 * 预期结果:获取AippInfo失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetTensorAippInfo_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    int32_t inputIndex = 0;
    uint32_t batchNum = 0;
    uint32_t aippNodesCount = 0;
    auto ret =
        dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetTensorAippInfo(inputIndex, &batchNum, &aippNodesCount);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称:model_manager_built_model_GetTensorAippPara_inputIndex_success
 * 测试用例描述:创建builtModel,获取AippPara成功
 * 预置条件:创建builtModel
 * 操作步骤:1.创建builder
 *         2.通过build申请builtModel
 *         3.获取AippPara
 * 预期结果:获取AippPara成功
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetTensorAippPara_inputIndex_success)
{
    ControlGenerateCompute::GetInstance().SetStatus(RECOMPILE, ge::FAIL);

    const string modelName = "NPUCL";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    int32_t inputIndex = 0;
    vector<void*> aippParas;
    ret = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetTensorAippPara(inputIndex, aippParas);
    EXPECT_EQ(SUCCESS, ret);
    for (auto& aippPara : aippParas)
        if (aippPara != nullptr) {
            HIAI_TensorAippPara_Destroy(&aippPara);
            delete aippPara;
            aippPara = nullptr;
        }
}

/*
 * 测试用例名称:model_manager_built_model_GetTensorAippPara_inputIndex_fail
 * 测试用例描述:创建builtModel,获取AippPara失败
 * 预置条件:未执行build
 * 操作步骤:1.创建builtModel
 *         2.获取AippPara
 * 预期结果:获取AippPara失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetTensorAippPara_inputIndex_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    int32_t inputIndex = 0;
    vector<void*> aippParas;
    auto ret = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetTensorAippPara(inputIndex, aippParas);
    EXPECT_EQ(FAILURE, ret);
}

// 构造aippParaNum=0的场景
int HIAI_ModelManger_getTensorAippInfo_stub(HIAI_ModelManager* manager, const char* modelName, unsigned int index,
    unsigned int* aippCount, unsigned int* batchCount)
{
    std::cout << __func__ << std::endl;
    if (index > 0) {
        return 1;
    }
    *aippCount = 0;
    *batchCount = 1;
    return 0;
}
/*
 * 测试用例名称:model_manager_built_model_GetTensorAippPara_inputIndex_success_02
 * 测试用例描述:创建builtModel,获取AippPara成功
 * 预置条件:aippParaNum为0
 * 操作步骤:1.创建builtModel
 *         2.获取AippPara
 * 预期结果:获取AippPara为空
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetTensorAippPara_inputIndex_success_02)
{
    ControlGenerateCompute::GetInstance().SetStatus(RECOMPILE, ge::FAIL);

    MOCKER(HIAI_ModelManger_getTensorAippInfo)
    .stubs()
    .will(invoke(HIAI_ModelManger_getTensorAippInfo_stub));
    const string modelName = "NPUCL";
    const string modelPath = "../bin/llt/framework/domi/modelmanager/om/get_brow_baltimoreB370.om";
    shared_ptr<IModelBuilder> builder = CreateModelBuilder();
    ModelBuildOptions buildoptions;
    shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder->Build(buildoptions, modelName, modelPath, builtModel);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(builtModel, nullptr);
    int32_t inputIndex = 0;
    vector<void*> aippParas;
    ret = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetTensorAippPara(inputIndex, aippParas);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_EQ(aippParas.empty(), true);
}

/*
 * 测试用例名称:model_manager_built_model_GetTensorAippPara_inputIndex_fail
 * 测试用例描述:创建builtModel,获取InputTensorDescs失败
 * 预置条件:未执行build
 * 操作步骤:1.创建builtModel
 *         2.获取InputTensorDescs
 * 预期结果:获取InputTensorDescs失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetInputTensorDescs_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto descVec = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetInputTensorDescs();
    EXPECT_EQ(descVec.size(), 0);
}

/*
 * 测试用例名称:model_manager_built_model_GetOutputTensorDescs_fail
 * 测试用例描述:创建builtModel,获取AippPara失败
 * 预置条件:未执行build
 * 操作步骤:1.创建builtModel
 *         2.获取OutputTensorDescs
 * 预期结果:获取OutputTensorDescs失败
 * 修改历史:
 */
TEST_F(Test_BuiltModel, model_manager_built_model_GetOutputTensorDescs_fail)
{
    shared_ptr<IBuiltModel> builtModel = CreateBuiltModel();
    auto descVec = dynamic_pointer_cast<BuiltModelImpl>(builtModel)->GetOutputTensorDescs();
    EXPECT_EQ(descVec.size(), 0);
}
