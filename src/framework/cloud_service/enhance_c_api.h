/*!
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * \file include/enhance_c_api.h
 * \brief C API of ENHANCE Function
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
