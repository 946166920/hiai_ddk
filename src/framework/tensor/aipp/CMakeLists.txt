hi_cc_library_static(
  NAME
    ai::fmk::tensor::aipp_para_static
  SRCS
    aipp_para_impl.cpp
    hiai_tensor_aipp_para_legacy.c
    hiai_tensor_aipp_para_local.c
    hiai_tensor_aipp_para.c
  CDEFS
    HIAI_TENSOR_API_VISIABLE
  COPTS
    -frtti
  DEPS
    huawei::c_sec
)

hi_cc_system(HI_LINUX_HOST
  NAME
    ai::fmk::tensor::aipp_para_static
  CDEFS
    _Float16=uint16_t
)

hi_cc_system(HI_ANDROID_AOSP
  NAME
    ai::fmk::tensor::aipp_para_static
  CDEFS
    HIAI_C_API_VISIABLE
)

# hi_generator(
#   NAMES
#     ai::fmk::tensor::aipp_para_static
# )