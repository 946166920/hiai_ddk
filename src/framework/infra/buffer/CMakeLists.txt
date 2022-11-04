
hi_cc_library_static(
  NAME
    ai::fmk::infra::native_handle_static
  SRCS
    hiai_native_handle.c
  # DEPS
  #   huawei::c_sec
)

hi_cc_library_static(
  NAME
    ai::fmk::infra::buffer_static
  SRCS
    hiai_shared_buffer.c
    local_buffer.cpp
  # DEPS
  #   huawei::c_sec
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)

# hi_cc_system("HI_ANDROID_NDK;HI_LINUX_HOST"
#   NAME
#     ai::fmk::infra::buffer_static
#   CDEFS
#     HIAI_UTIL_API_VISIABLE
#     HIAI_TENSOR_API_VISIABLE
# )

hi_cc_library_static(
  NAME
    ai::fmk::infra::base_buffer_static
  SRCS
    base_buffer.cpp
  # DEPS
  #   huawei::c_sec     # 符号依赖
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)

# hi_cc_system("HI_ANDROID_NDK;HI_LINUX_HOST"
#   NAME
#     ai::fmk::infra::base_buffer_static
#   CDEFS
#     HIAI_UTIL_API_VISIABLE
#     HIAI_TENSOR_API_VISIABLE
# )