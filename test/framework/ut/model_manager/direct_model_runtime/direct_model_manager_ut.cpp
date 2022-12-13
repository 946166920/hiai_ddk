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

#include "model_runtime/direct/direct_model_manager.h"
#include "model_runtime/direct/direct_built_model.h"
#include "model_runtime/direct/direct_model_util.h"
#include "model_manager/core/model_manager_impl.h"
#include "util/hiai_foundation_dl_helper.h"
#include "util/file_util.h"

using namespace std;
using namespace hiai;

vector<int> g_AbnormalValue = {-1, INT_MAX};

#define ON_ERROR_VALUE -100
#define ON_SERVICE_DIED_VLAUE -200
#define ON_TIMEOUT_VALUE -300

struct ManagerTestParams {
    string modelFile;
    bool isASync;
};

static vector<ManagerTestParams> g_TestParams = {{"bin/llt/framework/domi/modelmanager/tf_softmax_v100.om", false},
    {"bin/llt/framework/domi/modelmanager/tf_softmax_hcs_npucl.om", true}};

static void OnRunDone(void* userData, HIAI_Status errCode, HIAI_NDTensorBuffer* outputs[], int32_t outputNum)
{
    cout << "++++++" << __func__ << " userData: " << userData << " errCode: " << errCode
         << " outputs: " << (void*)outputs << " outputNum: " << outputNum << endl;
    if (outputs == nullptr) {
        cout << "outputs is null" << endl;
        return;
    }

    for (int32_t i = 0; i < outputNum; i++) {
        HIAI_NDTensorBuffer* output = outputs[i];
        void* data = HIAI_NDTensorBuffer_GetData(output);
        cout << "data=" << data << endl;
    }

    if (userData != nullptr) {
        delete (RunAsyncContext*)userData;
    }
}

static void OnServiceDied(void* userData)
{
    cout << "++++++" << __func__ << " userData:" << userData << endl;
}

class DirectModelManager_UTest : public testing::TestWithParam<ManagerTestParams> {
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

        initOptions = HIAI_ModelInitOptions_Create();
        buildOptions = HIAI_ModelBuildOptions_Create();
        HIAI_ModelInitOptions_SetBuildOptions(initOptions, buildOptions);
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

    void ModelManagerRun(const ManagerTestParams& param)
    {
        HIAI_Status ret = HIAI_SUCCESS;
        int32_t inputNum = GetInputTensorNum();
        int32_t outputNum = GetOutputTensorNum();

        cout << "input num:" << inputNum << " output num:" << outputNum << " outputs addr:" << (void*)outputs.data()
            << endl;

        // 仅内部加载, 推理失败
        ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
        EXPECT_FALSE(ret == HIAI_SUCCESS);

        // 外部加载推理
        if (param.isASync) {
            cout << "++++++HIAI_DIRECT_ModelManager_RunAsync " << endl;
            ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
            EXPECT_TRUE(ret == HIAI_SUCCESS);
            int32_t timeoutInMS = 100;
            RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
            ret = HIAI_DIRECT_ModelManager_RunAsync(
                modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
            EXPECT_TRUE(ret == HIAI_SUCCESS);
            this_thread::sleep_for(chrono::milliseconds(100));
        } else {
            cout << "++++++HIAI_DIRECT_ModelManager_Run" << endl;
            ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
            EXPECT_TRUE(ret == HIAI_SUCCESS);

            ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
            EXPECT_TRUE(ret == HIAI_SUCCESS);
        }
    }
private:
    HIAI_ModelManager* modelManager = nullptr;
    HIAI_BuiltModel* builtModel = nullptr;
    HIAI_ModelManagerListener* managerListener = nullptr;
    vector<HIAI_NDTensorBuffer*> inputs;
    vector<HIAI_NDTensorBuffer*> outputs;
    HIAI_ModelBuildOptions* buildOptions = nullptr;
    HIAI_ModelInitOptions* initOptions = nullptr;
    void* handle = nullptr;
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
};

INSTANTIATE_TEST_CASE_P(Init, DirectModelManager_UTest, testing::ValuesIn(g_TestParams));

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Destroy
* 测试用例描述:
    1.入参为空场景
    2.入参非空场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Destroy)
{
    HIAI_ModelManager* manager = HIAI_DIRECT_ModelManager_Create();
    EXPECT_TRUE(manager != nullptr);
    HIAI_DIRECT_ModelManager_Destroy(&manager);
    HIAI_DIRECT_ModelManager_Destroy(&manager);
    manager = nullptr;
    HIAI_DIRECT_ModelManager_Destroy(&manager);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.入参manager为空场景
    2.入参options为空场景
    3.入参builtModel为空场景
    4.同步加载
    5.异步加载
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Init_001)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    // 入参manager为空
    ret = HIAI_DIRECT_ModelManager_Init(nullptr, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // 入参options为空
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, nullptr, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    // 入参builtModel为空
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, nullptr, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证重复加载场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Init_002)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_Status ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    // 验证重复加载场景
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // 验证重复异步加载场景
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_PerfMode场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_PerfMode 场景
    for (auto value : g_AbnormalValue) {
        HIAI_ModelInitOptions_SetPerfMode(initOptions, static_cast<HIAI_PerfMode>(value));
        cout << "++++++HIAI_PerfMode=" << HIAI_ModelInitOptions_GetPerfMode(initOptions) << endl;
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }

    for (int perfmode = HIAI_PERFMODE_UNSET; perfmode <= HIAI_PERFMODE_EXTREME; perfmode++) {
        HIAI_ModelInitOptions_SetPerfMode(initOptions, static_cast<HIAI_PerfMode>(perfmode));
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_FORMAT_MODE_OPTION场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_004)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_FORMAT_MODE_OPTION 场景, 只支持 HIAI_FORMAT_MODE_USE_NCHW
    for (auto value : g_AbnormalValue) {
        HIAI_ModelBuildOptions_SetFormatModeOption(buildOptions, static_cast<HIAI_FORMAT_MODE_OPTION>(value));
        auto mode = HIAI_ModelBuildOptions_GetFormatModeOption(buildOptions);
        EXPECT_TRUE(static_cast<HIAI_FORMAT_MODE_OPTION>(value) != mode);
    }

    HIAI_ModelBuildOptions_SetFormatModeOption(buildOptions, HIAI_FORMAT_MODE_USE_ORIGIN);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    HIAI_ModelBuildOptions_SetFormatModeOption(buildOptions, HIAI_FORMAT_MODE_USE_NCHW);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_PRECISION_MODE_OPTION场景,HIAI_PRECISION_MODE_FP32、HIAI_PRECISION_MODE_FP16都支持
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_005)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_PRECISION_MODE_OPTION 场景, 只支持 HIAI_PRECISION_MODE_FP32
    for (auto value : g_AbnormalValue) {
        ret =
            HIAI_ModelBuildOptions_SetPrecisionModeOption(buildOptions, static_cast<HIAI_PRECISION_MODE_OPTION>(value));
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }

    HIAI_ModelBuildOptions_SetPrecisionModeOption(buildOptions, HIAI_PRECISION_MODE_FP16);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    HIAI_ModelBuildOptions_SetPrecisionModeOption(buildOptions, HIAI_PRECISION_MODE_FP32);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_TUNING_STRATEGY场景,只支持 HIAI_TUNING_STRATEGY_OFF
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_006)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    };

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_TUNING_STRATEGY 场景, 只支持 HIAI_TUNING_STRATEGY_OFF
    for (auto value : g_AbnormalValue) {
        ret = HIAI_ModelBuildOptions_SetTuningStrategy(buildOptions, static_cast<HIAI_TUNING_STRATEGY>(value));
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }

    HIAI_ModelBuildOptions_SetTuningStrategy(buildOptions, HIAI_TUNING_STRATEGY_ON_CLOUD_TUNING);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    HIAI_ModelBuildOptions_SetTuningStrategy(buildOptions, HIAI_TUNING_STRATEGY_OFF);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_DYNAMIC_SHAPE_ENABLE_MODE场景,只支持 HIAI_DYNAMIC_SHAPE_DISABLE
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_007)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_DYNAMIC_SHAPE_ENABLE_MODE 场景, 只支持 HIAI_DYNAMIC_SHAPE_DISABLE
    HIAI_DynamicShapeConfig* dynamicShapeConfig = HIAI_DynamicShapeConfig_Create();
    HIAI_ModelBuildOptions_SetDynamicShapeConfig(buildOptions, dynamicShapeConfig);

    for (auto value : g_AbnormalValue) {
        HIAI_DynamicShapeConfig_SetEnableMode(dynamicShapeConfig, static_cast<HIAI_DYNAMIC_SHAPE_ENABLE_MODE>(value));
        auto mode = HIAI_DynamicShapeConfig_GetEnableMode(dynamicShapeConfig);
        EXPECT_TRUE(static_cast<HIAI_DYNAMIC_SHAPE_ENABLE_MODE>(value) != mode);
    }

    HIAI_DynamicShapeConfig_SetEnableMode(dynamicShapeConfig, HIAI_DYNAMIC_SHAPE_ENABLE);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    HIAI_DynamicShapeConfig_SetEnableMode(dynamicShapeConfig, HIAI_DYNAMIC_SHAPE_DISABLE);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证不同HIAI_DEVICE_CONFIG_MODE场景,只支持 HIAI_DEVICE_CONFIG_MODE_AUTO
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Init_008)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // 验证不同 HIAI_DEVICE_CONFIG_MODE 场景, 只支持 HIAI_DEVICE_CONFIG_MODE_AUTO
    HIAI_ModelDeviceConfig* devConfig = HIAI_ModelDeviceConfig_Create();
    HIAI_ModelBuildOptions_SetModelDeviceConfig(buildOptions, devConfig);

    for (auto value : g_AbnormalValue) {
        HIAI_ModelDeviceConfig_SetDeviceConfigMode(devConfig, static_cast<HIAI_DEVICE_CONFIG_MODE>(value));
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }

    vector<HIAI_DEVICE_CONFIG_MODE> devConfigModes = {
        HIAI_DEVICE_CONFIG_MODE_AUTO, HIAI_DEVICE_CONFIG_MODE_MODEL_LEVEL, HIAI_DEVICE_CONFIG_MODE_OP_LEVEL};

    for (auto& mode : devConfigModes) {
        HIAI_ModelDeviceConfig_SetDeviceConfigMode(devConfig, mode);
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        if (mode == HIAI_DEVICE_CONFIG_MODE_AUTO) {
            EXPECT_TRUE(ret == HIAI_SUCCESS);
        } else {
            EXPECT_TRUE(ret != HIAI_SUCCESS);
        }
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Init
* 测试用例描述:
    1.验证现获取desc，再加载场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Init_009)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    size_t index = 0;
    HIAI_NDTensorDesc* desc = HIAI_DIRECT_BuiltModel_GetInputTensorDesc(builtModel, index);
    EXPECT_TRUE(desc != nullptr);
    HIAI_NDTensorDesc_Destroy(&desc);

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_SetPriority
* 测试用例描述:
    1.入参 manager为空场景
    2.模型未外部加载场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, SetPriority_001)
{
    // 入参manager为空
    HIAI_Status ret = HIAI_DIRECT_ModelManager_SetPriority(nullptr, HIAI_PRIORITY_MIDDLE);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // 模型未加载场景
    ret = HIAI_DIRECT_ModelManager_SetPriority(modelManager, HIAI_PRIORITY_MIDDLE);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_SetPriority
* 测试用例描述:
    1.非法 priority场景
    2.成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, SetPriority_002)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_Status ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    for (auto value : g_AbnormalValue) {
        HIAI_ModelPriority priority = static_cast<HIAI_ModelPriority>(value);
        ret = HIAI_DIRECT_ModelManager_SetPriority(modelManager, priority);
        EXPECT_FALSE(ret == HIAI_SUCCESS);
    }

    for (int priority = HIAI_PRIORITY_HIGH; priority <= HIAI_PRIORITY_LOW; priority++) {
        ret = HIAI_DIRECT_ModelManager_SetPriority(modelManager, static_cast<HIAI_ModelPriority>(priority));
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_SetPriority
* 测试用例描述:
    1.验证HIAI_Foundation_GetSymbol异常场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, SetPriority_003)
{
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_Status ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    MOCKER(HIAI_Foundation_GetSymbol).expects(once()).will(returnValue((void*)nullptr));

    ret = HIAI_DIRECT_ModelManager_SetPriority(modelManager, HIAI_PRIORITY_MIDDLE);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run
* 测试用例描述:
    1.入参manager为空场景
    2.入参input为空场景
    3.入参inputNum非法场景
    4.入参output为空场景
    5.入参outputNum非法场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Run_001)
{
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    cout << "input num:" << inputNum << " output num:" << outputNum << endl;
    HIAI_Status ret = HIAI_DIRECT_ModelManager_Run(nullptr, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, nullptr, inputNum, outputs.data(), outputNum);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum + 1, outputs.data(), outputNum);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, nullptr, outputNum);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), -1);
    EXPECT_FALSE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* 测试用例描述:
    1.内部加载后直接推理场景
    2.外部加载推理成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Run_002)
{
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }
    ModelManagerRun(param);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* 测试用例描述:
    1.从文件加载后直接推理场景
    2.外部加载推理成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, RestoreFromFile_Run)
{
    ManagerTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile.c_str());
    EXPECT_TRUE(builtModel != nullptr);
    ModelManagerRun(param);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run
* 测试用例描述:
    获取符号表失败情形
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Run_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().will(returnValue((void*)nullptr));

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run
* 测试用例描述:
    测试HIAI_ModelManager_runModel_v3符号不存在, 执行HIAI_ModelManager_runModel场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Run_004)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    void* func = dlsym(handle, "HIAI_ModelManager_runModel");
    MOCKER(&HIAI_Foundation_GetSymbol)
        .stubs()
        .with(contains("HIAI_ModelManager_runModel_v3"))
        .will(returnValue((void*)nullptr));

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(contains("HIAI_ModelManager_runModel")).will(returnValue(func));

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run
* 测试用例描述:
    测试HIAI_ModelManager_runModel_v3符号不存在, 执行HIAI_ModelManager_runModel,输入为5D场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Run_005)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    vector<int> dims = {1, 2, 3, 4, 5};
    HIAI_NDTensorDesc* ndTensorDesc =
        HIAI_NDTensorDesc_Create(dims.data(), dims.size(), HIAI_DATATYPE_FLOAT16, HIAI_FORMAT_ND);
    HIAI_NDTensorBuffer* ndTensorBuffer = HIAI_NDTensorBuffer_CreateFromNDTensorDesc(ndTensorDesc);
    HIAI_NDTensorDesc_Destroy(&ndTensorDesc);
    inputs.push_back(ndTensorBuffer);
    inputNum += 1;

    void* func = dlsym(handle, "HIAI_ModelManager_runModel");
    MOCKER(&HIAI_Foundation_GetSymbol)
        .stubs()
        .with(contains("HIAI_ModelManager_runModel_v3"))
        .will(returnValue((void*)nullptr));

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().with(contains("HIAI_ModelManager_runModel")).will(returnValue(func));

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_RunAsync
* 测试用例描述:
    1.入参manager为空场景
    2.入参input为空场景
    3.入参inputNum非法场景
    4.入参output为空场景
    5.入参outputNum非法场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, RunAsync_001)
{
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    int32_t timeoutInMS = 0;
    RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
    cout << "input num:" << inputNum << " output num:" << outputNum << endl;
    HIAI_Status ret = HIAI_DIRECT_ModelManager_RunAsync(
        nullptr, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, nullptr, inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum + 1, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, nullptr, outputNum, timeoutInMS, userData);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), -1, timeoutInMS, userData);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    if (userData != nullptr) {
        delete userData;
    }
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* 测试用例描述:
    1. 测试OnError场景
    2. 测试OnServiceDied场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, RunAsync_002)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    // 外部加载推理
    cout << "++++++HIAI_DIRECT_ModelManager_RunAsync " << endl;
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    // 测试OnError场景
    int32_t timeoutInMS = ON_ERROR_VALUE;
    RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));

    // 测试OnServiceDied场景
    timeoutInMS = ON_SERVICE_DIED_VLAUE;
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));

    // 测试OnServiceDied场景
    timeoutInMS = ON_TIMEOUT_VALUE;
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Deinit
* 测试用例描述:
    1.入参为空场景
    2.模型未加载模型管家
    3.成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, DeInit)
{
    HIAI_Status ret = HIAI_DIRECT_ModelManager_Deinit(nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Cancel
* 测试用例描述:
    1.入参为空场景
    2.模型未加载模型管家
    3.成功场景
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectModelManager_UTest, Cancel_001)
{
    HIAI_Status ret = HIAI_DIRECT_ModelManager_Cancel(nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Cancel(modelManager);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Cancel(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* 测试用例名称: HIAI_DIRECT_ModelManager_Cancel
* 测试用例描述:
    获取符号表失败情形
* 预置条件:
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_F(DirectModelManager_UTest, Cancel_002)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);

    MOCKER(&HIAI_Foundation_GetSymbol).stubs().will(returnValue((void*)nullptr));
    ret = HIAI_DIRECT_ModelManager_Cancel(modelManager);
    EXPECT_FALSE(ret != HIAI_SUCCESS);
}
