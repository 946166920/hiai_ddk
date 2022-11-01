hi_add_subdirectory(version)
hi_add_subdirectory(tensor)

hi_cc_library_static(
  NAME
    ai::fmk::util::base_types_objs
  SRCS
    hiai_base_types.c
  DEPS
    huawei::c_sec
  COPTS
    -frtti
    -dl
)

hi_cc_library_static(
  NAME
    ai::fmk::util::native_handle_objs
  INCS
    third_party/system/core/include/cutils/
  SRCS
    native_handle_creator.cpp
  CDEFS
    HIAI_UTIL_API_VISIABLE
)

hi_cc_library_static(
  NAME
    ai::fmk::util::file_static
  SRCS
    file_util.cpp
  CDEFS
    HIAI_UTIL_API_VISIABLE
  DEPS
    ai::infra::log_static
    ai::fmk::infra::base_buffer_objs
)

hi_cc_library_shared(
  NAME
    ai::fmk::util_shared
  DEPS
    ai::fmk::util::file_static
    ai::fmk::util::version_objs
    ai::fmk::util::hiai_version_objs
  CDEFS
    HIAI_UTIL_API_VISIABLE
  COPTS
    -march=armv8.2-a+simd+fp16
    -std=c++11
)

hi_cc_system(HI_ANDROID_AOSP
  NAME
    ai::fmk::util_shared
  DEPS
    ai::fmk::infra::buffer_objs
    ai::fmk::infra::base_buffer_objs
    ai::fmk::infra::native_handle_objs
    ai::infra::dl_helper_objs
  DEPS
    ai::fmk::common_shared
    huawei::c_sec
    ai::fmk::hiai_c_def_shared
    # ai::infra::mmpa
    ai::infra::log
  MULTILIB
    64
)

hi_cc_library_static(
  NAME
    ai::fmk::util::dl_helper_static
  SRCS
    hiai_foundation_dl_helper.c
  CDEFS
    HIAI_C_API_VISIABLE
  DEPS
    ai::fmk::util::hiai_version_objs
)

hi_cc_library_static(
  NAME
    ai::fmk::util::model_type_util_objs
  SRCS
    model_type_util.cpp
  DEPS
    huawei::c_sec
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::fmk::util::model_type_util_objs
  CDEFS
    __AICP_TA__
  DEPS
    huawei::itrustee_sdk_securec_headers
    huawei::itrustee_basic_headers
)

# hi_generator(
#   NAMES
#     ai::fmk::util::file_static
#     ai::fmk::util::base_types_objs
#     ai::fmk::util::dl_helper_static
#     ai::fmk::util::native_handle_objs
#     ai::fmk::util::model_type_util_objs
#     ai::fmk::util_shared
# )
