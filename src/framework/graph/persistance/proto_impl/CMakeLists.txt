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
    ${ROOT_DIR}/third_party/protobuf/src
  SRCS
    ai::fmk::graph::persistance::proto_srcs
  CDEFS
    GOOGLE_PROTOBUF_NO_RTTI
    GOOGLE_PROTOBUF_NO_THREADLOCAL
  COPTS
    -fvisibility=hidden
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::proto_impl_static
  INCS
    ${CC_OUT_DIR}     # 依赖ai::fmk::graph::persistance::proto_impl::ge_ir_static生成的.pb.h文件
    ${ROOT_DIR}/third_party/protobuf/src
  SRCS
    *.cpp
  CDEFS
    GRAPH_API_VISIABLE
  COPTS
    -fvisibility=hidden
  # DEPS
  #   ai::infra::log
  #   ai::fmk::graph::persistance::proto_impl::ge_ir_static
)