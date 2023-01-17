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
#include <functional>

// inc
#include "framework/c/compatible/hiai_model_tensor_info.h"
#include "framework/infra/log/log.h"
#include "infra/base/securestl.h"
#include "infra/base/assertion.h"

// src/framework
#include "inc/util/file_util.h"
#include "model_runtime/data_type_convert.h"

#include "direct_model_manager_container.h"
#include "direct_model_manager_util.h"
#include "securec.h"

#ifdef AI_SUPPORT_AIPP_API
#include "framework/c/compatible/hiai_tensor_aipp_para.h"
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
DirectBuiltModelImpl::DirectBuiltModelImpl(std::shared_ptr<BaseBuffer>& buffer, std::string modelName)
    : modelBuffer_(buffer), modelName_(std::move(modelName))
{
}

DirectBuiltModelImpl::DirectBuiltModelImpl(std::string modelFile) : modelFile_(std::move(modelFile))
{
    if (modelName_.empty() && !modelFile_.empty()) {
        modelName_ = "default_" + modelFile_.substr(modelFile_.find_last_of("/\\") + 1);
    }
}

DirectBuiltModelImpl::~DirectBuiltModelImpl()
{
    for (auto& inputDesc : inputs_) {
        HIAI_NDTensorDesc_Destroy(&inputDesc);
    }
    for (auto& outputDesc : outputs_) {
        HIAI_NDTensorDesc_Destroy(&outputDesc);
    }
}

const std::shared_ptr<BaseBuffer>& DirectBuiltModelImpl::GetModelBuffer() const
{
    return modelBuffer_;
}

const std::string& DirectBuiltModelImpl::GetModelName() const
{
    return modelName_;
}

const std::string& DirectBuiltModelImpl::GetModelFile() const
{
    return modelFile_;
}

void DirectBuiltModelImpl::SetModelName(std::string modelName)
{
    modelName_ = std::move(modelName);
}

Status DirectBuiltModelImpl::CheckCompatibility(bool& isCompatible)
{
    auto manager = std::unique_ptr<HIAI_ModelManager, void (*)(HIAI_ModelManager*)>(
        DirectModelManagerUtil::CreateModelManager(nullptr), [](HIAI_ModelManager* p) {
            DirectModelManagerUtil::DestroyModelManager(&p);
        });
    HIAI_EXPECT_NOT_NULL(manager);

    if (modelBuffer_ != nullptr) {
        void* func = HIAI_Foundation_GetSymbol("HIAI_CheckModelCompatibility_from_buffer");
        HIAI_EXPECT_NOT_NULL(func);

        isCompatible = ((bool (*)(HIAI_ModelManager*, void*, const uint32_t))func)(
            manager.get(), static_cast<void*>(const_cast<uint8_t*>(modelBuffer_->GetData())), modelBuffer_->GetSize());
    } else {
        void* func = HIAI_Foundation_GetSymbol("HIAI_CheckModelCompatibility_from_file");
        HIAI_EXPECT_NOT_NULL(func);

        isCompatible = ((bool (*)(HIAI_ModelManager*, const char*))func)(manager.get(), modelFile_.c_str());
    }
    return SUCCESS;
}

namespace {
/* model tensor type T must be consistent with getFunc/releaseFunc, guaranteed by the caller */
template <typename T>
std::unique_ptr<T, std::function<void(T*)>> GetModelIOTensorInfo(
    const std::string& modelName, HIAI_ModelManager* manager, const char* getFuncName, const char* releaseFuncName)
{
    void* getfunc = HIAI_Foundation_GetSymbol(getFuncName);
    HIAI_EXPECT_NOT_NULL_R(getfunc, nullptr);
    void* freeFunc = HIAI_Foundation_GetSymbol(releaseFuncName);
    HIAI_EXPECT_NOT_NULL_R(freeFunc, nullptr);

    T* tensorInfo = ((T* (*)(HIAI_ModelManager*, const char*)) getfunc)(manager, modelName.c_str());
    HIAI_EXPECT_NOT_NULL_R(tensorInfo, nullptr);

    return std::unique_ptr<T, std::function<void(T*)>>(tensorInfo, [freeFunc](T* info) {
        (reinterpret_cast<void (*)(T*)>(freeFunc))(info);
    });
}

std::unique_ptr<HIAI_ModelTensorInfo, std::function<void(HIAI_ModelTensorInfo*)>> GetModelTensorInfo(
    HIAI_ModelManager* manager, const std::string& modelName)
{
    static const char* getFunc = "HIAI_ModelManager_getModelTensorInfo";
    static const char* releaseFunc = "HIAI_ModelManager_releaseModelTensorInfo";

    return GetModelIOTensorInfo<HIAI_ModelTensorInfo>(modelName, manager, getFunc, releaseFunc);
}

// Transform ge::DataType to HIAI_DataType
HIAI_NDTensorDesc* TransToHiAIDataTypeTensor(const HIAI_NDTensorDesc* desc)
{
    HIAI_DataType dataType = HIAI_DATATYPE_FLOAT32;
    ge::DataType dType = static_cast<ge::DataType>(HIAI_NDTensorDesc_GetDataType(desc));
    auto dataTypeTmp = hiai::DATA_TYPE_MAP.find(dType);
    if (dataTypeTmp != hiai::DATA_TYPE_MAP.end()) {
        dataType = static_cast<HIAI_DataType>(dataTypeTmp->second);
    }
    std::vector<int32_t> dims;
    size_t dimNum = HIAI_NDTensorDesc_GetDimNum(desc);
    for (size_t i = 0; i < dimNum; i++) {
        dims.push_back(HIAI_NDTensorDesc_GetDim(desc, i));
    }
    HIAI_Format format = HIAI_NDTensorDesc_GetFormat(desc);
    return HIAI_NDTensorDesc_Create(dims.data(), dimNum, dataType, format);
}

Status TransToHiAIDataTypeTensors(
    HIAI_NDTensorDesc* descs[], int num, std::vector<HIAI_NDTensorDesc*>& tensors)
{
    std::vector<HIAI_NDTensorDesc*> convertTensors;
    for (int i = 0; i < num; i++) {
        HIAI_NDTensorDesc* desc = TransToHiAIDataTypeTensor(descs[i]);
        HIAI_EXPECT_NOT_NULL(desc);
        convertTensors.emplace_back(desc);
    }

    tensors.swap(convertTensors);
    return SUCCESS;
}

Status GetIONDTensorDescByNDTensorInfo(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    static const char* getFunc = "HIAI_ModelManager_GetModelNDTensorInfo";
    static const char* releaseFunc = "HIAI_ModelManager_ReleaseModelNDTensorInfo";
    auto modelTensorInfo = GetModelIOTensorInfo<HIAI_ModelNDTensorInfo>(modelName, manager, getFunc, releaseFunc);
    HIAI_EXPECT_NOT_NULL(modelTensorInfo);

    HIAI_EXPECT_EXEC(TransToHiAIDataTypeTensors(modelTensorInfo->inputShape, modelTensorInfo->inputCnt, inputs));
    HIAI_EXPECT_EXEC(TransToHiAIDataTypeTensors(modelTensorInfo->outputShape, modelTensorInfo->outputCnt, outputs));

    return SUCCESS;
}

Status GetDescByTensorInfoV2(
    const HIAI_ModelTensorInfoV2* tensorInfo, HIAI_IO_TYPE type, std::vector<HIAI_NDTensorDesc*>& descs)
{
    void* func = HIAI_Foundation_GetSymbol("HIAI_ModelTensorInfoV2_getIOCount");
    HIAI_EXPECT_NOT_NULL(func);

    int count = ((int (*)(const HIAI_ModelTensorInfoV2*, HIAI_IO_TYPE))func)(tensorInfo, type);
    HIAI_EXPECT_TRUE(count > 0);

    void* descFunc = HIAI_Foundation_GetSymbol("HIAI_ModelTensorInfoV2_getTensorDescription");
    HIAI_EXPECT_NOT_NULL(descFunc);

    std::vector<HIAI_NDTensorDesc*> ndTensorDescs;
    for (int i = 0; i < count; i++) {
        HIAI_TensorDescriptionV2* tensorDescV2 = ((HIAI_TensorDescriptionV2 *
            (*)(const HIAI_ModelTensorInfoV2*, HIAI_IO_TYPE, int)) descFunc)(tensorInfo, type, i);
        HIAI_EXPECT_NOT_NULL(tensorDescV2);

        DataType dataType = DataType::FLOAT32;
        auto dataTypeTmp = DATA_TYPE_MAP.find((ge::DataType)tensorDescV2->dataType);
        if (dataTypeTmp != DATA_TYPE_MAP.end()) {
            dataType = dataTypeTmp->second;
        }

        std::vector<int> dims {tensorDescV2->number, tensorDescV2->channel, tensorDescV2->height, tensorDescV2->width};
        auto ndTensorDesc = HIAI_NDTensorDesc_Create(const_cast<int32_t*>(dims.data()), dims.size(),
            static_cast<HIAI_DataType>(dataType), HIAI_Format::HIAI_FORMAT_NCHW);
        HIAI_EXPECT_NOT_NULL(ndTensorDesc);
        ndTensorDescs.push_back(ndTensorDesc);
    }

    descs.swap(ndTensorDescs);
    return SUCCESS;
}

Status GetIONDTensorDescByTensorInfoV2(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    static const char* getFunc = "HIAI_ModelManager_getModelTensorInfoV2";
    static const char* releaseFunc = "HIAI_ModelManager_releaseModelTensorInfoV2";
    auto tensorV2 = GetModelIOTensorInfo<HIAI_ModelTensorInfoV2>(modelName, manager, getFunc, releaseFunc);
    HIAI_EXPECT_NOT_NULL(tensorV2);

    HIAI_EXPECT_EXEC(GetDescByTensorInfoV2(tensorV2.get(), HIAI_IO_TYPE::HIAI_IO_INPUT, inputs));
    HIAI_EXPECT_EXEC(GetDescByTensorInfoV2(tensorV2.get(), HIAI_IO_TYPE::HIAI_IO_OUTPUT, outputs));

    return SUCCESS;
}

Status HIAI_ModelTensorInfoToNDTensorDesc(const int* shape, int count, std::vector<HIAI_NDTensorDesc*>& descs)
{
    std::vector<HIAI_NDTensorDesc*> ndTensorDescs;
    for (int i = 0, pos = 0; i < count; ++i, pos += 4) {
        std::vector<int> dims = {shape[pos], shape[pos + 1], shape[pos + 2], shape[pos + 3]};
        auto ndTensorDesc = HIAI_NDTensorDesc_Create(const_cast<int32_t*>(dims.data()), dims.size(),
            HIAI_DataType::HIAI_DATATYPE_FLOAT32, HIAI_Format::HIAI_FORMAT_NCHW);
        HIAI_EXPECT_NOT_NULL(ndTensorDesc);
        ndTensorDescs.push_back(ndTensorDesc);
    }
    descs.swap(ndTensorDescs);
    return SUCCESS;
}

Status GetIONDTensorDescByTensorInfo(std::string& modelName, HIAI_ModelManager* manager,
    std::vector<HIAI_NDTensorDesc*>& inputs, std::vector<HIAI_NDTensorDesc*>& outputs)
{
    auto modelTensorInfo = GetModelTensorInfo(manager, modelName);
    HIAI_EXPECT_NOT_NULL(modelTensorInfo);
    HIAI_EXPECT_EXEC(
        HIAI_ModelTensorInfoToNDTensorDesc(modelTensorInfo->inputShape, modelTensorInfo->inputCnt, inputs));
    HIAI_EXPECT_EXEC(
        HIAI_ModelTensorInfoToNDTensorDesc(modelTensorInfo->outputShape, modelTensorInfo->outputCnt, outputs));

    return SUCCESS;
}
} // namespace

std::shared_ptr<HIAI_ModelManager> DirectBuiltModelImpl::GetLoadedModelManager()
{
    // find one reusable modelmanger, no need to load again.
    std::shared_ptr<HIAI_ModelManager> manager =
        DirectModelManagerContainer::GetInstance().GetModelManager(reinterpret_cast<void*>(this));
    if (manager != nullptr) {
        return manager;
    }

    // load from self when not found
    HIAI_ModelManager* cManager = DirectModelManagerUtil::CreateModelManager(nullptr);
    HIAI_EXPECT_NOT_NULL_R(cManager, nullptr);

    ModelLoadInfo loadInfo(modelName_, modelFile_, modelBuffer_, HIAI_PerfMode::HIAI_PERFMODE_NORMAL);
    int timeStamp = DirectModelManagerUtil::LoadModel(cManager, loadInfo);
    if (timeStamp < 0) {
        DirectModelManagerUtil::DestroyModelManager(&cManager);
        return nullptr;
    }

    return std::shared_ptr<HIAI_ModelManager>(cManager, [](HIAI_ModelManager* mm) {
        (void)DirectModelManagerUtil::UnLoadModel(mm);
        DirectModelManagerUtil::DestroyModelManager(&mm);
    });
}

Status DirectBuiltModelImpl::GetIONDTensorDesc()
{
    if (!inputs_.empty() && !outputs_.empty()) {
        return SUCCESS;
    }

    auto manager = GetLoadedModelManager();
    HIAI_EXPECT_NOT_NULL(manager);

    if (GetIONDTensorDescByNDTensorInfo(modelName_, manager.get(), inputs_, outputs_) == SUCCESS) {
        return SUCCESS;
    }

    if (GetIONDTensorDescByTensorInfoV2(modelName_, manager.get(), inputs_, outputs_) == SUCCESS) {
        return SUCCESS;
    }

    return GetIONDTensorDescByTensorInfo(modelName_, manager.get(), inputs_, outputs_);
}


int32_t DirectBuiltModelImpl::GetModelIOTensorNum(bool isInput)
{
    HIAI_EXPECT_EXEC_R(GetIONDTensorDesc(), 0);

    return isInput ? inputs_.size() : outputs_.size();
}

namespace {
inline HIAI_NDTensorDesc* GetTensorDesc(std::vector<HIAI_NDTensorDesc*> descs, size_t index)
{
    if (index < descs.size()) {
        return HIAI_NDTensorDesc_Clone(descs[index]);
    } else {
        FMK_LOGE("index[%u] is invalid.", index);
        return nullptr;
    }
}
} // namespace

HIAI_NDTensorDesc* DirectBuiltModelImpl::GetModelIOTensorDesc(size_t index, bool isInput)
{
    HIAI_EXPECT_EXEC_R(GetIONDTensorDesc(), nullptr);

    return isInput ? GetTensorDesc(inputs_, index) : GetTensorDesc(outputs_, index);
}

#ifdef AI_SUPPORT_AIPP_API
namespace {
int GetTensorAippInputCnt(HIAI_ModelManager* manager, const std::string& modelName)
{
    int inputCount = 0;
    auto modelTensorInfo = GetModelTensorInfo(manager, modelName);
    HIAI_EXPECT_NOT_NULL_R(modelTensorInfo, inputCount);

    return modelTensorInfo->inputCnt;
}

int GetTensorAippInfoByIndex(HIAI_ModelManager* manager, const std::string& modelName, int index, unsigned int* aippNum,
    unsigned int* batchCount)
{
    int ret = -1;
    void* func = HIAI_Foundation_GetSymbol("HIAI_ModelManger_getTensorAippInfo");
    HIAI_EXPECT_NOT_NULL_R(func, ret);

    ret = ((int (*)(HIAI_ModelManager*, const char*, unsigned int, unsigned int*, unsigned int*))func)(
        manager, modelName.c_str(), index, aippNum, batchCount);
    if (ret != 0) {
        FMK_LOGE("getTensorAippInfo failed, name: %s, index: %u", modelName.c_str(), index);
    }
    return ret;
}
} // namespace

Status DirectBuiltModelImpl::GetTensorAippInfo(int32_t index, uint32_t* aippParaNum, uint32_t* batchCount)
{
    auto manager = GetLoadedModelManager();
    HIAI_EXPECT_NOT_NULL(manager);

    int realIndex = index;
    if (realIndex == -1) {
        int inputCount = GetTensorAippInputCnt(manager.get(), modelName_);
        HIAI_EXPECT_TRUE(inputCount > 0);

        realIndex = inputCount - 1;
    }

    int ret = GetTensorAippInfoByIndex(manager.get(), modelName_, realIndex, aippParaNum, batchCount);
    HIAI_EXPECT_TRUE(ret == 0);

    if (*aippParaNum == 0) {
        FMK_LOGI("index: %u of model: %s does not contain aipp info", realIndex, modelName_.c_str());
    }
    return SUCCESS;
}

Status DirectBuiltModelImpl::GetAippPara(HIAI_ModelManager* manager, int32_t index, unsigned int aippCount,
    unsigned int batchCount, std::vector<void*>& aippParaVec)
{
    void* func = HIAI_Foundation_GetSymbol("HIAI_ModelManger_getTensorAippPara");
    HIAI_EXPECT_NOT_NULL(func);

    std::vector<HIAI_TensorAippPara*> hiaiAippParaVec(aippCount);
    auto ret = ((int (*)(HIAI_ModelManager*, const char*, unsigned int, HIAI_TensorAippPara*[], unsigned int,
        unsigned int))func)(manager, modelName_.c_str(), index, hiaiAippParaVec.data(), aippCount, batchCount);
    if (ret != 0) {
        FMK_LOGE("getTensorAippPara fail, name: %s, tensor index: %u", modelName_.c_str(), index);
        return FAILURE;
    }

    void* getfunc = HIAI_Foundation_GetSymbol("HIAI_TensorAipp_getRawBuffer");
    HIAI_EXPECT_NOT_NULL(getfunc);
    void* sizeFunc = HIAI_Foundation_GetSymbol("HIAI_TensorAipp_getRawBufferSize");
    HIAI_EXPECT_NOT_NULL(sizeFunc);

    std::vector<void*> tmpAippParaVec;
    for (auto& tmpPara : hiaiAippParaVec) {
        void* bufferPara = ((void* (*)(HIAI_TensorAippPara*))getfunc)(tmpPara);
        if (bufferPara == nullptr) {
            continue;
        }
        auto commPara = reinterpret_cast<HIAI_MR_TensorAippCommPara*>(bufferPara);
        commPara->batchNum = batchCount;

        int size = ((int (*)(HIAI_TensorAippPara*))sizeFunc)(tmpPara);
        auto aippPara = HIAI_MR_TensorAippPara_CreateWithHandle(bufferPara, size, tmpPara);
        HIAI_EXPECT_NOT_NULL(aippPara);

        tmpAippParaVec.push_back(aippPara);
    }

    aippParaVec.swap(tmpAippParaVec);
    return SUCCESS;
}


Status DirectBuiltModelImpl::GetAippParaByIndex(
    HIAI_ModelManager* manager, const std::string& modelName, int32_t index, std::vector<void*>& aippParaVec)
{
    unsigned int aippCount = 0;
    unsigned int batchCount = 0;
    int ret = GetTensorAippInfoByIndex(manager, modelName, index, &aippCount, &batchCount);
    HIAI_EXPECT_TRUE(ret == 0);

    if (aippCount == 0) {
        FMK_LOGI("index: %u of model: %s does NOT contain aipp info", index, modelName_.c_str());
        return SUCCESS;
    }

    HIAI_EXPECT_EXEC(GetAippPara(manager, index, aippCount, batchCount, aippParaVec));
    return SUCCESS;
}

Status DirectBuiltModelImpl::GetTensorAippPara(int32_t index, std::vector<void*>& aippParaVec)
{
    auto manager = GetLoadedModelManager();
    HIAI_EXPECT_NOT_NULL(manager);

    std::vector<void*> tmpAippParaVec;
    if (index != -1) {
        HIAI_EXPECT_EXEC(GetAippParaByIndex(manager.get(), modelName_, index, tmpAippParaVec));
    } else {
        int32_t inputCount = GetTensorAippInputCnt(manager.get(), modelName_);
        HIAI_EXPECT_TRUE(inputCount > 0);

        for (int32_t i = 0; i < inputCount; ++i) {
            HIAI_EXPECT_EXEC(GetAippParaByIndex(manager.get(), modelName_, i, tmpAippParaVec));
        }
    }
    aippParaVec.swap(tmpAippParaVec);
    return SUCCESS;
}
#endif

namespace {
inline bool IsModelBufferValid(const std::shared_ptr<BaseBuffer>& modelBuffer)
{
    return (modelBuffer != nullptr) && (modelBuffer->GetData() != nullptr) && (modelBuffer->GetSize() > 0);
}
} // namespace

Status DirectBuiltModelImpl::LoadToBuffer()
{
    if (IsModelBufferValid(modelBuffer_)) {
        return SUCCESS;
    }

    modelBuffer_ = FileUtil::LoadToBuffer(modelFile_);
    return (modelBuffer_ != nullptr) ? SUCCESS : FAILURE;
}

Status DirectBuiltModelImpl::Save(void** data, size_t* size)
{
    HIAI_EXPECT_TRUE(data != nullptr && size != nullptr);

    HIAI_EXPECT_EXEC(LoadToBuffer());

    size_t modelBufferSize = modelBuffer_->GetSize();
    auto dstData = new (std::nothrow) uint8_t[modelBufferSize];
    HIAI_EXPECT_NOT_NULL(dstData);

    void* srcData = static_cast<void*>(const_cast<uint8_t*>(modelBuffer_->GetData()));
    (void)memcpy_s(dstData, modelBufferSize, srcData, modelBufferSize);

    *size = modelBufferSize;
    *data = dstData;
    return SUCCESS;
}

Status DirectBuiltModelImpl::SaveToFile(const char* file)
{
    HIAI_EXPECT_NOT_NULL(file);

    /* if modelfile exsits, no need save model buffer to file again. */
    if (!modelFile_.empty()) {
        FMK_LOGE("model file[%s] exsits, not need to save again.", modelFile_.c_str());
        return FAILURE;
    }

    HIAI_EXPECT_TRUE(IsModelBufferValid(modelBuffer_));

    HIAI_EXPECT_EXEC(FileUtil::WriteBufferToFile(modelBuffer_->GetData(), modelBuffer_->GetSize(), file));
    return SUCCESS;
}

Status DirectBuiltModelImpl::SaveToExternalBuffer(void* data, size_t size, size_t* realSize)
{
    HIAI_EXPECT_TRUE(data != nullptr && size > 0 && realSize != nullptr);

    HIAI_EXPECT_EXEC(LoadToBuffer());

    size_t bufferSize = modelBuffer_->GetSize();
    if (memcpy_s(data, size, modelBuffer_->GetData(), bufferSize) != EOK) {
        FMK_LOGE("data size is not enough for model.");
        return FAILURE;
    }

    *realSize = bufferSize;
    return SUCCESS;
}

} // namespace hiai