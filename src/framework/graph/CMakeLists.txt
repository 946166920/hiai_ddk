#cmake_minimum_required(VERSION 3.13...3.20)

hi_add_subdirectory(utils)
# hi_add_subdirectory(infershape)
hi_add_subdirectory(persistance)

set(cpp_out_dir ${CMAKE_CURRENT_BINARY_DIR})


# hi_message(FATAL_ERROR "cxx graph/CMakeLists.txt 001: ${ROOT_DIR}/out/hiai/lib/libprotobuf.so")

hi_proto_gen(
  FULL
  NAME
    ai::fmk::graph::proto_full_srcs
  PROTO_PATH
    ${ROOT_DIR}/inc/common/proto
  PROTO_SRCS
    om.proto
    task.proto
  PROTO_OUT
    ${cpp_out_dir}
)

# hi_cc_prebuilt_library(
#   SHARED
#   NAME
#     ai::fmk::graph::protobuf_full
#   LIB
#     ${ROOT_DIR}/out/hiai/lib/libprotobuf.so
#   INCS
#     ${cpp_out_dir}
# )

hi_message(WARNING "cxxx cpp_out_dir: ${cpp_out_dir}")

hi_cc_library_static(
  NAME
    ai::fmk::graph::proto_objs
  INCS
    ${cpp_out_dir}
    ${cpp_out_dir}/../
  SRCS
    ai::fmk::graph::proto_full_srcs
  # DEPS
  #   ai::fmk::graph::protobuf_full
)


# hi_cc_library_proto(
#   NAME
#     ai::fmk::graph::proto_objs
#   COPTS
#     -fvisibility=default
#   PROTOC
#     ${ROOT_DIR}/tools/open_source/protoc/protoc-3.13.0/protoc
#   PROTO_PATH
#     ${ROOT_DIR}/vendor/hisi/npu/inc/common
#   SRCS
#     proto/om.proto
#     proto/task.proto
# )

# hi_cc_library_shared(
#   NAME
#   ai::fmk::graph::proto_objs
#   DEPS
#     huawei::c_sec
# )



# hi_cc_system(HI_LINUX_TEE
#   NAME
#     ai::fmk::graph::proto_objs
#   CDEFS
#     GOOGLE_PROTOBUF_NO_RTTI
#     GOOGLE_PROTOBUF_NO_THREADLOCAL
#   PUB_DEPS
#     huawei::itrustee_basic_headers
#     google::protobuf_headers
# )

hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::graph::proto_objs
  PUB_DEPS
    google::protobuf_lite_static
    google::protobuf_headers
)

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::proto_objs
#   INCS
#     external/protobuf/src
#   DEPS
#     protobuf-cpp-full
# )

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::proto_objs
#   PUB_DEPS
#     google::protobuf_full_shared
# )

# hi_message(FATAL_ERROR ${ROOT_DIR}/inc/common/proto/insert_op.proto)
hi_proto_gen(
  FULL
  NAME
    ai::fmk::graph::proto_aipp_srcs
  PROTO_PATH
    ${ROOT_DIR}/inc/common/proto
  PROTO_SRCS
    insert_op.proto
  PROTO_OUT
    ${cpp_out_dir}
)

hi_cc_prebuilt_library(
  SHARED
  NAME
    ai::fmk::graph::protobuf_aipp
  LIB
    ${ROOT_DIR}/out/hiai/lib/libprotobuf.so
  INCS
    # ${ROOT_DIR}/out/proto_lite
    ${cpp_out_dir}
    ${ROOT_DIR}/third_party/protobuf/src
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::aipp_proto_objs
  INCS
    # ${ROOT_DIR}/out/proto_lite
    ${cpp_out_dir}
    ${cpp_out_dir}/../
    ${ROOT_DIR}/third_party/protobuf/src
  SRCS
    ai::fmk::graph::proto_aipp_srcs
  DEPS
    ai::fmk::graph::protobuf_aipp
)

# hi_cc_library_proto(
#   NAME
#     ai::fmk::graph::aipp_proto_objs
#   COPTS
#     -fvisibility=default
#   PROTOC
#     ${ROOT_DIR}/tools/open_source/protoc/protoc-3.13.0/protoc
#   PROTO_PATH
#     ${ROOT_DIR}/vendor/hisi/npu/inc/common
#   SRCS
#     proto/insert_op.proto
# )

# hi_cc_library_shared(
#   NAME
#   ai::fmk::graph::aipp_proto_objs
#   DEPS
#     huawei::c_sec
# )

hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::graph::aipp_proto_objs
  PUB_DEPS
    google::protobuf_lite_static
)

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::aipp_proto_objs
#   INCS
#     external/protobuf/src
#   DEPS
#     protobuf-cpp-full
# )

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::aipp_proto_objs
#   PUB_DEPS
#     google::protobuf_full_shared
# )

hi_cc_library_shared(
  NAME
    ai::fmk::graph::proto_shared
  COPTS
    -fvisibility=default
  PUB_DEPS
    ai::fmk::graph::proto_objs
)

hi_cc_function(AI_SUPPORT_AIPP
  NAME
    ai::fmk::graph::proto_shared
  DEPS
    ai::fmk::graph::aipp_proto_objs
)

hi_cc_library_static(
  NAME
    ai::fmk::graph::core_objs
  SRCS
    core/cgraph/*.cpp
    core/edge/*.cpp
    core/node/*.cpp
    core/op/*.cpp
    detail/*.cpp
    ./*.cpp
  CDEFS
    GRAPH_API_VISIABLE
)

# hi_cc_system(HI_ANDROID_AOSP
#   NAME
#     ai::fmk::graph::core_objs
#   INCS
#     external/protobuf/src
#   DEPS
#     huawei::c_sec
#     ai::fmk::graph::proto_shared
# )

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph::core_objs
#   DEPS
#     huawei::c_sec
#     ai::fmk::graph::proto_shared
# )

hi_cc_system(HI_ANDROID_NDK
  NAME
    ai::fmk::graph::core_objs
  DEPS
    huawei::c_sec
)

hi_cc_system(HI_LINUX_TEE
  NAME
    ai::fmk::graph::core_objs
  DEPS
    ai::fmk::graph::proto_objs
    huawei::itrustee_sdk_securec_headers
)

hi_cc_library_shared(
  NAME
    ai::fmk::graph_shared
  CDEFS
    GRAPH_API_VISIABLE
  DEPS
    huawei::c_sec
    ai::infra::log
    ai::fmk::graph::core_objs
    ai::fmk::graph::utils_objs
    # ai::fmk::graph::infershape_objs
    ai::fmk::graph::persistance::dynamic_proxy_objs
)

# hi_cc_system(HI_LINUX_HOST
#   NAME
#     ai::fmk::graph_shared
#   DEPS
#     ai::fmk::graph::infershape_gen
# )

# hi_cc_library_static(
#   NAME
#     ai::fmk::graph_static
#   DEPS
#     ai::fmk::graph::core_objs
#     ai::fmk::graph::utils_objs
#     ai::fmk::graph::proto_objs
#     ai::fmk::graph::persistance::proto_impl_static
#     ai::fmk::graph::persistance::proto_impl::ge_ir_objs
#     ai::fmk::graph::persistance::static_proxy_objs
# )

# libhiai_ir.so external
hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_shared
  DEPS
    ai::infra::log
    huawei::c_sec
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
    ai::fmk::graph::core_objs
    ai::fmk::graph::utils_objs
    ai::fmk::graph::persistance::proto_impl_objs
    ai::fmk::graph::persistance::static_proxy_objs
    ai::fmk::compatible_objs
    ai::fmk::common::helper_objs
    ai::infra::math::fp16_t_objs
    ai::fmk::util::tensor::trans_tensor_objs
    google::protobuf_lite_static
)

# libhiai_ir_internal.so
hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_internal_shared
  DEPS
    ai::infra::log
    huawei::c_sec
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
    ai::fmk::graph::core_objs
    ai::fmk::graph::utils_objs
    ai::fmk::graph::persistance::dynamic_proxy_objs
    ai::fmk::compatible_objs
    ai::fmk::common::helper_objs
    ai::infra::math::fp16_t_objs
    ai::fmk::util::tensor::trans_tensor_objs
    google::protobuf_lite_static
  LOPTS
    -Wl,-soname,libhiai_ir.so
)

# hi_generator(
#   NAMES
#     ai::fmk::graph::proto_objs
#     ai::fmk::graph::aipp_proto_objs
#     ai::fmk::graph::proto_shared
#     ai::fmk::graph::core_objs
#     ai::fmk::graph_shared
# )