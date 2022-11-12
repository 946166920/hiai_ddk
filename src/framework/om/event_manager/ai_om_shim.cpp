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

#include <cstdlib>
#include <cstring>
#include <functional>
#include <dlfcn.h>
#include <unistd.h> // for access

#include "securec.h"
#include "common/debug/log.h"
#include "infra/om/event_manager/ai_om.h"

#ifndef REAL_FILE_NAME
#define REAL_FILE_NAME strrchr(__FILE__, '/')
#endif

#if defined __LP64__
static const char* LIB_FILE = "/vendor/lib64/libai_om_event_manager.so";
static const char* OLD_LIB_FILE = "/vendor/lib64/libhiai_om.so";
#else
static const char* LIB_FILE = "/vendor/lib/libai_om_event_manager.so";
static const char* OLD_LIB_FILE = "/vendor/lib/libhiai_om.so";
#endif

static void* LoadLibrary()
{
    void* handle = dlopen(LIB_FILE, RTLD_NOW);
    if (handle == nullptr) {
        FMK_LOGE("dlopen failed: %s, try open:%s.", dlerror(), OLD_LIB_FILE);
        handle = dlopen(OLD_LIB_FILE, RTLD_NOW);
        if (handle == nullptr) {
            FMK_LOGE("dlopen: %s failed.", OLD_LIB_FILE);
        }
    }
    return handle;
}

static void* GetHandle()
{
    static void* handle = LoadLibrary();
    return handle;
}

__attribute__((destructor)) static void UnloadLibrary()
{
    if (GetHandle() != nullptr) {
        dlclose(GetHandle());
    }
}

static void* GetSymbol(const char* name)
{
    if (GetHandle() != nullptr) {
        return dlsym(GetHandle(), name);
    }
    return nullptr;
}

#define LOAD_FUNCTION(func)                                                                                            \
    const char* functionName = #func;                                                                                  \
    std::function<decltype(func)> functionObject = reinterpret_cast<decltype(func)*>(GetSymbol(#func)); \
    FMK_LOGI("try calling %s", #func)

#define CHECK_FUNCTION_HAS_RETVAL()                                                                                    \
    if (!functionObject || GetHandle() == nullptr) {                                                                   \
        FMK_LOGE("dlsym(%s) failed: %s", functionName, dlerror());                                                    \
        return decltype(functionObject)::result_type{};                                                                \
    }

#define CALL_FUNCTION(...) functionObject(__VA_ARGS__)

AI_OM_Config* AI_OM_Config_Create()
{
    LOAD_FUNCTION(AI_OM_Config_Create);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION();
}

void AI_OM_Config_Destroy(AI_OM_Config* config)
{
    LOAD_FUNCTION(AI_OM_Config_Destroy);
    if (!functionObject || GetHandle() == nullptr) {
        FMK_LOGE("dlsym(%s) failed: %s", functionName, dlerror());
        return;
    }
    CALL_FUNCTION(config);
}

bool AI_OM_Config_Set(AI_OM_Config* config, const char* name, const char* value)
{
    LOAD_FUNCTION(AI_OM_Config_Set);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(config, name, value);
}

const char* AI_OM_Config_Get(AI_OM_Config* config, const char* name)
{
    LOAD_FUNCTION(AI_OM_Config_Get);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(config, name);
}

size_t AI_OM_Config_ForEach(AI_OM_Config* config, AI_OM_Config_Visitor visitor, void* userData)
{
    LOAD_FUNCTION(AI_OM_Config_ForEach);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(config, visitor, userData);
}

char* AI_OM_Config_BuildString(AI_OM_Config* config)
{
    LOAD_FUNCTION(AI_OM_Config_BuildString);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(config);
}

AI_OM_Config* AI_OM_Config_ParseString(const char* configStr)
{
    LOAD_FUNCTION(AI_OM_Config_ParseString);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(configStr);
}

AI_OM_EventListener* AI_OM_EventListener_Create(AI_OM_EventHandler handler, void* userData)
{
    LOAD_FUNCTION(AI_OM_EventListener_Create);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(handler, userData);
}

void AI_OM_EventListener_Destroy(AI_OM_EventListener* listener)
{
    LOAD_FUNCTION(AI_OM_EventListener_Destroy);
    if (!functionObject || GetHandle() == nullptr) {
        FMK_LOGE("dlsym(%s) failed: %s", functionName, dlerror());
        return;
    }
    CALL_FUNCTION(listener);
}

uint32_t AI_OM_RegisterListener(AI_OM_EventListener* listener)
{
    LOAD_FUNCTION(AI_OM_RegisterListener);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(listener);
}

uint32_t AI_OM_PostEvent(const AI_OM_EventMsg* event)
{
    LOAD_FUNCTION(AI_OM_PostEvent);
    CHECK_FUNCTION_HAS_RETVAL();
    return CALL_FUNCTION(event);
}
