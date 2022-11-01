/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: Model Manager Service src
 *
 */
#include "compatible/HiAiModelManagerService.h"

#include <algorithm>
#include <dlfcn.h>

#include "infra/base/securestl.h"
#include "framework/infra/log/log_fmk_interface.h"
#include "framework/infra/log/log.h"
#include "compatible/AiTensor.h"
#include "util/version_util.h"
#include "framework/util/base_buffer.h"
#include "framework/common/types.h"
#include "model_builder/model_builder.h"
#include "model/built_model_aipp.h"
#include "model_builder/om/model_build_options_util.h"
#include "model_manager/model_manager_aipp.h"
#include "model_runtime/core/hiai_model_runtime_repo.h"
#include "compatible/AippTensor.h"
#include "tensor/aipp_para.h"
#include "tensor/aipp/aipp_para_impl.h"
#include <atomic>

using namespace std;

namespace hiai {

class AiTensorWrapper : public AiTensor {
public:
    explicit AiTensorWrapper(std::shared_ptr<INDTensorBuffer> tensor) : tensor_(tensor)
    {
    }
    ~AiTensorWrapper()
    {
    }
    void* GetBuffer() const override
    {
        return tensor_ == nullptr ? nullptr : tensor_->GetData();
    }

    uint32_t GetSize() const override
    {
        return tensor_ == nullptr ? 0 : tensor_->GetSize();
    }

    void* GetTensorBuffer() const
    {
        return GetBuffer();
    }

private:
    using AiTensor::GetTensorDimension;
    using AiTensor::Init;
    using AiTensor::SetTensorDimension;

private:
    std::shared_ptr<INDTensorBuffer> tensor_ {nullptr};
};

class ManagerListenerImpl : public IModelManagerListener {
public:
    ManagerListenerImpl(std::shared_ptr<AiModelManagerClientListener> listener) : listenerImpl_(listener)
    {
    }

    virtual ~ManagerListenerImpl() = default;

    void OnRunDone(const Context& context, Status result, std::vector<std::shared_ptr<INDTensorBuffer>>& outputs)
    {
        H_LOG_INTERFACE_FILTER(ITF_COUNT);
        if (listenerImpl_ == nullptr) {
            FMK_LOGE("listenerImpl_ is null");
            return;
        }

        AiContext contextV1;
        std::map<std::string, std::string> keyValues = context.GetContent();
        for (auto& it : keyValues) {
            contextV1.AddPara(it.first, it.second);
        }

        vector<shared_ptr<AiTensor>> tensors;
        for (const auto& it : outputs) {
            auto tmp = make_shared_nothrow<AiTensorWrapper>(it);
            if (tmp == nullptr) {
                return;
            }
            tensors.push_back(tmp);
        }

        int32_t piStamp = std::stoi(context.GetValue("task_id"));
        listenerImpl_->OnProcessDone(contextV1, result, tensors, piStamp);
    }

    void OnServiceDied()
    {
        H_LOG_INTERFACE_FILTER(ITF_COUNT);
        if (listenerImpl_ == nullptr) {
            FMK_LOGE("listenerImpl_ is null");
            return;
        }
        listenerImpl_->OnServiceDied();
    }

private:
    std::shared_ptr<AiModelManagerClientListener> listenerImpl_;
    // 保存推理前AiTensor地址回调给客户端
};

static void GetModelBuildOptions(const AiModelDescription* modelDesc, ModelBuildOptions& buildOptions)
{
    PrecisionMode precisionMode;
    modelDesc->GetPrecisionMode(precisionMode);
    buildOptions.precisionMode = precisionMode;
    DynamicShapeConfig dynamicShapeConfig;
    modelDesc->GetDynamicShapeConfig(dynamicShapeConfig);
    buildOptions.dynamicShapeConfig = dynamicShapeConfig;
    std::vector<TensorDimension> inputDims;
    modelDesc->GetInputDims(inputDims);
    std::vector<NDTensorDesc> inputTensorDescs;
    for (auto& dim : inputDims) {
        NDTensorDesc desc;
        desc.dims = {static_cast<int32_t>(dim.GetNumber()), static_cast<int32_t>(dim.GetChannel()),
            static_cast<int32_t>(dim.GetHeight()), static_cast<int32_t>(dim.GetWidth())};
        desc.dataType = DataType::FLOAT32;
        desc.format = Format::NCHW;
        inputTensorDescs.emplace_back(desc);
    }
    buildOptions.inputTensorDescs = inputTensorDescs;
    ModelDeviceConfig modelDeviceConfig;
    buildOptions.modelDeviceConfig = modelDeviceConfig;
    buildOptions.formatMode = FormatMode::USE_NCHW;
    buildOptions.tuningStrategy = modelDesc->GetTuningStrategy();
}

static AIStatus CovertTensorDimension(
    const std::vector<NDTensorDesc>& inputTensor, std::vector<TensorDimension>& outputTensor)
{
    if (inputTensor.empty()) {
        FMK_LOGE("tensor is empty");
        return AI_FAILED;
    }

    for (auto& it : inputTensor) {
        if (it.dims.size() != 4) {
            FMK_LOGE("invalid dims size %zu, CovertTensorDimension failed", it.dims.size());
            return AI_FAILED;
        }
        TensorDimension dim;
        dim.SetNumber(it.dims[0]);
        dim.SetChannel(it.dims[1]);
        dim.SetHeight(it.dims[2]);
        dim.SetWidth(it.dims[3]);
        outputTensor.push_back(dim);
    }
    return AI_SUCCESS;
}

static void Convert2Context(AiContext& aiContext, Context& context)
{
    std::vector<std::string> keys;
    AIStatus ret = aiContext.GetAllKeys(keys);
    if (ret != AI_SUCCESS) {
        FMK_LOGW("GetAllKeys return %d", ret);
        return;
    }

    for (auto& key : keys) {
        context.SetValue(key, aiContext.GetPara(key));
    }
}

class AIModelMngerClientTaskIdGenerator {
public:
    AIModelMngerClientTaskIdGenerator() = delete;
    ~AIModelMngerClientTaskIdGenerator() = delete;

    /*
     * @brief 生成模型id，模型id保证唯一.
     *
     */
    static uint32_t Generate();
};

static std::atomic<uint32_t> taskIdMaxNum = {0};

uint32_t AIModelMngerClientTaskIdGenerator::Generate()
{
    const int32_t ID_OFFSET = 100000000;
    taskIdMaxNum = (taskIdMaxNum + 1) % ID_OFFSET;
    return taskIdMaxNum;
}

AiModelMngerClient::AiModelMngerClient()
{
}

AiModelMngerClient::~AiModelMngerClient()
{
}

AIStatus AiModelMngerClient::Init(std::shared_ptr<AiModelManagerClientListener> listener)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    inited_ = true;
    listener_ = listener;
    return AI_SUCCESS;
}

// 1. 模型名不能重复
// 2. 单个加载失败,卸载全部并返回失败
AIStatus AiModelMngerClient::Load(std::vector<std::shared_ptr<AiModelDescription>>& modelDescList)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (inited_ == false) {
        FMK_LOGE("not init, load failed");
        return AI_FAILED;
    }

    if (listener_ != nullptr) {
        listenerImpl_ = make_shared_nothrow<ManagerListenerImpl>(listener_);
        if (listenerImpl_ == nullptr) {
            FMK_LOGE("malloc ManagerListenerImpl return null, load failed");
            return AI_FAILED;
        }
    }

    std::vector<std::string> modelNameList;
    bool isRepeatLoadorError = false;

    for (auto& modelDesc : modelDescList) {
        string modelName = modelDesc->GetName();
        // 查询之前是否已加载该模型
        map<string, shared_ptr<IModelManager>>::iterator it = modelManagerMap_.find(modelName);
        if (it != modelManagerMap_.end()) {
            FMK_LOGE("%s model already loaded, load failed", modelName.c_str());
            isRepeatLoadorError = true;
            break;
        }

        // 查询当前模型列表中模型名是否重复
        vector<std::string>::iterator iter = find(modelNameList.begin(), modelNameList.end(), modelName);
        if (iter != modelNameList.end()) {
            FMK_LOGE("same model name %s, load failed", modelName.c_str());
            isRepeatLoadorError = true;
            break;
        }

        // 创建模型管家
        std::shared_ptr<IModelManager> modelManager = CreateModelManager();
        if (modelManager == nullptr) {
            FMK_LOGE("CreateModelManager return null, load failed");
            isRepeatLoadorError = true;
            break;
        }

        std::shared_ptr<IBuffer> modelBuffer =
            CreateLocalBuffer(modelDesc->GetModelBuffer(), modelDesc->GetModelNetSize(), false);
        if (modelBuffer == nullptr) {
            FMK_LOGE("CreateLocalBuffer return null, load failed");
            isRepeatLoadorError = true;
            break;
        }

        std::shared_ptr<IBuiltModel> builtModel = nullptr;

        ModelBuildOptions buildOptions;
        GetModelBuildOptions(modelDesc.get(), buildOptions);

        ModelInitOptions initOptions;
        initOptions.perfMode = static_cast<PerfMode>(modelDesc->GetFrequency());
        Status ret = FAILURE;

        if (ModelBuildOptionsUtil::IsHasBuildOptions(buildOptions) && IsHclModelRuntimeCanBuild()) {
            std::shared_ptr<IModelBuilder> modelBuilder = CreateModelBuilder();
            if (modelBuilder == nullptr) {
                FMK_LOGE("CreateModelBuilder return null, load failed");
                isRepeatLoadorError = true;
                break;
            }
            ret = modelBuilder->Build(buildOptions, modelName, modelBuffer, builtModel);
            if (ret != SUCCESS || builtModel == nullptr) {
                FMK_LOGE("build model failed in load", ret);
                isRepeatLoadorError = true;
                break;
            }
        } else {
            builtModel = CreateBuiltModel();
            if (builtModel == nullptr) {
                FMK_LOGE("CreateBuiltModel return null, load failed");
                isRepeatLoadorError = true;
                break;
            }

            ret = builtModel->RestoreFromBuffer(modelBuffer);
            if (ret != SUCCESS) {
                FMK_LOGE("RestoreFromBuffer return %d, load failed", ret);
                isRepeatLoadorError = true;
                break;
            }

            (void)builtModel->SetName(modelName);
            initOptions.buildOptions = buildOptions;
        }

        // 加载模型
        ret = modelManager->Init(initOptions, builtModel, listenerImpl_);
        if (ret != SUCCESS) {
            FMK_LOGE("manager Init return %d, load failed", ret);
            isRepeatLoadorError = true;
            break;
        }

        modelNameList.emplace_back(modelName);
        std::lock_guard<std::mutex> lock(modelManagerMutex_);
        modelManagerMap_.insert(map<string, shared_ptr<IModelManager>>::value_type(modelName, modelManager));
        builtModelMap_.insert(map<string, shared_ptr<IBuiltModel>>::value_type(modelName, builtModel));
    }

    // 模型重复或加载失败场景,卸载所有模型
    if (isRepeatLoadorError) {
        UnLoadModel();
        return AI_FAILED;
    }
    return AI_SUCCESS;
}

AIStatus AiModelMngerClient::Process(AiContext& aiContext, std::vector<std::shared_ptr<AiTensor>>& pinputTensor,
    std::vector<std::shared_ptr<AiTensor>>& poutputTensor, uint32_t timeout, int32_t& piStamp)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (inited_ == false) {
        FMK_LOGE("not init, Process failed");
        return AI_FAILED;
    }

    string modelName = aiContext.GetPara("model_name");
    if (modelName.empty()) {
        FMK_LOGE("Process failed, modelName is empty");
        return AI_FAILED;
    }

    map<string, shared_ptr<IModelManager>>::iterator it = modelManagerMap_.find(modelName);
    if (it == modelManagerMap_.end()) {
        FMK_LOGE("%s not loaded", modelName.c_str());
        return AI_FAILED;
    }

    vector<std::shared_ptr<INDTensorBuffer>> inputs;
    vector<std::shared_ptr<INDTensorBuffer>> outputs;
    vector<std::shared_ptr<IAIPPPara>> aippParasV2;
    vector<std::shared_ptr<AippPara>> aippParasV1;

    for (auto& inTensor : pinputTensor) {
        std::shared_ptr<AippTensor> aippTensor = std::dynamic_pointer_cast<AippTensor>(inTensor);
        if (aippTensor != nullptr && aippTensor->GetAiTensor() != nullptr) {
            inputs.push_back(aippTensor->GetAiTensor()->tensor_);
            aippParasV1 = aippTensor->GetAippParas();
            for (auto& para : aippParasV1) {
                aippParasV2.push_back(para->GetAIPPPara());
            }
        } else {
            inputs.push_back(inTensor->tensor_);
        }
    }

    for (auto& outTensor : poutputTensor) {
        outputs.push_back(outTensor->tensor_);
    }

    if (inputs.size() == 0 || outputs.size() == 0) {
        FMK_LOGE("Invalid input or output.");
        return AI_FAILED;
    }

    piStamp = (int32_t)AIModelMngerClientTaskIdGenerator::Generate();
    Context context;
    Convert2Context(aiContext, context);
    context.SetValue("task_id", to_string(piStamp));
    Status ret = FAILURE;
    if (it->second != nullptr) {
        // 判断是否为AIPP模型
        if (aippParasV2.empty()) {
            if (listener_ == nullptr) {
                ret = it->second->Run(inputs, outputs);
            } else {
                ret = it->second->RunAsync(context, inputs, outputs, timeout);
            }
        } else {
            shared_ptr<IModelManagerAipp> managerAipp = std::dynamic_pointer_cast<IModelManagerAipp>(it->second);
            if (managerAipp != nullptr) {
                ret = managerAipp->RunAippModel(context, inputs, aippParasV2, outputs, timeout);
            }
        }

        if (ret != SUCCESS) {
            FMK_LOGE("run %s failed", modelName.c_str());
            ret = AI_FAILED;
        }
    }
    return static_cast<AIStatus>(ret);
}

AIStatus AiModelMngerClient::CheckModelCompatibility(AiModelDescription& modelDesc, bool& isModelCompatibility)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    shared_ptr<IBuffer> modelBuffer = CreateLocalBuffer(modelDesc.GetModelBuffer(), modelDesc.GetModelNetSize(), false);
    if (modelBuffer == nullptr) {
        FMK_LOGE("CreateLocalBuffer return null, %s failed", __func__);
        return AI_FAILED;
    }

    std::shared_ptr<IBuiltModel> builtModel = nullptr;
    builtModel = CreateBuiltModel();
    if (builtModel == nullptr) {
        FMK_LOGE("builtModel is null");
        return AI_FAILED;
    }

    auto ret = builtModel->RestoreFromBuffer(modelBuffer);
    if (ret != AI_SUCCESS) {
        FMK_LOGE("RestoreFromBuffer failed, return %d", ret);
        return AI_FAILED;
    }

    return builtModel->CheckCompatibility(isModelCompatibility);
}

AIStatus AiModelMngerClient::GetModelIOTensorDim(
    const std::string& modelName, std::vector<TensorDimension>& inputTensor, std::vector<TensorDimension>& outputTensor)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    // 查询是否已加载该模型
    map<string, shared_ptr<IBuiltModel>>::iterator it = builtModelMap_.find(modelName);
    if (it == builtModelMap_.end()) {
        FMK_LOGE("%s not loaded, %s failed", modelName.c_str(), __func__);
        return AI_FAILED;
    }
    vector<NDTensorDesc> inTensorDescs = it->second->GetInputTensorDescs();
    vector<NDTensorDesc> outTensorDescs = it->second->GetOutputTensorDescs();
    if (CovertTensorDimension(inTensorDescs, inputTensor) != AI_SUCCESS) {
        return AI_FAILED;
    }
    if (CovertTensorDimension(outTensorDescs, outputTensor) != AI_SUCCESS) {
        return AI_FAILED;
    }

    return AI_SUCCESS;
}

static AIStatus GetModelAippParaFromIndex(std::map<std::string, std::shared_ptr<IBuiltModel>> builtModelMap,
    const std::string& modelName, int32_t index, std::vector<std::shared_ptr<AippPara>>& aippParas)
{
    if (index >= INT_MAX) {
        FMK_LOGE("index is too large.");
        return AI_FAILED;
    }
    // 查询是否已加载该模型
    map<string, shared_ptr<IBuiltModel>>::iterator it = builtModelMap.find(modelName);
    if (it == builtModelMap.end()) {
        FMK_LOGE("%s not loaded", modelName.c_str());
        return AI_FAILED;
    }

    std::shared_ptr<IBuiltModelAipp> builtModelAipp = std::dynamic_pointer_cast<IBuiltModelAipp>(it->second);
    if (builtModelAipp == nullptr) {
        FMK_LOGE("is not a aipp model[%s]", modelName.c_str());
        return AI_FAILED;
    }

    uint32_t aippParaNum;
    uint32_t batchCount;
    auto ret = builtModelAipp->GetTensorAippInfo(index, &aippParaNum, &batchCount);
    if (ret != AI_SUCCESS) {
        FMK_LOGE("%s GetTensorAippInfo index %d return %d", modelName.c_str(), index, ret);
        return ret;
    }

    std::vector<void*> tensorAippParas;
    ret = builtModelAipp->GetTensorAippPara(index, tensorAippParas);
    if (ret != AI_SUCCESS) {
        FMK_LOGE("%s GetTensorAippPara index %d return %d", modelName.c_str(), index, ret);
        return ret;
    }

    aippParas.resize(tensorAippParas.size());
    for (size_t i = 0; i < tensorAippParas.size(); i++) {
        aippParas[i] = make_shared_nothrow<AippPara>();
        if (aippParas[i] == nullptr) {
            FMK_LOGE("new AippPara i=%d failed", i);
            return AI_FAILED;
        }
        auto result = aippParas[i]->Init(batchCount);
        if (result != AI_SUCCESS) {
            FMK_LOGE("aipp para init failed");
            return result;
        }
        std::shared_ptr<AIPPParaImpl> aippParaImpl =
            std::dynamic_pointer_cast<AIPPParaImpl>(aippParas[i]->GetAIPPPara());
        aippParaImpl->Init(tensorAippParas[i]);
    }
    return AI_SUCCESS;
}

AIStatus AiModelMngerClient::GetModelAippPara(
    const std::string& modelName, std::vector<std::shared_ptr<AippPara>>& aippPara)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return GetModelAippParaFromIndex(builtModelMap_, modelName, -1, aippPara);
}

AIStatus AiModelMngerClient::GetModelAippPara(
    const std::string& modelName, uint32_t index, std::vector<std::shared_ptr<AippPara>>& aippParas)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    return GetModelAippParaFromIndex(builtModelMap_, modelName, index, aippParas);
}

char* AiModelMngerClient::GetVersion()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    char* romVersion;
    romVersion = const_cast<char*>(VersionUtil::GetVersion());
    return romVersion;
}

AIStatus AiModelMngerClient::UnLoadModel()
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    std::lock_guard<std::mutex> lock(modelManagerMutex_);
    for (auto& it : modelManagerMap_) {
        if (it.second != nullptr) {
            it.second->DeInit();
        }
    }

    modelManagerMap_.clear();
    builtModelMap_.clear();
    return AI_SUCCESS;
}

AIStatus AiModelMngerClient::SetModelPriority(const std::string& modelName, ModelPriority modelPriority)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    if (modelPriority != ModelPriority::PRIORITY_LOW && modelPriority != ModelPriority::PRIORITY_MIDDLE &&
        modelPriority != ModelPriority::PRIORITY_HIGH) {
        FMK_LOGE("invalid priority[%d]", static_cast<int32_t>(modelPriority));
        return AI_FAILED;
    }

    // 查询是否已加载该模型
    map<string, shared_ptr<IModelManager>>::iterator it = modelManagerMap_.find(modelName);
    if (it == modelManagerMap_.end()) {
        FMK_LOGE("%s not loaded", modelName.c_str());
        return AI_FAILED;
    }

    return it->second->SetPriority(modelPriority);
}

void AiModelMngerClient::Cancel(const std::string& modelName)
{
    H_LOG_INTERFACE_FILTER(ITF_COUNT);
    // 查询是否已加载该模型
    map<string, shared_ptr<IModelManager>>::iterator it = modelManagerMap_.find(modelName);
    if (it == modelManagerMap_.end()) {
        FMK_LOGW("%s not loaded", modelName.c_str());
        return;
    }

    Status ret = it->second->Cancel();
}

} // namespace hiai
