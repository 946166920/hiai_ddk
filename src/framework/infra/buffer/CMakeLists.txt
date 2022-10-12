
hi_cc_objects(
  NAME
    ai::fmk::infra::native_handle_objs
  SRCS
    hiai_native_handle.c
  DEPS
    huawei::c_sec
)

hi_system(HI_LINUX_HOST
  NAME
    ai::fmk::infra::native_handle_objs
  CDEFS
    HIAI_DDK
    HIAI_C_API_VISIABLE
)

hi_cc_objects(
  NAME
    ai::fmk::infra::buffer_objs
  SRCS
    hiai_shared_buffer.c
    local_buffer.cpp
  DEPS
    huawei::c_sec
)

hi_system("HI_ANDROID_NDK;HI_LINUX_HOST"
  NAME
    ai::fmk::infra::buffer_objs
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)

hi_cc_objects(
  NAME
    ai::fmk::infra::base_buffer_objs
  SRCS
    base_buffer.cpp
  DEPS
    huawei::c_sec
)

hi_system("HI_ANDROID_NDK;HI_LINUX_HOST"
  NAME
    ai::fmk::infra::base_buffer_objs
  CDEFS
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
)

hi_generator(
  NAMES
    ai::fmk::infra::native_handle_objs
    ai::fmk::infra::buffer_objs
    ai::fmk::infra::base_buffer_objs
)
