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

static void OnRunDone(void* userData, HIAI_Status errCode, HIAI_MR_NDTensorBuffer* outputs[], int32_t outputNum)
{
    cout << "++++++" << __func__ << " userData: " << userData << " errCode: " << errCode
         << " outputs: " << (void*)outputs << " outputNum: " << outputNum << endl;
    if (outputs == nullptr) {
        cout << "outputs is null" << endl;
        return;
    }

    for (int32_t i = 0; i < outputNum; i++) {
        HIAI_MR_NDTensorBuffer* output = outputs[i];
        void* data = HIAI_MR_NDTensorBuffer_GetData(output);
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
        managerListener = new HIAI_MR_ModelManagerListener;
        if (managerListener != nullptr) {
            managerListener->onRunDone = OnRunDone;
            managerListener->onServiceDied = OnServiceDied;
        }

        initOptions = HIAI_MR_ModelInitOptions_Create();
        buildOptions = HIAI_MR_ModelBuildOptions_Create();
        HIAI_MR_ModelInitOptions_SetBuildOptions(initOptions, buildOptions);
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
            HIAI_MR_NDTensorBuffer_Destroy(&tensorBuffer);
        }

        for (auto& tensorBuffer : outputs) {
            HIAI_MR_NDTensorBuffer_Destroy(&tensorBuffer);
        }

        if (initOptions != nullptr) {
            HIAI_MR_ModelInitOptions_Destroy(&initOptions);
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
                HIAI_MR_NDTensorBuffer* buffer = HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(desc);
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
                HIAI_MR_NDTensorBuffer* buffer = HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(desc);
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

        // ???????????????, ????????????
        ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
        EXPECT_FALSE(ret == HIAI_SUCCESS);

        // ??????????????????
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
    HIAI_MR_ModelManager* modelManager = nullptr;
    HIAI_MR_BuiltModel* builtModel = nullptr;
    HIAI_MR_ModelManagerListener* managerListener = nullptr;
    vector<HIAI_MR_NDTensorBuffer*> inputs;
    vector<HIAI_MR_NDTensorBuffer*> outputs;
    HIAI_MR_ModelBuildOptions* buildOptions = nullptr;
    HIAI_MR_ModelInitOptions* initOptions = nullptr;
    void* handle = nullptr;
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
};

INSTANTIATE_TEST_CASE_P(Init, DirectModelManager_UTest, testing::ValuesIn(g_TestParams));

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Destroy
* ??????????????????:
    1.??????????????????
    2.??????????????????
*/
TEST_F(DirectModelManager_UTest, Destroy)
{
    HIAI_MR_ModelManager* manager = HIAI_DIRECT_ModelManager_Create();
    EXPECT_TRUE(manager != nullptr);
    HIAI_DIRECT_ModelManager_Destroy(&manager);
    HIAI_DIRECT_ModelManager_Destroy(&manager);
    manager = nullptr;
    HIAI_DIRECT_ModelManager_Destroy(&manager);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.??????manager????????????
    2.??????options????????????
    3.??????builtModel????????????
    4.????????????
    5.????????????
*/
TEST_F(DirectModelManager_UTest, Init_001)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_infershaped.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    // ??????manager??????
    ret = HIAI_DIRECT_ModelManager_Init(nullptr, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // ??????options??????
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, nullptr, builtModel, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // ??????builtModel??????
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, nullptr, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????????????????
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

    // ????????????????????????
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // ??????????????????????????????
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_PerfMode??????
*/
TEST_P(DirectModelManager_UTest, Init_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_PerfMode ??????
    for (auto value : g_AbnormalValue) {
        HIAI_MR_ModelInitOptions_SetPerfMode(initOptions, static_cast<HIAI_PerfMode>(value));
        cout << "++++++HIAI_PerfMode=" << HIAI_MR_ModelInitOptions_GetPerfMode(initOptions) << endl;
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }

    for (int perfmode = HIAI_PERFMODE_UNSET; perfmode <= HIAI_PERFMODE_EXTREME; perfmode++) {
        HIAI_MR_ModelInitOptions_SetPerfMode(initOptions, static_cast<HIAI_PerfMode>(perfmode));
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_FORMAT_MODE_OPTION??????
*/
TEST_P(DirectModelManager_UTest, Init_004)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_FORMAT_MODE_OPTION ??????, ????????? HIAI_FORMAT_MODE_USE_NCHW
    for (auto value : g_AbnormalValue) {
        HIAI_MR_ModelBuildOptions_SetFormatModeOption(buildOptions, static_cast<HIAI_FORMAT_MODE_OPTION>(value));
        auto mode = HIAI_MR_ModelBuildOptions_GetFormatModeOption(buildOptions);
        EXPECT_TRUE(static_cast<HIAI_FORMAT_MODE_OPTION>(value) != mode);
    }

    HIAI_MR_ModelBuildOptions_SetFormatModeOption(buildOptions, HIAI_FORMAT_MODE_USE_ORIGIN);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    HIAI_MR_ModelBuildOptions_SetFormatModeOption(buildOptions, HIAI_FORMAT_MODE_USE_NCHW);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_PRECISION_MODE_OPTION??????,HIAI_PRECISION_MODE_FP32???HIAI_PRECISION_MODE_FP16?????????
*/
TEST_P(DirectModelManager_UTest, Init_005)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_PRECISION_MODE_OPTION ??????, ????????? HIAI_PRECISION_MODE_FP32
    for (auto value : g_AbnormalValue) {
        ret = HIAI_MR_ModelBuildOptions_SetPrecisionModeOption(
            buildOptions, static_cast<HIAI_PRECISION_MODE_OPTION>(value));
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }

    HIAI_MR_ModelBuildOptions_SetPrecisionModeOption(buildOptions, HIAI_PRECISION_MODE_FP16);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    HIAI_MR_ModelBuildOptions_SetPrecisionModeOption(buildOptions, HIAI_PRECISION_MODE_FP32);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_TUNING_STRATEGY??????,????????? HIAI_TUNING_STRATEGY_OFF
*/
TEST_P(DirectModelManager_UTest, Init_006)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    };

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_TUNING_STRATEGY ??????, ????????? HIAI_TUNING_STRATEGY_OFF
    for (auto value : g_AbnormalValue) {
        ret = HIAI_MR_ModelBuildOptions_SetTuningStrategy(buildOptions, static_cast<HIAI_TUNING_STRATEGY>(value));
        EXPECT_TRUE(ret != HIAI_SUCCESS);
    }

    HIAI_MR_ModelBuildOptions_SetTuningStrategy(buildOptions, HIAI_TUNING_STRATEGY_ON_CLOUD_TUNING);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    HIAI_MR_ModelBuildOptions_SetTuningStrategy(buildOptions, HIAI_TUNING_STRATEGY_OFF);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_DYNAMIC_SHAPE_ENABLE_MODE??????,????????? HIAI_DYNAMIC_SHAPE_DISABLE
*/
TEST_P(DirectModelManager_UTest, Init_007)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_DYNAMIC_SHAPE_ENABLE_MODE ??????, ????????? HIAI_DYNAMIC_SHAPE_DISABLE
    HIAI_MR_DynamicShapeConfig* dynamicShapeConfig = HIAI_MR_DynamicShapeConfig_Create();
    HIAI_MR_ModelBuildOptions_SetDynamicShapeConfig(buildOptions, dynamicShapeConfig);

    for (auto value : g_AbnormalValue) {
        HIAI_MR_DynamicShapeConfig_SetEnableMode(
            dynamicShapeConfig, static_cast<HIAI_DYNAMIC_SHAPE_ENABLE_MODE>(value));
        auto mode = HIAI_MR_DynamicShapeConfig_GetEnableMode(dynamicShapeConfig);
        EXPECT_TRUE(static_cast<HIAI_DYNAMIC_SHAPE_ENABLE_MODE>(value) != mode);
    }

    HIAI_MR_DynamicShapeConfig_SetEnableMode(dynamicShapeConfig, HIAI_DYNAMIC_SHAPE_ENABLE);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    HIAI_MR_DynamicShapeConfig_SetEnableMode(dynamicShapeConfig, HIAI_DYNAMIC_SHAPE_DISABLE);
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
    HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.????????????HIAI_DEVICE_CONFIG_MODE??????,????????? HIAI_DEVICE_CONFIG_MODE_AUTO
*/
TEST_P(DirectModelManager_UTest, Init_008)
{
    HIAI_Status ret = HIAI_SUCCESS;
    ManagerTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_MR_ModelManagerListener* listener = nullptr;
    if (param.isASync) {
        listener = managerListener;
    }

    // ???????????? HIAI_DEVICE_CONFIG_MODE ??????, ????????? HIAI_DEVICE_CONFIG_MODE_AUTO
    HIAI_MR_ModelDeviceConfig* devConfig = HIAI_MR_ModelDeviceConfig_Create();
    HIAI_MR_ModelBuildOptions_SetModelDeviceConfig(buildOptions, devConfig);

    for (auto value : g_AbnormalValue) {
        HIAI_MR_ModelDeviceConfig_SetDeviceConfigMode(devConfig, static_cast<HIAI_DEVICE_CONFIG_MODE>(value));
        ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, listener);
        HIAI_DIRECT_ModelManager_Deinit(modelManager);
        EXPECT_TRUE(ret == HIAI_SUCCESS);
    }

    vector<HIAI_DEVICE_CONFIG_MODE> devConfigModes = {
        HIAI_DEVICE_CONFIG_MODE_AUTO, HIAI_DEVICE_CONFIG_MODE_MODEL_LEVEL, HIAI_DEVICE_CONFIG_MODE_OP_LEVEL};

    for (auto& mode : devConfigModes) {
        HIAI_MR_ModelDeviceConfig_SetDeviceConfigMode(devConfig, mode);
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
* ??????????????????: HIAI_DIRECT_ModelManager_Init
* ??????????????????:
    1.???????????????desc??????????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_SetPriority
* ??????????????????:
    1.?????? manager????????????
    2.???????????????????????????
*/
TEST_F(DirectModelManager_UTest, SetPriority_001)
{
    // ??????manager??????
    HIAI_Status ret = HIAI_DIRECT_ModelManager_SetPriority(nullptr, HIAI_PRIORITY_MIDDLE);
    EXPECT_TRUE(ret != HIAI_SUCCESS);

    // ?????????????????????
    ret = HIAI_DIRECT_ModelManager_SetPriority(modelManager, HIAI_PRIORITY_MIDDLE);
    EXPECT_TRUE(ret != HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_SetPriority
* ??????????????????:
    1.?????? priority??????
    2.????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_SetPriority
* ??????????????????:
    1.??????HIAI_Foundation_GetSymbol????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run
* ??????????????????:
    1.??????manager????????????
    2.??????input????????????
    3.??????inputNum????????????
    4.??????output????????????
    5.??????outputNum????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* ??????????????????:
    1.?????????????????????????????????
    2.??????????????????????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* ??????????????????:
    1.????????????????????????????????????
    2.??????????????????????????????
*/
TEST_P(DirectModelManager_UTest, RestoreFromFile_Run)
{
    ManagerTestParams param = GetParam();
    builtModel = HIAI_DIRECT_BuiltModel_RestoreFromFile(param.modelFile.c_str());
    EXPECT_TRUE(builtModel != nullptr);
    ModelManagerRun(param);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Run
* ??????????????????:
    ???????????????????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run
* ??????????????????:
    ??????HIAI_ModelManager_runModel_v3???????????????, ??????HIAI_ModelManager_runModel??????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run
* ??????????????????:
    ??????HIAI_ModelManager_runModel_v3???????????????, ??????HIAI_ModelManager_runModel,?????????5D??????
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
    HIAI_MR_NDTensorBuffer* ndTensorBuffer = HIAI_MR_NDTensorBuffer_CreateFromNDTensorDesc(ndTensorDesc);
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run_006
* ??????????????????:
    1. ??????????????????
    2. ????????????IOTensor(??????????????????)
    3. ??????????????????
    4. ????????????????????????
*/
TEST_F(DirectModelManager_UTest, Run_006)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, nullptr);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Run_007
* ??????????????????:
    1. ????????????IOTensor??????????????????
    2. ??????????????????????????????, ???????????????????????????
    3. ??????????????????
    4. ????????????????????????
*/
TEST_F(DirectModelManager_UTest, Run_007)
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

    ret = HIAI_DIRECT_ModelManager_Run(modelManager, inputs.data(), inputNum, outputs.data(), outputNum);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_RunAsync
* ??????????????????:
    1.??????manager????????????
    2.??????input????????????
    3.??????inputNum????????????
    4.??????output????????????
    5.??????outputNum????????????
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
* ??????????????????: HIAI_DIRECT_ModelManager_Run/HIAI_DIRECT_ModelManager_RunAsync
* ??????????????????:
    1. ??????OnError??????
    2. ??????OnServiceDied??????
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

    // ??????????????????
    cout << "++++++HIAI_DIRECT_ModelManager_RunAsync " << endl;
    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    // ??????OnError??????
    int32_t timeoutInMS = ON_ERROR_VALUE;
    RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));

    // ??????OnServiceDied??????
    timeoutInMS = ON_SERVICE_DIED_VLAUE;
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));

    // ??????OnServiceDied??????
    timeoutInMS = ON_TIMEOUT_VALUE;
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
    this_thread::sleep_for(chrono::milliseconds(100));
}


/*
* ??????????????????: HIAI_DIRECT_ModelManager_RunAsync_003
* ??????????????????:
    1. ???????????????????????????
    2. ????????????IO?????????????????????
    3. ????????????????????????
    4. ????????????????????????
*/
TEST_F(DirectModelManager_UTest, RunAsync_003)
{
    HIAI_Status ret = HIAI_SUCCESS;
    string modelFile = "bin/llt/framework/domi/modelmanager/tf_softmax_v100.om";
    modelBuffer = FileUtil::LoadToBuffer(modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    ret = HIAI_DIRECT_ModelManager_Init(modelManager, initOptions, builtModel, managerListener);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    int32_t inputNum = GetInputTensorNum();
    int32_t outputNum = GetOutputTensorNum();

    int32_t timeoutInMS = 100;
    RunAsyncContext* userData = new (std::nothrow) RunAsyncContext();
    ret = HIAI_DIRECT_ModelManager_RunAsync(
        modelManager, inputs.data(), inputNum, outputs.data(), outputNum, timeoutInMS, userData);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

    this_thread::sleep_for(chrono::milliseconds(100));
    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);
}

/*
* ??????????????????: HIAI_DIRECT_ModelManager_Deinit
* ??????????????????:
    1.??????????????????
    2.???????????????????????????
    3.????????????
*/
TEST_P(DirectModelManager_UTest, DeInit)
{
    HIAI_Status ret = HIAI_DIRECT_ModelManager_Deinit(nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Deinit(modelManager);
    EXPECT_TRUE(ret == HIAI_SUCCESS);

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
* ??????????????????: HIAI_DIRECT_ModelManager_Cancel
* ??????????????????:
    1.??????????????????
    2.???????????????????????????
    3.????????????
*/
TEST_P(DirectModelManager_UTest, Cancel_001)
{
    HIAI_Status ret = HIAI_DIRECT_ModelManager_Cancel(nullptr);
    EXPECT_FALSE(ret == HIAI_SUCCESS);

    ret = HIAI_DIRECT_ModelManager_Cancel(modelManager);
    EXPECT_FALSE(ret != HIAI_SUCCESS);

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
* ??????????????????: HIAI_DIRECT_ModelManager_Cancel
* ??????????????????:
    ???????????????????????????
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
