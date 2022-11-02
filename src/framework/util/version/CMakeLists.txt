hi_cc_library_static(
  NAME
    ai::fmk::util::version_static
  SRCS
    version_util.cpp
  # DEPS
  #   # ai::infra::mmpa
  #   huawei::c_sec
  CDEFS
    HIAI_UTIL_API_VISIABLE
)

# hi_cc_system(HI_ANDROID_NDK
#   NAME
#     ai::fmk::util::version_static
#   CDEFS
#     HIAI_UTIL_API_VISIABLE
# )

hi_cc_library_static(
  NAME
    ai::fmk::util::hiai_version_static
  SRCS
    hiai_version.c
  CDEFS
    HIAI_C_API_VISIABLE
  DEPS
    # ai::infra::mmpa
    huawei::c_sec   # 符号依赖
)
