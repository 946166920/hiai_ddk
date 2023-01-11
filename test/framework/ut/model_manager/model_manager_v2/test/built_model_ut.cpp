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

#include "securec.h"

#include "model/built_model.h"
#include "model/built_model_aipp.h"
#include "model/built_model/customdata_util.h"
#include "util/file_util.h"
#include "framework/common/types.h"

using namespace std;
using namespace hiai;

namespace hiai {
    const uint32_t MODEL_VERSION = 0x10000000;
    const uint32_t MODEL_FILE_HEAD_LEN = 256;
}
class BuiltModelUt : public testing::Test {
public:
    void SetUp()
    {
        builtModel_ = CreateBuiltModel();
    }

    void TearDown()
    {
        GlobalMockObject::verify();
    }

    std::shared_ptr<IBuiltModel> builtModel_ {nullptr};
};

/*
 * 测试用例名称: TestCase_Built_Model_restore_001
 * 测试用例描述: RestoreFromBuffer, buffer大小小于CUST4字节
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_restore_001)
{
    std::shared_ptr<IBuffer> buffer = CreateLocalBuffer(3);
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(buffer));
}

std::shared_ptr<IBuffer> CreateV2DynamicAipp()
{
    std::shared_ptr<IBuffer> buffer = CreateLocalBuffer(1000);

    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};
    memcpy_s((uint8_t*)buffer->GetData(), buffer->GetSize(), CUST_DATA_TAG, strlen(CUST_DATA_TAG));
    int customDataLen = customModelData.type.size() + customModelData.value.size();
    memcpy_s((uint8_t*)buffer->GetData() + 4, buffer->GetSize() - 4, &customDataLen, sizeof(int32_t));
    int customDataTypeLen = customModelData.type.size();
    memcpy_s((uint8_t*)buffer->GetData() + 8, buffer->GetSize() - 8, &customDataTypeLen, sizeof(int32_t));
    memcpy_s((uint8_t*)buffer->GetData() + 12, buffer->GetSize() - 12, // 12 偏移量
             customModelData.type.data(), customModelData.type.size());
    int customDataValueLen = customModelData.value.size();
    memcpy_s((uint8_t*)buffer->GetData() + 32, buffer->GetSize() - 32, &customDataValueLen, sizeof(int));
    memcpy_s((uint8_t*)buffer->GetData() + 36, buffer->GetSize() - 36, customModelData.value.data(),
        customModelData.value.size());

    ModelFileHeader header;
    header.modeltype = HCS_PARTITION_MODEL;
    memcpy_s((uint8_t*)buffer->GetData() + 344, buffer->GetSize() - 344, &header, sizeof(ModelFileHeader));
    cout << "customModelData.value.size()=" << customModelData.value.size() <<endl;
    return buffer;
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_002
 * 测试用例描述: RestoreFromBuffer, 带有aipp标志的模型，且customData信息完善
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_restore_002)
{
    std::shared_ptr<IBuffer> buffer = CreateV2DynamicAipp();
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromBuffer(buffer));
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_003
 * 测试用例描述: RestoreFromBuffer, 带有aipp标志的模型,但CustomModelData不完整
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_restore_003)
{
    /*
    buffer structure:
    size   4             4              4     type.size      4      value.size
        -----------------------------------------------------------------------------
        |"cust"|type.size+value.size|type.size|type.data|value.size|value.data|model|
        -----------------------------------------------------------------------------
    type.size = 20
    value.size = 308
    4+4+4+20+4+308=344
    */
    std::shared_ptr<IBuffer> buffer = CreateLocalBuffer(1000);
    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};
    memcpy_s(buffer->GetData(), buffer->GetSize(), CUST_DATA_TAG, strlen(CUST_DATA_TAG));
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(buffer));

    int customDataLen = customModelData.type.size() + customModelData.value.size();
    memcpy_s((uint8_t*)buffer->GetData() + 4, buffer->GetSize() - 4, &customDataLen, sizeof(int32_t));
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(buffer));

    int customDataTypeLen = customModelData.type.size();
    memcpy_s((uint8_t*)buffer->GetData() + 8, buffer->GetSize() - 8, &customDataTypeLen, sizeof(int32_t));
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(buffer));

    memcpy_s((uint8_t*)buffer->GetData() + 12, buffer->GetSize() - 12,
        customModelData.type.data(), customModelData.type.size());
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(buffer));

    int customDataValueLen = customModelData.value.size();
    memcpy_s((uint8_t*)buffer->GetData() + 32, buffer->GetSize() - 32, &customDataValueLen, sizeof(int));
    ModelFileHeader header;
    header.modeltype = HCS_PARTITION_MODEL;
    memcpy_s(
        (uint8_t*)buffer->GetData() + 344, buffer->GetSize() - 344, &header, sizeof(ModelFileHeader));
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromBuffer(buffer));
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_004
 * 测试用例描述: RestoreFromBuffer, 不带有aipp标志的模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_restore_004)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    std::shared_ptr<BaseBuffer> buffer = FileUtil::LoadToBuffer(file);
    std::shared_ptr<IBuffer> localBuffer =
        CreateLocalBuffer(static_cast<void*>(buffer->MutableData()), buffer->GetSize(), false);

    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromBuffer(localBuffer));
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_005
 * 测试用例描述: RestoreFromBuffer, 多次restore
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_restore_005)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    std::shared_ptr<BaseBuffer> buffer = FileUtil::LoadToBuffer(file);
    std::shared_ptr<IBuffer> localBuffer =
        CreateLocalBuffer(static_cast<void*>(buffer->MutableData()), buffer->GetSize(), false);

    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromBuffer(localBuffer));
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromBuffer(localBuffer));
}

void CreateV2AippModelFile(const char* file)
{
    std::shared_ptr<IBuffer> buffer = CreateV2DynamicAipp();
    FileUtil::CreateEmptyFile(file);
    FileUtil::WriteBufferToFile(buffer->GetData(), buffer->GetSize(), file);
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_006
 * 测试用例描述: RestoreFromFile, v2AippModel
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_restore_006)
{
    const char* file = "out/v2AippMode.om";
    CreateV2AippModelFile(file);

    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_007
 * 测试用例描述: RestoreFromFile, 非aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_restore_007)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));
}

/*
 * 测试用例名称: TestCase_Built_Model_restore_008
 * 测试用例描述: RestoreFromFile, 多次restore
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_restore_008)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));
    EXPECT_EQ(FAILURE, builtModel_->RestoreFromFile(file));
}

/*
 * 测试用例名称: TestCase_Built_Model_save_001
 * 测试用例描述: SaveToExternalBuffer, 非aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_001)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    std::shared_ptr<IBuffer> externBuffer = CreateLocalBuffer(500);
    size_t realSize = 0;
    EXPECT_EQ(SUCCESS, builtModel_->SaveToExternalBuffer(externBuffer, realSize));
    EXPECT_EQ(100, realSize);
}

/*
 * 测试用例名称: TestCase_Built_Model_save_002
 * 测试用例描述: SaveToExternalBuffer, aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_002)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

    builtModel_->SetCustomData(customModelData);
    std::shared_ptr<IBuffer> externBuffer = CreateLocalBuffer(500);
    size_t realSize = 0;
    EXPECT_EQ(SUCCESS, builtModel_->SaveToExternalBuffer(externBuffer, realSize));
    EXPECT_EQ(100 + 344, realSize);
}

/*
 * 测试用例名称: TestCase_Built_Model_save_003
 * 测试用例描述: SaveToExternalBuffer, aipp模型, 外部内存较小
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_save_003)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

    builtModel_->SetCustomData(customModelData);
    std::shared_ptr<IBuffer> externBuffer = CreateLocalBuffer(343);
    size_t realSize = 0;
    EXPECT_EQ(FAILURE, builtModel_->SaveToExternalBuffer(externBuffer, realSize));
    EXPECT_EQ(0, realSize);
}

/*
 * 测试用例名称: TestCase_Built_Model_save_004
 * 测试用例描述: SaveToFile, 非aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_004)
{
    const char* omfile = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_npucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(omfile));

    // file 为null
    EXPECT_EQ(FAILURE, builtModel_->SaveToFile(nullptr));

    const char* file = "out/test_built_model_save_004.om";
    EXPECT_EQ(SUCCESS, builtModel_->SaveToFile(file));
}

/*
 * 测试用例名称: TestCase_Built_Model_save_005
 * 测试用例描述: SaveToFile, aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_005)
{
    const char* omfile = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(omfile));

    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

    builtModel_->SetCustomData(customModelData);

    const char* file = "out/test_built_model_save_005.om";
    EXPECT_EQ(SUCCESS, builtModel_->SaveToFile(file));
}

/*
 * 测试用例名称: TestCase_Built_Model_save_006
 * 测试用例描述: SaveToBuffer, 非aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_006)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    std::shared_ptr<IBuffer> bufferToUser = nullptr;
    EXPECT_EQ(SUCCESS, builtModel_->SaveToBuffer(bufferToUser));
}

/*
 * 测试用例名称: TestCase_Built_Model_save_007
 * 测试用例描述: SaveToBuffer, aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_save_007)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    AippPreprocessConfig aippPreprocessConfig;
    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

    builtModel_->SetCustomData(customModelData);

    std::shared_ptr<IBuffer> bufferToUser = nullptr;
    ASSERT_EQ(SUCCESS, builtModel_->SaveToBuffer(bufferToUser));
}

/*
 * 测试用例名称: TestCase_Built_Model_inputDesc_001
 * 测试用例描述: GetInputTensorDescs, 非aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_inputDesc_001)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    std::vector<NDTensorDesc> desc = builtModel_->GetInputTensorDescs();
    ASSERT_EQ(2, desc.size());
    ASSERT_EQ(1, desc[0].dims[0]);
    ASSERT_EQ(2, desc[0].dims[1]);
    ASSERT_EQ(255, desc[0].dims[2]);
    ASSERT_EQ(255, desc[0].dims[3]);
}

/*
 * 测试用例名称: TestCase_Built_Model_inputDesc_002
 * 测试用例描述: GetInputTensorDescs, aipp模型
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_inputDesc_002)
{
    const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    AippPreprocessConfig aippPreprocessConfig;
    aippPreprocessConfig.graphDataIdx = 0;
    aippPreprocessConfig.tensorDataIdx = 0;
    aippPreprocessConfig.configDataCnt = 2;
    aippPreprocessConfig.configDataInfo[0].idx = 1;
    aippPreprocessConfig.configDataInfo[0].type = 0; // AIPP_FUNC_IMAGE_CROP_V2
    aippPreprocessConfig.configDataInfo[1].idx = 2;
    aippPreprocessConfig.configDataInfo[1].type = 1; // AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2

    std::vector<AippPreprocessConfig> aippConfigList;
    aippConfigList.push_back(aippPreprocessConfig);
    CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
        {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

    builtModel_->SetCustomData(customModelData);

    std::vector<NDTensorDesc> desc = builtModel_->GetInputTensorDescs();
    ASSERT_EQ(4, desc.size());
    ASSERT_EQ(1, desc[0].dims[0]);
    ASSERT_EQ(2, desc[0].dims[1]);
    ASSERT_EQ(255, desc[0].dims[2]);
    ASSERT_EQ(255, desc[0].dims[3]);
}

/*
 * 测试用例名称: TestCase_Built_Model_getaippinfo_001
 * 测试用例描述: 获取AIPP参数
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_getaippinfo_001)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    int32_t index = 0;
    uint32_t aippParaNum = 0;
    uint32_t batchCount = 0;

    std::shared_ptr<IBuiltModelAipp> managerAipp = std::dynamic_pointer_cast<IBuiltModelAipp>(builtModel_);
    auto ret = managerAipp->GetTensorAippInfo(index, &aippParaNum, &batchCount);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(0, aippParaNum);
    EXPECT_NE(0, batchCount);
}

/*
 * 测试用例名称: TestCase_Built_Model_getaippinfo_002
 * 测试用例描述: 获取AIPP参数
 * 预期结果 :失败
 */
TEST_F(BuiltModelUt, Built_Model_getaippinfo_002)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    int32_t index = 0;
    std::shared_ptr<IBuiltModelAipp> managerAipp = std::dynamic_pointer_cast<IBuiltModelAipp>(builtModel_);
    auto ret = managerAipp->GetTensorAippInfo(index, nullptr, nullptr);
    EXPECT_EQ(FAILURE, ret);
}

/*
 * 测试用例名称: TestCase_Built_Model_getaipppara_001
 * 测试用例描述: 获取AIPP参数
 * 预期结果 :成功
 */
TEST_F(BuiltModelUt, Built_Model_getaipppara_001)
{
    const char* file = "bin/llt/framework/domi/modelmanager/tf_softmax_hcs_cpucl.om";
    EXPECT_EQ(SUCCESS, builtModel_->RestoreFromFile(file));

    int32_t index = 0;
    std::vector<std::shared_ptr<IAIPPPara>> aippParas;

    std::shared_ptr<IBuiltModelAipp> managerAipp = std::dynamic_pointer_cast<IBuiltModelAipp>(builtModel_);
    auto ret = managerAipp->GetTensorAippPara(index, aippParas);
    EXPECT_EQ(SUCCESS, ret);
    EXPECT_NE(0, aippParas.size());
    for (auto aippPara : aippParas) {
        EXPECT_NE(aippPara.get(), nullptr);
        EXPECT_NE(aippPara->GetBatchCount(), 0);
        EXPECT_EQ(aippPara->GetInputIndex(), index);
        DtcPara dtcPara = aippPara->GetDtcPara(0);
        EXPECT_EQ(dtcPara.pixelVarReciChn0, 0.0);
        EXPECT_EQ(dtcPara.pixelVarReciChn1, 0.0);
        EXPECT_EQ(dtcPara.pixelVarReciChn2, 0.0);
        EXPECT_EQ(dtcPara.pixelVarReciChn3, 0.0);
    }
}
