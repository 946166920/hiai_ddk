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
#include "direct_built_model_impl.h"

#include <climits>
#include "model_runtime/data_type_convert.h"
#include "framework/c/hiai_nd_tensor_desc.h"
#include "infra/base/securestl.h"
#include "framework/infra/log/log.h"
#include "securec.h"

#ifdef AI_SUPPORT_AIPP_API
#include "framework/c/hiai_tensor_aipp_para.h"
#endif

#include "util/hiai_foundation_dl_helper.h"

#ifdef __cplusplus
extern "C" {
#endif
struct HIAI_TensorDescriptionV2 {
    int number;
    int channel;
    int height;
    int width;
    HIAI_DataType dataType;
    int size;
    const char* name;
};
#ifdef __cplusplus
}
#endif

namespace hiai {
static void HIAI_ModelTensorInfoToNDTensorDesc(const int* shape, int count, std::vector<HIAI_NDTensorDesc*>& descs)
{
    for (int i = 0, pos = 0; i < count; ++i, pos += 4) {
        std::vector<int> dims = {shape[pos], shape[pos + 1], shape[pos + 2], shape[pos + 3]};
        auto ndTensorDesc = HIAI_NDTensorDesc_Create(const_cast<int32_t*>(dims.data()), dims.size(),
            HIAI_DataType::HIAI_DATATYPE_FLOAT32, HIAI_Format::HIAI_FORMAT_NCHW);
        descs.push_back(ndTensorDesc);
    }
}

static Status GetDescByTensorInfoV2(
    const HIAI_ModelTensorInfoV2* tensorInfo, HIAI_IO_TYPE type, std::vector<HIAI_NDTensorDesc*>& descs)
{
    void* getTensorIOCount = HIAI_Foundation_GetSymbol("HIAI_ModelTensorInfoV2_getIOCount");
    void* getTensorDesc = HIAI_Foundation_GetSymbol("HIAI_ModelTensorInfoV2_getTensorDescription");
    if (getTensorIOCount == nullptr || getTensorDesc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    int count = ((int (*)(const HIAI_ModelTensorInfoV2*, HIAI_IO_TYPE))getTensorIOCount)(tensorInfo, type);
    if (count <= 0) {
        FMK_LOGE("HIAI_ModelTensorInfoV2_getIOCount failed.");
        return FAILURE;
    }

    for (int i = 0; i < count; i++) {
        HIAI_TensorDescriptionV2* tensorDescV2 = ((HIAI_TensorDescriptionV2 *
            (*)(const HIAI_ModelTensorInfoV2*, HIAI_IO_TYPE, int)) getTensorDesc)(tensorInfo, type, i);
        if (tensorDescV2 == nullptr) {
            FMK_LOGE("input shape is null");
            return FAILURE;
        }

        DataType dataType = DataType::FLOAT32;
        auto dataTypeTmp = DATA_TYPE_MAP.find((ge::DataType)tensorDescV2->dataType);
        if (dataTypeTmp != DATA_TYPE_MAP.end()) {
            dataType = dataTypeTmp->second;
        }

        std::vector<int> dims = {
            tensorDescV2->number, tensorDescV2->channel, tensorDescV2->height, tensorDescV2->width};
        auto ndTensorDesc = HIAI_NDTensorDesc_Create(const_cast<int32_t*>(dims.data()), dims.size(),
            static_cast<HIAI_DataType>(dataType), HIAI_Format::HIAI_FORMAT_NCHW);
        descs.push_back(ndTensorDesc);
    }

    return SUCCESS;
}

static HIAI_ModelTensorInfo* GetModelTensorInfo(HIAI_ModelManager* manager, const std::string& modelName)
{
    void* getModelTensorInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_getModelTensorInfo");
    if (getModelTensorInfoFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return nullptr;
    }

    return ((HIAI_ModelTensorInfo* (*)(HIAI_ModelManager*, const char*)) getModelTensorInfoFunc)(
        manager, modelName.c_str());
}

static void FreeModelTensorInfo(HIAI_ModelTensorInfo* modelTensorInfo)
{
    void* freeModelTensorInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_releaseModelTensorInfo");
    if (freeModelTensorInfoFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return;
    }

    ((void (*)(HIAI_ModelTensorInfo*))freeModelTensorInfoFunc)(modelTensorInfo);
}

DirectBuiltModelImpl::DirectBuiltModelImpl(
    std::shared_ptr<BaseBuffer>& buffer, std::string modelName, bool isNeedRelease)
    : isNeedRelease_(isNeedRelease), modelBuffer_(buffer), modelName_(std::move(modelName))
{
}

DirectBuiltModelImpl::DirectBuiltModelImpl(std::string modelFile)
    : modelFile_(std::move(modelFile))
{
}

DirectBuiltModelImpl::~DirectBuiltModelImpl()
{
    if (currentModelUtil_ != nullptr && currentModelUtil_->isLoaded_ &&
        currentModelUtil_->listener_ == nullptr) {
        (void)currentModelUtil_->UnLoadModel();
    }

    for (auto& inputDesc : inputs_) {
        HIAI_NDTensorDesc_Destroy(&inputDesc);
    }
    for (auto& outputDesc : outputs_) {
        HIAI_NDTensorDesc_Destroy(&outputDesc);
    }

    if (isNeedRelease_) {
        free(static_cast<void*>(const_cast<uint8_t*>(modelBuffer_->GetData())));
    }
}

Status DirectBuiltModelImpl::Init()
{
    SharedManagerInfos_ = make_shared_nothrow<SharedManagerInfos>();
    if (SharedManagerInfos_ == nullptr) {
        return FAILURE;
    }

    if (!modelFile_.empty() && modelName_.empty()) { // restore from file
        modelName_ = "default_" + modelFile_.substr(modelFile_.find_last_of("/\\") + 1);
    }

    SharedManagerInfos_->modelFile = modelFile_;
    SharedManagerInfos_->modelBuffer = modelBuffer_;
    SharedManagerInfos_->modelName = modelName_;
    return SUCCESS;
}

Status DirectBuiltModelImpl::CheckCompatibility(bool& isCompatible)
{
    void* CheckModelCompatibilityFunc = HIAI_Foundation_GetSymbol("HIAI_CheckModelCompatibility_from_buffer");
    void* CheckModelCompatibilityFromFileFun = HIAI_Foundation_GetSymbol("HIAI_CheckModelCompatibility_from_file");
    if (CheckModelCompatibilityFunc == nullptr || CheckModelCompatibilityFromFileFun == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    if (SharedManagerInfos_ == nullptr && Init() != SUCCESS) {
        FMK_LOGE("Init failed.");
        return FAILURE;
    }

    auto modelUtil = make_shared_nothrow<DirectModelUtil>();
    if (modelUtil == nullptr) {
        FMK_LOGE("create modelUtil failed");
        return FAILURE;
    }
    (void)modelUtil->CreateModelManager(nullptr);
    auto manager = modelUtil->GetModelManager();
    if (manager == nullptr) {
        FMK_LOGE("create model manager failed");
        return FAILURE;
    }

    if (modelBuffer_ != nullptr) {
        isCompatible = ((bool (*)(HIAI_ModelManager*, void*, const uint32_t))CheckModelCompatibilityFunc)(
        manager, modelBuffer_->MutableData(), modelBuffer_->GetSize());
    } else {
        isCompatible = ((bool (*)(HIAI_ModelManager*, const char*))CheckModelCompatibilityFromFileFun)(
        manager, SharedManagerInfos_->modelFile.c_str());
    }

    (void)modelUtil->DestroyModelManager();
    return SUCCESS;
}

Status DirectBuiltModelImpl::InnerLoad()
{
    if (currentModelUtil_ != nullptr && currentModelUtil_->isLoaded_) {
        return SUCCESS;
    }
    if (SharedManagerInfos_ == nullptr && Init() != SUCCESS) {
        FMK_LOGE("Init failed.");
        return FAILURE;
    }

    for (auto& info : SharedManagerInfos_->managers) {
        if (info != nullptr &&
            info->CheckCanReuse(HIAI_PerfMode::HIAI_PERFMODE_NORMAL, nullptr, false)) { // modelmanager已加载
            currentModelUtil_ = info;
            return SUCCESS;
        }
    }

    // 需要加载模型
    currentModelUtil_ = make_shared_nothrow<DirectModelUtil>();
    if (currentModelUtil_ == nullptr) {
        FMK_LOGE("create modelUtil failed");
        return FAILURE;
    }
    (void)currentModelUtil_->CreateModelManager(nullptr);
    int ret = 0;
    if (SharedManagerInfos_->modelFile.empty()) {
        ret = currentModelUtil_->LoadModel(SharedManagerInfos_->modelBuffer, SharedManagerInfos_->modelName,
            HIAI_PerfMode::HIAI_PERFMODE_NORMAL);
    } else {
        ret = currentModelUtil_->LoadModel(SharedManagerInfos_->modelFile, SharedManagerInfos_->modelName,
            HIAI_PerfMode::HIAI_PERFMODE_NORMAL);
    }
    if (ret < 0) {
        FMK_LOGE("load model[%s] failed.", SharedManagerInfos_->modelName.c_str());
        return FAILURE;
    }
    SharedManagerInfos_->managers.emplace_back(currentModelUtil_);

    return SUCCESS;
}

Status DirectBuiltModelImpl::GetModelIOTensorNum(uint32_t& inputCount, uint32_t& outputCount)
{
    if (InnerLoad() != SUCCESS) {
        FMK_LOGE("InnerLoad failed.");
        return FAILURE;
    }

    // 获取输入输出
    if (GetIONDTensorDesc() != SUCCESS) {
        FMK_LOGE("GetIONDTensorDesc failed");
        return FAILURE;
    }

    inputCount = inputs_.size();
    outputCount = outputs_.size();
    return SUCCESS;
}

Status DirectBuiltModelImpl::GetModelIOTensorDescs(
    std::vector<HIAI_NDTensorDesc*>& inputDescs, std::vector<HIAI_NDTensorDesc*>& outputDescs)
{
    if (InnerLoad() != SUCCESS) {
        FMK_LOGE("InnerLoad failed.");
        return FAILURE;
    }

    // 获取输入输出
    if (GetIONDTensorDesc() != SUCCESS) {
        FMK_LOGE("GetIONDTensorDesc failed");
        return FAILURE;
    }

    inputDescs = inputs_;
    outputDescs = outputs_;
    return SUCCESS;
}

#ifdef AI_SUPPORT_AIPP_API
static int GetTensorAippInputCnt(HIAI_ModelManager* manager, const std::string& modelName)
{
    int inputCount = 0;
    HIAI_ModelTensorInfo* modelTensorInfo = GetModelTensorInfo(manager, modelName);
    if (modelTensorInfo == nullptr) {
        FMK_LOGE("Unable to get model tensor info by model name: %s", modelName.c_str());
        return inputCount;
    }

    inputCount = modelTensorInfo->inputCnt;
    (void)FreeModelTensorInfo(modelTensorInfo);
    return inputCount;
}

static int GetTensorAippInfoByIndex(HIAI_ModelManager* manager, const std::string& modelName, int index,
    unsigned int* aippNum, unsigned int* batchCount)
{
    void* getTensorAippInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManger_getTensorAippInfo");
    if (getTensorAippInfoFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    return ((int (*)(HIAI_ModelManager*, const char*, unsigned int, unsigned int*,
        unsigned int*))getTensorAippInfoFunc)(manager, modelName.c_str(), index, aippNum, batchCount);
}

Status DirectBuiltModelImpl::GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    if (InnerLoad() != SUCCESS) {
        FMK_LOGE("InnerLoad failed.");
        return FAILURE;
    }

    int realIndex = index;
    if (index == -1) {
        int inputCount = GetTensorAippInputCnt(currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName);
        if (inputCount <= 0) {
            return FAILURE;
        }
        realIndex = inputCount - 1;
    }

    int ret = GetTensorAippInfoByIndex(
        currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName, realIndex, aippParaNum, batchCount);
    if (ret != 0) {
        FMK_LOGE("getTensorAippInfo failed, name: %s, index: %u", SharedManagerInfos_->modelName.c_str(), realIndex);
        return FAILURE;
    }

    if (*aippParaNum == 0) {
        FMK_LOGI("index: %u of model: %s does NOT contain aipp configuration info", realIndex,
            SharedManagerInfos_->modelName.c_str());
    }
    return SUCCESS;
}

Status DirectBuiltModelImpl::GetAippPara(
    int32_t index, unsigned int aippCount, unsigned int batchCount, std::vector<void*>& aippParaVec)
{
    void* getTensorAippParaFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManger_getTensorAippPara");
    void* getRawBufferFunc = HIAI_Foundation_GetSymbol("HIAI_TensorAipp_getRawBuffer");
    void* getRawBufferSizeFunc = HIAI_Foundation_GetSymbol("HIAI_TensorAipp_getRawBufferSize");
    if (getTensorAippParaFunc == nullptr || getRawBufferFunc == nullptr) {
        FMK_LOGE("GetSymbol failed");
        return FAILURE;
    }

    std::vector<HIAI_TensorAippPara*> tmpAippParaVec(aippCount);
    auto ret = ((int (*)(HIAI_ModelManager*, const char*, unsigned int, HIAI_TensorAippPara*[], unsigned int,
        unsigned int))getTensorAippParaFunc)(currentModelUtil_->GetModelManager(),
        SharedManagerInfos_->modelName.c_str(), index, tmpAippParaVec.data(), aippCount, batchCount);
    if (ret != 0) {
        FMK_LOGE("Unable to get tensor aipp info by model name: %s, tensor index: %u",
            SharedManagerInfos_->modelName.c_str(), index);
        return FAILURE;
    }

    for (auto& tmpPara : tmpAippParaVec) {
        void* bufferPara = ((void* (*)(HIAI_TensorAippPara*))getRawBufferFunc)(tmpPara);
        if (bufferPara == nullptr) {
            continue;
        }
        auto commPara = reinterpret_cast<HIAI_TensorAippCommPara*>(bufferPara);
        commPara->batchNum = batchCount;
        int size = 0;
        if (getRawBufferSizeFunc != nullptr) {
            size = ((int (*)(HIAI_TensorAippPara*))getRawBufferSizeFunc)(tmpPara);
        }
        auto aippPara = HIAI_TensorAippPara_CreateWithHandle(bufferPara, size, tmpPara);
        if (aippPara != nullptr) {
            aippParaVec.push_back(aippPara);
        }
    }
    tmpAippParaVec.clear();

    return SUCCESS;
}

Status DirectBuiltModelImpl::GetTensorAippPara(int32_t index, std::vector<void*>& aippParaVec)
{
    if (InnerLoad() != SUCCESS) {
        FMK_LOGE("InnerLoad failed.");
        return FAILURE;
    }

    int32_t inputCount = 1;
    if (index == -1) {
        inputCount = GetTensorAippInputCnt(currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName);
        if (inputCount <= 0) {
            return FAILURE;
        }
    }

    aippParaVec.clear();
    for (int32_t i = 0; i < inputCount; ++i) {
        unsigned int aippCount = 0;
        unsigned int batchCount = 0;
        if (index != -1) {
            i = index; // inputCount defaults to 1, get aippPara specified by index and end loop.
        }

        int ret = GetTensorAippInfoByIndex(
            currentModelUtil_->GetModelManager(), SharedManagerInfos_->modelName, i, &aippCount, &batchCount);
        if (ret != 0) {
            FMK_LOGE("getTensorAippInfo failed, name: %s, index: %u", SharedManagerInfos_->modelName.c_str(), index);
            return FAILURE;
        }
        if (aippCount == 0) {
            FMK_LOGI("index: %u of model: %s does NOT contain aipp configuration info", index,
                SharedManagerInfos_->modelName.c_str());
            return SUCCESS;
        }

        auto status = GetAippPara(i, aippCount, batchCount, aippParaVec);
        if (status != SUCCESS) {
            FMK_LOGE("GetAippPara failed.");
            return FAILURE;
        }
    }
    return SUCCESS;
}
#endif

// Transform ge::DataType to HIAI_DataType
static HIAI_NDTensorDesc* TransDataTypeToHiaiDataType(const HIAI_NDTensorDesc* desc)
{
    HIAI_DataType dataType = HIAI_DATATYPE_FLOAT32;
    HIAI_DataType dType = HIAI_NDTensorDesc_GetDataType(desc);
    auto dataTypeTmp = hiai::DATA_TYPE_MAP.find((ge::DataType)dType);
    if (dataTypeTmp != hiai::DATA_TYPE_MAP.end()) {
        dataType = (HIAI_DataType)dataTypeTmp->second;
    }
    std::vector<int32_t> dims;
    size_t dimNum = HIAI_NDTensorDesc_GetDimNum(desc);
    for (size_t i = 0; i < dimNum; i++) {
        dims.push_back(HIAI_NDTensorDesc_GetDim(desc, i));
    }
    HIAI_Format format = HIAI_NDTensorDesc_GetFormat(desc);
    return HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
}

static Status GetIONDTensorDescByNDTensorInfo(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    void* getNDTensorFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_GetModelNDTensorInfo");
    void* freeNDTensorInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_ReleaseModelNDTensorInfo");
    if (getNDTensorFunc == nullptr || freeNDTensorInfoFunc == nullptr) {
        FMK_LOGW("GetSymbol failed");
        return FAILURE;
    }

    HIAI_ModelNDTensorInfo* modelTensorInfo =
        ((HIAI_ModelNDTensorInfo* (*)(HIAI_ModelManager*, const char*)) getNDTensorFunc)(manager, modelName.c_str());
    if (modelTensorInfo == nullptr) {
        FMK_LOGE("modelTensorInfo is null");
        return FAILURE;
    }

    for (int i = 0; i < modelTensorInfo->inputCnt; i++) {
        HIAI_NDTensorDesc* desc = TransDataTypeToHiaiDataType(modelTensorInfo->inputShape[i]);
        if (desc != nullptr) {
            inputs.emplace_back(desc);
        }
    }

    for (int i = 0; i < modelTensorInfo->outputCnt; i++) {
        HIAI_NDTensorDesc* desc = TransDataTypeToHiaiDataType(modelTensorInfo->outputShape[i]);
        if (desc != nullptr) {
            outputs.emplace_back(desc);
        }
    }

    ((void (*)(HIAI_ModelNDTensorInfo*))freeNDTensorInfoFunc)(modelTensorInfo);
    return SUCCESS;
}

static Status GetIONDTensorDescByTensorInfoV2(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    void* getTensorInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_getModelTensorInfoV2");
    void* freeTensorInfoFunc = HIAI_Foundation_GetSymbol("HIAI_ModelManager_releaseModelTensorInfoV2");
    if (getTensorInfoFunc == nullptr || freeTensorInfoFunc == nullptr) {
        FMK_LOGW("GetSymbol failed");
        return FAILURE;
    }

    HIAI_ModelTensorInfoV2* tensorV2 =
        ((HIAI_ModelTensorInfoV2* (*)(HIAI_ModelManager*, const char*)) getTensorInfoFunc)(manager, modelName.c_str());

    if (GetDescByTensorInfoV2(tensorV2, HIAI_IO_TYPE::HIAI_IO_INPUT, inputs) != SUCCESS) {
        FMK_LOGE("get input shape failed.");
        ((void (*)(HIAI_ModelTensorInfoV2*))freeTensorInfoFunc)(tensorV2);
        return FAILURE;
    }

    if (GetDescByTensorInfoV2(tensorV2, HIAI_IO_TYPE::HIAI_IO_OUTPUT, outputs) != SUCCESS) {
        FMK_LOGE("get output shape failed.");
        ((void (*)(HIAI_ModelTensorInfoV2*))freeTensorInfoFunc)(tensorV2);
        return FAILURE;
    }

    ((void (*)(HIAI_ModelTensorInfoV2*))freeTensorInfoFunc)(tensorV2);
    return SUCCESS;
}

static Status GetIONDTensorDescByTensorInfo(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    HIAI_ModelTensorInfo* modelTensorInfo = GetModelTensorInfo(manager, modelName);
    if (modelTensorInfo == nullptr) {
        FMK_LOGE("Unable to get model tensor info by model name: %s", modelName.c_str());
        return FAILURE;
    }

    (void)HIAI_ModelTensorInfoToNDTensorDesc(modelTensorInfo->inputShape, modelTensorInfo->inputCnt, inputs);
    (void)HIAI_ModelTensorInfoToNDTensorDesc(modelTensorInfo->outputShape, modelTensorInfo->outputCnt, outputs);

    (void)FreeModelTensorInfo(modelTensorInfo);
    return SUCCESS;
}

Status DirectBuiltModelImpl::GetIONDTensorDesc()
{
    if (!inputs_.empty() && !outputs_.empty()) {
        return SUCCESS;
    }

    auto status = GetIONDTensorDescByNDTensorInfo(SharedManagerInfos_->modelName,
        currentModelUtil_->GetModelManager(), inputs_, outputs_);
    if (status == SUCCESS) {
        return status;
    }
    status = GetIONDTensorDescByTensorInfoV2(SharedManagerInfos_->modelName,
        currentModelUtil_->GetModelManager(), inputs_, outputs_);
    if (status == SUCCESS) {
        return status;
    }
    status = GetIONDTensorDescByTensorInfo(SharedManagerInfos_->modelName,
        currentModelUtil_->GetModelManager(), inputs_, outputs_);
    return status;
}
} // namespace hiai