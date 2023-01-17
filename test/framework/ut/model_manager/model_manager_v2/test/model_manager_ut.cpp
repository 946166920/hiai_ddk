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
#include <dlfcn.h>

#include "model_manager/model_manager.h"
#include "model/built_model_aipp.h"
#include "tensor/image_config_tensor_util.h"
#include "tensor/image_tensor_buffer.h"

using namespace std;
using namespace hiai;

class ModelManagerUt : public testing::Test {
public:
    void SetUp()
    {
        builtModel_ = CreateBuiltModel();
        const char* file = "bin/llt/framework/domi/modelmanager/om/tf_softmax_hcs_cpucl.om";
        builtModel_->RestoreFromFile(file);

        modelManager_ = CreateModelManager();
    }

    void SetCustomData(AippPreprocessConfig& aippPreprocessConfig)
    {
        std::vector<AippPreprocessConfig> aippConfigList;
        aippConfigList.push_back(aippPreprocessConfig);
        CustomModelData customModelData {AIPP_PREPROCESS_TYPE,
            {reinterpret_cast<char*>(aippConfigList.data()), aippConfigList.size() * sizeof(AippPreprocessConfig)}};

        builtModel_->SetCustomData(customModelData);
    }

    void TearDown()
    {
        GlobalMockObject::verify();
    }

public:
    std::shared_ptr<IBuiltModel> builtModel_ {nullptr};
    std::shared_ptr<IModelManager> modelManager_ {nullptr};
};

/*
 * 测试用例名称: TestCase_Model_Manager_Run_001
 * 测试用例描述: Run, 同步推理，非Aipp
 * 预期结果 :成功
 */
TEST_F(ModelManagerUt, Model_Manager_Run_001)
{
    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    EXPECT_EQ(SUCCESS, modelManager_->Run(inputs, outputs));

    uint32_t stats = 0;
    MOCKER(dlsym).stubs().will(returnValue(reinterpret_cast<void*>(&stats)));
    EXPECT_NE(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));
}

/*
 * 测试用例名称: TestCase_Model_Manager_Run_002
 * 测试用例描述: Run, 同步推理，Aipp,6种小算子
 * 预期结果 :成功
 */
TEST_F(ModelManagerUt, Model_Manager_Run_002)
{
    AippPreprocessConfig aippPreprocessConfig;
    aippPreprocessConfig.graphDataIdx = 0;
    aippPreprocessConfig.tensorDataIdx = 0;
    aippPreprocessConfig.configDataCnt = 6;
    aippPreprocessConfig.configDataInfo[0].idx = 1;
    aippPreprocessConfig.configDataInfo[0].type = 0; // AIPP_FUNC_IMAGE_CROP_V2
    aippPreprocessConfig.configDataInfo[1].idx = 2;
    aippPreprocessConfig.configDataInfo[1].type = 1; // AIPP_FUNC_IMAGE_CHANNEL_SWAP_V2
    aippPreprocessConfig.configDataInfo[2].idx = 3;
    aippPreprocessConfig.configDataInfo[2].type = 2; // AIPP_FUNC_IMAGE_COLOR_SPACE_CONVERTION_V2
    aippPreprocessConfig.configDataInfo[3].idx = 4;
    aippPreprocessConfig.configDataInfo[3].type = 4; // AIPP_FUNC_IMAGE_DATA_TYPE_CONVERSION_V2
    aippPreprocessConfig.configDataInfo[4].idx = 5;
    aippPreprocessConfig.configDataInfo[4].type = 6; // AIPP_FUNC_IAMGE_PADDING_V2
    aippPreprocessConfig.configDataInfo[5].idx = 6;
    aippPreprocessConfig.configDataInfo[5].type = 3; // AIPP_FUNC_IAMGE_PADDING_V2
    aippPreprocessConfig.aippParamInfo.enableCrop = true;
    aippPreprocessConfig.aippParamInfo.cropPara.cropSizeW = 255;
    aippPreprocessConfig.aippParamInfo.cropPara.cropSizeH = 255;
    aippPreprocessConfig.aippParamInfo.cscPara.imageFormat = ImageFormat::RGB888;
    aippPreprocessConfig.aippParamInfo.resizePara.resizeOutputSizeW = 200;
    aippPreprocessConfig.aippParamInfo.resizePara.resizeOutputSizeH = 200;

    SetCustomData(aippPreprocessConfig);

    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    std::shared_ptr<IImageTensorBuffer> input =
        CreateImageTensorBuffer(1, 255, 255, ImageFormat::AYUV444, ImageColorSpace::BT_601_NARROW, 0);
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    inputs.push_back(input);

    CropPara crop;
    crop.cropStartPosW = 4;
    crop.cropStartPosH = 4;
    crop.cropSizeW = 160;
    crop.cropSizeH = 160;
    std::shared_ptr<INDTensorBuffer> cropParaTesnor = CreateImageConfigTensor(crop);
    inputs.push_back(cropParaTesnor);

    ChannelSwapPara channelswp;
    channelswp.rbuvSwapSwitch = true;
    channelswp.axSwapSwitch = false;
    std::shared_ptr<INDTensorBuffer> channelswpParaTesnor = CreateImageConfigTensor(channelswp);
    inputs.push_back(channelswpParaTesnor);

    CscPara csc;
    csc.imageFormat = ImageFormat::RGB888;
    csc.outputFormat = ImageFormat::RGB888;
    csc.imageColorSpace = ImageColorSpace::JPEG;
    std::shared_ptr<INDTensorBuffer> cscParaTesnor = CreateImageConfigTensor(csc);
    cout << cscParaTesnor->GetSize() << endl;
    inputs.push_back(cscParaTesnor);

    DtcPara dtcPara;
    dtcPara.pixelMeanChn0 = 1;
    dtcPara.pixelMeanChn1 = 0;
    dtcPara.pixelMeanChn2 = 0;
    dtcPara.pixelMeanChn3 = 0;
    dtcPara.pixelMinChn0 = 1.1f;
    dtcPara.pixelMinChn1 = 0.0f;
    dtcPara.pixelMinChn2 = 0.0f;
    dtcPara.pixelMinChn3 = 0.0f;
    dtcPara.pixelVarReciChn0 = 1.1f;
    dtcPara.pixelVarReciChn1 = 1.1f;
    dtcPara.pixelVarReciChn2 = 1.1f;
    dtcPara.pixelVarReciChn3 = 1.0f;
    std::shared_ptr<INDTensorBuffer> dtctesnor = CreateImageConfigTensor(dtcPara);
    inputs.push_back(dtctesnor);

    PadPara paddingPara;
    paddingPara.paddingSizeTop = 1;
    paddingPara.paddingSizeBottom = 1;
    paddingPara.paddingSizeLeft = 1;
    paddingPara.paddingSizeRight = 1;
    std::shared_ptr<INDTensorBuffer> padTensor = CreateImageConfigTensor(paddingPara);
    inputs.push_back(padTensor);

    ResizePara resizePara;
    resizePara.resizeOutputSizeW = 100;
    resizePara.resizeOutputSizeH = 100;
    std::shared_ptr<INDTensorBuffer> resizeTensor = CreateImageConfigTensor(resizePara);
    inputs.push_back(resizeTensor);

    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    EXPECT_EQ(SUCCESS, modelManager_->Run(inputs, outputs));
}

/*
 * 测试用例名称: TestCase_Model_Manager_Run_003
 * 测试用例描述: Run, 同步推理，Aipp,只有一种小算子
 * 预期结果 :成功
 */
TEST_F(ModelManagerUt, Model_Manager_Run_003)
{
    AippPreprocessConfig aippPreprocessConfig;
    aippPreprocessConfig.graphDataIdx = 0;
    aippPreprocessConfig.tensorDataIdx = 0;
    aippPreprocessConfig.configDataCnt = 1;
    aippPreprocessConfig.configDataInfo[0].idx = 1;
    aippPreprocessConfig.configDataInfo[0].type = 0; // AIPP_FUNC_IMAGE_CROP_V2
    aippPreprocessConfig.aippParamInfo.enableCrop = true;
    aippPreprocessConfig.aippParamInfo.cropPara.cropSizeW = 255;
    aippPreprocessConfig.aippParamInfo.cropPara.cropSizeH = 255;
    aippPreprocessConfig.aippParamInfo.enableCsc = true;
    aippPreprocessConfig.aippParamInfo.enableResize = true;
    aippPreprocessConfig.aippParamInfo.enableDtc = true;
    aippPreprocessConfig.aippParamInfo.enablePadding = true;
    aippPreprocessConfig.aippParamInfo.cscPara.imageFormat = ImageFormat::RGB888;

    SetCustomData(aippPreprocessConfig);

    ModelInitOptions options;
    EXPECT_EQ(SUCCESS, modelManager_->Init(options, builtModel_, nullptr));

    std::shared_ptr<IImageTensorBuffer> input =
        CreateImageTensorBuffer(1, 500, 500, ImageFormat::AYUV444, ImageColorSpace::BT_601_NARROW, 0);
    std::vector<std::shared_ptr<INDTensorBuffer>> inputs;
    inputs.push_back(input);

    CropPara crop;
    crop.cropStartPosW = 4;
    crop.cropStartPosH = 4;
    crop.cropSizeW = 160;
    crop.cropSizeH = 160;
    std::shared_ptr<INDTensorBuffer> cropParaTesnor = CreateImageConfigTensor(crop);
    inputs.push_back(cropParaTesnor);

    std::vector<std::shared_ptr<INDTensorBuffer>> outputs;
    EXPECT_EQ(SUCCESS, modelManager_->Run(inputs, outputs));
}
