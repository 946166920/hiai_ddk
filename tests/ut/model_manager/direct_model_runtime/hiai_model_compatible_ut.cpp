#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <memory>
#include <vector>
#include <string>
#include <climits>
#include <iostream>

#include "model_runtime/direct/model_compatible/hiai_model_compatible.h"
#include "util/hiai_foundation_dl_helper.h"
#include "compatible/ir_transformer.h"
#include "securec.h"
#include "common/helper/om_file_helper.h"
#include "omg/quantize_optimizer/quantize_util.h"

using namespace std;
using namespace hiai;

struct CompatibleTestParams {
    string modelFile;
    string romVersion;
    HIAI_Status expectValue;
};

static vector<CompatibleTestParams> g_CompatibleTestParams = {
    { "", "", HIAI_FAILURE }, // 入参为空场景
    { "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om", "", HIAI_SUCCESS }, // IR_GRAPH_MODEL模型, 无需兼容性处理场景
    // STANDARD_IR_GRAPH_MODEL模型,ROM支持Deconv场景, version == 100.320.010.xxx
    { "bin/llt/framework/domi/modelmanager/tf_deconv_size_1.om", "100.320.010.000", HIAI_FAILURE },
    // STANDARD_IR_GRAPH_MODEL模型,ROM不支持Deconv场景, version < 100.320.010.xxx
    { "bin/llt/framework/domi/modelmanager/tf_deconv_size_1.om", "100.320.009.000", HIAI_SUCCESS },
        // STANDARD_IR_GRAPH_MODEL模型,ROM不支持Deconv场景, version > 100.320.010.021
    { "bin/llt/framework/domi/modelmanager/tf_deconv_size_1.om", "100.320.010.022", HIAI_SUCCESS },
    // STANDARD_IR_GRAPH_MODEL模型,ROM不支持Deconv场景, version = 100.320.011.017
    { "bin/llt/framework/domi/modelmanager/tf_deconv_size_1.om", "100.320.011.017", HIAI_FAILURE },
     // STANDARD_IR_GRAPH_MODEL模型,romVerions< 100.500.010.038
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.330.010.037", HIAI_FAILURE },
     // STANDARD_IR_GRAPH_MODEL模型,romVerions< 100.500.010.038
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.330.010.038", HIAI_SUCCESS },
     // STANDARD_IR_GRAPH_MODEL模型,romVerions< 100.330.011.038
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.330.011.037", HIAI_FAILURE },
    // STANDARD_IR_GRAPH_MODEL模型,romVerions < 100.500.010.038 场景
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.500.010.037", HIAI_FAILURE },
    // STANDARD_IR_GRAPH_MODEL模型,romVerions  100.500.010.038
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.500.010.038", HIAI_SUCCESS },
     // STANDARD_IR_GRAPH_MODEL模型,romVerions< 100.510.010.012
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.510.010.012", HIAI_SUCCESS },
    // STANDARD_IR_GRAPH_MODEL模型,romVerions> 100.320.xxx.xxx
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.320.000.000", HIAI_FAILURE },
    // STANDARD_IR_GRAPH_MODEL模型,romVerions = 100.310.xxx.xxx
    { "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", "100.310.000.000", HIAI_SUCCESS },
    { "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om", "", HIAI_SUCCESS },
    // STANDARD_IR_GRAPH_MODEL模型, 包含MODEL_CONFIG
    { "test/framework/ut/omg/data/TF_max_pool-deconv.om", "100.500.010.038", HIAI_SUCCESS },
    { "bin/llt/framework/domi/modelmanager/tf_conv_opversion_5.om", "100.320.010.000", HIAI_SUCCESS },
    { "bin/llt/framework/domi/modelmanager/tf_softmax_no_infershaped.om", "100.320.010.000", HIAI_SUCCESS },
    // 含有非法字段(非proto定义的)变异模型，在proto解析时候被丢弃,导致proto byte size和proto 文件大小不一致
    {"bin/llt/framework/domi/modelmanager/googlenet_error.om", "100.510.010.011", HIAI_FAILURE},
    // 含有非法字段(非proto定义的,全为0)变异模型，在proto解析时候被丢弃,导致proto byte size和proto 文件大小不一致
    {"bin/llt/framework/domi/modelmanager/resnet18_modeldef_0.om", "100.510.010.011", HIAI_FAILURE},

};

class ModelCompatible_UTest : public testing::TestWithParam<CompatibleTestParams> {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        GlobalMockObject::verify();
        unsetenv("ROM_HIAI_VERSION");

        if (input != nullptr) {
            HIAI_MemBuffer_destroy(input);
            input = nullptr;
        }

        if (output != nullptr) {
            HIAI_MemBuffer_destroy(output);
            output = nullptr;
        }
    }
private:
    HIAI_MemBuffer* input = nullptr;
    HIAI_MemBuffer* output = nullptr;
};

INSTANTIATE_TEST_CASE_P(Compatible, ModelCompatible_UTest, testing::ValuesIn(g_CompatibleTestParams));

static Status CheckOneSideQuantize(ge::ComputeGraph& graph, bool& isOneSideQuant)
{
    (void)graph;
    isOneSideQuant = true;
    return HIAI_SUCCESS;
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述:
    1.入参input为空场景
    2.非标准IR模型场景
    3.标准IR模型场景
* 预置条件: 模型文件
* 操作步骤:
    1. 执行CompatibleUtil::MakeModelLoadable保存模型输出为预期输出
    2. 行HIAI_MakeDirectCompatibleModel生成实际输出
    3. 比较实际输出与预期输出,预期应该相同
* 预期结果:
* 修改历史:
*/
TEST_P(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_001)
{
    HIAI_Status ret = HIAI_SUCCESS;
    CompatibleTestParams param = GetParam();
    string romVersion = param.romVersion;
    if (!romVersion.empty()) {
        setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);
    }

    string modelFile = param.modelFile;
    cout << "++++++file:" << modelFile << endl;
    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret == param.expectValue);
    if (output != nullptr) {
        cout << "++++++" << modelFile << " inputSize:" << input->size << " outputSize:" << output->size << endl;
    }
#if 0
    if (ret == HIAI_SUCCESS && output != nullptr) {
        CommonUtils::GetInstance()->Init();
        ExecuteOption option;
        shared_ptr<IModelBufferOld> buffer = nullptr;
        HIAI_CreateModelBufferFromFile("test", modelFile.c_str(), std::move(option), buffer);

        std::shared_ptr<IModelBufferOld> loadableModel = nullptr;
        ModelUseItfType modelType = MODEL_USE_C_ITF;
        ret = CompatibleUtil::MakeModelLoadable(buffer, loadableModel, modelType);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
        EXPECT_TRUE(loadableModel->Size() == output->size);
        cout << "++++++file:" << modelFile << " input size:" << input->size <<
            " output size:" << output->size << " expect output size:" << loadableModel->Size() << endl;
    }
#endif
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述:
    memcpy_s异常场景
* 预置条件: 模型文件
* 预期结果:
* 修改历史:
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_002)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.500.010.038";
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om";
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);
    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());

    MOCKER(memcpy_s).stubs().will(returnValue((int)-1));

    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述:
    IRTransformer::IsCompatible失败场景
* 预置条件: 模型文件
* 预期结果:
* 修改历史:
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.500.000.000";
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_conv_merged.om";
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);

    MOCKER(&IRTransformer::IsCompatible).stubs().will(returnValue(false));

    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述:
    模型数据异常场景
* 预置条件: 模型文件
* 预期结果:
* 修改历史:
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_004)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.320.009.000";
    if (!romVersion.empty()) {
        setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);
    }

    string modelFile = "bin/llt/framework/domi/modelmanager/tf_deconv_size_1.om";
    cout << "++++++file:" << modelFile << endl;
    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    input->size -= 10;

    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}


/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述: 获取单边量化属性失败
* 预置条件: 模型文件
* 预期结果: 失败
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_005)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.500.000.000";
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_conv_merged.om";
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);

    MOCKER(&IRTransformer::IsCompatible).stubs().will(returnValue(false));
    MOCKER(&hiai::QuantizeUtil::CheckOneSideQuantize).stubs().will(returnValue(HIAI_FAILURE));

    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述: 获取单边量化成功
* 预置条件: 模型文件
* 预期结果: 成功
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_006)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.500.000.000";
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_conv_merged.om";
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);

    MOCKER(&IRTransformer::IsCompatible).stubs().will(returnValue(false));
    MOCKER(&hiai::QuantizeUtil::CheckOneSideQuantize).stubs().will(invoke(CheckOneSideQuantize));

    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_MakeDirectCompatibleModel
* 测试用例描述: 反量化模型失败
* 预置条件: 模型文件
* 预期结果: 失败
*/
TEST_F(ModelCompatible_UTest, HIAI_MakeDirectCompatibleModel_007)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string romVersion = "100.500.000.000";
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_conv_merged.om";
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);

    MOCKER(&IRTransformer::IsCompatible).stubs().will(returnValue(false));
    MOCKER(&hiai::QuantizeUtil::CheckOneSideQuantize).stubs().will(invoke(CheckOneSideQuantize));
    MOCKER(&hiai::QuantizeUtil::DequantizeComputeGraph).stubs().will(returnValue(HIAI_FAILURE));

    input = HIAI_MemBuffer_create_from_file(modelFile.c_str());
    ret = HIAI_MakeDirectCompatibleModel(input, &output);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

