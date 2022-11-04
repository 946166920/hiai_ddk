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
#include "model_runtime/core/hiai_model_runtime.h"

#include <stdlib.h>
#include <dlfcn.h>
// inc
#include "framework/c/hiai_error_types.h"
#include "framework/infra/log/log.h"
#include "securec.h"

void HIAI_ModelRuntime_UnLoad(HIAI_ModelRuntime* self)
{
    if (self != NULL) {
        if (self->handle != NULL) {
            dlclose(self->handle);
        }
        free(self);
    }
}

static HIAI_SymbolName g_symbolFuncNames[HRANI_SIZE] = {
    {"ModelBuilder_Build", HRANI_MODELBUILDER_BUILD},
    {"ModelBuilder_BuildV2", HRANI_MODELBUILDER_BUILDV2},
    {"ModelBuilder_BuildFromSharedBuffer", HRANI_MODELBUILDER_BUILD_FROM_SHARED_BUFFER},
    {"BuiltModel_SaveToExternalBuffer", HRANI_BUILTMODEL_SAVE_TO_EXTERNAL_BUFFER},
    {"BuiltModel_Save", HRANI_BUILTMODEL_SAVE},
    {"BuiltModel_SaveToFile", HRANI_BUILTMODEL_SAVE_TO_FILE},
    {"BuiltModel_Restore", HRANI_BUILTMODEL_RESTORE},
    {"BuiltModel_RestoreFromFile", HRANI_BUILTMODEL_RESTORE_FROM_FILE},
    {"BuiltModel_CheckCompatibility", HRANI_BUILTMODEL_CHECK_COMPATIBILITY},
    {"BuiltModel_GetName", HRANI_BUILTMODEL_GET_NAME},
    {"BuiltModel_SetName", HRANI_BUILTMODEL_SET_NAME},
    {"BuiltModel_GetInputTensorNum", HRANI_BUILTMODEL_GET_INPUT_TENSOR_NUM},
    {"BuiltModel_GetInputTensorDesc", HRANI_BUILTMODEL_GET_INPUT_TENSOR_DESC},
    {"BuiltModel_GetOutputTensorNum", HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_NUM},
    {"BuiltModel_GetOutputTensorDesc", HRANI_BUILTMODEL_GET_OUTPUT_TENSOR_DESC},
    {"BuiltModel_Destroy", HRANI_BUILTMODEL_DESTROY},
    {"BuiltModel_GetTensorAippInfo", HRANI_BUILTMODEL_GET_TENSOR_AIPP_INFO},
    {"BuiltModel_GetTensorAippPara", HRANI_BUILTMODEL_GET_TENSOR_AIPP_PARA},
    {"ModelManager_Create", HRANI_MODELMANAGER_CREATE},
    {"ModelManager_Destroy", HRANI_MODELMANAGER_DESTROY},
    {"ModelManager_Init", HRANI_MODELMANAGER_INIT},
    {"ModelManager_InitV2", HRANI_MODELMANAGER_INITV2},
    {"ModelManager_SetPriority", HRANI_MODELMANAGER_SET_PRIORITY},
    {"ModelManager_Run", HRANI_MODELMANAGER_RUN},
    {"ModelManager_RunAsync", HRANI_MODELMANAGER_RUN_ASYNC},
    {"ModelManager_Cancel", HRANI_MODELMANAGER_CANCEL},
    {"ModelManager_Deinit", HRANI_MODELMANAGER_DEINIT},
    {"ModelManager_InitWithSharedMemAllocator", HRANI_MODELMANAGER_INIT_WITH_SHARED_MEM_ALLOCATOR},
    {"ModelManager_runAippModelV2", HRANI_MODELMANAGER_RUN_AIPP_MODEL_V2},
    {"ModelManager_RunV3", HRANI_MODELMANAGER_RUN_V3},
    {"ModelManager_RunAsyncV3", HRANI_MODELMANAGER_RUN_ASYNC_V3},
    {"ModelManager_runAippModelV3", HRANI_MODELMANAGER_RUN_AIPP_MODEL_V3},
};

static HIAI_SymbolName g_symbolOptionsNames[OPTIONS_SIZE] = {
    {"ModelBuildOptions_Create", HIAI_MODELBUILDOPTIONS_CREATE},
    {"ModelBuildOptions_Destroy", HIAI_MODELBUILDOPTIONS_DESTROY},
    {"ModelBuildOptions_SetInputTensorDescs", HIAI_MODELBUILDOPTIONS_SETINPUTTENSORDESCS},
    {"ModelBuildOptions_GetInputSize", HIAI_MODELBUILDOPTIONS_GETINPUTSIZE},
    {"ModelBuildOptions_GetInputTensorDescs", HIAI_MODELBUILDOPTIONS_GETINPUTTENSORDESCS},
    {"ModelBuildOptions_SetFormatModeOption", HIAI_MODELBUILDOPTIONS_SETFORMATMODEOPTION},
    {"ModelBuildOptions_GetFormatModeOption", HIAI_MODELBUILDOPTIONS_GETFORMATMODEOPTION},
    {"ModelBuildOptions_SetPrecisionModeOption", HIAI_MODELBUILDOPTIONS_SETPRECISIONMODEOPTION},
    {"ModelBuildOptions_GetPrecisionModeOption", HIAI_MODELBUILDOPTIONS_GETPRECISIONMODEOPTION},
    {"ModelBuildOptions_SetDynamicShapeConfig", HIAI_MODELBUILDOPTIONS_SETDYNAMICSHAPECONFIG},
    {"ModelBuildOptions_GetDynamicShapeConfig", HIAI_MODELBUILDOPTIONS_GETDYNAMICSHAPECONFIG},
    {"ModelBuildOptions_SetModelDeviceConfig", HIAI_MODELBUILDOPTIONS_SETMODELDEVICECONFIG},
    {"ModelBuildOptions_GetModelDeviceConfig", HIAI_MODELBUILDOPTIONS_GETMODELDEVICECONFIG},
    {"ModelBuildOptions_SetTuningStrategy", HIAI_MODELBUILDOPTIONS_SETTUNINGSTRATEGY},
    {"ModelBuildOptions_GetTuningStrategy", HIAI_MODELBUILDOPTIONS_GETTUNINGSTRATEGY},
    {"ModelBuildOptions_SetEstimatedOutputSize", HIAI_MODELBUILDOPTIONS_SETESTIMATEOUTPUTSIZE},
    {"ModelBuildOptions_GetEstimatedOutputSize", HIAI_MODELBUILDOPTIONS_GETESTIMATEOUTPUTSIZE},
    {"ModelBuildOptions_SetQuantizeConfig", HIAI_MODELBUILDOPTIONS_SETQUANTIZECONFIG},
    {"ModelBuildOptions_GetQuantizeConfigData", HIAI_MODELBUILDOPTIONS_GETQUANTIZECONFIGDATA},
    {"ModelBuildOptions_GetQuantizeConfigSize", HIAI_MODELBUILDOPTIONS_GETQUANTIZECONFIGSIZE},
    {"DynamicShapeConfig_Create", HIAI_DYNAMICSHAPECONFIG_CREATE},
    {"DynamicShapeConfig_SetEnableMode", HIAI_DYNAMICSHAPECONFIG_SETENABLEMODE},
    {"DynamicShapeConfig_GetEnableMode", HIAI_DYNAMICSHAPECONFIG_GETENABLEMODE},
    {"DynamicShapeConfig_SetMaxCacheNum", HIAI_DYNAMICSHAPECONFIG_SETMAXCACHENUM},
    {"DynamicShapeConfig_GetMaxCacheNum", HIAI_DYNAMICSHAPECONFIG_GETMAXCACHENUM},
    {"DynamicShapeConfig_SetCacheMode", HIAI_DYNAMICSHAPECONFIG_SETCACHEMODE},
    {"DynamicShapeConfig_GetCacheMode", HIAI_DYNAMICSHAPECONFIG_GETCACHEMODE},
    {"DynamicShapeConfig_Destroy", HIAI_DYNAMICSHAPECONFIG_DESTROY},
    {"OpDeviceOrder_Create", HIAI_OPDEVICEORDER_CREATE},
    {"OpDeviceOrder_SetOpName", HIAI_OPDEVICEORDER_SETOPNAME},
    {"OpDeviceOrder_GetOpName", HIAI_OPDEVICEORDER_GETOPNAME},
    {"OpDeviceOrder_SetDeviceOrder", HIAI_OPDEVICEORDER_SETDEVICEORDER},
    {"OpDeviceOrder_GetSupportedDeviceNum", HIAI_OPDEVICEORDER_GETSUPPORTEDDEVICENUM},
    {"OpDeviceOrder_GetSupportedDevices", HIAI_OPDEVICEORDER_GetSUPPORTEDDEVICES},
    {"OpDeviceOrder_Destroy", HIAI_OPDEVICEORDER_DESTROY},
    {"CLCustomization_Create", HIAI_CLCUSTOMIZATION_CREATE},
    {"CLCustomization_SetOpName", HIAI_CLCUSTOMIZATION_SETOPNAME},
    {"CLCustomization_GetOpName", HIAI_CLCUSTOMIZATION_GETOPNAME},
    {"CLCustomization_SetCustomization", HIAI_CLCUSTOMIZATION_SETCUSTOMIZATION},
    {"CLCustomization_GetCustomization", HIAI_CLCUSTOMIZATION_GETCUSTOMIZATION},
    {"CLCustomization_Destroy", HIAI_CLCUSTOMIZATION_DESTROY},
    {"ModelDeviceConfig_Create", HIAI_MODELDEVICECONFIG_CREATE},
    {"ModelDeviceConfig_SetDeviceConfigMode", HIAI_MODELDEVICECONFIG_SETDEVICECONFIGMODE},
    {"ModelDeviceConfig_GetDeviceConfigMode", HIAI_MODELDEVICECONFIG_GETDEVICECONFIGMODE},
    {"ModelDeviceConfig_SetFallBackMode", HIAI_MODELDEVICECONFIG_SETFALLBACKMODE},
    {"ModelDeviceConfig_GetFallBackMode", HIAI_MODELDEVICECONFIG_GETFALLBACKMODE},
    {"ModelDeviceConfig_SetModelDeviceOrder", HIAI_MODELDEVICECONFIG_SETMODELDEVICEORDER},
    {"ModelDeviceConfig_GetConfigModelNum", HIAI_MODELDEVICECONFIG_GETCONFIGMODELNUM},
    {"ModelDeviceConfig_GetModelDeviceOrder", HIAI_MODELDEVICECONFIG_GETMODELDEVICEORDER},
    {"ModelDeviceConfig_SetOpDeviceOrder", HIAI_MODELDEVICECONFIG_SETOPDEVICEORDER},
    {"ModelDeviceConfig_GetConfigOpNum", HIAI_MODELDEVICECONFIG_GETCONFIGOPNUM},
    {"ModelDeviceConfig_GetOpDeviceOrder", HIAI_MODELDEVICECONFIG_GETOPDEVICEORDER},
    {"ModelDeviceConfig_SetDeviceMemoryReusePlan", HIAI_MODELDEVICECONFIG_SETDEVICEMEMORYREUSEPLAN},
    {"ModelDeviceConfig_GetDeviceMemoryReusePlan", HIAI_MODELDEVICECONFIG_GETDEVICEMEMORYREUSEPLAN},
    {"ModelDeviceConfig_SetCLCustomization", HIAI_MODELDEVICECONFIG_SETCLCUSTOMIZATION},
    {"ModelDeviceConfig_GetCLCustomization", HIAI_MODELDEVICECONFIG_GETCLCUSTOMIZATION},
    {"ModelDeviceConfig_Destroy", HIAI_MODELDEVICECONFIG_DESTROY},
    {"ModelInitOptions_Create", HIAI_MODELINITOPTIONS_CREATE},
    {"ModelInitOptions_SetPerfMode", HIAI_MODELINITOPTIONS_SETPERFMODE},
    {"ModelInitOptions_GetPerfMode", HIAI_MODELINITOPTIONS_GETPERFMODE},
    {"ModelInitOptions_SetBuildOptions", HIAI_MODELINITOPTIONS_SETBUILDOPTIONS},
    {"ModelInitOptions_GetBuildOptions", HIAI_MODELINITOPTIONS_GETBUILDOPTIONS},
    {"ModelInitOptions_Destroy", HIAI_MODELINITOPTIONS_DESTROY},
};

static char* HIAI_ModelRuntime_ConcateName(const char* prefix, char* funcName)
{
    size_t len = strlen(funcName) + strlen(prefix) + 1;
    char* name = malloc(len);
    if (name == NULL) {
        FMK_LOGE("malloc name failed");
        return NULL;
    }

    if (strcpy_s(name, len, prefix) != EOK) {
        FMK_LOGE("strcpy_s name failed");
        free(name);
        return NULL;
    }
    if (strcat_s(name, len, funcName) != EOK) {
        FMK_LOGE("strcat_s name failed");
        free(name);
        return NULL;
    }
    name[len - 1] = '\0';
    return name;
}

static int32_t HIAI_ModelRuntime_GetSymbols(
    const char* prefix, int32_t size, HIAI_SymbolName symbolNames[], void* handle, void* symbolList[])
{
    int32_t count = 0;

    for (int32_t i = 0; i < size; i++) {
        char* name = HIAI_ModelRuntime_ConcateName(prefix, symbolNames[i].name);
        if (name == NULL) {
            FMK_LOGE("concate name failed");
            return 0;
        }

        void* ptr = dlsym(handle, name);
        if (ptr != NULL) {
            symbolList[symbolNames[i].index] = ptr;
            count++;
        }
        free(name);
    }

    return count;
}

static HIAI_Status HIAI_ModelRuntime_LoadFromAllSymbols(HIAI_ModelRuntime* runtime, const char* prefix)
{
    int32_t count =
        HIAI_ModelRuntime_GetSymbols(prefix, HRANI_SIZE, g_symbolFuncNames, runtime->handle, runtime->symbolList);
    if (count > 0) {
        (void)HIAI_ModelRuntime_GetSymbols(
            prefix, OPTIONS_SIZE, g_symbolOptionsNames, runtime->handle, runtime->optionsSymbolList);
        FMK_LOGI("HIAI_ModelRuntime_LoadFromAllSymbols success.");
        return HIAI_SUCCESS;
    }

    FMK_LOGW("runtime is not open symbols.");
    return HIAI_FAILURE;
}

#define HIAI_HCL_MODEL_RUNTIME_SO "libhiai_hcl_model_runtime.so"
#define HIAI_BINARY_MODEL_RUNTIME_SO "libhiai_binary_model_runtime.so"
#ifdef __aarch64__
#define FMK_HCL_MODEL_RUNTIME_SO "/vendor/lib64/libai_fmk_hcl_model_runtime.so"
#else
#define FMK_HCL_MODEL_RUNTIME_SO "/vendor/lib/libai_fmk_hcl_model_runtime.so"
#endif

#define HCL_PREFIX "HIAI_HCL_"
#define BINARY_PREFIX "HIAI_BINARY_"

typedef void* (*GET_PLUGIN_SO_HANDLE_FUNC)(const char* soName);
static void* HIAI_ModelRuntime_GetPluginAppRuntimeHandle(void)
{
    const char* libraryName = "libhiai_enhance.so";
    dlerror();
    void* handle = dlopen(libraryName, RTLD_LAZY);
    if (handle == NULL) {
        FMK_LOGW("dlopen failed, lib[%s], errmsg[%s]", libraryName, dlerror());
        return 0L;
    }
    const char* functionName = "GetPluginSoHandleDefault";
    GET_PLUGIN_SO_HANDLE_FUNC getPluginSoHandleFunc = NULL;
    *(void**)(&getPluginSoHandleFunc) = dlsym(handle, functionName);
    if (getPluginSoHandleFunc == NULL) {
        FMK_LOGW("dlsym failed, lib[%s], errmsg[%s]", functionName, dlerror());
        if (dlclose(handle) != 0) {
            FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
        }
        return 0L;
    }

    void* pluginRuntimeHandle = getPluginSoHandleFunc(HIAI_HCL_MODEL_RUNTIME_SO);
    if (pluginRuntimeHandle == 0L) {
        FMK_LOGW("get lib[%s] handle fail.", HIAI_HCL_MODEL_RUNTIME_SO);
        if (dlclose(handle) != 0) {
            FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
        }
        return 0L;
    }

    if (dlclose(handle) != 0) {
        FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
        return 0L;
    }
    return pluginRuntimeHandle;
}

typedef void (*HIAI_ModelRuntime_Init_Ptr)(HIAI_ModelRuntime* runtime);
static void HIAI_ModelRuntime_LoadSymbols(HIAI_ModelRuntime* runtime, const char* prefix)
{
    if (runtime == NULL) {
        return;
    }
    if (HIAI_ModelRuntime_LoadFromAllSymbols(runtime, prefix) == HIAI_SUCCESS) {
        return;
    }

    HIAI_ModelRuntime_Init_Ptr init = (HIAI_ModelRuntime_Init_Ptr)dlsym(runtime->handle, "HIAI_ModelRuntime_Init");
    if (init == NULL) {
        FMK_LOGE("not found HIAI_ModelRuntime_Init.");
        HIAI_ModelRuntime_UnLoad(runtime);
        return;
    }
    init(runtime);
}

static HIAI_ModelRuntime* HIAI_ModelRuntime_LoadPlugin(void)
{
    HIAI_ModelRuntime* runtime = (HIAI_ModelRuntime*)malloc(sizeof(HIAI_ModelRuntime));
    if (runtime == NULL) {
        FMK_LOGE("malloc HIAI_ModelRuntime fail.");
        return NULL;
    }
    (void)memset_s(runtime, sizeof(HIAI_ModelRuntime), 0, sizeof(HIAI_ModelRuntime));

    runtime->handle = HIAI_ModelRuntime_GetPluginAppRuntimeHandle();
    if (runtime->handle == NULL) {
        HIAI_ModelRuntime_UnLoad(runtime);
        return NULL;
    }

    runtime->runtimeType = PLUGIN_MODEL_RUNTIME_HCL;
    return runtime;
}

static HIAI_ModelRuntime* HIAI_ModelRuntime_LoadSo(HIAI_ModelRuntimeType type, const char* libName)
{
    if (libName == NULL) {
        return NULL;
    }
    HIAI_ModelRuntime* runtime = (HIAI_ModelRuntime*)malloc(sizeof(HIAI_ModelRuntime));
    if (runtime == NULL) {
        FMK_LOGE("malloc HIAI_ModelRuntime fail.");
        return NULL;
    }
    (void)memset_s(runtime, sizeof(HIAI_ModelRuntime), 0, sizeof(HIAI_ModelRuntime));

    dlerror();
    runtime->handle = dlopen(libName, RTLD_NOW);
    if (runtime->handle == NULL) {
        FMK_LOGW("dlopen %s fail: %s.", libName, dlerror());
        HIAI_ModelRuntime_UnLoad(runtime);
        return NULL;
    }

    runtime->runtimeType = type;
    return runtime;
}

HIAI_ModelRuntime* HIAI_ModelRuntime_Load(HIAI_ModelRuntimeType type)
{
    HIAI_ModelRuntime* modelRuntime = NULL;
    switch (type) {
        case PLUGIN_MODEL_RUNTIME_HCL: {
            modelRuntime = HIAI_ModelRuntime_LoadPlugin();
            HIAI_ModelRuntime_LoadSymbols(modelRuntime, HCL_PREFIX);
            break;
        }
        case HIAI_MODEL_RUNTIME_HCL: {
            modelRuntime = HIAI_ModelRuntime_LoadSo(type, HIAI_HCL_MODEL_RUNTIME_SO);
            HIAI_ModelRuntime_LoadSymbols(modelRuntime, HCL_PREFIX);
            break;
        }
        case HIAI_MODEL_RUNTIME_BINARY: {
            modelRuntime = HIAI_ModelRuntime_LoadSo(type, HIAI_BINARY_MODEL_RUNTIME_SO);
            HIAI_ModelRuntime_LoadSymbols(modelRuntime, BINARY_PREFIX);
            break;
        }
        case FMK_MODEL_RUNTIME_HCL: {
            modelRuntime = HIAI_ModelRuntime_LoadSo(type, FMK_HCL_MODEL_RUNTIME_SO);
            HIAI_ModelRuntime_LoadSymbols(modelRuntime, HCL_PREFIX);
            break;
        }
        default: {
            break;
        }
    }
    return modelRuntime;
}
