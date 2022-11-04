hi_add_subdirectory(tensor)
hi_add_subdirectory(version)

hi_cc_library_static(
  NAME
    ai::fmk::util::base_types_static
  SRCS
    hiai_base_types.c
  COPTS
    -frtti
    -dl
)

hi_cc_library_static(
  NAME
    ai::fmk::util::native_handle_static
  INCS
    ${ROOT_DIR}/third_party/system/core/include/cutils/
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
  # DEPS    # 符号依赖
  #   ai::infra::log_static
  #   ai::fmk::infra::base_buffer_static
)
hi_cc_library_static(
  NAME
    ai::fmk::common::file_util_static
  SRCS
    ../common/file_util.cpp
)

# hi_cc_library_shared(
#   NAME
#     ai::fmk::util_shared
#   DEPS
#     huawei::c_sec
#     ai::fmk::util::file_static
#     ai::fmk::util::version_static
#     ai::fmk::util::hiai_version_static
#   CDEFS
#     HIAI_UTIL_API_VISIABLE
#   COPTS
#     -march=armv8.2-a+simd+fp16
#     -std=c++11
# )

hi_cc_library_static(
  NAME
    ai::fmk::util::dl_helper_static
  SRCS
    hiai_foundation_dl_helper.c
  CDEFS
    HIAI_C_API_VISIABLE
  # DEPS
  #   ai::fmk::util::hiai_version_static    # 符号依赖
)

hi_cc_library_static(
  NAME
    ai::fmk::util::model_type_util_static
  SRCS
    model_type_util.cpp
  # DEPS
  #   huawei::c_sec
)
