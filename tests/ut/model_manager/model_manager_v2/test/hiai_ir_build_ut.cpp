#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <dlfcn.h>
#include "model_builder/hiai_ir_build.h"
#include "graph/graph.h"
#include "graph/operator_hiai_reg.h"
#include "graph/op/all_ops.h"
#include "graph/op/image_para_utils.h"
#include "tensor/image_format.h"
#include "tensor/image_process_config_types.h"
#include "base/error_types.h"

using namespace std;
using namespace hiai;
using namespace ge;

class HiaiIRBuildUt : public testing::Test {
public:
    void SetUp()
    {
        handle_ = dlopen("libhiai_ir_build_aipp_ddk.so", RTLD_NOW);
        MOCKER(&dlopen).stubs().will(returnValue(handle_));
    }

    Status BuildIRAPIModel(Graph& graph)
    {
        shared_ptr<Model> model = make_shared<Model>("model", "ir_model");
        model->SetGraph(graph);

        HiaiIrBuild builder;
        ModelBuildOptions options;
        std::shared_ptr<IBuiltModel> builtModel = nullptr;
        auto ret = builder.Build(options, "ir_model", model, builtModel);
        EXPECT_TRUE(builtModel != nullptr);
        return ret;
    }

    void TearDown()
    {
        if (handle_ != nullptr) {
            dlclose(handle_);
            handle_ = nullptr;
        }
        GlobalMockObject::verify();
    }
public:
    void* handle_{nullptr};
};

hiai::op::Data CreateData(Shape& shape, const std::string& name)
{
    ge::TensorDesc dataDesc(shape, FORMAT_NCHW, DT_UINT8);
    hiai::op::Data data = hiai::op::Data(name);
    data.update_input_desc_x(dataDesc);
    return data;
}

hiai::op::ConfigData CreateCrop(uint32_t cropStartPosW, uint32_t cropStartPosH, uint32_t cropSizeW, uint32_t cropSizeH)
{
    CropPara cropPara;
    cropPara.cropStartPosW = cropStartPosW;
    cropPara.cropStartPosH = cropStartPosH;
    cropPara.cropSizeW = cropSizeW;
    cropPara.cropSizeH = cropSizeH;

    return CreateConfigData(cropPara, "cropConfigData", hiai::op::ImageCropV2::TYPE);
}

/*
 * 测试用例名称   : model_manager_build_ir_model
 * 测试用例描述: ir api 构建模型进行编译
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(HiaiIRBuildUt, model_manager_build_ir_model)
{
    Graph graph("bigGraph");
    TensorDesc xDesc(Shape({1, 2, 3, 4}), FORMAT_NCHW, DT_FLOAT);
    hiai::op::Data x = hiai::op::Data("x");
    x.update_input_desc_x(xDesc);
    x.update_output_desc_y(xDesc);

    auto softmax = hiai::op::Softmax("softmax").set_input_x(x).set_attr_axis(1);

    std::vector<Operator> inputs {x};
    std::vector<Operator> outputs {softmax};
    graph.SetInputs(inputs).SetOutputs(outputs);

    auto ret = BuildIRAPIModel(graph);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_crop
 * 测试用例描述: ir api 构建模型进行编译, 有动态crop算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(HiaiIRBuildUt, model_mannger_build_ir_model_aipp_dynamic_crop)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 3, 256, 256});
    hiai::op::Data data = CreateData(shape, "data1");
    hiai::op::ConfigData cropConfigData = CreateCrop(0, 0, 320, 150);
    hiai::op::Data data2 = CreateData(shape, "data2");
    cropConfigData.set_input_x(data2);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2").set_input_x(data).set_input_param(cropConfigData);

    std::vector<Operator> inputs {data, cropConfigData};
    std::vector<Operator> outputs {imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    auto ret = BuildIRAPIModel(graph);
    EXPECT_EQ(hiai::SUCCESS, ret);
}