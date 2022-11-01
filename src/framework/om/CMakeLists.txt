hi_cc_library_static(
  NAME
    ai::fmk::om::om_wrapper_objs
  SRCS
    event_manager/om_wrapper.cpp
    event_manager/ai_om_shim.cpp
  DEPS
    huawei::c_sec
)

hi_cc_library_shared(
  NAME
    ai::fmk::om_shared
  SRCS
    event_manager/ai_om_shim.cpp
    # npu_om_comm.cpp
    # om_event_listener.cpp
    # profiling_handler.cpp
  COPTS
    -frtti
  CDEFS
    AI_SUPPORT_PROFILING
    DEV_VISIBILITY
  DEPS
    ai::infra::log
    huawei::c_sec
    # ai::infra::om::trace_engine_shared
)

hi_cc_system(HI_LINUX_HOST
  NAME
    ai::fmk::om_shared
  DEPS
    dl
)

hi_cc_system(HI_ANDROID_AOSP
  NAME
    ai::fmk::om_shared
  MULTILIB
    first
)

hi_cc_function(AI_SUPPORT_DUMP
  NAME
    ai::fmk::om_shared
  SRCS
    dump_handler.cpp
  CDEFS
    AI_SUPPORT_DUMP
)

hi_cc_function(AI_SUPPORT_LOG_SETTING
  NAME
    ai::fmk::om_shared
  SRCS
    log_handler.cpp
  CDEFS
    AI_SUPPORT_LOG_SETTING
)

# hi_generator(
#   NAMES
#     ai::fmk::om_shared
# )