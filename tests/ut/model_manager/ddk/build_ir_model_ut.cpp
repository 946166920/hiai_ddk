#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <mockcpp/mockable.h>
#include <dlfcn.h>
#include "hiai_ir_build.h"
#include "model_manager.h"
#include "graph/graph.h"
#include "graph/model.h"
#include "graph/operator_hiai_reg.h"
#include "graph/op/all_ops.h"
#include "graph/op/image_para_utils.h"
#include "framework/tensor/image_format.h"
#include "framework/tensor/image_process_config_types.h"

#include "control_generate_compute.h"

using testing::Test;
using namespace std;
using namespace hiai;
using namespace ge;

class BuildIRModel : public testing::Test {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        ControlGenerateCompute::GetInstance().Clear();
    }
};

hiai::op::Data CreateData(Shape& shape, const std::string& name)
{
    ge::TensorDesc dataDesc(shape, FORMAT_NCHW, DT_UINT8);
    hiai::op::Data data = hiai::op::Data(name);
    data.update_input_desc_x(dataDesc);
    return data;
}

/*
 * 测试用例名称   : model_manager_build_ir_model_format_default
 * 测试用例描述： ir api 构建模型进行编译， format为默认
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_manager_build_ir_model_format_default)
{
    ControlGenerateCompute::GetInstance().SetStatus(COMPILE, ge::FAIL);

    Graph graph("bigGraph");
    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
    hiai::op::Data x = hiai::op::Data("x");
    x.update_input_desc_x(xDesc);
    x.update_output_desc_y(xDesc);

    auto softmax = hiai::op::Softmax("softmax").set_input_x(x).set_attr_axis(1);

    std::vector<Operator> inputs {x};
    std::vector<Operator> outputs {softmax};
    graph.SetInputs(inputs).SetOutputs(outputs);

    shared_ptr<Model> model = make_shared<Model>("model", "ir_model");
    model->SetGraph(graph);

    HiaiIrBuild builder;
    ModelBuildOptions options;
    options.estimatedOutputSize = 2264;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder.Build(options, "ir_model", model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);

    EXPECT_TRUE(builtModel != nullptr);
    size_t size = 2264;
    shared_ptr<IBuffer> buffer = CreateLocalBuffer(size);
    size_t realSize = 0;
    ret = builtModel->SaveToExternalBuffer(buffer, realSize);
    EXPECT_EQ(hiai::SUCCESS, ret);

    ret = builtModel->SaveToFile("./model_manager_build_ir_model_format_default.om");
    EXPECT_EQ(hiai::SUCCESS, ret);

    std::shared_ptr<IBuffer> buffer2;
    ret = builtModel->SaveToBuffer(buffer2);
    EXPECT_EQ(hiai::SUCCESS, ret);
    EXPECT_EQ(2264, buffer2->GetSize());

    std::shared_ptr<IModelManager> manager = CreateModelManager();
    ModelInitOptions initOptions;
    ret = manager->Init(initOptions, builtModel, nullptr);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_manager_build_ir_model_format_nchw
 * 测试用例描述： ir api 构建模型进行编译， format为nchw
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_manager_build_ir_model_format_nchw)
{
    Graph graph("bigGraph");
    TensorDesc xDesc(Shape({4, 5, 6, 7}), FORMAT_NCHW, DT_FLOAT);
    hiai::op::Data x = hiai::op::Data("x");
    x.update_input_desc_x(xDesc);
    x.update_output_desc_y(xDesc);

    auto softmax = hiai::op::Softmax("softmax").set_input_x(x).set_attr_axis(1);

    std::vector<Operator> inputs{x};
    std::vector<Operator> outputs{softmax};
    graph.SetInputs(inputs).SetOutputs(outputs);

    shared_ptr<Model> model = make_shared<Model>("model", "ir_model");
    model->SetGraph(graph);

    HiaiIrBuild builder;
    ModelBuildOptions options;
    options.formatMode = FormatMode::USE_NCHW;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    auto ret = builder.Build(options, "ir_model", model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_crop
 * 测试用例描述： ir api 构建模型进行编译, 有动态crop算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_crop)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara cropPara;
    cropPara.cropStartPosW = 0;
    cropPara.cropStartPosH = 0;
    cropPara.cropSizeW = 320;
    cropPara.cropSizeH = 150;

    hiai::op::ConfigData cropConfigData = CreateConfigData(cropPara, "cropConfigData", hiai::op::ImageCropV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    cropConfigData.set_input_x(data2);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);

    std::vector<Operator> inputs{data, cropConfigData};
    std::vector<Operator> outputs{imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynmic_resize
 * 测试用例描述： ir api 构建模型进行编译, 有动态resize算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynmic_resize)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 3, 400, 400});
    hiai::op::Data data = CreateData(shape, "data1");
    ResizePara para;
    para.resizeOutputSizeW = 224;
    para.resizeOutputSizeH = 224;

    hiai::op::ConfigData resizeConfigData = CreateConfigData(para, "resizeConfigData", hiai::op::ImageResizeV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    resizeConfigData.set_input_x(data2);

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(data)
                       .set_input_param(resizeConfigData);

    std::vector<Operator> inputs{data, resizeConfigData};
    std::vector<Operator> outputs{imageresizev2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_csc
 * 测试用例描述： ir api 构建模型进行编译, 有动态csc算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_csc)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 3, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CscPara para;
    para.outputFormat = ImageFormat::RGB888;

    hiai::op::ConfigData cscConfigData = CreateConfigData(para, "cscConfigData", hiai::op::ImageColorSpaceConvertionV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    cscConfigData.set_input_x(data2);

    auto cscV2 = hiai::op::ImageColorSpaceConvertionV2("cscV2")
                       .set_input_x(data)
                       .set_input_param(cscConfigData);

    std::vector<Operator> inputs{data, cscConfigData};
    std::vector<Operator> outputs{cscV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_swap
 * 测试用例描述： ir api 构建模型进行编译, 有动态swap算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_swap)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    ChannelSwapPara para;
    para.rbuvSwapSwitch = true;
    para.axSwapSwitch = true;

    hiai::op::ConfigData channelSwapConfigData = CreateConfigData(para, "channelSwapConfigData", hiai::op::ImageChannelSwapV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    channelSwapConfigData.set_input_x(data2);

    auto channelSwapv2 = hiai::op::ImageChannelSwapV2("channelSwapv2")
                       .set_input_x(data)
                       .set_input_param(channelSwapConfigData);

    std::vector<Operator> inputs{data, channelSwapConfigData};
    std::vector<Operator> outputs{channelSwapv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_pad
 * 测试用例描述： ir api 构建模型进行编译, 有动态pad算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_pad)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    PadPara para;
    para.paddingSizeTop = 10;
    para.paddingSizeBottom = 10;
    para.paddingSizeLeft = 20;
    para.paddingSizeRight = 20;
    para.paddingValueChn0 = 40.0;
    para.paddingValueChn1 = 50.0;
    para.paddingValueChn2 = 60.0;
    para.paddingValueChn3 = 70.0;
    hiai::op::ConfigData padConfigData = CreateConfigData(para, "padConfigData", hiai::op::ImagePadV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    padConfigData.set_input_x(data2);

    auto imagePadV2 = hiai::op::ImagePadV2("imagePadV2")
                       .set_input_x(data)
                       .set_input_param(padConfigData);

    std::vector<Operator> inputs{data, padConfigData};
    std::vector<Operator> outputs{imagePadV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_dtc
 * 测试用例描述： ir api 构建模型进行编译, 有动态dtc算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_dtc)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    DtcPara para;

    hiai::op::ConfigData dtcConfigData = CreateConfigData(para, "dtcConfigData", hiai::op::ImageDataTypeConvertionV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    dtcConfigData.set_input_x(data2);

    auto imageDataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("imageDataTypeConvertionV2")
                       .set_input_x(data)
                       .set_input_param(dtcConfigData);

    std::vector<Operator> inputs{data, dtcConfigData};
    std::vector<Operator> outputs{imageDataTypeConvertionV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_crop_static_resize
 * 测试用例描述： ir api 构建模型进行编译, 动态连静态混合
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_crop_static_resize)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara cropPara;
    cropPara.cropStartPosW = 0;
    cropPara.cropStartPosH = 0;
    cropPara.cropSizeW = 320;
    cropPara.cropSizeH = 160;

    hiai::op::ConfigData cropConfigData = CreateConfigData(cropPara, "cropConfigData", hiai::op::ImageCropV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    cropConfigData.set_input_x(data2);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);

    ResizePara para;
    para.resizeOutputSizeW = 320;
    para.resizeOutputSizeH = 150;
    hiai::op::Const resizeConstData = CreateConfigConst(para, "resizeConstData");

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(imagecropv2)
                       .set_input_param(resizeConstData);

    std::vector<Operator> inputs{data, cropConfigData};
    std::vector<Operator> outputs{imageresizev2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_crop_dynamic_resize
 * 测试用例描述： ir api 构建模型进行编译, 静态连动态混合
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_crop_dynamic_resize)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara cropPara;
    cropPara.cropStartPosW = 0;
    cropPara.cropStartPosH = 0;
    cropPara.cropSizeW = 320;
    cropPara.cropSizeH = 160;

    hiai::op::Const cropConstData = CreateConfigConst(cropPara, "cropConstData");

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConstData);

    ResizePara para;
    para.resizeOutputSizeW = 320;
    para.resizeOutputSizeH = 150;
    hiai::op::ConfigData resizeConfigData = CreateConfigData(para, "resizeConfigData", hiai::op::ImageResizeV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");
    resizeConfigData.set_input_x(data2);

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(imagecropv2)
                       .set_input_param(resizeConfigData);

    std::vector<Operator> inputs{data, resizeConfigData};
    std::vector<Operator> outputs{imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_func_all
 * 测试用例描述： ir api 构建模型进行编译, 所有6个动态AIPP算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_func_all)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara crop_para;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 240;
    crop_para.cropSizeH = 160;
    hiai::op::ConfigData cropConfigData = CreateConfigData(crop_para, "cropConfigData", hiai::op::ImageCropV2::TYPE);
    hiai::op::Data data10 = CreateData(shape, "data10");
    cropConfigData.set_input_x(data10);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);

    ChannelSwapPara channelswp_ara;
    channelswp_ara.rbuvSwapSwitch = true;
    channelswp_ara.axSwapSwitch = true;
    hiai::op::ConfigData channelSwapConfigData = CreateConfigData(channelswp_ara, "channelSwapConfigData", hiai::op::ImageChannelSwapV2::TYPE);
    hiai::op::Data data11 = CreateData(shape, "data11");
    channelSwapConfigData.set_input_x(data11);

    auto channelSwapv2 = hiai::op::ImageChannelSwapV2("channelSwapv2")
                       .set_input_x(imagecropv2)
                       .set_input_param(channelSwapConfigData);

    CscPara csc_para;
    csc_para.outputFormat = ImageFormat::YUV444SP;
    csc_para.imageColorSpace = ImageColorSpace::JPEG;
    hiai::op::ConfigData cscConfigData = CreateConfigData(csc_para, "cscConfigData", hiai::op::ImageColorSpaceConvertionV2::TYPE);
    hiai::op::Data data12 = CreateData(shape, "data12");
    cscConfigData.set_input_x(data12);

    auto cscV2 = hiai::op::ImageColorSpaceConvertionV2("cscV2")
                       .set_input_x(channelSwapv2)
                       .set_input_param(cscConfigData);

    ResizePara resize_para;
    resize_para.resizeOutputSizeW = 320;
    resize_para.resizeOutputSizeH = 150;
    hiai::op::ConfigData resizeConfigData = CreateConfigData(resize_para, "resizeConfigData", hiai::op::ImageResizeV2::TYPE);
    hiai::op::Data data13 = CreateData(shape, "data13");
    resizeConfigData.set_input_x(data13);

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(cscV2)
                       .set_input_param(resizeConfigData);

    DtcPara dtc_para;
    hiai::op::ConfigData dtcConfigData = CreateConfigData(dtc_para, "dtcConfigData", hiai::op::ImageDataTypeConvertionV2::TYPE);
    hiai::op::Data data14 = CreateData(shape, "data14");
    dtcConfigData.set_input_x(data14);

    auto imageDataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("imageDataTypeConvertionV2")
                       .set_input_x(imageresizev2)
                       .set_input_param(dtcConfigData);

    PadPara padding_para;
    padding_para.paddingSizeTop = 10;
    padding_para.paddingSizeBottom = 10;
    padding_para.paddingSizeLeft = 20;
    padding_para.paddingSizeRight = 20;
    hiai::op::ConfigData padConfigData = CreateConfigData(padding_para, "padConfigData", hiai::op::ImagePadV2::TYPE);
    hiai::op::Data data15 = CreateData(shape, "data15");
    padConfigData.set_input_x(data15);

    auto imagePadV2 = hiai::op::ImagePadV2("imagePadV2")
                       .set_input_x(imageDataTypeConvertionV2)
                       .set_input_param(padConfigData);

    std::vector<Operator> inputs{data, cropConfigData, channelSwapConfigData, cscConfigData, resizeConfigData, dtcConfigData, padConfigData};
    std::vector<Operator> outputs{imagePadV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_two_input_two_aipp
 * 测试用例描述： ir api 构建模型进行编译, 两个输入都有动态AIPP算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_two_input_two_aipp)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara crop_para;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 240;
    crop_para.cropSizeH = 160;
    hiai::op::ConfigData cropConfigData = CreateConfigData(crop_para, "cropConfigData", hiai::op::ImageCropV2::TYPE);
    hiai::op::Data data4 = CreateData(shape, "data4");
    cropConfigData.set_input_x(data4);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);

    Shape shape2({1, 4, 180, 320});
    hiai::op::Data data2 = CreateData(shape2, "data2");
    ResizePara resize_para;
    resize_para.imageFormat = ImageFormat::XRGB8888;
    resize_para.resizeOutputSizeW = 320;
    resize_para.resizeOutputSizeH = 150;

    hiai::op::ConfigData resizeConfigData = CreateConfigData(resize_para, "resizeConfigData", hiai::op::ImageResizeV2::TYPE);
    hiai::op::Data data3 = CreateData(shape, "data3");
    resizeConfigData.set_input_x(data3);

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(data2)
                       .set_input_param(resizeConfigData);

    std::vector<Operator> inputs{cropConfigData, data, resizeConfigData, data2};
    std::vector<Operator> outputs{imagecropv2, imageresizev2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_two_input_one_aipp
 * 测试用例描述： ir api 构建模型进行编译, 两个输入一个有AIPP算子一个没有
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_two_input_one_aipp)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara crop_para;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 240;
    crop_para.cropSizeH = 160;
    hiai::op::ConfigData cropConfigData = CreateConfigData(crop_para, "cropConfigData", hiai::op::ImageCropV2::TYPE);
    hiai::op::Data data3 = CreateData(shape, "data3");
    cropConfigData.set_input_x(data3);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);

    Shape shape2({1, 4, 180, 320});
    hiai::op::Data data2 = CreateData(shape2, "data2");

    std::vector<Operator> inputs{cropConfigData, data, data2};
    std::vector<Operator> outputs{imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_one_input_three_output
 * 测试用例描述： ir api 构建模型进行编译, 一个输入三个输出都有AIPP
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_one_input_three_output)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara crop_para;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 240;
    crop_para.cropSizeH = 160;
    hiai::op::Const cropConfigData = CreateConfigConst(crop_para, "cropConfigData");
    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConfigData);


    Shape shape2({1, 4, 180, 320});
    hiai::op::Data data2 = CreateData(shape2, "data2");
    ResizePara resize_para;
    resize_para.imageFormat = ImageFormat::XRGB8888;
    resize_para.resizeOutputSizeW = 320;
    resize_para.resizeOutputSizeH = 150;
    hiai::op::Const resizeConfigData = CreateConfigConst(resize_para, "resizeConfigData1");
    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev21")
                       .set_input_x(data)
                       .set_input_param(resizeConfigData);

    ResizePara resize_para2;
    resize_para2.imageFormat = ImageFormat::XRGB8888;
    resize_para2.resizeOutputSizeW = 320;
    resize_para2.resizeOutputSizeH = 140;
    hiai::op::Const resizeConfigData2 = CreateConfigConst(resize_para2, "resizeConfigData2");
    auto imageresizev22 = hiai::op::ImageResizeV2("imageresizev22")
                       .set_input_x(data)
                       .set_input_param(resizeConfigData2);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imageresizev22, imageresizev2, imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_resize
 * 测试用例描述： ir api 构建模型进行编译, 有静态resize算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_resize)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 3, 400, 400});
    hiai::op::Data data = CreateData(shape, "data1");
    ResizePara para;
    para.imageFormat = ImageFormat::YUV420SP;
    para.resizeOutputSizeW = 224;
    para.resizeOutputSizeH = 224;

    hiai::op::Const resizeConstData = CreateConfigConst(para, "resizeConstData");

    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(data)
                       .set_input_param(resizeConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imageresizev2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_crop
 * 测试用例描述： ir api 构建模型进行编译, 有静态crop算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_crop)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara cropPara;
    cropPara.imageFormat = ImageFormat::XRGB8888;
    cropPara.cropStartPosW = 0;
    cropPara.cropStartPosH = 0;
    cropPara.cropSizeW = 320;
    cropPara.cropSizeH = 140;

    hiai::op::Const cropConstData = CreateConfigConst(cropPara, "cropConstData");

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imagecropv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_pad
 * 测试用例描述： ir api 构建模型进行编译, 有静态pad算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_pad)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    PadPara para;
    para.imageFormat = ImageFormat::XRGB8888;
    para.paddingSizeTop = 10;
    para.paddingSizeBottom = 10;
    para.paddingSizeLeft = 20;
    para.paddingSizeRight = 20;
    hiai::op::Const padConstData = CreateConfigConst(para, "padConstData");
    auto imagePadV2 = hiai::op::ImagePadV2("imagePadV2")
                       .set_input_x(data)
                       .set_input_param(padConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imagePadV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_rotate
 * 测试用例描述： ir api 构建模型进行编译, 有静态rotate算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_rotate)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 160, 320});
    ge::TensorDesc dataDesc(shape, FORMAT_NCHW, DT_FLOAT);
    hiai::op::Data data = hiai::op::Data("data1");
    data.update_input_desc_x(dataDesc);
    RotatePara para;
    para.imageFormat = ImageFormat::XRGB8888;
    para.rotationAngle = 90.0f;
    para.rotate = true;
    hiai::op::Const rotateConstData = CreateConfigConst(para, "rotateConstData");
    auto imageRotateV2 = hiai::op::ImageRotateV2("imageRotateV2")
                       .set_input_x(data)
                       .set_input_param(rotateConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imageRotateV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_swap
 * 测试用例描述： ir api 构建模型进行编译, 有静态swap算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_swap)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 160, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    ChannelSwapPara para;
    para.imageFormat = ImageFormat::XRGB8888;
    para.rbuvSwapSwitch = true;
    para.axSwapSwitch = true;
    hiai::op::Const channelSwapConstData = CreateConfigConst(para, "channelSwapConstData");
    auto channelSwapv2 = hiai::op::ImageChannelSwapV2("channelSwapv2")
                       .set_input_x(data)
                       .set_input_param(channelSwapConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{channelSwapv2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_dtc
 * 测试用例描述： ir api 构建模型进行编译, 有静态dtc算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_dtc)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 180, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    DtcPara para;
    para.imageFormat = ImageFormat::XRGB8888;
    para.pixelMeanChn0 = 0;
    para.pixelMeanChn1 = 1;
    para.pixelMeanChn2 = 2;
    para.pixelMeanChn3 = 3;
    para.pixelMinChn0 = 4.0f;
    para.pixelMinChn1 = 5.0f;
    para.pixelMinChn2 = 6.0f;
    para.pixelMinChn3 = 7.0f;
    para.pixelVarReciChn0 = 8.0f;
    para.pixelVarReciChn1 = 9.0f;
    para.pixelVarReciChn2 = 10.0f;
    para.pixelVarReciChn3 = 11.0f;
    hiai::op::Const dtcConstData = CreateConfigConst(para, "dtcConstData");

    auto imageDataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("imageDataTypeConvertionV2")
                       .set_input_x(data)
                       .set_input_param(dtcConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imageDataTypeConvertionV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_csc
 * 测试用例描述： ir api 构建模型进行编译, 有静态csc算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_csc)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 3, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CscPara para;
    para.imageFormat = ImageFormat::YUV420SP;
    para.outputFormat = ImageFormat::RGB888;
    para.imageColorSpace = ImageColorSpace::JPEG;

    hiai::op::Const cscConstData = CreateConfigConst(para, "cscConstData");

    auto cscV2 = hiai::op::ImageColorSpaceConvertionV2("cscV2")
                       .set_input_x(data)
                       .set_input_param(cscConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{cscV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_static_func_all
 * 测试用例描述： ir api 构建模型进行编译, 所有静态算子
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_static_func_all)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 200, 320});
    hiai::op::Data data = CreateData(shape, "data1");
    CropPara crop_para;
    crop_para.imageFormat = ImageFormat::XRGB8888;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 240;
    crop_para.cropSizeH = 160;
    hiai::op::Const cropConstData = CreateConfigConst(crop_para, "cropConstData");
    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
                       .set_input_x(data)
                       .set_input_param(cropConstData);

    ChannelSwapPara channelswp_ara;
    channelswp_ara.imageFormat = ImageFormat::RGB888;
    channelswp_ara.rbuvSwapSwitch = true;
    channelswp_ara.axSwapSwitch = true;
    hiai::op::Const channelSwapConstData = CreateConfigConst(channelswp_ara, "channelSwapConstData");
    auto channelSwapv2 = hiai::op::ImageChannelSwapV2("channelSwapv2")
                       .set_input_x(imagecropv2)
                       .set_input_param(channelSwapConstData);

    CscPara csc_para;
    csc_para.outputFormat = ImageFormat::YUV444SP;
    csc_para.imageColorSpace = ImageColorSpace::JPEG;
    hiai::op::Const cscConstData = CreateConfigConst(csc_para, "cscConstData");
    auto cscV2 = hiai::op::ImageColorSpaceConvertionV2("cscV2")
                       .set_input_x(channelSwapv2)
                       .set_input_param(cscConstData);

    ResizePara resize_para;
    resize_para.resizeOutputSizeW = 320;
    resize_para.resizeOutputSizeH = 150;
    hiai::op::Const resizeConstData = CreateConfigConst(resize_para, "resizeConstData");
    auto imageresizev2 = hiai::op::ImageResizeV2("imageresizev2")
                       .set_input_x(cscV2)
                       .set_input_param(resizeConstData);

    DtcPara dtc_para;
    hiai::op::Const dtcConstData = CreateConfigConst(dtc_para, "dtcConstData");
    auto imageDataTypeConvertionV2 = hiai::op::ImageDataTypeConvertionV2("imageDataTypeConvertionV2")
                       .set_input_x(imageresizev2)
                       .set_input_param(dtcConstData);

    PadPara padding_para;
    padding_para.paddingSizeTop = 10;
    padding_para.paddingSizeBottom = 10;
    padding_para.paddingSizeLeft = 20;
    padding_para.paddingSizeRight = 20;
    hiai::op::Const padConstData = CreateConfigConst(padding_para, "padConstData");
    auto imagePadV2 = hiai::op::ImagePadV2("imagePadV2")
                       .set_input_x(imageDataTypeConvertionV2)
                       .set_input_param(padConstData);

    std::vector<Operator> inputs{data};
    std::vector<Operator> outputs{imagePadV2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}

/*
 * 测试用例名称   : model_mannger_build_ir_model_aipp_dynamic_two_output
 * 测试用例描述： ir build, one input, two output
 * 预置条件 :ir api 构建模型
 * 操作步骤:
 * 预期结果 :构建模型成功
 * 修改历史 :
 */
TEST_F(BuildIRModel, model_mannger_build_ir_model_aipp_dynamic_two_output)
{
    ge::Graph graph("graph_defalut");
    Shape shape({1, 4, 64, 64});
    ge::TensorDesc dataDesc(shape, FORMAT_NCHW, DT_FLOAT);
    hiai::op::Data data = hiai::op::Data("data1");
    data.update_input_desc_x(dataDesc);
    CropPara crop_para;
    crop_para.cropStartPosW = 0;
    crop_para.cropStartPosH = 0;
    crop_para.cropSizeW = 6;
    crop_para.cropSizeH = 7;
    hiai::op::ConfigData cropConfigData = CreateConfigData(crop_para, "cropConfigData", hiai::op::ImageCropV2::TYPE);

    auto imagecropv2 = hiai::op::ImageCropV2("imagecropv2")
        .set_input_x(data)
        .set_input_param(cropConfigData);

    PadPara para;
    para.paddingSizeTop = 0;
    para.paddingSizeBottom = 0;
    para.paddingSizeLeft = 0;
    para.paddingSizeRight = 0;
    para.paddingValueChn0 = 0.0;
    hiai::op::ConfigData padConfigData = CreateConfigData(para, "padConfigData", hiai::op::ImagePadV2::TYPE);
    hiai::op::Data data2 = CreateData(shape, "data2");

    auto imagePadV2 = hiai::op::ImagePadV2("imagePadV2")
        .set_input_x(imagecropv2)
        .set_input_param(padConfigData);

    TensorDesc constTensorDesc(Shape({1, 4, 6, 7}), FORMAT_NCHW, DT_FLOAT);
    TensorPtr constTensor = std::make_shared<hiai::Tensor>(constTensorDesc);
    vector<float> dataValue(1 * 4 * 6 * 7, 0);
    constTensor->SetData((uint8_t*)dataValue.data(), 1 * 4 * 6 * 7 * sizeof(float));
    auto constTemp1 = hiai::op::Const("constTemp1").set_attr_value(constTensor);
    auto constTemp2 = hiai::op::Const("constTemp2").set_attr_value(constTensor);

    auto add1 = hiai::op::Add("add1")
        .set_input_x1(imagePadV2)
        .set_input_x2(constTemp1);

    auto add2 = hiai::op::Add("add2")
        .set_input_x1(imagePadV2)
        .set_input_x2(constTemp2);

    std::vector<Operator> inputs{data, cropConfigData, padConfigData};
    std::vector<Operator> outputs{add1, add2};
    graph.SetInputs(inputs);
    graph.SetOutputs(outputs);

    std::shared_ptr<ge::Model> model = std::make_shared<ge::Model>("model_name", "custom version3.0");
    model->SetGraph(graph);

    ModelBuildOptions buildoptions;
    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    std::string modelName = "test_model";
    HiaiIrBuild build;

    auto ret = build.Build(buildoptions, modelName, model, builtModel);
    EXPECT_EQ(hiai::SUCCESS, ret);
}