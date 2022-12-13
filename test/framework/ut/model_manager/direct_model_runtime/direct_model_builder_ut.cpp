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
#include <memory>
#include <vector>
#include <string>

#include "model_runtime/direct/direct_model_builder.h"
#include "model_runtime/direct/direct_built_model.h"
#include "model_runtime/direct/direct_model_compatible_proxy.h"
#include "framework/graph/core/cgraph/graph_modifier.h"
#include "framework/graph/core/edge/edge.h"
#include "util/file_util.h"
#include "util/base_buffer.h"
#include "util/hiai_foundation_dl_helper.h"
#include "c/hiai_version.h"
#include "infra/base/dynamic_load_helper.h"

#include "graph/model.h"
#include "graph/attr_value.h"
#include "graph/core/op/op_desc.h"
#include "graph/utils/graph_utils.h"
#include "graph/utils/attr_utils.h"
#include "graph/op/array_defs.h"
#include "graph/op/const_defs.h"
#include "graph/debug/ge_op_types.h"
#include "graph/op/math_defs.h"
#include "graph/op/nn_defs.h"

using namespace std;
using namespace hiai;

struct BuilderTestParams {
    const char* modelName;
    string modelFile;
    size_t modeSize;
    HIAI_Status expectValue;
};

#define DEFAULT_MODEL_SIZE 1024

static vector<BuilderTestParams> g_TestParams = {
    { nullptr, "bin/llt/framework/domi/modelmanager/aipp.om", DEFAULT_MODEL_SIZE, HIAI_FAILURE}, // 入参modelName为nullptr场景
    { "", "bin/llt/framework/domi/modelmanager/aipp.om", DEFAULT_MODEL_SIZE, HIAI_SUCCESS }, // modelName为空字符串场景
    { "aipp", "", DEFAULT_MODEL_SIZE, HIAI_FAILURE }, // 入参inputModelData为空场景
    { "aipp", "bin/llt/framework/domi/modelmanager/aipp.om", 0, HIAI_FAILURE }, // size为0场景
    { "origin_format", "bin/llt/framework/domi/modelmanager/tf_use_origin_format.om", DEFAULT_MODEL_SIZE, HIAI_SUCCESS }, // 成功场景
};

class DirectModelBuilder_UTest : public testing::TestWithParam<BuilderTestParams> {
public:
    void SetUp()
    {
        setenv("ROM_HIAI_VERSION", "100.510.010.012", 1);
        buildOptions = HIAI_ModelBuildOptions_Create();
    }

    void TearDown()
    {
        unsetenv("ROM_HIAI_VERSION");
        GlobalMockObject::verify();
        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }

        if (buildOptions != nullptr) {
            HIAI_ModelBuildOptions_Destroy(&buildOptions);
        }
    }
private:
    HIAI_BuiltModel* builtModel = nullptr;
    HIAI_ModelBuildOptions* buildOptions = nullptr;
};

INSTANTIATE_TEST_CASE_P(build, DirectModelBuilder_UTest, testing::ValuesIn(g_TestParams));

/*
* 测试用例名称: HIAI_DIRECT_ModelBuilder_Build
* 测试用例描述:
    1.入参modelName为nullptr场景
    2.modelName为空字符串场景
    3.入参inputModelData为空场景
    4.size为0场景
    5.成功场景
* 预置条件: 模型文件
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelBuilder_UTest, build_001)
{
    HIAI_Status ret = HIAI_SUCCESS;
    BuilderTestParams param = GetParam();
    string modelFile = param.modelFile;

    void* inputModelData = nullptr;
    size_t modelSize = 0;
    std::shared_ptr<hiai::BaseBuffer> baseBuffer = FileUtil::LoadToBuffer(modelFile);
    if (baseBuffer != nullptr) {
        inputModelData = baseBuffer->MutableData();
        if (param.modeSize > 0) {
            modelSize = baseBuffer->GetSize();
        }
    }

    ret = HIAI_DIRECT_ModelBuilder_Build(buildOptions, param.modelName, inputModelData, modelSize, &builtModel);
    EXPECT_TRUE(ret == param.expectValue);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(builtModel != nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelBuilder_Build
* 测试用例描述:
    1. HIAI_ModelBuildOptions不支持场景
* 预置条件: 模型文件
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelBuilder_UTest, build_002)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    string modelName = "tf_softmax_infershaped.om";

    void* inputModelData = nullptr;
    size_t modelSize = 0;
    std::shared_ptr<hiai::BaseBuffer> baseBuffer = FileUtil::LoadToBuffer(modelFile);
    if (baseBuffer != nullptr) {
        inputModelData = baseBuffer->MutableData();
        modelSize = baseBuffer->GetSize();
    }

    HIAI_ModelBuildOptions_SetFormatModeOption(buildOptions, HIAI_FORMAT_MODE_USE_ORIGIN);
    ret = HIAI_DIRECT_ModelBuilder_Build(buildOptions, modelName.c_str(), inputModelData, modelSize, &builtModel);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}


struct IR_API_MODEL_BuilderTestParams {
    string version;
    HIAI_Status expectStatus;
};

static std::vector<IR_API_MODEL_BuilderTestParams> gIrApiModelParams = {
    {"", HIAI_FAILURE},
    {"1.2.3.4", HIAI_FAILURE},
    {"100.320.010.023", HIAI_FAILURE}, // 低于100.320.011版本不支持IR build
    {"100.320.011.019", HIAI_SUCCESS},
    {"100.320.011.018", HIAI_SUCCESS},
    {"100.320.012.011", HIAI_SUCCESS},
    {"100.320.012.010", HIAI_SUCCESS},
    {"100.330.010.011", HIAI_SUCCESS},
    {"100.330.010.010", HIAI_SUCCESS},
    {"100.330.011.011", HIAI_SUCCESS},
    {"100.330.011.010", HIAI_SUCCESS},
    {"100.330.012.011", HIAI_SUCCESS},
    {"100.330.012.010", HIAI_SUCCESS},
    {"100.500.010.011", HIAI_SUCCESS},
    {"100.500.010.010", HIAI_SUCCESS},
};

class DirectModelBuilder_IR_API_MODEL_UTest : public testing::TestWithParam<IR_API_MODEL_BuilderTestParams> {
public:
    void SetUp()
    {
        buildOptions = HIAI_ModelBuildOptions_Create();
    }

    void TearDown()
    {
        unsetenv("ROM_HIAI_VERSION");
        GlobalMockObject::verify();
        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }

        if (buildOptions != nullptr) {
            HIAI_ModelBuildOptions_Destroy(&buildOptions);
        }
    }

    ge::Node* AddNode(ge::ComputeGraphPtr graph, const string& name, const string& type, int32_t inAnchorsNum = 1,
        int32_t outAnchorsNum = 1)
    {
        ge::TensorDesc tensorDesc;
        ge::OpDescPtr opdesc = shared_ptr<ge::OpDesc>(new (std::nothrow) ge::OpDesc(name, type));
        if (type == hiai::op::Data::TYPE) {
            ge::TensorDesc tensorDesc(Shape({1, 1, 28, 28}), ge::FORMAT_NCHW, ge::DT_FLOAT);
        }
        for (int32_t i = 0; i < inAnchorsNum; i++) {
            opdesc->AddInputDesc(tensorDesc);
        }
        for (int32_t i = 0; i < outAnchorsNum; i++) {
            opdesc->AddOutputDesc(tensorDesc);
        }

        ge::Node* node = graph->ROLE(GraphModifier).AddNode(opdesc);
        return node;
    }

    ge::ComputeGraphPtr CreateMatmulNode()
    {
        ge::ComputeGraphPtr graph = ge::ComputeGraph::Make("default");
        ge::OpDescPtr xOpDesc =
            std::shared_ptr<ge::OpDesc>(new (std::nothrow) ge::OpDesc("data", hiai::op::Data::TYPE));
        ge::OpDescPtr yOpDesc =
            std::shared_ptr<ge::OpDesc>(new (std::nothrow) ge::OpDesc("data1", hiai::op::Data::TYPE));
        ge::OpDescPtr zOpDesc =
            std::shared_ptr<ge::OpDesc>(new (std::nothrow) ge::OpDesc("matmul", hiai::op::MatMul::TYPE));
        ge::TensorDesc xDesc(ge::Shape({1, 1, 28, 28}), ge::FORMAT_NCHW, ge::DT_FLOAT);
        ge::TensorDesc yDesc(ge::Shape({1, 1, 28, 28}), ge::FORMAT_NCHW, ge::DT_FLOAT);
        xOpDesc->AddInputDesc(xDesc);
        xOpDesc->AddOutputDesc(xDesc);
        yOpDesc->AddInputDesc(yDesc);
        yOpDesc->AddOutputDesc(yDesc);
        ge::Node* inputNode = graph->ROLE(GraphModifier).AddNode(xOpDesc);
        ge::Node* inputNode1 = graph->ROLE(GraphModifier).AddNode(yOpDesc);
        ge::Node* matmulNode = AddNode(graph, "matmul", hiai::op::MatMul::TYPE, 2, 1);
        (void)graph->ROLE(GraphModifier).AddEdge({*inputNode, 0}, {*matmulNode, 0});
        (void)graph->ROLE(GraphModifier).AddEdge({*inputNode1, 0}, {*matmulNode, 1});
        return graph;
    }

private:
    HIAI_BuiltModel* builtModel = nullptr;
    HIAI_ModelBuildOptions* buildOptions = nullptr;
};

INSTANTIATE_TEST_CASE_P(build, DirectModelBuilder_IR_API_MODEL_UTest, testing::ValuesIn(gIrApiModelParams));

/*
* 测试用例名称: HIAI_DIRECT_ModelBuilder_Build
* 测试用例描述:
    1. ge::Model兼容性成功场景
* 预置条件: 模型文件
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelBuilder_IR_API_MODEL_UTest, DirectModelBuilder_IR_API_MODEL_001)
{
    IR_API_MODEL_BuilderTestParams param = GetParam();
    std::string romVersion = param.version;
    setenv("ROM_HIAI_VERSION", romVersion.c_str(), 1);
    MOCKER(HIAI_GetVersion).stubs().will(returnValue(romVersion.c_str()));

    ge::ComputeGraphPtr graph = CreateMatmulNode();
    EXPECT_NE(graph, nullptr);
    ge::Graph irGraph = ge::GraphUtils::CreateGraphFromComputeGraph(graph);
    ge::Model irModel;
    irModel.SetGraph(irGraph);
    ge::Buffer irModelBuff;
    EXPECT_TRUE(irModel.Save(irModelBuff) == ge::GRAPH_SUCCESS);
    hiai::Status mockerRet = hiai::FAILURE;

    void* data = static_cast<void*>(const_cast<uint8_t*>(irModelBuff.GetData()));
    size_t size = irModelBuff.GetSize();
    HIAI_Status ret = HIAI_DIRECT_ModelBuilder_Build(buildOptions, "ir_model", data, size, &builtModel);
    EXPECT_TRUE(ret == param.expectStatus);
}
