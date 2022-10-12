/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2020. All rights reserved.
 * Description: ai om
 */

#ifndef INC_OM_AI_OM_H
#define INC_OM_AI_OM_H

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AI_API_EXPORT
#ifndef _MSC_VER
#define AI_API_EXPORT __attribute__((__visibility__("default")))
#else
#define AI_API_EXPORT __declspec(dllexport)
#endif
#endif

enum AI_OM_EventType {
    AI_NONE_EVENT = -1, // for omit compiler warning
    AI_PROFILING_ON,
    AI_PROFILING_OFF,
    AI_DUMP_ON,
    AI_DUMP_OFF,
    AI_LOG_SETTING,
    AI_ISPNN_PROFILING_ON,
    AI_ISPNN_PROFILING_OFF,
    AI_GLOBAL_PROFILING_ON,
    AI_GLOBAL_PROFILING_OFF,
};

typedef struct AI_OM_Config AI_OM_Config;

/*
 * Create AI_OM_Config object.
 * @return AI_OM_Config object.
 */
AI_API_EXPORT AI_OM_Config* AI_OM_Config_Create();

/*
 * Destroy AI_OM_Config object.
 */
AI_API_EXPORT void AI_OM_Config_Destroy(AI_OM_Config* config);

/*
 * Add a config item to AI_OM_Config object.
 * @param config AI_OM_Config object.
 * @param name config item's name.
 * @param value config item's value.
 * @return true on success, false on failure.
 */
AI_API_EXPORT bool AI_OM_Config_Set(AI_OM_Config* config, const char* name, const char* value);

/*
 * Get a config item's value from AI_OM_Config.
 * @param config AI_OM_Config object.
 * @param name config item's name.
 * @return config item's value if exists(config object manage it, caller no need to free it), nullptr if not exists.
 */
AI_API_EXPORT const char* AI_OM_Config_Get(AI_OM_Config* config, const char* name);

/*
 * AI_OM_Config visitor function pointer type.
 * @param name config item's name.
 * @param value config item's value.
 * @param userdata addtional userdata bind with AI_OM_Config_Accept call.
 */
typedef void (*AI_OM_Config_Visitor)(const char* name, const char* value, void* data);

/*
 * visit each config items in AI_OM_Config object.
 * @param config AI_OM_Config object.
 * @param visitor visitor function for each config item.
 * @param userdata addtional userdata for visitor.
 * @return number of config items.
 */
AI_API_EXPORT size_t AI_OM_Config_ForEach(AI_OM_Config* config, AI_OM_Config_Visitor visitor, void* userData);

/*
 * Build a AI_OM_Config object to string.
 * @param config AI_OM_Config object.
 * @return string builded from AI_OM_Config object, the caller need to free it.
 */
AI_API_EXPORT char* AI_OM_Config_BuildString(AI_OM_Config* config);

/*
 * Parse a prebuilt config string to AI_OM_Config object.
 * @param configStr prebuilt config string.
 * @return AI_OM_Config object if success(the caller need to destroy it).
 */
AI_API_EXPORT AI_OM_Config* AI_OM_Config_ParseString(const char* configStr);

struct AI_OM_EventMsg {
    AI_OM_EventType type;
    int pid; // client pid, for service distinct event source.
    AI_OM_Config* config;
};

typedef struct AI_OM_EventListener AI_OM_EventListener;

/*
 * OM event handling function pointer type define.
 * @param event  AI_OM_EventMsg ojbect.
 * @param userdata user binding data.
 */
typedef void (*AI_OM_EventHandler)(const AI_OM_EventMsg* event, void* userdata);

/*
 * Create AI_OM_EventListener object.
 * @param handler point to a OM event handling function.
 * @param userdata user binding data.
 * @return AI_OM_EventListener object when success, nullptr on failure.
 */
AI_API_EXPORT AI_OM_EventListener* AI_OM_EventListener_Create(AI_OM_EventHandler handler, void* userData);

/*
 * Destroy AI_OM_EventListener object.
 * @param listener AI_OM_EventListener object.
 */
AI_API_EXPORT void AI_OM_EventListener_Destroy(AI_OM_EventListener* listener);

/*
 * Register AI_OM_EventListener object to OM module.
 * @param listener AI_OM_EventListener object
 * @return 0 on success, otherwise errorcode returned.
 */
AI_API_EXPORT uint32_t AI_OM_RegisterListener(AI_OM_EventListener* listener);

/*
 * Post and AI_OM_EventMsg AI_OM_EventListener object to OM moudle.
 * @param listener AI_OM_EventListener object
 * @return 0 on success, otherwise errorcode returned.
 */
AI_API_EXPORT uint32_t AI_OM_PostEvent(const AI_OM_EventMsg* event);

#ifdef __cplusplus
}
#endif

#endif // INC_OM_AI_OM_H