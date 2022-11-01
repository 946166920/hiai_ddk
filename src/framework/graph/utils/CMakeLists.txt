hi_cc_library_static(
  NAME
    ai::fmk::graph::utils_objs
  CDEFS
    GRAPH_API_VISIABLE
  SRCS
    *.cpp
    checker/*.cpp
    replacer/*.cpp
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::fmk::graph::utils_objs
  DEPS
    ai::fmk::graph::proto_objs
    google::protobuf_headers
    huawei::itrustee_sdk_securec_headers
)

hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::graph::utils_objs
  DEPS
    huawei::c_sec
)

hi_cc_system("HI_ANDROID_ROM;HI_LINUX_HOST"
  NAME
    ai::fmk::graph::utils_objs
  DEPS
    huawei::c_sec
    ai::fmk::graph::proto_shared
)

# hi_generator(
#   NAMES
#     ai::fmk::graph::utils_objs
# )