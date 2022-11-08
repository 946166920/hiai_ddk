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
#include "proto_factory.h"

#include "graph/persistance/interface/attr_def.h"
#include "graph/persistance/interface/attr_list_def.h"
#include "graph/persistance/interface/attr_map_def.h"
#include "graph/persistance/interface/graph_def.h"
#include "graph/persistance/interface/model_def.h"
#include "graph/persistance/interface/named_attr_def.h"
#include "graph/persistance/interface/op_def.h"
#include "graph/persistance/interface/shape_def.h"
#include "graph/persistance/interface/tensor_def.h"
#include "graph/persistance/interface/tensor_desc_def.h"

#include "framework/graph/debug/ge_log.h"

#include <dlfcn.h>
#include <mutex>

#include "securec.h"

namespace hiai {

#if defined(HIAI_DDK)
const char* const HIAI_ENHANCE_LIB_NAME = "libhiai_enhance.so";
using GET_PLUGIN_SO_HANDLE_FUNC = void* (*)(const char* soName);

const char* const PERSISTANCE_DDK_LIB_NAME = "libhiai_graph_proto.so";
const char* const PERSISTANCE_APP_LIB_NAME = "libai_fmk_graph_proto_app.so";
const char* const PERSISTANCE_ABS_ROM_LIB_NAME = "/vendor/lib64/libai_fmk_graph_proto.so";
#endif

#if defined(GRAPH_HOST)
const char* const PERSISTANCE_LIB_NAME = "libai_fmk_graph_proto_host.so";
#elif defined(GRAPH_DEVICE)
const char* const PERSISTANCE_LIB_NAME = "libai_fmk_graph_proto.so";
#elif defined(GRAPH_APP)
const char* const PERSISTANCE_LIB_NAME = "libai_fmk_graph_proto_app.so";
#endif

static std::mutex loadSoMutex;

ProtoFactory* ProtoFactory::Instance()
{
    static ProtoFactory instance;
    return &instance;
}

ProtoFactory::~ProtoFactory()
{
    std::lock_guard<std::mutex> guard(loadSoMutex);
    if (handle_ != nullptr) {
        dlclose(handle_);
        handle_ = nullptr;
    }

    createModelDef_ = nullptr;
    destroyModelDef_ = nullptr;
    createGraphDef_ = nullptr;
    destroyGraphDef_ = nullptr;
    createOpDef_ = nullptr;
    destroyOpDef_ = nullptr;
    createTensorDescDef_ = nullptr;
    destroyTensorDescDef_ = nullptr;
    createTensorDef_ = nullptr;
    destroyTensorDef_ = nullptr;
    createShapeDef_ = nullptr;
    destroyShapeDef_ = nullptr;
    createAttrDef_ = nullptr;
    destroyAttrDef_ = nullptr;
    createAttrMapDef_ = nullptr;
    destroyAttrMapDef_ = nullptr;
    createNamedAttrDef_ = nullptr;
    destroyNamedAttrDef_ = nullptr;
}

namespace {
#if defined(HIAI_DDK)
void* GetPluginHandle()
{
    do {
        // 1. get enhance handle
        void* enhanceHandle = dlopen(HIAI_ENHANCE_LIB_NAME, RTLD_NOW | RTLD_NODELETE);
        if (enhanceHandle == nullptr) {
            FMK_LOGW("dlopen failed, lib[%s], errmsg[%s]", HIAI_ENHANCE_LIB_NAME, dlerror());
            break;
        }
        // 2. get hiaiversion
        const char* verFunctionName = "GetPluginHiAIVersion";
        const char* className = "com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient";
        const char* methodName = "getPluginHiAIVersion";
        const char* methodSignature =
            "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;";
        const char* packageName = "com.huawei.hiai";
        const char* (*function)(const char*, const char*, const char*, const char*, const char*) =
            (const char* (*)(const char*, const char*, const char*, const char*, const char*))dlsym(
                enhanceHandle, verFunctionName);

        if (function == nullptr) {
            FMK_LOGW("dlsym failed, lib[%s], errmsg[%s]", verFunctionName, dlerror());
            break;
        }
        const char* pluginVersion = function(className, methodName, methodSignature, packageName, "");
        if (pluginVersion == nullptr || strncmp(pluginVersion, "000.000.000.000", strlen(pluginVersion)) == 0) {
            FMK_LOGW("pluginVersion is %s", pluginVersion);
            break;
        }
        // 3. get pulgin handle
        const char* functionName = "GetPluginSoHandleDefault";
        GET_PLUGIN_SO_HANDLE_FUNC getPluginSoHandleFunc = nullptr;
        *(void**)(&getPluginSoHandleFunc) = dlsym(enhanceHandle, functionName);
        if (getPluginSoHandleFunc == nullptr) {
            FMK_LOGW("dlsym failed, lib[%s], errmsg[%s]", functionName, dlerror());
            if (dlclose(enhanceHandle) != 0) {
                FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
            }
            break;
        }

        void* pluginH = getPluginSoHandleFunc(PERSISTANCE_APP_LIB_NAME);
        if (pluginH == nullptr) {
            if (dlclose(enhanceHandle) != 0) {
                FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
            }
            break;
        }
        if (dlclose(enhanceHandle) != 0) {
            FMK_LOGW("dlclose failed, errmsg[%s]", dlerror());
            break;
        }
        return pluginH;
    } while (0);

    return nullptr;
}
#endif
} // namespace

bool ProtoFactory::LoadPersistanceSo()
{
    if (handle_ != nullptr) {
        return true;
    }

#if defined(GRAPH_HOST) || defined(GRAPH_DEVICE) || defined(GRAPH_APP)
    handle_ = dlopen(PERSISTANCE_LIB_NAME, RTLD_NOW | RTLD_NODELETE);
    if (handle_ == nullptr) {
        FMK_LOGE("dlopen host %s failed, errmsg[%s]", PERSISTANCE_LIB_NAME, dlerror());
        return false;
    }
    return true;
#endif

#if defined(HIAI_DDK)
    // dlopen libhiai_graph_proto.so in ddk
    handle_ = dlopen(PERSISTANCE_DDK_LIB_NAME, RTLD_NOW | RTLD_NODELETE);
    if (handle_ != nullptr) {
        FMK_LOGI("dlopen %s in ddk success.", PERSISTANCE_DDK_LIB_NAME);
        return true;
    }
    FMK_LOGW("dlopen %s in ddk failed, errmsg[%s]", PERSISTANCE_DDK_LIB_NAME, dlerror());

    // dlopen libai_fmk_graph_proto_app.so in plugin
    handle_ = GetPluginHandle();
    if (handle_ != nullptr) {
        FMK_LOGI("dlopen %s in pulgin success.", PERSISTANCE_APP_LIB_NAME);
        return true;
    }
    FMK_LOGW("dlopen plugin %s failed, errmsg[%s]", PERSISTANCE_APP_LIB_NAME, dlerror());

    // dlopen libai_fmk_graph_proto in rom
    handle_ = dlopen(PERSISTANCE_ABS_ROM_LIB_NAME, RTLD_NOW | RTLD_NODELETE);
    if (handle_ != nullptr) {
        FMK_LOGI("dlopen %s success.", PERSISTANCE_ABS_ROM_LIB_NAME);
        return true;
    }
    FMK_LOGW("dlopen %s failed, errmsg[%s]", PERSISTANCE_ABS_ROM_LIB_NAME, dlerror());
#endif

    return false;
}

void* ProtoFactory::GetSymbols(const std::string& funcName)
{
    std::lock_guard<std::mutex> guard(loadSoMutex);
    if (!LoadPersistanceSo()) {
        return nullptr;
    }
    return dlsym(handle_, funcName.c_str());
}

IModelDef* ProtoFactory::CreateModelDef()
{
    if (createModelDef_ != nullptr) {
        return createModelDef_();
    }
    *reinterpret_cast<void**>(&createModelDef_) = GetSymbols("CreateModelDef");
    if (createModelDef_ != nullptr) {
        return createModelDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyModelDef(IModelDef* modelDef)
{
    if (destroyModelDef_ != nullptr) {
        return destroyModelDef_(modelDef);
    }
    *reinterpret_cast<void**>(&destroyModelDef_) = GetSymbols("DestroyModelDef");
    if (destroyModelDef_ != nullptr) {
        return destroyModelDef_(modelDef);
    }
}

IGraphDef* ProtoFactory::CreateGraphDef()
{
    if (createGraphDef_ != nullptr) {
        return createGraphDef_();
    }
    *reinterpret_cast<void**>(&createGraphDef_) = GetSymbols("CreateGraphDef");
    if (createGraphDef_ != nullptr) {
        return createGraphDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyGraphDef(IGraphDef* graphDef)
{
    if (destroyGraphDef_ != nullptr) {
        return destroyGraphDef_(graphDef);
    }
    *reinterpret_cast<void**>(&destroyGraphDef_) = GetSymbols("DestroyGraphDef");
    if (destroyGraphDef_ != nullptr) {
        return destroyGraphDef_(graphDef);
    }
}

IOpDef* ProtoFactory::CreateOpDef()
{
    if (createOpDef_ != nullptr) {
        return createOpDef_();
    }
    *reinterpret_cast<void**>(&createOpDef_) = GetSymbols("CreateOpDef");
    if (createOpDef_ != nullptr) {
        return createOpDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyOpDef(IOpDef* opDef)
{
    if (destroyOpDef_ != nullptr) {
        return destroyOpDef_(opDef);
    }
    *reinterpret_cast<void**>(&destroyOpDef_) = GetSymbols("DestroyOpDef");
    if (destroyOpDef_ != nullptr) {
        return destroyOpDef_(opDef);
    }
}

ITensorDescDef* ProtoFactory::CreateTensorDescDef()
{
    if (createTensorDescDef_ != nullptr) {
        return createTensorDescDef_();
    }
    *reinterpret_cast<void**>(&createTensorDescDef_) = GetSymbols("CreateTensorDescDef");
    if (createTensorDescDef_ != nullptr) {
        return createTensorDescDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyTensorDescDef(ITensorDescDef* tensorDescDef)
{
    if (destroyTensorDescDef_ != nullptr) {
        return destroyTensorDescDef_(tensorDescDef);
    }
    *reinterpret_cast<void**>(&destroyTensorDescDef_) = GetSymbols("DestroyTensorDescDef");
    if (destroyTensorDescDef_ != nullptr) {
        return destroyTensorDescDef_(tensorDescDef);
    }
}

ITensorDef* ProtoFactory::CreateTensorDef()
{
    if (createTensorDef_ != nullptr) {
        return createTensorDef_();
    }
    *reinterpret_cast<void**>(&createTensorDef_) = GetSymbols("CreateTensorDef");
    if (createTensorDef_ != nullptr) {
        return createTensorDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyTensorDef(ITensorDef* tensorDef)
{
    if (destroyTensorDef_ != nullptr) {
        return destroyTensorDef_(tensorDef);
    }
    *reinterpret_cast<void**>(&destroyTensorDef_) = GetSymbols("DestroyTensorDef");
    if (destroyTensorDef_ != nullptr) {
        return destroyTensorDef_(tensorDef);
    }
}

IShapeDef* ProtoFactory::CreateShapeDef()
{
    if (createShapeDef_ != nullptr) {
        return createShapeDef_();
    }
    *reinterpret_cast<void**>(&createShapeDef_) = GetSymbols("CreateShapeDef");
    if (createShapeDef_ != nullptr) {
        return createShapeDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyShapeDef(IShapeDef* shapeDef)
{
    if (destroyShapeDef_ != nullptr) {
        return destroyShapeDef_(shapeDef);
    }
    *reinterpret_cast<void**>(&destroyShapeDef_) = GetSymbols("DestroyShapeDef");
    if (destroyShapeDef_ != nullptr) {
        return destroyShapeDef_(shapeDef);
    }
}

IAttrDef* ProtoFactory::CreateAttrDef()
{
    if (createAttrDef_ != nullptr) {
        return createAttrDef_();
    }
    *reinterpret_cast<void**>(&createAttrDef_) = GetSymbols("CreateAttrDef");
    if (createAttrDef_ != nullptr) {
        return createAttrDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyAttrDef(IAttrDef* attrDef)
{
    if (destroyAttrDef_ != nullptr) {
        return destroyAttrDef_(attrDef);
    }
    *reinterpret_cast<void**>(&destroyAttrDef_) = GetSymbols("DestroyAttrDef");
    if (destroyAttrDef_ != nullptr) {
        return destroyAttrDef_(attrDef);
    }
}

IAttrMapDef* ProtoFactory::CreateAttrMapDef()
{
    if (createAttrMapDef_ != nullptr) {
        return createAttrMapDef_();
    }
    *reinterpret_cast<void**>(&createAttrMapDef_) = GetSymbols("CreateAttrMapDef");
    if (createAttrMapDef_ != nullptr) {
        return createAttrMapDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyAttrMapDef(IAttrMapDef* attrMapDef)
{
    if (destroyAttrMapDef_ != nullptr) {
        return destroyAttrMapDef_(attrMapDef);
    }
    *reinterpret_cast<void**>(&destroyAttrMapDef_) = GetSymbols("DestroyAttrMapDef");
    if (destroyAttrMapDef_ != nullptr) {
        return destroyAttrMapDef_(attrMapDef);
    }
}

INamedAttrDef* ProtoFactory::CreateNamedAttrDef()
{
    if (createNamedAttrDef_ != nullptr) {
        return createNamedAttrDef_();
    }
    *reinterpret_cast<void**>(&createNamedAttrDef_) = GetSymbols("CreateNamedAttrDef");
    if (createNamedAttrDef_ != nullptr) {
        return createNamedAttrDef_();
    }
    return nullptr;
}

void ProtoFactory::DestroyNamedAttrDef(INamedAttrDef* namedDef)
{
    if (destroyNamedAttrDef_ != nullptr) {
        return destroyNamedAttrDef_(namedDef);
    }
    *reinterpret_cast<void**>(&destroyNamedAttrDef_) = GetSymbols("DestroyNamedAttrDef");
    if (destroyNamedAttrDef_ != nullptr) {
        return destroyNamedAttrDef_(namedDef);
    }
}

} // namespace hiai