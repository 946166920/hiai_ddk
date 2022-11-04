# hi_cc_library_static(
#   NAME
#     ai::fmk::graph::persistance::dynamic_proxy_static
#   SRCS
#     dynamic_proto_factory.cpp
#   CDEFS
#     GRAPH_DEVICE
#   # DEPS
#   #   huawei::c_sec
#   #   dl
# )

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::static_proxy_static
  INCS
    ${CMAKE_CURRENT_BINARY_DIR}/../proto_impl   # 依赖ai::fmk::graph::persistance::proto_impl::ge_ir_static生成的.pb.h文件
    ${ROOT_DIR}/third_party/protobuf/src
  SRCS
    static_proto_factory.cpp
  CDEFS
    GRAPH_DEVICE
  # DEPS
  #   huawei::c_sec
  #   ai::fmk::graph::persistance::proto_impl::ge_ir_static
  #   google::protobuf_headers
)