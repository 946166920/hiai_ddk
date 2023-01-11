/*!
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
#ifndef ENHANCE_C_API_H_
#define ENHANCE_C_API_H_

#ifdef ENHANCE_EXPORTS
#ifdef _MSC_VER
#define ENHANCE_DLL __declspec(dllexport)
#else
#define ENHANCE_DLL __attribute__((__visibility__("default")))
#endif
#else
#define ENHANCE_DLL
#endif

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * \brief Get so from plugin resource
 * \param className className
 * \param functionName functionName
 * \param methodSignature methodSignature
 * \param soName soName
 */
ENHANCE_DLL long GetPluginSoHandle(const char* className, const char* functionName, const char* methodSignature,
    const char* packageName, const char* soName);
/*!
 * \brief Get so from plugin resource
 * \param soName soName
 */
ENHANCE_DLL long GetPluginSoHandleDefault(const char* soName);
/*!
 * \brief Get plugin version
 * \param className className
 * \param functionName functionName
 * \param methodSignature methodSignature
 * \param notSupportCase notSupportCase like "deconv"
 */
ENHANCE_DLL const char* GetPluginHiAIVersion(
    const char* className,
    const char* functionName,
    const char* methodSignature,
    const char* packageName,
    const char* notSupportCase);
/*!
 * \brief Get plugin version
 */
ENHANCE_DLL const char* GetPluginHiAIVersionDefault(void);
/*!
 * \brief Get plugin versionName
 */
ENHANCE_DLL const char* GetPluginVersionName(void);
/*!
 * \brief Set jnienv pointer
 * \param jniEnv jniEnv jni pointer
 * \param context context
 */
ENHANCE_DLL bool CloudServiceEnable(void* jniEnv, void* context);

/*!
 * \brief clear jnienv pointer
 */
ENHANCE_DLL bool CloudServiceDisable(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif // ENHANCE_C_API_H_
