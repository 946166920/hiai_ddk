set(cpp_out_dir ${CMAKE_CURRENT_BINARY_DIR})
hi_proto_gen(
  LITE
  NAME
    ai::fmk::graph::persistance::proto_srcs
  PROTO_PATH
    ${CMAKE_CURRENT_LIST_DIR}
  PROTO_SRCS
    proto/ge_ir.proto
  PROTO_OUT
    ${cpp_out_dir}
)
hi_get_target_property(ai::fmk::graph::persistance::proto_srcs HI_GEN_INCS incgen)
hi_message(WARNING "cxxx incgen: ${incgen}")

# hi_cc_prebuilt_library(
#   SHARED
#   NAME
#     ai::fmk::graph::persistance::protobuf_lite
#   LIB
#     ${ROOT_DIR}/out/hiai/lib/libprotobuf-lite.so
#   INCS
#     ${cpp_out_dir}
# )

hi_message(WARNING "cxxx cpp_out_dir: ${cpp_out_dir}")

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
  INCS
    ${cpp_out_dir}
    ${cpp_out_dir}/../
  SRCS
    ai::fmk::graph::persistance::proto_srcs
  # DEPS
  #   ai::fmk::graph::persistance::protobuf_lite
)

# hi_cc_library_proto(
#   NAME
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
#   PROTOC
#     ${ROOT_DIR}/tools/open_source/protoc/protoc-3.13.0/protoc
#   PROTO_PATH
#     ${ROOT_DIR}/vendor/hisi/npu/inc/common
#   SRCS
#     proto/ge_ir.proto
# )

# hi_message(WARNING "cxxx staticinc: ${ROOT_DIR}/out/proto_lite")

hi_cc_library_static(
  NAME
    ai::fmk::graph::persistance::proto_impl_objs
  INCS
    # ${ROOT_DIR}/out/proto_lite
    ${cpp_out_dir}
  SRCS
    *.cpp
  CDEFS
    GRAPH_API_VISIABLE
  DEPS
    ai::infra::log
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
)


# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
#   INCS
#     vendor/open_source/protobuf/src/
#   COPTS
#     -fvisibility=default
# )

# hi_cc_library_shared(
#   NAME
#     ai::fmk::graph::persistance::proto_impl_shared
#   SRCS
#     *.cpp
#   CDEFS
#     GRAPH_API_VISIABLE
#   DEPS
#     ai::infra::log
#   PUB_DEPS
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
# )

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::persistance::proto_impl_shared
#   CDEFS
#     GRAPH_DEVICE
# )

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::persistance::proto_impl_shared
#   CDEFS
#     GRAPH_HOST
# )

# hi_cc_library_static(
#   NAME
#     ai::fmk::graph::persistance::proto_impl_objs
#   SRCS
#     *.cpp
#   CDEFS
#     GRAPH_API_VISIABLE
#   DEPS
#     ai::infra::log
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
# )

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::persistance::proto_impl_shared
#   INCS
#     external/protobuf/src
#   COPTS
#     -fvisibility=default
#   DEPS
#     protobuf-cpp-full
# )

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
#   COPTS
#     -fvisibility=default
# )

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::persistance::proto_impl_shared
#   DEPS
#     google::protobuf_full_shared
#   CDEFS
#     GRAPH_HOST
#   COPTS
#     -fvisibility=default
# )

# hi_cc_system(HI_LINUX_TEE
#   NAME
#     ai::fmk::graph::persistance::proto_impl_objs
#   COPTS
#     -fvisibility=default
#   DEPS
#     huawei::itrustee_basic_headers
#     google::protobuf_headers
# )

# hi_cc_system(HI_LINUX_TEE
#   NAME
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
#   CDEFS
#     GOOGLE_PROTOBUF_NO_RTTI
#     GOOGLE_PROTOBUF_NO_THREADLOCAL
#   COPTS
#     -fvisibility=default
#   DEPS
#     huawei::itrustee_basic_headers
#     google::protobuf_headers
# )

hi_cc_system("HI_ANDROID_NDK"
  NAME
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
  CDEFS
    GOOGLE_PROTOBUF_NO_RTTI
    GOOGLE_PROTOBUF_NO_THREADLOCAL
  COPTS
    -fvisibility=hidden
  PUB_DEPS
    google::protobuf_headers
)

hi_cc_system("HI_ANDROID_NDK"
  NAME
    ai::fmk::graph::persistance::proto_impl_objs
  COPTS
    -fvisibility=hidden
)

# hi_generator(
#   NAMES
#     ai::fmk::graph::persistance::proto_impl::ge_ir_static
#     ai::fmk::graph::persistance::proto_impl_shared
# )