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
#include <map>
#include <dlfcn.h>

#include "model_runtime/direct/direct_built_model.h"
#include "util/file_util.h"
#include "util/hiai_foundation_dl_helper.h"

using namespace std;
using namespace hiai;

#define DEFAULT_MODEL_SIZE 1024 * 1024
struct BuiltModelTestParams {
    const char* modelFile;
    size_t modelSize;
    HIAI_BuiltModel_Compatibility isCompatibie;
    HIAI_Status expectResult;
};

static vector<BuiltModelTestParams> g_TestParams = {
    {nullptr, DEFAULT_MODEL_SIZE, HIAI_BUILTMODEL_COMPATIBLE, HIAI_FAILURE},
    {"", DEFAULT_MODEL_SIZE, HIAI_BUILTMODEL_COMPATIBLE, HIAI_FAILURE},
    {".", DEFAULT_MODEL_SIZE, HIAI_BUILTMODEL_COMPATIBLE, HIAI_FAILURE},
    {"bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om", DEFAULT_MODEL_SIZE, HIAI_BUILTMODEL_COMPATIBLE,
        HIAI_SUCCESS},
    {"bin/llt/framework/domi/modelmanager/tf_softmax_hcs_npucl.om", DEFAULT_MODEL_SIZE, HIAI_BUILTMODEL_COMPATIBLE,
        HIAI_SUCCESS},
};

class DirectBuiltModel_UTest : public testing::TestWithParam<BuiltModelTestParams> {
public:
    void SetUp()
    {
        handle = dlopen("libai_client_stub_ddk.so", RTLD_NOW);
        if (handle == nullptr) {
            cout << "dlopen libai_client_stub_ddk.so failed" << endl;
        }
    }
    void TearDown()
    {
        GlobalMockObject::verify();
        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }

        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }

        if (handle != nullptr) {
            dlclose(handle);
        }
    }

private:
    HIAI_MR_BuiltModel* builtModel {nullptr};
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
    uint8_t* data = nullptr;
    size_t size = 0;
    void* handle = nullptr;
};

INSTANTIATE_TEST_CASE_P(normal, DirectBuiltModel_UTest, testing::ValuesIn(g_TestParams));

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SaveToExternalBuffer
* 测试用例描述:
    1.校验入参model为空场景
    2.校验入参data为空场景
    3.校验size小于实际模型大小场景
    4.保存成功场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, SaveToExternalBuffer)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }

    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    if (param.modelSize > 0) {
        data = new uint8_t[param.modelSize];
    }

    size_t realSize = 0;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(builtModel, (void*)data, param.modelSize, &realSize);
    EXPECT_TRUE(ret == param.expectResult);

    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(realSize > 0 && realSize <= param.modelSize);
        // 更改size使其小于实际模型大小
        ret = HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(builtModel, (void*)data, realSize - 1, &realSize);
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SaveToExternalBuffer
* 测试用例描述:
    1.model restore from file
    2.保存成功场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, RestoreFromFile_SaveToExternalBuffer)
{
    BuiltModelTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile);

    if (param.modelSize > 0) {
        data = new uint8_t[param.modelSize];
    }

    size_t size = 0;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SaveToExternalBuffer(builtModel, (void*)data, param.modelSize, &size);
    EXPECT_TRUE(ret == param.expectResult);

    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(size > 0 && size <= param.modelSize);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_Save
* 测试用例描述:
    1.校验入参model为空场景
    2.成功保存场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, Save)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }

    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_Save(builtModel, (void**)&data, &size);
    EXPECT_TRUE(ret == param.expectResult);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(size > 0 && data != nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_Save
* 测试用例描述:
    1.从文件 restore
    2.成功保存场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, RestoreFromFile_Save)
{
    BuiltModelTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(builtModel != nullptr);
    } else {
        EXPECT_TRUE(builtModel == nullptr);
    }
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_Save(builtModel, (void**)&data, &size);
    EXPECT_TRUE(ret == param.expectResult);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(size > 0 && data != nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_Restore
* 测试用例描述:
    1.校验入参data为空场景
    2.校验入参size为0场景
    3.Restore成功场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, Restore)
{
    BuiltModelTestParams param = GetParam();
    // data为空场景
    builtModel = HIAI_DIRECT_BuiltModel_Restore(nullptr, DEFAULT_MODEL_SIZE);
    EXPECT_TRUE(builtModel == nullptr);

    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }

    if (modelBuffer != nullptr) {
        // size为0场景
        size = 0;
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), size);
        EXPECT_TRUE(builtModel == nullptr);

        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
        EXPECT_TRUE(builtModel != nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_RestoreFromFile
* 测试用例描述:
    1.校验入参 file 为空场景
    2.Restore成功场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, RestoreFromFile)
{
    BuiltModelTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(builtModel != nullptr);
    } else {
        EXPECT_TRUE(builtModel == nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SaveToFile
* 测试用例描述:
    1.校验入参model为空场景
    2.校验入参file为空场景
    3.校验入参file路径非法场景
    4.Save成功场景
*/
TEST_P(DirectBuiltModel_UTest, SaveToFile)
{
    BuiltModelTestParams param = GetParam();

    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    string saveFile = "./HIAI_DIRECT_BuiltModel_SaveToFile.om";
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, saveFile.c_str());
    EXPECT_TRUE(ret == param.expectResult);

    if (ret == HIAI_SUCCESS) {
        unlink(saveFile.c_str());

        ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, nullptr);
        EXPECT_TRUE(ret != HIAI_SUCCESS);

        saveFile = "";
        ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, saveFile.c_str());
        EXPECT_TRUE(ret != HIAI_SUCCESS);

        saveFile = ".";
        ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, saveFile.c_str());
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SaveToFile
* 测试用例描述:
    异常场景: 模型文件已经存在时候，不需要再保存到file中
*/
TEST_F(DirectBuiltModel_UTest, RestoreFromFile_SaveToFile)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_npucl.om";
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(modelFile.c_str());
    EXPECT_TRUE(builtModel != nullptr);

    string saveFile = "./HIAI_DIRECT_BuiltModel_SaveToFile.om";
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, saveFile.c_str());
    EXPECT_TRUE(ret == HIAI_FAILURE);

    if (ret == HIAI_SUCCESS) {
        unlink(saveFile.c_str());
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SaveToFile
* 测试用例描述:
    将模型buffer保存到外部的file中
*/
TEST_F(DirectBuiltModel_UTest, RestoreFromBuffer_SaveToFile)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }
    EXPECT_TRUE(builtModel != nullptr);

    string saveFile = "./HIAI_DIRECT_BuiltModel_SaveToFile.om";
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SaveToFile(builtModel, saveFile.c_str());
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    if (ret == HIAI_SUCCESS) {
        unlink(saveFile.c_str());
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_CheckCompatibility
* 测试用例描述:
    1.校验入参data为空场景
    2.检测兼容模型场景
    3.检测不兼容模型场景
*/
TEST_P(DirectBuiltModel_UTest, CheckCompatibility_001)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_BuiltModel_Compatibility compatibility = HIAI_BUILTMODEL_COMPATIBLE;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_CheckCompatibility(builtModel, &compatibility);
    EXPECT_TRUE(ret == param.expectResult);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(compatibility == param.isCompatibie);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_CheckCompatibility
* 测试用例描述:
    获取符号表失败场景
*/
TEST_F(DirectBuiltModel_UTest, CheckCompatibility_002)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().will(returnValue((void*)nullptr));

    HIAI_BuiltModel_Compatibility compatibility = HIAI_BUILTMODEL_COMPATIBLE;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_CheckCompatibility(builtModel, &compatibility);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_CheckCompatibility
* 测试用例描述:
    1、restore from file
    2、检查兼容性
*/
TEST_P(DirectBuiltModel_UTest, RestoreFromFile_CheckCompatibility)
{
    BuiltModelTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(builtModel != nullptr);
    } else {
        EXPECT_TRUE(builtModel == nullptr);
    }

    HIAI_BuiltModel_Compatibility compatibility = HIAI_BUILTMODEL_COMPATIBLE;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_CheckCompatibility(builtModel, &compatibility);
    EXPECT_TRUE(ret == param.expectResult);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(compatibility == param.isCompatibie);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetName
* 测试用例描述:
    1.校验入参model为空场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, GetName)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    const char* modelName = HIAI_DIRECT_BuiltModel_GetName(builtModel);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(modelName != nullptr);
    } else {
        EXPECT_TRUE(modelName == nullptr);
    }

    if (builtModel != nullptr && HIAI_DIRECT_BuiltModel_SetName(builtModel, "modelname") == HIAI_SUCCESS) {
        modelName = HIAI_DIRECT_BuiltModel_GetName(builtModel);
        EXPECT_TRUE(strcmp(modelName, "modelname") == 0);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_SetName
* 测试用例描述:
    1.校验入参model为空场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, SetName)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_Status ret = HIAI_DIRECT_BuiltModel_SetName(builtModel, "modelname");
    EXPECT_TRUE(ret == param.expectResult);
    if (ret == HIAI_SUCCESS) {
        ret = HIAI_DIRECT_BuiltModel_SetName(builtModel, nullptr);
        EXPECT_TRUE(ret == HIAI_FAILURE);
        ret = HIAI_DIRECT_BuiltModel_SetName(builtModel, "");
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetInputTensorNum
* 测试用例描述:
    1.校验入参model为空场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, GetInputTensorNum)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int ret = HIAI_DIRECT_BuiltModel_GetInputTensorNum(builtModel);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(ret > 0);
    } else {
        EXPECT_TRUE(ret == 0);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetInputTensorDesc
* 测试用例描述:
    1.校验入参model为空场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, GetInputTensorDesc)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    size_t index = 0;
    HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetInputTensorDesc(builtModel, index);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(desc != nullptr);
        HIAI_NDTensorDesc_Destroy(&desc);
        // 非法index场景
        index = 1024;
        desc = HIAI_DIRECT_BuiltModel_GetInputTensorDesc(builtModel, index);
        EXPECT_TRUE(desc == nullptr);
    } else {
        EXPECT_TRUE(desc == nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorNum
* 测试用例描述:
    1.校验入参model为空场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, GetOutputTensorNum_001)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int ret = HIAI_DIRECT_BuiltModel_GetOutputTensorNum(builtModel);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(ret > 0);
    } else {
        EXPECT_TRUE(ret == 0);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorNum
* 测试用例描述:
    1.测试HIAI_ModelManager_GetModelNDTensorInfo失败时,
      HIAI_DIRECT_BuiltModel_GetOutputTensorNum内部调用HIAI_ModelManager_GetModelTensorInfoV2
* 预置条件: 模型文件
*/
TEST_F(DirectBuiltModel_UTest, GetOutputTensorNum_002)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    EXPECT_TRUE(builtModel != nullptr);

    std::map<string, void*> symbolList = {
        {"HIAI_ModelManager_create", nullptr},
        {"HIAI_GetVersion", nullptr},
        {"HIAI_MemBuffer_create_from_buffer", nullptr},
        {"HIAI_ModelManager_loadFromModelBuffers", nullptr},
        {"HIAI_ModelManager_unloadModel", nullptr},
        {"HIAI_ModelManager_destroy", nullptr},
        {"HIAI_ModelBuffer_create_from_buffer", nullptr},
        {"HIAI_ModelBuffer_destroy", nullptr},
        {"HIAI_MemBuffer_destroy", nullptr},
        {"HIAI_ModelManager_getModelTensorInfoV2", nullptr},
        {"HIAI_ModelManager_releaseModelTensorInfoV2", nullptr},
        {"HIAI_ModelTensorInfoV2_getIOCount", nullptr},
        {"HIAI_ModelTensorInfoV2_getTensorDescription", nullptr},
    };

    for (auto& symbol : symbolList) {
        symbol.second = dlsym(handle, symbol.first.c_str());
        MOCKER(&HIAI_Foundation_GetSymbol)
            .stubs()
            .with(contains(symbol.first.c_str()))
            .will(returnValue(symbol.second));
    }

    // 打桩HIAI_ModelManager_GetModelNDTensorInfo/HIAI_ModelManager_ReleaseModelNDTensorInfo
    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(endWith("NDTensorInfo")).will(returnValue((void*)nullptr));
    int num = HIAI_DIRECT_BuiltModel_GetOutputTensorNum(builtModel);
    EXPECT_TRUE(num > 0);
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorNum
* 测试用例描述:
    1.测试HIAI_ModelManager_getModelTensorInfoV2/HIAI_ModelManager_GetModelNDTensorInfo失败时
      HIAI_DIRECT_BuiltModel_GetOutputTensorNum内部调用HIAI_ModelManager_getModelTensorInfo
* 预置条件: 模型文件
*/
TEST_F(DirectBuiltModel_UTest, GetOutputTensorNum_003)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    EXPECT_TRUE(builtModel != nullptr);

    std::map<string, void*> symbolList = {{"HIAI_ModelManager_create", nullptr}, {"HIAI_GetVersion", nullptr},
        {"HIAI_MemBuffer_create_from_buffer", nullptr}, {"HIAI_ModelManager_loadFromModelBuffers", nullptr},
        {"HIAI_ModelManager_unloadModel", nullptr}, {"HIAI_ModelManager_destroy", nullptr},
        {"HIAI_ModelBuffer_create_from_buffer", nullptr}, {"HIAI_ModelBuffer_destroy", nullptr},
        {"HIAI_MemBuffer_destroy", nullptr}, {"HIAI_ModelManager_getModelTensorInfo", nullptr},
        {"HIAI_ModelManager_releaseModelTensorInfo", nullptr}};

    // 打桩 HIAI_ModelManager_getModelTensorInfoV2/HIAI_ModelManager_releaseModelTensorInfoV2
    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(endWith("TensorInfoV2")).will(returnValue((void*)nullptr));

    // 打桩 HIAI_ModelManager_GetModelNDTensorInfo/HIAI_ModelManager_ReleaseModelNDTensorInfo
    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(endWith("NDTensorInfo")).will(returnValue((void*)nullptr));

    for (auto& symbol : symbolList) {
        symbol.second = dlsym(handle, symbol.first.c_str());
        MOCKER(&HIAI_Foundation_GetSymbol)
            .stubs()
            .with(contains(symbol.first.c_str()))
            .will(returnValue(symbol.second));
    }

    int num = HIAI_DIRECT_BuiltModel_GetOutputTensorNum(builtModel);
    EXPECT_TRUE(num > 0);
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorNum
* 测试用例描述:
    1.测试HIAI_ModelManager_GetModelNDTensorInfo/HIAI_ModelTensorInfoV2_getIOCount失败时,
      HIAI_DIRECT_BuiltModel_GetOutputTensorNum内部调用HIAI_ModelManager_getModelTensorInfo
* 预置条件: 模型文件
*/
TEST_F(DirectBuiltModel_UTest, GetOutputTensorNum_004)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    EXPECT_TRUE(builtModel != nullptr);

    std::map<string, void*> symbolList = {{"HIAI_ModelManager_create", nullptr}, {"HIAI_GetVersion", nullptr},
        {"HIAI_MemBuffer_create_from_buffer", nullptr}, {"HIAI_ModelManager_loadFromModelBuffers", nullptr},
        {"HIAI_ModelManager_unloadModel", nullptr}, {"HIAI_ModelManager_destroy", nullptr},
        {"HIAI_ModelBuffer_create_from_buffer", nullptr}, {"HIAI_ModelBuffer_destroy", nullptr},
        {"HIAI_MemBuffer_destroy", nullptr}, {"HIAI_ModelManager_getModelTensorInfoV2", nullptr},
        {"HIAI_ModelManager_releaseModelTensorInfoV2", nullptr},
        {"HIAI_ModelTensorInfoV2_getTensorDescription", nullptr}, {"HIAI_ModelManager_getModelTensorInfo", nullptr},
        {"HIAI_ModelManager_releaseModelTensorInfo", nullptr}};

    for (auto& symbol : symbolList) {
        symbol.second = dlsym(handle, symbol.first.c_str());
        MOCKER(&HIAI_Foundation_GetSymbol)
            .stubs()
            .with(contains(symbol.first.c_str()))
            .will(returnValue(symbol.second));
    }

    // 打桩HIAI_ModelManager_GetModelNDTensorInfo/HIAI_ModelManager_ReleaseModelNDTensorInfo
    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(endWith("NDTensorInfo")).will(returnValue((void*)nullptr));

    MOCKER(&HIAI_Foundation_GetSymbol)
        .stubs()
        .with(contains("HIAI_ModelTensorInfoV2_getIOCount"))
        .will(returnValue((void*)nullptr));

    int num = HIAI_DIRECT_BuiltModel_GetOutputTensorNum(builtModel);
    EXPECT_TRUE(num >= 0);
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorDesc
* 测试用例描述:
    1.校验入参model为空场景
    2.校验非法index场景
    2.成功场景
*/
TEST_P(DirectBuiltModel_UTest, GetOutputTensorDesc)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    size_t index = 0;
    HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(builtModel, index);
    if (param.expectResult == HIAI_SUCCESS) {
        EXPECT_TRUE(desc != nullptr);
        HIAI_NDTensorDesc_Destroy(&desc);
        // 非法index场景
        index = 1024;
        desc = HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(builtModel, index);
        EXPECT_TRUE(desc == nullptr);
    } else {
        EXPECT_TRUE(desc == nullptr);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetOutputTensorDesc
* 测试用例描述:
    获取libai_client符号表失败场景
* 预置条件: 模型文件
*/
TEST_F(DirectBuiltModel_UTest, exception)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    // MOCKER(HIAI_Foundation_GetSymbol).expects(once()).will(returnValue((void*)nullptr));
    MOCKER(HIAI_Foundation_GetSymbol).stubs().will(returnValue((void*)nullptr));
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    size_t index = 0;
    HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(builtModel, index);
    EXPECT_TRUE(desc == nullptr);
}

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_Destroy
* 测试用例描述:
    1.校验入参model为空场景
    2.校验入参model非空场景
    3.验证重复Destory场景
* 预置条件: 模型文件
*/
TEST_P(DirectBuiltModel_UTest, destroy)
{
    BuiltModelTestParams param = GetParam();
    if (param.modelFile != nullptr) {
        modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    }
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
    HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
}