hi_cc_library_static(
  NAME
    ai::fmk::util::tensor::trans_tensor_objs
  SRCS
    trans_tensor.cpp
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::fmk::util::tensor::trans_tensor_objs
  CDEFS
    USE_STATIC_LIB
  DEPS
    huawei::itrustee_basic_headers
    huawei::itrustee_sdk_securec_headers
)

hi_cc_system("HI_ANDROID_AOSP;HI_LINUX_HOST;HI_ANDROID_NDK"
  NAME
    ai::fmk::util::tensor::trans_tensor_objs
  DEPS
    huawei::c_sec
)

# hi_generator(
#   NAMES
#     ai::fmk::util::tensor::trans_tensor_objs
# )