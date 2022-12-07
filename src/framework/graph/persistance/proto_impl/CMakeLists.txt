set(CC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

hi_proto_gen(
  LITE
  NAME
    ai::fmk::graph::persistance::proto_srcs
  PROTO_PATH
    ${CMAKE_CURRENT_LIST_DIR}
  PROTO_SRCS
    proto/ge_ir.proto
  PROTO_OUT
    ${CC_OUT_DIR}
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
  INCS
    ${CC_OUT_DIR}
  SRCS
    ai::fmk::graph::persistance::proto_srcs
  CDEFS
    GOOGLE_PROTOBUF_NO_RTTI
    GOOGLE_PROTOBUF_NO_THREADLOCAL
  COPTS
    -fvisibility=hidden
  PUB_DEPS
    google::protobuf_headers
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::proto_impl_static
  INCS
    ${CC_OUT_DIR}
  SRCS
    *.cpp
  CDEFS
    GRAPH_API_VISIABLE
  COPTS
    -fvisibility=default
  PUB_DEPS
    google::protobuf_headers
)