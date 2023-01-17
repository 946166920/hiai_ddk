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
#ifndef HIAI_FRAMEWORK_LEGACY_HIAI_MODEL_MANAGER_H
#define HIAI_FRAMEWORK_LEGACY_HIAI_MODEL_MANAGER_H

#include "hiai_model_manager_type.h"
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

typedef struct HIAI_ModelManagerListenerLegacy {
    void (*onLoadDone)(void* userdata, int taskStamp);
    void (*onRunDone)(void* userdata, int taskStamp);
    void (*onUnloadDone)(void* userdata, int taskStamp);
    void (*onTimeout)(void* userdata, int taskStamp);
    void (*onError)(void* userdata, int taskStamp, int errCode);
    void (*onServiceDied)(void* userdata);

    void* userdata;
} HIAI_ModelManagerListenerLegacy;

typedef struct HIAI_ModelManager HIAI_ModelManager;

typedef struct HIAI_ModelManagerMemAllocator_struct {
    hiai::NativeHandle (*Allocate)(int32_t size);
    void (*Free)(hiai::NativeHandle handle);
} HIAI_ModelManagerMemAllocator;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __HIAI_MODEL_MANAGER_H__
