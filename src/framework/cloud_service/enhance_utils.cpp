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
#include "enhance_utils.h"
#include <string>
#include "mutex"
#include <sys/syscall.h>
#include <securec.h>

#define gettid() syscall(SYS_gettid);
#define MAX_PLUGIN_VERSION_LEN 32
static std::unordered_map<std::thread::id, std::pair<JNIEnv*, void*>> g_tidEnvMap {};
static std::mutex appLock;
using namespace std;

namespace hiai {
JNIEnv* GetJniEnv()
{
    std::lock_guard<std::mutex> lock(appLock);
    std::thread::id threadTid = std::this_thread::get_id();
    auto iter = g_tidEnvMap.find(threadTid);
    if (iter != g_tidEnvMap.end()) {
        auto envPair = iter->second;

        return envPair.first;
    }
    return nullptr;
}

void* GetAppContext()
{
    std::lock_guard<std::mutex> lock(appLock);
    std::thread::id threadTid = std::this_thread::get_id();
    auto iter = g_tidEnvMap.find(threadTid);
    if (iter != g_tidEnvMap.end()) {
        auto envPair = iter->second;

        return envPair.second;
    }
    return nullptr;
}

bool SetJniEnv(JNIEnv* jniEnv, void* context)
{
    std::lock_guard<std::mutex> lock(appLock);
    std::thread::id threadTid = std::this_thread::get_id();
    auto iter = g_tidEnvMap.find(threadTid);
    if (iter != g_tidEnvMap.end()) {
        return true;
    }
    for (auto iterator : g_tidEnvMap) {
        if (iterator.second.first == jniEnv) {
            FMK_LOGE("multi thread has the same jniEnv");
            return false;
        }
    }
    std::pair<JNIEnv*, void*> tempMap;
    tempMap = make_pair(jniEnv, context);
    g_tidEnvMap.insert(make_pair(threadTid, tempMap));
    return true;
}

bool UtilsEnable(void* jniEnv, void* context)
{
    if (AnyNull(jniEnv, context)) {
        FMK_LOGE("SetNativeJNIEnv failed, jniEnv or context is nullptr");
        return false;
    }
    JNIEnv* jniEnvNew = reinterpret_cast<JNIEnv*>(jniEnv);
    return SetJniEnv(jniEnvNew, context);
}

bool UtilsDisable()
{
    std::thread::id threadTid = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(appLock);
    auto iter = g_tidEnvMap.find(threadTid);
    if (iter != g_tidEnvMap.end()) {
        g_tidEnvMap.erase(iter);
        return true;
    }
    return false;
}

bool CheckVaild()
{
    JNIEnv* jniEnv = GetJniEnv();
    void* context = GetAppContext();
    if (AnyNull(jniEnv, context)) {
        FMK_LOGW("jniEnv or context is nullptr");
        return false;
    }
    return true;
}

const char* String2ConstCharPtr(std::string str)
{
    static char version[MAX_PLUGIN_VERSION_LEN] = {0};
    static std::mutex versionMutex;
    versionMutex.lock();
    if (strcpy_s(version, MAX_PLUGIN_VERSION_LEN, str.c_str()) != EOK) {
        versionMutex.unlock();
        return nullptr;
    }
    versionMutex.unlock();
    return static_cast<const char*>(version);
}

std::string Jstring2string(jstring jStr)
{
    JNIEnv* jniEnv = GetJniEnv();
    if (AnyNull(jniEnv, jStr)) {
        FMK_LOGE("jniEnv or jStr is nullptr");
        return "";
    }
    const jclass stringClass = jniEnv->GetObjectClass(jStr);
    const jmethodID getBytes = jniEnv->GetMethodID(stringClass, "getBytes", "(Ljava/lang/String;)[B");
    const jbyteArray stringJbytes = (jbyteArray)jniEnv->CallObjectMethod(jStr, getBytes, jniEnv->NewStringUTF("UTF-8"));
    size_t length = (size_t)jniEnv->GetArrayLength(stringJbytes);
    jbyte* pBytes = jniEnv->GetByteArrayElements(stringJbytes, NULL);
    std::string ret = std::string((char*)pBytes, length);
    jniEnv->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);
    jniEnv->DeleteLocalRef(stringJbytes);
    jniEnv->DeleteLocalRef(stringClass);
    return ret;
}

jstring String2jString(std::string str)
{
    JNIEnv* jniEnv = GetJniEnv();
    if (AnyNull(jniEnv)) {
        FMK_LOGE("jniEnv is nullptr");
        jstring n;
        return n;
    }
    return jniEnv->NewStringUTF(str.c_str());
}
} // namespace hiai
