hi_cc_library_static(
  NAME
    ai::infra::log_static
  COPTS
    -fvisibility=default
)

hi_cc_system("HI_ANDROID_NDK"
  NAME
    ai::infra::log_static
  SRCS
    android_log.c
  DEPS
    log     # liblog.so in ndk
)

hi_cc_library_shared(
  NAME
    ai::infra::log_shared
  DEPS
    ai::infra::log_static
)

# hi_target_output_name(ai::infra::log_shared hiai_log)

hi_enable_empty_srcs(
  NAME
  ai::infra::log_shared
)

hi_cc_library(
  NAME
    ai::infra::log
  IMPLS
    AI_USE_LOG_SHARED ai::infra::log_shared
    AI_USE_LOG_STATIC ai::infra::log_static
)
