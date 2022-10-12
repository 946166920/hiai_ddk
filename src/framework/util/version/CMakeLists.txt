hi_cc_objects(
  NAME
    ai::fmk::util::version_objs
  SRCS
    version_util.cpp
  DEPS
    ai::infra::mmpa
    huawei::c_sec
)

hi_system(HI_ANDROID_NDK
  NAME
    ai::fmk::util::version_objs
  CDEFS
    HIAI_UTIL_API_VISIABLE
)

hi_cc_objects(
  NAME
    ai::fmk::util::hiai_version_objs
  SRCS
    hiai_version.c
  CDEFS
    HIAI_C_API_VISIABLE
  DEPS
    ai::infra::mmpa
    huawei::c_sec
)

hi_generator(
  NAMES
    ai::fmk::util::version_objs
    ai::fmk::util::hiai_version_objs
)