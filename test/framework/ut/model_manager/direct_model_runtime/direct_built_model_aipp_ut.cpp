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

#include "model_runtime/direct/direct_built_model.h"
#include "model_runtime/direct/direct_built_model_aipp.h"
#include "util/file_util.h"

using namespace std;
using namespace hiai;

struct AippInfoTestParams {
    string modelFile;
    int index;
    HIAI_Status expectValue;
};

static vector<AippInfoTestParams> g_AippInfoParams = {
    {"", 0, HIAI_FAILURE}, // 校验入参builtModel为空场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", INT_MAX, HIAI_FAILURE}, // 校验入参index非法场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", -1, HIAI_SUCCESS}, // 获取全部TensorAippInfo场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", 0, HIAI_SUCCESS}, // 获取指定TensorAippInfo场景
};

class DirectBuiltModelAipp_UTest : public testing::TestWithParam<AippInfoTestParams> {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }
    }

private:
    HIAI_BuiltModel* builtModel {nullptr};
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
};

INSTANTIATE_TEST_CASE_P(AippInfo, DirectBuiltModelAipp_UTest, testing::ValuesIn(g_AippInfoParams));

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetTensorAippInfo
* 测试用例描述:
    1.校验入参model为空场景
    2.校验入参index非法场景
    3.获取全部TensorAippInfo场景
    4.获取指定TensorAippInfo场景
* 预置条件: 模型文件
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectBuiltModelAipp_UTest, GetTensorAippInfo)
{
    AippInfoTestParams param = GetParam();
    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }
    uint32_t aippParaNum = 0;
    uint32_t batchCount = 0;
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_GetTensorAippInfo(builtModel, param.index, &aippParaNum, &batchCount);
    EXPECT_TRUE(ret == param.expectValue);
    if (ret == HIAI_SUCCESS) {
        EXPECT_TRUE(aippParaNum > 0 && batchCount > 0);
    }
}

struct AippParaParams {
    string modelFile;
    int32_t index;
    uint32_t aippParaNum;
    HIAI_Status expectValue;
};

vector<AippParaParams> g_AippParaParams = {
    {"", 0, 1, HIAI_FAILURE}, // 入参builtModel为空场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", INT_MAX, 1, HIAI_FAILURE}, // 入参index非法场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", -1, 0, HIAI_FAILURE}, // 入参aippParaNum为0场景
    {"bin/llt/framework/domi/modelmanager/aipp.om", -1, 1, HIAI_SUCCESS}, // 成功场景
};

class DirectBuiltModelAipp_GetTensorAippPara_UTest : public testing::TestWithParam<AippParaParams> {
public:
    void SetUp()
    {
    }

    void TearDown()
    {
        if (builtModel != nullptr) {
            HIAI_DIRECT_BuiltModel_Destroy(&builtModel);
        }
    }

private:
    HIAI_BuiltModel* builtModel = nullptr;
    shared_ptr<BaseBuffer> modelBuffer {nullptr};
};

INSTANTIATE_TEST_CASE_P(AippPara, DirectBuiltModelAipp_GetTensorAippPara_UTest, testing::ValuesIn(g_AippParaParams));

/*
* 测试用例名称: HIAI_DIRECT_BuiltModel_GetTensorAippPara
* 测试用例描述:
    1.校验入参model为空场景
    2.校验入参index非法场景
    3.校验入参aippParas为空场景
    4.校验入参aippParaNum为0场景
    5.获取全部TensorAippPara场景
    6.获取指定TensorAippPara场景
* 预置条件: 模型文件
* 操作步骤:
* 预期结果:
* 修改历史:
*/
TEST_P(DirectBuiltModelAipp_GetTensorAippPara_UTest, GetTensorAippPara)
{
    AippParaParams param = GetParam();
    int32_t index = param.index;
    uint32_t aippParaNum = param.aippParaNum;
    uint32_t batchCount = aippParaNum;

    modelBuffer = FileUtil::LoadToBuffer(param.modelFile);
    if (modelBuffer != nullptr) {
        builtModel = HIAI_DIRECT_BuiltModel_Restore(modelBuffer->MutableData(), modelBuffer->GetSize());
    }

    HIAI_TensorAippPara** aippParas = new HIAI_TensorAippPara*[aippParaNum];
    HIAI_Status ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, index, aippParas, aippParaNum, batchCount);
    EXPECT_TRUE(ret == param.expectValue);
    if (ret == HIAI_SUCCESS) {
        for (uint32_t i = 0; i < aippParaNum; i++) {
            if (aippParas[i] != nullptr) {
                HIAI_TensorAippPara_Destroy(&aippParas[i]);
            }
        }
        // 入参aippParas为空场景
        ret = HIAI_DIRECT_BuiltModel_GetTensorAippPara(builtModel, index, nullptr, aippParaNum, batchCount);
        EXPECT_TRUE(ret == HIAI_FAILURE);
    }
    delete[] aippParas;
}
