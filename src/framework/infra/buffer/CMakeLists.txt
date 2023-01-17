
hi_cc_library_static(
  NAME
    ai::fmk::infra::native_handle_static
  SRCS
    hiai_native_handle.c
)

hi_cc_library_static(
  NAME
    ai::fmk::infra::buffer_static
  SRCS
    hiai_shared_buffer.c
    local_buffer.cpp
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)

hi_cc_library_static(
  NAME
    ai::fmk::infra::base_buffer_static
  SRCS
    base_buffer.cpp
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)
