/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: HIAIModelManager(deprecated)
 */
#ifndef HIAI_FRAMEWORK_LEGACY_HIAI_MODEL_MANAGER_H
#define HIAI_FRAMEWORK_LEGACY_HIAI_MODEL_MANAGER_H

#include "framework/c/hiai_c_api_export.h"
#include "framework/c/hiai_base_types.h"
#include "framework/c/hiai_native_handle.h"
#include "framework/c/hiai_version.h"

#include "HiAiModelManagerType.h"
#include "hiai_model_buffer.h"
#include "hiai_tensor_buffer.h"
#include "hiai_model_tensor_info.h"
#ifdef AI_SUPPORT_AIPP_API
#include "hiai_tensor_aipp_para.h"
#endif
#include "hiai_mem_buffer.h"
#include "util/base_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* modelNetName;

    const char* modelNetPath;
    bool isModelNetEncrypted;
    const char* modelNetKey;

    const char* modelNetParamPath;
    bool isModelNetParamEncrypted;
    const char* modelNetParamKey;

    HIAI_ModelType modelType;
    HIAI_Framework frameworkType;
    HIAI_DevPerf perf;
} HIAI_ModelDescription;

#define HIAI_MODEL_DESCRIPTION_INIT \
    { \
        "", "", false, "", "", false, "", HIAI_MODELTYPE_OFFLINE, HIAI_FRAMEWORK_CAFFE, HIAI_DEVPERF_DEFAULT \
    }

typedef struct HIAI_ModelManagerListenerLegacy {
    void (*onLoadDone)(void* userdata, int taskStamp);
    void (*onRunDone)(void* userdata, int taskStamp);
    void (*onUnloadDone)(void* userdata, int taskStamp);
    void (*onTimeout)(void* userdata, int taskStamp);
    void (*onError)(void* userdata, int taskStamp, int errCode);
    void (*onServiceDied)(void* userdata);

    void* userdata;
} HIAI_ModelManagerListenerLegacy;

#define HIAI_MODEL_MANAGER_LISTENER_INIT \
    { \
        NULL, NULL, NULL, NULL, NULL, NULL, NULL \
    }

typedef struct HIAI_ModelManager HIAI_ModelManager;

AICP_C_API_EXPORT HIAI_ModelManager* HIAI_ModelManager_create(HIAI_ModelManagerListenerLegacy* listener);

typedef struct HIAI_ModelManagerMemAllocator_struct {
    hiai::NativeHandle (*Allocate)(int32_t size);
    void (*Free)(hiai::NativeHandle handle);
} HIAI_ModelManagerMemAllocator;

#define HIAI_MODEL_MANAGER_MEMALLOCATOR_INIT \
    { \
        NULL, NULL \
    }

AICP_C_API_EXPORT HIAI_ModelManager* HIAI_ModelManager_createWithMem(
    HIAI_ModelManagerListenerLegacy* listener, HIAI_ModelManagerMemAllocator* memAllocator);

AICP_C_API_EXPORT void HIAI_ModelManager_destroy(HIAI_ModelManager* manager);

/**
 * @return >0 means load success, and the return value is a taskStamp, < 0 means error occured
 **/
AICP_C_API_EXPORT int HIAI_ModelManager_loadFromModelBuffers(
    HIAI_ModelManager* manager, HIAI_ModelBuffer* bufferArray[], int nBuffers);

AICP_C_API_EXPORT int HIAI_ModelManager_loadFromModelDescriptions(
    HIAI_ModelManager* manager, HIAI_ModelDescription descsArray[], int nDescs);

AICP_C_API_EXPORT int HIAI_ModelManager_runModel(HIAI_ModelManager* manager, HIAI_TensorBuffer* input[], int nInput,
    HIAI_TensorBuffer* output[], int nOutput, int ulTimeout, const char* modelName);

#ifdef AI_SUPPORT_AIPP_API
AICP_C_API_EXPORT int HIAI_ModelManager_runAippModel(HIAI_ModelManager* manager, HIAI_TensorBuffer* input[], int nInput,
    HIAI_TensorAippPara* aippPara[], int nAipp, HIAI_TensorBuffer* output[], int nOutput, int ulTimeout,
    const char* modelName);
#endif

AICP_C_API_EXPORT int HIAI_ModelManager_setInputsAndOutputs(HIAI_ModelManager* manager, const char* modelname,
    HIAI_TensorBuffer* input[], int nInput, HIAI_TensorBuffer* output[], int nOutput);

AICP_C_API_EXPORT int HIAI_ModelManager_startCompute(HIAI_ModelManager* manager, const char* modelname);

AICP_C_API_EXPORT int HIAI_ModelManager_unloadModel(HIAI_ModelManager* manager);

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description
//      set model priority, and it effects in next inference
// Params
//      @manager: modelManager instance
//      @modelName: model name
//      @modelPriority: execution priority of the model
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AICP_C_API_EXPORT int HIAI_ModelManager_setModelPriority(
    HIAI_ModelManager* manager, const char* modelName, HIAI_ModelPriority modelPriority);

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description
//      cancel compute task of a model. No guarantee of cancellation success
// Params
//      @manager: modelManager instance
//      @modelName: model name
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AICP_C_API_EXPORT void HIAI_ModelManager_cancelCompute(HIAI_ModelManager* manager, const char* modelName);

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description
//      compile onlineModel to offlineModel
// Params
//      @manager: modelManager instance
//      @frameworkType: frameworkType. (eg: CAFFE / TENSORFLOW)
//      @inputModelBuffers: a full-onlineModel
//      @inputModelBuffersNum: the number of inputModelBuffers
//      @outputModelBuffer: offlineModel MemBuffer
//      @outModelSize: the actual offlineModel size.
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AICP_C_API_EXPORT int HIAI_ModelManager_buildModel(HIAI_ModelManager* manager, HIAI_Framework frameworkType,
    HIAI_MemBuffer* inputModelBuffers[], const unsigned int inputModelBuffersNum, HIAI_MemBuffer* outputModelBuffer,
    unsigned int* outModelSize);

AICP_C_API_EXPORT int HIAI_ModelManager_buildModel_v2(HIAI_ModelManager* manager, HIAI_Framework frameworkType,
    HIAI_MemBuffer* inputModelBuffers[], const unsigned int inputModelBuffersNum, HIAI_MemBuffer* outputModelBuffer,
    unsigned int* outModelSize, bool useOriginFormat);
// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description
//      read an offlineModel file, check the compatibility.
// Params
//      @manager: modelManager instance
//      @modelPath: the path of the offlineModel file.
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AICP_C_API_EXPORT bool HIAI_CheckModelCompatibility_from_file(HIAI_ModelManager* manager, const char* modelPath);

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Description
//      read an offlineModel buffer, check the compatibility.
// Params
//      @manager: modelManager instance
//      @buffer: a buffer stores an offlineModel file content.
//      @size: buffer size.
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AICP_C_API_EXPORT bool HIAI_CheckModelCompatibility_from_buffer(
    HIAI_ModelManager* manager, void* buffer, const unsigned int size);

#ifdef AI_SUPPORT_AIPP_API
AICP_C_API_EXPORT int HIAI_ModelManger_getTensorAippInfo(HIAI_ModelManager* manager, const char* modelName,
    const unsigned int index, unsigned int* aippCount, unsigned int* batchCount);

AICP_C_API_EXPORT int HIAI_ModelManger_getTensorAippPara(HIAI_ModelManager* manager, const char* modelName,
    const unsigned int index, HIAI_TensorAippPara* aippParas[], unsigned int aippCount, unsigned int batchCount);
#endif

AICP_C_API_EXPORT HIAI_ModelTensorInfo* HIAI_ModelManager_getModelTensorInfo(
    HIAI_ModelManager* manager, const char* modelName);

AICP_C_API_EXPORT HIAI_ModelTensorInfoV2* HIAI_ModelManager_getModelTensorInfoV2(
    HIAI_ModelManager* manager, const char* modelName);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __HIAI_MODEL_MANAGER_H__
