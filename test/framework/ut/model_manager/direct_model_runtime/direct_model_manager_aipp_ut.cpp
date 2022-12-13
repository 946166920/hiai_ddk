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
#include <climits>
#include <thread>
#include <dlfcn.h>

#include "model_runtime/direct/direct_built_model.h"
#include "model_runtime/direct/direct_built_model_aipp.h"
#include "model_runtime/direct/direct_model_manager_aipp.h"
#include "model_manager/core/model_manager_impl.h"
#include "util/hiai_foundation_dl_helper.h"
#include "util/file_util.h"

using namespace std;
using namespace hiai;

struct ManagerAippTestParams {
    string modelFile;
    bool isASync;
};

static vector<ManagerAippTestParams> g_TestParams = {
    {"bin/llt/framework/domi/modelmanager/aipp.om", false}, {"bin/llt/framework/domi/modelmanager/aipp.om", true}};

static void OnRunDone(void* userData, HIAI_Status errCode, HIAI_NDTensorBuffer* outputs[], int32_t outputNum)
{
    cout << "++++++" << __func__ << " userData: " << userData << " errCode: " << errCode
         << " outputs: " << (void*)outputs << " outputNum: " << outputNum << endl;
    if (outputs == nullptr) {
        cout << "outputs is null" << endl;
        return;
    }

    if (userData != nullptr) {
        delete (RunAsyncContext*)userData;
        userData = nullptr;
    }
}

static void OnServiceDied(void* userData)
{
    cout << "++++++" << __func__ << " userData:" << userData << endl;
}

class DirectModelManagerAipp_UTest : public testing::TestWithParam<ManagerAippTestParams> {
public:
    void SetUp()
    {
        handle = dlopen("libai_client_stub_ddk.so", RTLD_NOW);
        if (handle == nullptr) {
            cout << "dlopen libai_client_stub_ddk.so failed" << endl;
        }

        modelManager = HIAI_DIRECT_ModelManager_Create();
        managerListener = new HIAI_ModelManagerListener;
        if (managerListener != nullptr) {
            managerListener->onRunDone = OnRunDone;
            managerListener->onServiceDied = OnServiceDied;
        }
    }

    void TearDown()
    {
        GlobalMockObject::verify();
        if (modelManager != nullptr) {
            HIAI_DIRECT_ModelManager_Destroy(&modelManager);
        }

        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }

        if (managerListener != nullptr) {
            delete managerListener;
            managerListener = nullptr;
        }

        for (auto& tensorBuffer : inputs) {
            HIAI_NDTensorBuffer_Destroy(&tensorBuffer);
        }

        for (auto& tensorBuffer : outputs) {
            HIAI_NDTensorBuffer_Destroy(&tensorBuffer);
        }

        if (initOptions != nullptr) {
            HIAI_ModelInitOptions_Destroy(&initOptions);
        }

        if (aippParas != nullptr) {
            for (uint32_t i = 0; i < aippParaNum; i++) {
                if (aippParas[i] != nullptr) {
                    HIAI_TensorAippPara_Destroy(&aippParas[i]);
                }
            }
            delete[] aippParas;
        }

        if (handle != nullptr) {
            dlclose(handle);
        }
    }

    int GetInputTensorNum()
    {
        if (builtModel == nullptr) {
            return 0;
        }

        int32_t inputNum = HIAI_DIRECT_BuiltModel_GetInputTensorNum(builtModel);
        for (int32_t i = 0; i < inputNum; i++) {
            HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetInputTensorDesc(builtModel, i);
            if (desc != nullptr) {
                HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNDTensorDesc(desc);
                inputs.emplace_back(buffer);
                HIAI_NDTensorDesc_Destroy(&desc);
            }
        }
        return inputNum;
    }

    int GetOutputTensorNum()
    {
        if (builtModel == nullptr) {
            return 0;
        }
        int32_t outputNum = HIAI_DIRECT_BuiltModel_GetOutputTensorNum(builtModel);
        for (int32_t i = 0; i < outputNum; i++) {
            HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetOutputTensorDesc(builtModel, i);
            if (desc != nullptr) {
                HIAI_NDTensorBuffer* buffer = HIAI_NDTensorBuffer_CreateFromNDTensorDesc(desc);
                outputs.emplace_back(buffer);
                HIAI_NDTensorDesc_Destroy(&desc);
            }
        }
        return outputNum;
    }

private:
    HIAI_ModelManager* modelManager = nullptr;
    HIAI_BuiltModel* builtModel = nullptr;
    HIAI_ModelManagerListener* managerListener = nullptr;
    vector<HIAI_NDTensorBuffer*> inputs;
    vector<HIAI_NDTensorBuffer*> outputs;
    HIAI_ModelInitOptions* initOptions = nullptr;
    HIAI_TensorAippPara** aippParas = nullptr;
    uint32_t aippParaNum = 0;
    uint32_t batchCount = 0;
    void* handle = nullptr;
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
};

INSTANTIATE_TEST_CASE_P(runAippModelV2, DirectModelManagerAipp_UTest, testing::ValuesIn(g_TestParams));

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_runAippModelV2
* 测试用例描述:
    1.入参manager为空场景
    2.入参input为空场景
    3.入参inputNum非法场景
    4.入参aippPara为空场景
    5.入参aippParaNum非法场景
    6.入参output为空场景
    7.入参outputNum非法场景
    8.入参userData为空场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManagerAipp_UTest, runAippModelV2_001)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/aipp.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_BuiltModel_GetTensorAippInfo(builtModel, -1, &aippParaNum, &batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    aippParas = new HIAI_TensorAippPara*[aippParaNum];
    ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, -1, aippParas, aippParaNum, batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);

    EXPECT_TRUE(ret == HIAI_SUCCESS);

    int32_t timeoutInMS = 100;
    void* userData = nullptr;

    // 入参manager为空场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(
        nullptr, inputs.data(), inputNum, aippParas, aippParaNum, outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参input为空场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(
        modelManager, nullptr, inputNum, aippParas, aippParaNum, outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参inputNum非法场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum - 1, aippParas, aippParaNum,
        outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参aippPara为空场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(
        modelManager, inputs.data(), inputNum, nullptr, aippParaNum, outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参aippParaNum非法场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum, aippParas, aippParaNum - 1,
        outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参output为空场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(
        modelManager, inputs.data(), inputNum, aippParas, aippParaNum, nullptr, outputNum, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参outputNum非法场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum, aippParas, aippParaNum,
        outputs.data(), outputNum - 1, timeoutInMS, nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    // 入参userData为空场景
    ret = HIAI_DIRECT_ModelManager_runAippModelV2(
        modelManager, inputs.data(), inputNum, aippParas, aippParaNum, outputs.data(), outputNum, timeoutInMS, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_runAippModelV2
* 测试用例描述:
    1. 测试HIAI_ModelManager_runAippModel_v3同步成功场景
    2. 测试HIAI_ModelManager_runAippModel_v3异步成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManagerAipp_UTest, runAippModelV2_002)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerAippTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_BuiltModel_GetTensorAippInfo(builtModel, -1, &aippParaNum, &batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    std::cout << "++++++aippParaNum=" << aippParaNum << endl;
    aippParas = new HIAI_TensorAippPara*[aippParaNum];
    ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, -1, aippParas, aippParaNum, batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    HIAI_ModelManagerListener* listener = nullptr;
    void* userData = nullptr;
    int32_t timeoutInMS = 100;
    if (param.isASync) {
        listener = managerListener;
        userData = new (std::nothrow) RunAsyncContext();
    }

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum, aippParas, aippParaNum,
        outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    if (param.isASync) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_runAippModelV2
* 测试用例描述:
    1.测试HIAI_ModelManager_runAippModel场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManagerAipp_UTest, runAippModelV2_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/aipp.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_BuiltModel_GetTensorAippInfo(builtModel, -1, &aippParaNum, &batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    std::cout << "++++++aippParaNum=" << aippParaNum << endl;

    aippParas = new HIAI_TensorAippPara*[aippParaNum];
    ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, -1, aippParas, aippParaNum, batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    void* userData = new (std::nothrow) RunAsyncContext();
    int32_t timeoutInMS = 100;
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    void* func = dlsym(handle, "HIAI_ModelManager_runAippModel");
    MOCKER(&HIAI_Foundation_GetSymbol)
        .stubs()
        .with(contains("HIAI_ModelManager_runAippModel_v3"))
        .will(returnValue((void*)nullptr));

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(contains("HIAI_ModelManager_runAippModel")).will(returnValue(func));

    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum, aippParas, aippParaNum,
        outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_runAippModelV2
* 测试用例描述:
    1.测试获取HIAI_ModelManager_runAippModel/HIAI_ModelManager_runAippModel_v3符号失败场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManagerAipp_UTest, runAippModelV2_004)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/aipp.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_BuiltModel_GetTensorAippInfo(builtModel, -1, &aippParaNum, &batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    std::cout << "++++++aippParaNum=" << aippParaNum << endl;

    aippParas = new HIAI_TensorAippPara*[aippParaNum];
    ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, -1, aippParas, aippParaNum, batchCount);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
    int32_t timeoutInMS = 100;
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().will(returnValue((void*)nullptr));

    ret = HIAI_DIRECT_ModelManager_runAippModelV2(modelManager, inputs.data(), inputNum, aippParas, aippParaNum,
        outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
    delete userData;
}
