hi_cc_objects(
  NAME
    ai::infra::log_objs
  COPTS
    -fvisibility=default
)

hi_system("HI_ANDROID_AOSP;HI_ANDROID_NDK"
  NAME
    ai::infra::log_objs
  SRCS
    android_log.c
  DEPS
    log     # liblog.so in android
)

hi_system(HI_LINUX_HOST
  NAME
    ai::infra::log_objs
  SRCS
    linux_log.c
  DEPS
    huawei::c_sec
)

hi_system(HI_LINUX_TEE
  NAME
    ai::infra::log_objs
  SRCS
    tee_log.c
  DEPS
    huawei::itrustee_sdk
    huawei::itrustee_sdk_securec_headers
)

hi_function(AI_SUPPORT_LOG_LEVEL_SETTING
  NAME
    ai::infra::log_objs
  SRCS
    log_level.cpp
)

hi_cc_library_shared(
  NAME
    ai::infra::log_shared
  DEPS
    ai::infra::log_objs
  COPTS
    -fvisibility=default
)

hi_cc_library_static(
  NAME
    ai::infra::log_static
  DEPS
    ai::infra::log_objs
)

hi_cc_library(
  NAME
    ai::infra::log
  IMPLS
    ai::infra::log_shared
    ai::infra::log_static
)

hi_generator(
  NAMES
    ai::infra::log_objs
    ai::infra::log
)