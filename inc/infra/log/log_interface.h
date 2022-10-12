#ifndef _INC_INFRA_LOG_INTERFACE_H_
#define _INC_INFRA_LOG_INTERFACE_H_

#ifndef BUILD_VERSION_ENG
#define H_LOG_INTERFACE(...)
#define H_LOG_INTERFACE_FILTER(...)
#else
#define H_LOG_INTERFACE() \
    AI_Log_Print(AI_LOG_INFO, "interface_test:", "%s|%s|%s|%d", "hiai", ITF_COMPONENT_NAME, __FUNCTION__, __LINE__)
#define H_LOG_INTERFACE_FILTER(N) \
    do { \
        static int _count = 0; \
        if (_count == 0) { \
            H_LOG_INTERFACE(); \
        } \
        if (++_count == N) { \
            _count = 0; \
        } \
    } while (0)
#endif

#endif // _INC_INFRA_LOG_INTERFACE_H_
