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
#include "framework/c/hiai_model_builder_types.h"

#include <pthread.h>
#include <stdbool.h>

#include "hiai_model_build_options_v1.h"
#include "hiai_model_runtime_repo.h"
#include "securec.h"
#include "framework/infra/log/log.h"
#include "limits.h"

static const uint32_t HIAI_MIN_DYNAMIC_SHAPE_CACHE_NUM = 1;
static const uint32_t HIAI_MAX_DYNAMIC_SHAPE_CACHE_NUM = 10;

static pthread_mutex_t g_optionFuncsInitMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct DynamicShapeConfigFuncs {
    HIAI_MR_DynamicShapeConfig* (*create)(void);
    void (*setEnableMode)(HIAI_MR_DynamicShapeConfig*, HIAI_DYNAMIC_SHAPE_ENABLE_MODE);
    HIAI_DYNAMIC_SHAPE_ENABLE_MODE (*getEnableMode)(const HIAI_MR_DynamicShapeConfig*);
    void (*setMaxCacheNum)(HIAI_MR_DynamicShapeConfig*, size_t);
    size_t (*getMaxCacheNum)(const HIAI_MR_DynamicShapeConfig*);
    void (*setCacheMode)(HIAI_MR_DynamicShapeConfig*, HIAI_DYNAMIC_SHAPE_CACHE_MODE);
    HIAI_DYNAMIC_SHAPE_CACHE_MODE (*getCacheMode)(const HIAI_MR_DynamicShapeConfig*);
    void (*destroy)(HIAI_MR_DynamicShapeConfig**);
} DynamicShapeConfigFuncs;

static void InitDynamicShapeConfigFuncs(DynamicShapeConfigFuncs* dynamicShapeFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_CREATE] != NULL) {
        dynamicShapeFuncs->create = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_CREATE];
        dynamicShapeFuncs->setEnableMode = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_SETENABLEMODE];
        dynamicShapeFuncs->getEnableMode = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_GETENABLEMODE];
        dynamicShapeFuncs->setMaxCacheNum = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_SETMAXCACHENUM];
        dynamicShapeFuncs->getMaxCacheNum = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_GETMAXCACHENUM];
        dynamicShapeFuncs->setCacheMode = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_SETCACHEMODE];
        dynamicShapeFuncs->getCacheMode = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_GETCACHEMODE];
        dynamicShapeFuncs->destroy = runtime->optionsSymbolList[HIAI_DYNAMICSHAPECONFIG_DESTROY];
    } else {
        // if you need add new func, don't add here.
        dynamicShapeFuncs->create = HIAI_DynamicShapeConfigV1_Create;
        dynamicShapeFuncs->setEnableMode = HIAI_DynamicShapeConfigV1_SetEnableMode;
        dynamicShapeFuncs->getEnableMode = HIAI_DynamicShapeConfigV1_GetEnableMode;
        dynamicShapeFuncs->setMaxCacheNum = HIAI_DynamicShapeConfigV1_SetMaxCacheNum;
        dynamicShapeFuncs->getMaxCacheNum = HIAI_DynamicShapeConfigV1_GetMaxCacheNum;
        dynamicShapeFuncs->setCacheMode = HIAI_DynamicShapeConfigV1_SetCacheMode;
        dynamicShapeFuncs->getCacheMode = HIAI_DynamicShapeConfigV1_GetCacheMode;
        dynamicShapeFuncs->destroy = HIAI_DynamicShapeConfigV1_Destroy;
    }
}

static DynamicShapeConfigFuncs* GetDynamicShapeConfigFuncs(void)
{
    static DynamicShapeConfigFuncs dynamicShapeFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_optionFuncsInitMutex);
        if (!isInited) {
            InitDynamicShapeConfigFuncs(&dynamicShapeFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_optionFuncsInitMutex);
    }

    return &dynamicShapeFuncs;
}

HIAI_MR_DynamicShapeConfig* HIAI_MR_DynamicShapeConfig_Create(void)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->create != NULL) {
        return dynamicShapeFuncs->create();
    }
    return NULL;
}

void HIAI_MR_DynamicShapeConfig_SetEnableMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_ENABLE_MODE mode)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->setEnableMode != NULL) {
        dynamicShapeFuncs->setEnableMode(config, mode);
    }
}

HIAI_DYNAMIC_SHAPE_ENABLE_MODE HIAI_MR_DynamicShapeConfig_GetEnableMode(const HIAI_MR_DynamicShapeConfig* config)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->getEnableMode != NULL) {
        return dynamicShapeFuncs->getEnableMode(config);
    }
    return HIAI_DYNAMIC_SHAPE_DISABLE;
}

void HIAI_MR_DynamicShapeConfig_SetMaxCacheNum(HIAI_MR_DynamicShapeConfig* config, size_t maxCacheNum)
{
    if (maxCacheNum < HIAI_MIN_DYNAMIC_SHAPE_CACHE_NUM || maxCacheNum > HIAI_MAX_DYNAMIC_SHAPE_CACHE_NUM) {
        FMK_LOGE("maxCacheNum is invalid");
        return;
    }

    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->setMaxCacheNum != NULL) {
        dynamicShapeFuncs->setMaxCacheNum(config, maxCacheNum);
    }
}

size_t HIAI_MR_DynamicShapeConfig_GetMaxCacheNum(const HIAI_MR_DynamicShapeConfig* config)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->getMaxCacheNum != NULL) {
        return dynamicShapeFuncs->getMaxCacheNum(config);
    }
    return 0;
}

void HIAI_MR_DynamicShapeConfig_SetCacheMode(HIAI_MR_DynamicShapeConfig* config, HIAI_DYNAMIC_SHAPE_CACHE_MODE mode)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->setCacheMode != NULL) {
        dynamicShapeFuncs->setCacheMode(config, mode);
    }
}

HIAI_DYNAMIC_SHAPE_CACHE_MODE HIAI_MR_DynamicShapeConfig_GetCacheMode(const HIAI_MR_DynamicShapeConfig* config)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->getCacheMode != NULL) {
        return dynamicShapeFuncs->getCacheMode(config);
    }
    return HIAI_DYNAMIC_SHAPE_CACHE_BUILDED_MODEL;
}

void HIAI_MR_DynamicShapeConfig_Destroy(HIAI_MR_DynamicShapeConfig** config)
{
    DynamicShapeConfigFuncs* dynamicShapeFuncs = GetDynamicShapeConfigFuncs();
    if (dynamicShapeFuncs->destroy != NULL) {
        dynamicShapeFuncs->destroy(config);
    }
}

typedef struct OpDeviceOrderFuncs {
    HIAI_MR_OpDeviceOrder* (*create)(void);
    void (*setOpName)(HIAI_MR_OpDeviceOrder*, const char*);
    const char* (*getOpName)(const HIAI_MR_OpDeviceOrder*);
    void (*setDeviceOrder)(HIAI_MR_OpDeviceOrder*, size_t, HIAI_EXECUTE_DEVICE*);
    size_t (*getSupportedDeviceNum)(const HIAI_MR_OpDeviceOrder*);
    HIAI_EXECUTE_DEVICE* (*getSupportedDevices)(const HIAI_MR_OpDeviceOrder*);
    void (*destroy)(HIAI_MR_OpDeviceOrder**);
} OpDeviceOrderFuncs;

static void InitOpDeviceOrderFuncs(OpDeviceOrderFuncs* opDeviceOrderFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_OPDEVICEORDER_CREATE] != NULL) {
        opDeviceOrderFuncs->create = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_CREATE];
        opDeviceOrderFuncs->setOpName = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_SETOPNAME];
        opDeviceOrderFuncs->getOpName = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_GETOPNAME];
        opDeviceOrderFuncs->setDeviceOrder = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_SETDEVICEORDER];
        opDeviceOrderFuncs->getSupportedDeviceNum =
            runtime->optionsSymbolList[HIAI_OPDEVICEORDER_GETSUPPORTEDDEVICENUM];
        opDeviceOrderFuncs->getSupportedDevices = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_GetSUPPORTEDDEVICES];
        opDeviceOrderFuncs->destroy = runtime->optionsSymbolList[HIAI_OPDEVICEORDER_DESTROY];
    } else {
        // if you need add new func, don't add here.
        opDeviceOrderFuncs->create = HIAI_OpDeviceOrderV1_Create;
        opDeviceOrderFuncs->setOpName = HIAI_OpDeviceOrderV1_SetOpName;
        opDeviceOrderFuncs->getOpName = HIAI_OpDeviceOrderV1_GetOpName;
        opDeviceOrderFuncs->setDeviceOrder = HIAI_OpDeviceOrderV1_SetDeviceOrder;
        opDeviceOrderFuncs->getSupportedDeviceNum = HIAI_OpDeviceOrderV1_GetSupportedDeviceNum;
        opDeviceOrderFuncs->getSupportedDevices = HIAI_OpDeviceOrderV1_GetSupportedDevices;
        opDeviceOrderFuncs->destroy = HIAI_OpDeviceOrderV1_Destroy;
    }
}

static OpDeviceOrderFuncs* GetOpDeviceOrderFuncs(void)
{
    static OpDeviceOrderFuncs opDeviceOrderFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_optionFuncsInitMutex);
        if (!isInited) {
            InitOpDeviceOrderFuncs(&opDeviceOrderFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_optionFuncsInitMutex);
    }

    return &opDeviceOrderFuncs;
}

HIAI_MR_OpDeviceOrder* HIAI_MR_OpDeviceOrder_Create(void)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->create != NULL) {
        return opDeviceOrderFuncs->create();
    }
    return NULL;
}

void HIAI_MR_OpDeviceOrder_SetOpName(HIAI_MR_OpDeviceOrder* config, const char* opName)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->setOpName != NULL) {
        opDeviceOrderFuncs->setOpName(config, opName);
    }
}

const char* HIAI_MR_OpDeviceOrder_GetOpName(const HIAI_MR_OpDeviceOrder* config)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->getOpName != NULL) {
        return opDeviceOrderFuncs->getOpName(config);
    }
    return NULL;
}

void HIAI_MR_OpDeviceOrder_SetDeviceOrder(
    HIAI_MR_OpDeviceOrder* config, size_t supportedDeviceNum, HIAI_EXECUTE_DEVICE* supportedDevices)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->setDeviceOrder != NULL) {
        opDeviceOrderFuncs->setDeviceOrder(config, supportedDeviceNum, supportedDevices);
    }
}

size_t HIAI_MR_OpDeviceOrder_GetSupportedDeviceNum(const HIAI_MR_OpDeviceOrder* config)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->getSupportedDeviceNum != NULL) {
        return opDeviceOrderFuncs->getSupportedDeviceNum(config);
    }
    return 0;
}

HIAI_EXECUTE_DEVICE* HIAI_MR_OpDeviceOrder_GetSupportedDevices(const HIAI_MR_OpDeviceOrder* config)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->getSupportedDevices != NULL) {
        return opDeviceOrderFuncs->getSupportedDevices(config);
    }
    return NULL;
}

void HIAI_MR_OpDeviceOrder_Destroy(HIAI_MR_OpDeviceOrder** config)
{
    OpDeviceOrderFuncs* opDeviceOrderFuncs = GetOpDeviceOrderFuncs();
    if (opDeviceOrderFuncs->destroy != NULL) {
        opDeviceOrderFuncs->destroy(config);
    }
}

typedef struct CLCustomizationFuncs {
    HIAI_MR_CLCustomization* (*create)(void);
    void (*setOpName)(HIAI_MR_CLCustomization*, const char*);
    const char* (*getOpName)(const HIAI_MR_CLCustomization*);
    void (*setCustomization)(HIAI_MR_CLCustomization*, const char*);
    const char* (*getCustomization)(const HIAI_MR_CLCustomization*);
    void (*destroy)(HIAI_MR_CLCustomization**);
} CLCustomizationFuncs;

static void InitCLCustomizationFuncs(CLCustomizationFuncs* cLCustomizationFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_CREATE] != NULL) {
        cLCustomizationFuncs->create = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_CREATE];
        cLCustomizationFuncs->setOpName = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_SETOPNAME];
        cLCustomizationFuncs->getOpName = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_GETOPNAME];
        cLCustomizationFuncs->setCustomization = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_SETCUSTOMIZATION];
        cLCustomizationFuncs->getCustomization = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_GETCUSTOMIZATION];
        cLCustomizationFuncs->destroy = runtime->optionsSymbolList[HIAI_CLCUSTOMIZATION_DESTROY];
    } else {
        // if you need add new func, don't add here.
        cLCustomizationFuncs->create = HIAI_CLCustomizationV1_Create;
        cLCustomizationFuncs->setOpName = HIAI_CLCustomizationV1_SetOpName;
        cLCustomizationFuncs->getOpName = HIAI_CLCustomizationV1_GetOpName;
        cLCustomizationFuncs->setCustomization = HIAI_CLCustomizationV1_SetCustomization;
        cLCustomizationFuncs->getCustomization = HIAI_CLCustomizationV1_GetCustomization;
        cLCustomizationFuncs->destroy = HIAI_CLCustomizationV1_Destroy;
    }
}

static CLCustomizationFuncs* GetCLCustomizationFuncs(void)
{
    static CLCustomizationFuncs cLCustomizationFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_optionFuncsInitMutex);
        if (!isInited) {
            InitCLCustomizationFuncs(&cLCustomizationFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_optionFuncsInitMutex);
    }

    return &cLCustomizationFuncs;
}

HIAI_MR_CLCustomization* HIAI_MR_CLCustomization_Create(void)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->create != NULL) {
        return cLCustomizationFuncs->create();
    }
    return NULL;
}

void HIAI_MR_CLCustomization_SetOpName(HIAI_MR_CLCustomization* config, const char* opName)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->setOpName != NULL) {
        cLCustomizationFuncs->setOpName(config, opName);
    }
}

const char* HIAI_MR_CLCustomization_GetOpName(const HIAI_MR_CLCustomization* config)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->getOpName != NULL) {
        return cLCustomizationFuncs->getOpName(config);
    }
    return NULL;
}

void HIAI_MR_CLCustomization_SetCustomization(HIAI_MR_CLCustomization* config, const char* customization)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->setCustomization != NULL) {
        cLCustomizationFuncs->setCustomization(config, customization);
    }
}

const char* HIAI_MR_CLCustomization_GetCustomization(const HIAI_MR_CLCustomization* config)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->getCustomization != NULL) {
        return cLCustomizationFuncs->getCustomization(config);
    }
    return NULL;
}

void HIAI_MR_CLCustomization_Destroy(HIAI_MR_CLCustomization** config)
{
    CLCustomizationFuncs* cLCustomizationFuncs = GetCLCustomizationFuncs();
    if (cLCustomizationFuncs->destroy != NULL) {
        cLCustomizationFuncs->destroy(config);
    }
}
// cl
typedef struct ModelDeviceConfigFuncs {
    HIAI_MR_ModelDeviceConfig* (*create)(void);

    void (*setDeviceConfigMode)(HIAI_MR_ModelDeviceConfig*, HIAI_DEVICE_CONFIG_MODE);
    HIAI_DEVICE_CONFIG_MODE (*getDeviceConfigMode)(const HIAI_MR_ModelDeviceConfig*);

    void (*setFallBackMode)(HIAI_MR_ModelDeviceConfig*, HIAI_FALLBACK_MODE);
    HIAI_FALLBACK_MODE (*getFallBackMode)(const HIAI_MR_ModelDeviceConfig*);

    void (*setModelDeviceOrder)(HIAI_MR_ModelDeviceConfig*, size_t, HIAI_EXECUTE_DEVICE*);
    size_t (*getConfigModelNum)(const HIAI_MR_ModelDeviceConfig*);
    HIAI_EXECUTE_DEVICE* (*getModelDeviceOrder)(const HIAI_MR_ModelDeviceConfig*);

    void (*setOpDeviceOrder)(HIAI_MR_ModelDeviceConfig*, size_t, HIAI_MR_OpDeviceOrder**);
    size_t (*getConfigOpNum)(const HIAI_MR_ModelDeviceConfig*);
    HIAI_MR_OpDeviceOrder** (*getOpDeviceOrder)(const HIAI_MR_ModelDeviceConfig*);

    void (*setDeviceMemoryReusePlan)(HIAI_MR_ModelDeviceConfig*, HIAI_DEVICE_MEMORY_REUSE_PLAN);
    HIAI_DEVICE_MEMORY_REUSE_PLAN (*getDeviceMemoryReusePlan)(const HIAI_MR_ModelDeviceConfig*);

    void (*setCLCustomization)(HIAI_MR_ModelDeviceConfig*, HIAI_MR_CLCustomization**);
    HIAI_MR_CLCustomization** (*getCLCustomization)(HIAI_MR_ModelDeviceConfig*);

    void (*destroy)(HIAI_MR_ModelDeviceConfig**);
} ModelDeviceConfigFuncs;

static void InitModelDeviceConfigFuncs(ModelDeviceConfigFuncs* modelDeviceConfigFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_CREATE] != NULL) {
        modelDeviceConfigFuncs->create = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_CREATE];
        modelDeviceConfigFuncs->setDeviceConfigMode =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETDEVICECONFIGMODE];
        modelDeviceConfigFuncs->getDeviceConfigMode =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETDEVICECONFIGMODE];
        modelDeviceConfigFuncs->setFallBackMode = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETFALLBACKMODE];
        modelDeviceConfigFuncs->getFallBackMode = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETFALLBACKMODE];
        modelDeviceConfigFuncs->setModelDeviceOrder =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETMODELDEVICEORDER];
        modelDeviceConfigFuncs->getConfigModelNum =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETCONFIGMODELNUM];
        modelDeviceConfigFuncs->getModelDeviceOrder =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETMODELDEVICEORDER];
        modelDeviceConfigFuncs->setOpDeviceOrder = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETOPDEVICEORDER];
        modelDeviceConfigFuncs->getConfigOpNum = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETCONFIGOPNUM];
        modelDeviceConfigFuncs->getOpDeviceOrder = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETOPDEVICEORDER];
        modelDeviceConfigFuncs->setDeviceMemoryReusePlan =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETDEVICEMEMORYREUSEPLAN];
        modelDeviceConfigFuncs->getDeviceMemoryReusePlan =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETDEVICEMEMORYREUSEPLAN];
        modelDeviceConfigFuncs->setCLCustomization =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_SETCLCUSTOMIZATION];
        modelDeviceConfigFuncs->getCLCustomization =
            runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_GETCLCUSTOMIZATION];
        modelDeviceConfigFuncs->destroy = runtime->optionsSymbolList[HIAI_MODELDEVICECONFIG_DESTROY];
    } else {
        // if you need add new func, don't add here.
        modelDeviceConfigFuncs->create = HIAI_ModelDeviceConfigV1_Create;
        modelDeviceConfigFuncs->setDeviceConfigMode = HIAI_ModelDeviceConfigV1_SetDeviceConfigMode;
        modelDeviceConfigFuncs->getDeviceConfigMode = HIAI_ModelDeviceConfigV1_GetDeviceConfigMode;
        modelDeviceConfigFuncs->setFallBackMode = HIAI_ModelDeviceConfigV1_SetFallBackMode;
        modelDeviceConfigFuncs->getFallBackMode = HIAI_ModelDeviceConfigV1_GetFallBackMode;
        modelDeviceConfigFuncs->setModelDeviceOrder = HIAI_ModelDeviceConfigV1_SetModelDeviceOrder;
        modelDeviceConfigFuncs->getConfigModelNum = HIAI_ModelDeviceConfigV1_GetConfigModelNum;
        modelDeviceConfigFuncs->getModelDeviceOrder = HIAI_ModelDeviceConfigV1_GetModelDeviceOrder;
        modelDeviceConfigFuncs->setOpDeviceOrder = HIAI_ModelDeviceConfigV1_SetOpDeviceOrder;
        modelDeviceConfigFuncs->getConfigOpNum = HIAI_ModelDeviceConfigV1_GetConfigOpNum;
        modelDeviceConfigFuncs->getOpDeviceOrder = HIAI_ModelDeviceConfigV1_GetOpDeviceOrder;
        modelDeviceConfigFuncs->setDeviceMemoryReusePlan = HIAI_ModelDeviceConfigV1_SetDeviceMemoryReusePlan;
        modelDeviceConfigFuncs->getDeviceMemoryReusePlan = HIAI_ModelDeviceConfigV1_GetDeviceMemoryReusePlan;
        modelDeviceConfigFuncs->setCLCustomization = HIAI_ModelDeviceConfigV1_SetCLCustomization;
        modelDeviceConfigFuncs->getCLCustomization = HIAI_ModelDeviceConfigV1_GetCLCustomization;
        modelDeviceConfigFuncs->destroy = HIAI_ModelDeviceConfigV1_Destroy;
    }
}

static ModelDeviceConfigFuncs* GetModelDeviceConfigFuncs(void)
{
    static ModelDeviceConfigFuncs modelDeviceConfigFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_optionFuncsInitMutex);
        if (!isInited) {
            InitModelDeviceConfigFuncs(&modelDeviceConfigFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_optionFuncsInitMutex);
    }

    return &modelDeviceConfigFuncs;
}

HIAI_MR_ModelDeviceConfig* HIAI_MR_ModelDeviceConfig_Create(void)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->create != NULL) {
        return modelDeviceConfigFuncs->create();
    }
    return NULL;
}

void HIAI_MR_ModelDeviceConfig_SetDeviceConfigMode(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_CONFIG_MODE deviceConfigMode)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setDeviceConfigMode != NULL) {
        modelDeviceConfigFuncs->setDeviceConfigMode(config, deviceConfigMode);
    }
}

HIAI_DEVICE_CONFIG_MODE HIAI_MR_ModelDeviceConfig_GetDeviceConfigMode(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getDeviceConfigMode != NULL) {
        return modelDeviceConfigFuncs->getDeviceConfigMode(config);
    }
    return HIAI_DEVICE_CONFIG_MODE_AUTO;
}

void HIAI_MR_ModelDeviceConfig_SetFallBackMode(HIAI_MR_ModelDeviceConfig* config, HIAI_FALLBACK_MODE fallBackMode)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setFallBackMode != NULL) {
        modelDeviceConfigFuncs->setFallBackMode(config, fallBackMode);
    }
}

HIAI_FALLBACK_MODE HIAI_MR_ModelDeviceConfig_GetFallBackMode(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getFallBackMode != NULL) {
        return modelDeviceConfigFuncs->getFallBackMode(config);
    }
    return HIAI_FALLBACK_MODE_ENABLE;
}

void HIAI_MR_ModelDeviceConfig_SetModelDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configModelNum, HIAI_EXECUTE_DEVICE* modelDeviceOrder)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setModelDeviceOrder != NULL) {
        modelDeviceConfigFuncs->setModelDeviceOrder(config, configModelNum, modelDeviceOrder);
    }
}

size_t HIAI_MR_ModelDeviceConfig_GetConfigModelNum(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getConfigModelNum != NULL) {
        return modelDeviceConfigFuncs->getConfigModelNum(config);
    }
    return 0;
}

HIAI_EXECUTE_DEVICE* HIAI_MR_ModelDeviceConfig_GetModelDeviceOrder(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getModelDeviceOrder != NULL) {
        return modelDeviceConfigFuncs->getModelDeviceOrder(config);
    }
    return NULL;
}

void HIAI_MR_ModelDeviceConfig_SetOpDeviceOrder(
    HIAI_MR_ModelDeviceConfig* config, size_t configOpNum, HIAI_MR_OpDeviceOrder** opDeviceOrder)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setOpDeviceOrder != NULL) {
        modelDeviceConfigFuncs->setOpDeviceOrder(config, configOpNum, opDeviceOrder);
    }
}

size_t HIAI_MR_ModelDeviceConfig_GetConfigOpNum(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getConfigOpNum != NULL) {
        return modelDeviceConfigFuncs->getConfigOpNum(config);
    }
    return 0;
}

HIAI_MR_OpDeviceOrder** HIAI_MR_ModelDeviceConfig_GetOpDeviceOrder(const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getOpDeviceOrder != NULL) {
        return modelDeviceConfigFuncs->getOpDeviceOrder(config);
    }
    return NULL;
}

void HIAI_MR_ModelDeviceConfig_SetDeviceMemoryReusePlan(
    HIAI_MR_ModelDeviceConfig* config, HIAI_DEVICE_MEMORY_REUSE_PLAN deviceMemoryReusePlan)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setDeviceMemoryReusePlan != NULL) {
        modelDeviceConfigFuncs->setDeviceMemoryReusePlan(config, deviceMemoryReusePlan);
    }
}

HIAI_DEVICE_MEMORY_REUSE_PLAN HIAI_MR_ModelDeviceConfig_GetDeviceMemoryReusePlan(
    const HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getDeviceMemoryReusePlan != NULL) {
        return modelDeviceConfigFuncs->getDeviceMemoryReusePlan(config);
    }
    return HIAI_DEVICE_MEMORY_REUSE_PLAN_UNSET;
}

void HIAI_MR_ModelDeviceConfig_SetCLCustomization(
    HIAI_MR_ModelDeviceConfig* config, HIAI_MR_CLCustomization** clCustomization)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->setCLCustomization != NULL) {
        modelDeviceConfigFuncs->setCLCustomization(config, clCustomization);
    }
}

HIAI_MR_CLCustomization** HIAI_MR_ModelDeviceConfig_GetCLCustomization(HIAI_MR_ModelDeviceConfig* config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->getCLCustomization != NULL) {
        return modelDeviceConfigFuncs->getCLCustomization(config);
    }
    return NULL;
}

void HIAI_MR_ModelDeviceConfig_Destroy(HIAI_MR_ModelDeviceConfig** config)
{
    ModelDeviceConfigFuncs* modelDeviceConfigFuncs = GetModelDeviceConfigFuncs();
    if (modelDeviceConfigFuncs->destroy != NULL) {
        modelDeviceConfigFuncs->destroy(config);
    }
}

typedef struct ModelBuildOptionsFuncs {
    HIAI_MR_ModelBuildOptions* (*create)(void);

    void (*setInputTensorDescs)(HIAI_MR_ModelBuildOptions*, size_t, HIAI_NDTensorDesc**);
    size_t (*getInputSize)(const HIAI_MR_ModelBuildOptions*);
    HIAI_NDTensorDesc** (*getInputTensorDescs)(const HIAI_MR_ModelBuildOptions*);

    void (*setFormatModeOption)(HIAI_MR_ModelBuildOptions*, HIAI_FORMAT_MODE_OPTION);
    HIAI_FORMAT_MODE_OPTION (*getFormatModeOption)(const HIAI_MR_ModelBuildOptions*);

    HIAI_Status (*setPrecisionModeOption)(HIAI_MR_ModelBuildOptions*, HIAI_PRECISION_MODE_OPTION);
    HIAI_PRECISION_MODE_OPTION (*getPrecisionModeOption)(const HIAI_MR_ModelBuildOptions*);

    void (*setDynamicShapeConfig)(HIAI_MR_ModelBuildOptions*, HIAI_MR_DynamicShapeConfig*);
    HIAI_MR_DynamicShapeConfig* (*getDynamicShapeConfig)(const HIAI_MR_ModelBuildOptions*);

    void (*setModelDeviceConfig)(HIAI_MR_ModelBuildOptions*, HIAI_MR_ModelDeviceConfig*);
    HIAI_MR_ModelDeviceConfig* (*getModelDeviceConfig)(const HIAI_MR_ModelBuildOptions*);

    HIAI_Status (*setTuningStrategy)(HIAI_MR_ModelBuildOptions*, HIAI_TUNING_STRATEGY);
    HIAI_TUNING_STRATEGY (*getTuningStrategy)(const HIAI_MR_ModelBuildOptions*);

    void (*setEstimatedOutputSize)(HIAI_MR_ModelBuildOptions*, size_t);
    size_t (*getEstimatedOutputSize)(const HIAI_MR_ModelBuildOptions*);

    HIAI_Status (*setQuantizeConfig)(HIAI_MR_ModelBuildOptions*, uint8_t*, size_t);
    uint8_t* (*getQuantizeConfigData)(const HIAI_MR_ModelBuildOptions*);
    size_t (*getQuantizeConfigSize)(const HIAI_MR_ModelBuildOptions*);

    void (*destroy)(HIAI_MR_ModelBuildOptions**);
} ModelBuildOptionsFuncs;

static void InitModelBuildOptionsFuncs(ModelBuildOptionsFuncs* modelBuildOptionsFuncs)
{
    HIAI_ModelRuntime* runtime = HIAI_ModelRuntimeRepo_GetSutiableHclRuntime();
    if (runtime != NULL && runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_CREATE] != NULL) {
        modelBuildOptionsFuncs->create = runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_CREATE];

        modelBuildOptionsFuncs->setInputTensorDescs =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETINPUTTENSORDESCS];
        modelBuildOptionsFuncs->getInputSize = runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETINPUTSIZE];
        modelBuildOptionsFuncs->getInputTensorDescs =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETINPUTTENSORDESCS];

        modelBuildOptionsFuncs->setFormatModeOption =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETFORMATMODEOPTION];
        modelBuildOptionsFuncs->getFormatModeOption =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETFORMATMODEOPTION];

        modelBuildOptionsFuncs->setPrecisionModeOption =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETPRECISIONMODEOPTION];
        modelBuildOptionsFuncs->getPrecisionModeOption =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETPRECISIONMODEOPTION];
        modelBuildOptionsFuncs->setDynamicShapeConfig =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETDYNAMICSHAPECONFIG];
        modelBuildOptionsFuncs->getDynamicShapeConfig =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETDYNAMICSHAPECONFIG];
        modelBuildOptionsFuncs->setModelDeviceConfig =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETMODELDEVICECONFIG];
        modelBuildOptionsFuncs->getModelDeviceConfig =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETMODELDEVICECONFIG];
        modelBuildOptionsFuncs->setTuningStrategy =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETTUNINGSTRATEGY];
        modelBuildOptionsFuncs->getTuningStrategy =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETTUNINGSTRATEGY];
        modelBuildOptionsFuncs->setEstimatedOutputSize =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETESTIMATEOUTPUTSIZE];
        modelBuildOptionsFuncs->getEstimatedOutputSize =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETESTIMATEOUTPUTSIZE];

        modelBuildOptionsFuncs->setQuantizeConfig =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_SETQUANTIZECONFIG];
        modelBuildOptionsFuncs->getQuantizeConfigData =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETQUANTIZECONFIGDATA];
        modelBuildOptionsFuncs->getQuantizeConfigSize =
            runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_GETQUANTIZECONFIGSIZE];

        modelBuildOptionsFuncs->destroy = runtime->optionsSymbolList[HIAI_MODELBUILDOPTIONS_DESTROY];
    } else {
        // if you need add new func, don't add here.
        modelBuildOptionsFuncs->create = HIAI_ModelBuildOptionsV1_Create;
        modelBuildOptionsFuncs->setInputTensorDescs = HIAI_ModelBuildOptionsV1_SetInputTensorDescs;
        modelBuildOptionsFuncs->getInputSize = HIAI_ModelBuildOptionsV1_GetInputSize;
        modelBuildOptionsFuncs->getInputTensorDescs = HIAI_ModelBuildOptionsV1_GetInputTensorDescs;
        modelBuildOptionsFuncs->setFormatModeOption = HIAI_ModelBuildOptionsV1_SetFormatModeOption;
        modelBuildOptionsFuncs->getFormatModeOption = HIAI_ModelBuildOptionsV1_GetFormatModeOption;
        modelBuildOptionsFuncs->setPrecisionModeOption = HIAI_ModelBuildOptionsV1_SetPrecisionModeOption;
        modelBuildOptionsFuncs->getPrecisionModeOption = HIAI_ModelBuildOptionsV1_GetPrecisionModeOption;
        modelBuildOptionsFuncs->setDynamicShapeConfig = HIAI_ModelBuildOptionsV1_SetDynamicShapeConfig;
        modelBuildOptionsFuncs->getDynamicShapeConfig = HIAI_ModelBuildOptionsV1_GetDynamicShapeConfig;
        modelBuildOptionsFuncs->setModelDeviceConfig = HIAI_ModelBuildOptionsV1_SetModelDeviceConfig;
        modelBuildOptionsFuncs->getModelDeviceConfig = HIAI_ModelBuildOptionsV1_GetModelDeviceConfig;
        modelBuildOptionsFuncs->setTuningStrategy = HIAI_ModelBuildOptionsV1_SetTuningStrategy;
        modelBuildOptionsFuncs->getTuningStrategy = HIAI_ModelBuildOptionsV1_GetTuningStrategy;
        modelBuildOptionsFuncs->setEstimatedOutputSize = HIAI_ModelBuildOptionsV1_SetEstimatedOutputSize;
        modelBuildOptionsFuncs->getEstimatedOutputSize = HIAI_ModelBuildOptionsV1_GetEstimatedOutputSize;

        modelBuildOptionsFuncs->setQuantizeConfig = HIAI_ModelBuildOptionsV1_SetQuantizeConfig;
        modelBuildOptionsFuncs->getQuantizeConfigData = HIAI_ModelBuildOptionsV1_GetQuantizeConfigData;
        modelBuildOptionsFuncs->getQuantizeConfigSize = HIAI_ModelBuildOptionsV1_GetQuantizeConfigSize;

        modelBuildOptionsFuncs->destroy = HIAI_ModelBuildOptionsV1_Destroy;
    }
}

static ModelBuildOptionsFuncs* GetModelBuildOptionsFuncs(void)
{
    static ModelBuildOptionsFuncs modelBuildOptionsFuncs;

    static bool isInited = false;
    if (!isInited) {
        pthread_mutex_lock(&g_optionFuncsInitMutex);
        if (!isInited) {
            InitModelBuildOptionsFuncs(&modelBuildOptionsFuncs);
            isInited = true;
        }
        pthread_mutex_unlock(&g_optionFuncsInitMutex);
    }

    return &modelBuildOptionsFuncs;
}

HIAI_MR_ModelBuildOptions* HIAI_MR_ModelBuildOptions_Create(void)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->create != NULL) {
        return modelBuildOptionsFuncs->create();
    }
    return NULL;
}

void HIAI_MR_ModelBuildOptions_SetInputTensorDescs(
    HIAI_MR_ModelBuildOptions* options, size_t inputNum, HIAI_NDTensorDesc** inputTensorDescs)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setInputTensorDescs != NULL) {
        modelBuildOptionsFuncs->setInputTensorDescs(options, inputNum, inputTensorDescs);
    }
}

size_t HIAI_MR_ModelBuildOptions_GetInputSize(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getInputSize != NULL) {
        return modelBuildOptionsFuncs->getInputSize(options);
    }
    return 0;
}

HIAI_NDTensorDesc** HIAI_MR_ModelBuildOptions_GetInputTensorDescs(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getInputTensorDescs != NULL) {
        return modelBuildOptionsFuncs->getInputTensorDescs(options);
    }
    return NULL;
}

void HIAI_MR_ModelBuildOptions_SetFormatModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_FORMAT_MODE_OPTION formatMode)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setFormatModeOption != NULL) {
        modelBuildOptionsFuncs->setFormatModeOption(options, formatMode);
    }
}

HIAI_FORMAT_MODE_OPTION HIAI_MR_ModelBuildOptions_GetFormatModeOption(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getFormatModeOption != NULL) {
        return modelBuildOptionsFuncs->getFormatModeOption(options);
    }
    return HIAI_FORMAT_MODE_USE_NCHW;
}

HIAI_Status HIAI_MR_ModelBuildOptions_SetPrecisionModeOption(
    HIAI_MR_ModelBuildOptions* options, HIAI_PRECISION_MODE_OPTION precisionMode)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setPrecisionModeOption != NULL) {
        return modelBuildOptionsFuncs->setPrecisionModeOption(options, precisionMode);
    }
    return HIAI_SUCCESS;
}

HIAI_PRECISION_MODE_OPTION HIAI_MR_ModelBuildOptions_GetPrecisionModeOption(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getPrecisionModeOption != NULL) {
        return modelBuildOptionsFuncs->getPrecisionModeOption(options);
    }
    return HIAI_PRECISION_MODE_FP32;
}

void HIAI_MR_ModelBuildOptions_SetDynamicShapeConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_DynamicShapeConfig* dynamicShapeConfig)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setDynamicShapeConfig != NULL) {
        modelBuildOptionsFuncs->setDynamicShapeConfig(options, dynamicShapeConfig);
    }
}

HIAI_MR_DynamicShapeConfig* HIAI_MR_ModelBuildOptions_GetDynamicShapeConfig(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getDynamicShapeConfig != NULL) {
        return modelBuildOptionsFuncs->getDynamicShapeConfig(options);
    }
    return NULL;
}

void HIAI_MR_ModelBuildOptions_SetModelDeviceConfig(
    HIAI_MR_ModelBuildOptions* options, HIAI_MR_ModelDeviceConfig* modelDeviceConfig)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setModelDeviceConfig != NULL) {
        modelBuildOptionsFuncs->setModelDeviceConfig(options, modelDeviceConfig);
    }
}

HIAI_MR_ModelDeviceConfig* HIAI_MR_ModelBuildOptions_GetModelDeviceConfig(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getModelDeviceConfig != NULL) {
        return modelBuildOptionsFuncs->getModelDeviceConfig(options);
    }
    return NULL;
}

HIAI_Status HIAI_MR_ModelBuildOptions_SetTuningStrategy(
    HIAI_MR_ModelBuildOptions* options, HIAI_TUNING_STRATEGY tuningStrategy)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setTuningStrategy != NULL) {
        return modelBuildOptionsFuncs->setTuningStrategy(options, tuningStrategy);
    }
    return HIAI_SUCCESS;
}

HIAI_TUNING_STRATEGY HIAI_MR_ModelBuildOptions_GetTuningStrategy(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getTuningStrategy != NULL) {
        return modelBuildOptionsFuncs->getTuningStrategy(options);
    }
    return HIAI_TUNING_STRATEGY_OFF;
}

void HIAI_MR_ModelBuildOptions_SetEstimatedOutputSize(HIAI_MR_ModelBuildOptions* options, size_t size)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setEstimatedOutputSize != NULL) {
        modelBuildOptionsFuncs->setEstimatedOutputSize(options, size);
    }
}

size_t HIAI_MR_ModelBuildOptions_GetEstimatedOutputSize(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getEstimatedOutputSize != NULL) {
        return modelBuildOptionsFuncs->getEstimatedOutputSize(options);
    }
    return 0;
}

HIAI_Status HIAI_MR_ModelBuildOptions_SetQuantizeConfig(HIAI_MR_ModelBuildOptions* options, uint8_t* data, size_t size)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->setQuantizeConfig != NULL) {
        return modelBuildOptionsFuncs->setQuantizeConfig(options, data, size);
    }
    return HIAI_SUCCESS;
}

uint8_t* HIAI_MR_ModelBuildOptions_GetQuantizeConfigData(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getQuantizeConfigData != NULL) {
        return modelBuildOptionsFuncs->getQuantizeConfigData(options);
    }
    return NULL;
}

size_t HIAI_MR_ModelBuildOptions_GetQuantizeConfigSize(const HIAI_MR_ModelBuildOptions* options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->getQuantizeConfigSize != NULL) {
        return modelBuildOptionsFuncs->getQuantizeConfigSize(options);
    }
    return 0;
}

void HIAI_MR_ModelBuildOptions_Destroy(HIAI_MR_ModelBuildOptions** options)
{
    ModelBuildOptionsFuncs* modelBuildOptionsFuncs = GetModelBuildOptionsFuncs();
    if (modelBuildOptionsFuncs->destroy != NULL) {
        modelBuildOptionsFuncs->destroy(options);
    }
}
