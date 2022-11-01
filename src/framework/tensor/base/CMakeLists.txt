# hi_cc_library_static(
#   NAME
#     ai::fmk::tensor::nd_tensor_v1_objs
  # SRCS
  #   hiai_nd_tensor_buffer_v1.c
#   DEPS
#     huawei::c_sec
# )

hi_cc_library_static(
  NAME
    ai::fmk::tensor::nd_tensor_static
  SRCS
    hiai_nd_tensor_buffer_local.c
    hiai_nd_tensor_buffer_legacy.cpp
    hiai_nd_tensor_buffer.c
    hiai_nd_tensor_desc.c
    hiai_nd_tensor_buffer_util.c
  DEPS
    huawei::c_sec
    ai::infra::log_static
    ai::fmk::util::dl_helper_static
    ai::fmk::util::base_types_objs
  CDEFS
    HIAI_C_API_VISIABLE
  COPTS
    -frtti
)

hi_cc_function(AI_SUPPORT_LEGACY_ROM_COMPATIBLE
  NAME
    ai::fmk::tensor::nd_tensor_static
  SRCS
    hiai_nd_tensor_buffer_legacy_compatible.cpp
)

hi_cc_library_static(
  NAME
    ai::fmk::tensor::nd_tensor_impl_objs
  SRCS
    nd_tensor_buffer_impl.cpp
  DEPS
    huawei::c_sec
  COPTS
    -frtti
)

hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::tensor::nd_tensor_impl_objs
  CDEFS
    HIAI_TENSOR_API_VISIABLE
)

hi_cc_library_static(
  NAME
    ai::fmk::tensor::nd_tensor_ext_objs
  SRCS
    hiai_nd_tensor_buffer_ext.cpp
  DEPS
    huawei::c_sec
  COPTS
    -frtti
)

# hi_generator(
#   NAMES
#     ai::fmk::tensor::nd_tensor_v1_objs
#     ai::fmk::tensor::nd_tensor_static
#     ai::fmk::tensor::nd_tensor_impl_objs
#     ai::fmk::tensor::nd_tensor_ext_objs
# )
