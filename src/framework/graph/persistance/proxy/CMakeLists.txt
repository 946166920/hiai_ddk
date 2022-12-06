hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::dynamic_proxy_static
  SRCS
    dynamic_proto_factory.cpp
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::static_proxy_static
  SRCS
    static_proto_factory.cpp
  CDEFS
    GRAPH_DEVICE
  DEPS
    google::protobuf_headers
    # 依赖ai::fmk::graph::persistance::proto_impl::ge_ir_static生成的.pb.h文件
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
)