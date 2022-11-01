hi_cc_library_static(
  NAME
    ai::infra::math::fp16_t_objs
  SRCS
    fp16_t.cpp
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::infra::math::fp16_t_objs
  CDEFS
    USE_STATIC_LIB
  DEPS
    huawei::itrustee_basic_headers
)

hi_cc_system("HI_ANDROID_AOSP;HI_LINUX_HOST"
  NAME
    ai::infra::math::fp16_t_objs
  DEPS
    huawei::c_sec
)

# hi_generator(
#   NAMES
#     ai::infra::math::fp16_t_objs
# )raph/persistance/proto_impl/CMakeLists.txt: