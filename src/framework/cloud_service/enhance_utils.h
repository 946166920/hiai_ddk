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
#ifndef ENHANCE_UTILS_H
#define ENHANCE_UTILS_H

#include <jni.h>
#include <string>
#include <unordered_map>
#include <thread>
#include "framework/infra/log/log.h"

namespace hiai {
/*!
 * \brief Get JNIENV
 */
JNIEnv* GetJniEnv();

/*!
 * \brief Get context
 */
void* GetAppContext();

/*!
 * \brief Set JNIENV and context to map
 * \param env the JNIENV pointer
 * \param context the context "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)J"
 */
bool SetJniEnv(JNIEnv* jniEnv, void* context);

/*!
 * \brief Set JNIENV and context
 * \param env the JNIENV pointer
 * \param context the context "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)J"
 */
bool UtilsEnable(void* jniEnv, void* context);

/*!
 * \brief Clear JNIENV and context
 */
bool UtilsDisable();

/*!
 * \brief Check JNIENV and context Vaild
 */
bool CheckVaild();

/*!
 * \brief jni class jsring to c++ class string
 * \param jStr jni class jstring
 * \return c++ class string
 */
std::string Jstring2string(jstring jStr);

/*!
 * \brief jni class jsring to c++ class string
 * \param jStr jni class jstring
 * \return c++ class string
 */
jstring String2jString(std::string str);

/*!
 * \brief c++ std string class to const char *
 * \param Str std stirng class
 * \return const char *
 */
const char* String2ConstCharPtr(std::string str);

template <typename T, typename P>
constexpr bool OneOf_(T val, P item)
{
    return val == item;
}
template <typename T, typename P, typename... Args>
constexpr bool OneOf_(T val, P item, Args... item_others)
{
    return val == item || OneOf_(val, item_others...);
}

/*!
 * \brief Check input is nullptr
 * \param packageName packageName
 * \param functionName functionName
 * \param methodSignature methodSignature
 */
template <typename... Args>
inline bool AnyNull(Args... ptrs)
{
    return OneOf_(nullptr, ptrs...);
}

template <typename R, typename... Args>
class CallEnhanceFunction_ {
public:
    R operator()(jclass arg1, jmethodID arg2, void* arg3, Args... args)
    {
        JNIEnv* jniEnv = GetJniEnv();
        R ret = static_cast<R>(jniEnv->CallStaticObjectMethod(arg1, arg2, arg3, std::forward<Args>(args)...));
        return ret;
    }
};

template <typename... Args>
class CallEnhanceFunction_<long, Args...> {
public:
    long operator()(jclass arg1, jmethodID arg2, void* arg3, Args... args)
    {
        JNIEnv* jniEnv = GetJniEnv();
        if (jniEnv == nullptr) {
            return 0;
        }
        long ret = jniEnv->CallStaticLongMethod(arg1, arg2, arg3, std::forward<Args>(args)...);
        return ret;
    }
};

template <typename... Args>
class CallEnhanceFunction_<std::string, Args...> {
public:
    std::string operator()(jclass arg1, jmethodID arg2, void* arg3, Args... args)
    {
        JNIEnv* jniEnv = GetJniEnv();
        if (jniEnv == nullptr) {
            return "";
        }
        jstring ret =
            static_cast<jstring>(jniEnv->CallStaticObjectMethod(arg1, arg2, arg3, std::forward<Args>(args)...));
        return Jstring2string(ret);
    }
};

/*!
 * \brief Run plugin function
 * \param packageName packageName
 * \param functionName functionName
 * \param methodSignature methodSignature
 * \param argsCount argsCount arg count
 * \code
 *   const char *notSupportCase = "deconv"
 *   jstring notSupportCaseStr = jniEnv->NewStringUTF(notSupportCase);
 *   string className = "com/huawei/hiai/computecapability/ComputeCapabilityDynamicClient";
 *   string functionName = "getPluginHiAIVersion";
 *   string methodSignature = "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String";
 *   jstring arg1 = String2jString("deconv");
 *   CallEnhanceFunction<string>(className, functionName, methodSignature, arg1);
 * \endcode
 */
template <typename R, typename... Args>
R CallEnhanceFunction(
    const std::string& className, const std::string& functionName, const std::string& methodSignature, Args... args)
{
    R ret;
    JNIEnv* jniEnv = GetJniEnv();
    void* context = GetAppContext();
    if (AnyNull(jniEnv, context)) {
        FMK_LOGE("jniEnv or context is nullptr");
        return ret;
    }
    // Get jniClass
    jclass runClass = static_cast<jclass>(jniEnv->NewGlobalRef(jniEnv->FindClass(className.c_str())));
    if (AnyNull(runClass)) {
        return ret;
    }
    // Get runmethodID
    jmethodID runmethodID = jniEnv->GetStaticMethodID(runClass, functionName.c_str(), methodSignature.c_str());
    if (AnyNull(runmethodID)) {
        return ret;
    }
    ret = CallEnhanceFunction_<R, Args...>()(runClass, runmethodID, context, std::forward<Args>(args)...);
    return ret;
}
} // namespace hiai
#endif
