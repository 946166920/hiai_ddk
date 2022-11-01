hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::dynamic_proxy_objs
  SRCS
    dynamic_proto_factory.cpp
  CDEFS
    GRAPH_DEVICE
  DEPS
    huawei::c_sec
    dl
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::static_proxy_objs
  SRCS
    static_proto_factory.cpp
  CDEFS
    GRAPH_DEVICE
  DEPS
    huawei::c_sec
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::fmk::graph::persistance::static_proxy_objs
  DEPS
    huawei::itrustee_basic_headers
    google::protobuf_headers
)
hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::graph::persistance::static_proxy_objs
  DEPS
    google::protobuf_headers
)

# hi_generator(
#   NAMES
#     ai::fmk::graph::persistance::dynamic_proxy_objs
# )